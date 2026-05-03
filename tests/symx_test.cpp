// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/symx.hpp"
#include "vedx64/ir.hpp"
#include <cstdio>
#include <cstring>

using namespace vedx64;
using namespace vedx64::symx;

static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { if (cond) ++g_pass; else { ++g_fail; std::printf("FAIL: %s\n", msg); } } while (0)

int main() {
    std::printf("=== symx tests ===\n");

    // --- Builder: constant folding ---
    {
        Builder b;
        const Expr* x = b.k(5, 64);
        const Expr* y = b.k(7, 64);
        const Expr* z = b.add(x, y);
        CHECK(z->op == ExprOp::ConstU && z->k == 12, "5 + 7 folds to 12");

        const Expr* sym = b.sym(64);
        const Expr* z2 = b.add(sym, b.k(0, 64));
        CHECK(z2 == sym, "x + 0 → x (peephole)");

        const Expr* z3 = b.bxor(sym, sym);
        CHECK(is_const(z3) && z3->k == 0, "x ^ x → 0");

        const Expr* eq = b.eq(b.k(3, 32), b.k(3, 32));
        CHECK(is_const(eq) && eq->k == 1, "3 == 3 folds to 1");
    }

    // --- Builder: hash-consing ---
    {
        Builder b;
        const Expr* a = b.k(42, 64);
        const Expr* b1 = b.k(42, 64);
        CHECK(a == b1, "ConstU is interned");
        const Expr* x = b.sym(64);
        const Expr* a1 = b.add(x, b.k(1, 64));
        const Expr* a2 = b.add(x, b.k(1, 64));
        CHECK(a1 == a2, "Add is hash-consed");
    }

    // --- Builder: structural peepholes ---
    {
        Builder b;
        const Expr* x = b.sym(64);
        // ~~x → x
        CHECK(b.bnot(b.bnot(x)) == x, "~~x → x");
        // extract(hi, lo, concat(h, l)) — entirely in low half
        const Expr* h = b.sym(32);
        const Expr* l = b.sym(32);
        const Expr* c = b.concat(h, l);   // 64-bit
        CHECK(b.extract(c, 31, 0)  == l, "extract(31,0, concat(h,l)) → l");
        CHECK(b.extract(c, 63, 32) == h, "extract(63,32, concat(h,l)) → h");
        // Spans the boundary — should split into two extracts.
        const Expr* spanned = b.extract(c, 47, 16);
        CHECK(spanned->op == ExprOp::Concat, "spanning extract splits into concat");
        // extract over zext: high bits become zero.
        const Expr* y = b.sym(8);
        const Expr* z = b.zext(y, 32);
        CHECK(b.extract(z, 7, 0) == y, "extract low byte of zext → y");
        const Expr* hi_z = b.extract(z, 23, 16);
        CHECK(is_const(hi_z) && hi_z->k == 0, "extract high byte of zext → 0");
        // concat(#0, x) → zext(x, w)
        const Expr* zero = b.k(0, 32);
        const Expr* zx = b.concat(zero, y);
        CHECK(zx->op == ExprOp::Zext, "concat(#0:32, y:8) → zext(y, 40)");
        // concat of adjacent extracts of same source → single extract
        const Expr* big = b.sym(64);
        const Expr* hi8 = b.extract(big, 15, 8);
        const Expr* lo8 = b.extract(big, 7, 0);
        CHECK(b.concat(hi8, lo8) == b.extract(big, 15, 0), "adjacent extract concat coalesces");
        // trunc(zext(x)) → x  (all three regimes)
        CHECK(b.trunc(b.zext(y, 64), 8)  == y, "trunc to original of zext → y");
        CHECK(b.trunc(b.zext(y, 64), 16)->op == ExprOp::Zext, "trunc-narrows zext");
    }

    // --- Builder: width-changing ops fold ---
    {
        Builder b;
        const Expr* x = b.k(0xDEADBEEFu, 32);
        const Expr* z = b.zext(x, 64);
        CHECK(is_const(z) && z->k == 0xDEADBEEFULL, "zext of constant");
        const Expr* s = b.sext(b.k(0xFFu, 8), 32);
        CHECK(is_const(s) && s->k == 0xFFFFFFFFu, "sext of -1:8 to 32 = -1:32");
        const Expr* e = b.extract(b.k(0x1234u, 16), 7, 0);
        CHECK(is_const(e) && e->k == 0x34, "extract low byte");
    }

    // --- Memory: concrete byte roundtrip ---
    {
        Builder b;
        Memory m(&b);
        const Expr* addr = b.k(0x1000, 64);
        m.store(addr, b.k(0xCAFEBABEDEADBEEFULL, 64), 8);
        const Expr* v = m.load(addr, 8);
        CHECK(is_const(v) && v->k == 0xCAFEBABEDEADBEEFULL, "memory roundtrip 8B");

        const Expr* v32 = m.load(b.k(0x1000, 64), 4);
        CHECK(is_const(v32) && v32->k == 0xDEADBEEFu, "memory roundtrip 4B (low half)");
    }

    // --- Engine: run a tiny program (concrete inputs) ---
    {
        // mov rax, 5      48 C7 C0 05 00 00 00
        // mov rcx, 7      48 C7 C1 07 00 00 00
        // add rax, rcx    48 01 C8
        // ret             C3
        static const uint8_t prog[] = {
            0x48, 0xC7, 0xC0, 0x05, 0x00, 0x00, 0x00,
            0x48, 0xC7, 0xC1, 0x07, 0x00, 0x00, 0x00,
            0x48, 0x01, 0xC8,
            0xC3,
        };
        Config cfg{}; cfg.max_steps_per_path = 32; cfg.stop_on_undef = false;
        Engine eng(cfg, [&](uint64_t addr, uint8_t* out, size_t n) {
            if (addr < 0x1000 || addr + n > 0x1000 + sizeof(prog)) return false;
            std::memcpy(out, prog + (addr - 0x1000), n);
            return true;
        });
        // Stop at the RET (at offset 17 → addr 0x1011).
        auto results = eng.run(0x1000, [&](const State& s) { return s.rip == 0x1011; });
        CHECK(results.size() == 1, "engine produced one finished path");
        if (!results.empty()) {
            const Expr* rax = results[0].gpr[0];
            CHECK(is_const(rax) && rax->k == 12, "RAX = 5 + 7 = 12");
        }
    }

    // --- Engine: symbolic input flows through ---
    {
        // mov rax, rdi   48 89 F8       ; symbolic input from RDI
        // add rax, 1     48 83 C0 01
        // ret            C3
        static const uint8_t prog[] = { 0x48, 0x89, 0xF8, 0x48, 0x83, 0xC0, 0x01, 0xC3 };
        Config cfg{}; cfg.max_steps_per_path = 16;
        Engine eng(cfg, [&](uint64_t addr, uint8_t* out, size_t n) {
            if (addr < 0x2000 || addr + n > 0x2000 + sizeof(prog)) return false;
            std::memcpy(out, prog + (addr - 0x2000), n);
            return true;
        });
        // Constrain RDI to 41 — RAX should fold to 42.
        eng.seed_state().gpr[7] = eng.builder().k(41, 64);
        auto results = eng.run(0x2000, [&](const State& s) { return s.rip == 0x2007; });
        CHECK(results.size() == 1, "symbolic-RDI engine produced one path");
        if (!results.empty()) {
            const Expr* rax = results[0].gpr[0];
            CHECK(is_const(rax) && rax->k == 42, "RAX = 41 + 1 = 42 (concretized)");
        }
    }

    // --- Engine: symbolic branch forks ---
    {
        // The lifter produces CBRANCH with a symbolic condition.
        //   cmp rdi, 0      48 83 FF 00
        //   je  +5          74 05
        //   mov rax, 1      48 C7 C0 01 00 00 00      ; fall-through path
        //   ret             C3
        //   mov rax, 2      48 C7 C0 02 00 00 00      ; taken path
        //   ret             C3
        static const uint8_t prog[] = {
            0x48, 0x83, 0xFF, 0x00,
            0x74, 0x08,
            0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00,
            0xC3,
            0x48, 0xC7, 0xC0, 0x02, 0x00, 0x00, 0x00,
            0xC3,
        };
        Config cfg{}; cfg.max_steps_per_path = 16;
        Engine eng(cfg, [&](uint64_t addr, uint8_t* out, size_t n) {
            if (addr < 0x3000 || addr + n > 0x3000 + sizeof(prog)) return false;
            std::memcpy(out, prog + (addr - 0x3000), n);
            return true;
        });
        // RDI stays symbolic. Stop at either RET — fall-through RET is at
        // 0x300D, taken-branch RET is at 0x3015.
        auto results = eng.run(0x3000, [&](const State& s) {
            return s.rip == 0x300D || s.rip == 0x3015;
        });
        CHECK(results.size() == 2, "symbolic compare → two paths");
        if (results.size() == 2) {
            // Distinct RIPs and distinct RAX values across the two paths.
            CHECK(results[0].rip != results[1].rip, "two paths reach different RIPs");
            const Expr* a = results[0].gpr[0];
            const Expr* b = results[1].gpr[0];
            CHECK(is_const(a) && is_const(b) && a->k != b->k, "RAX differs across the two paths");
        }
    }

#ifdef VEDX64_Z3
    // --- Z3-backed solver: actual SAT/UNSAT/concretization ---
    {
        Builder b;
        Solver s;
        CHECK(Solver::is_smt_backed(), "Z3 backend reports as SMT-backed");
        // Create a constraint: x == 42 under empty PC.
        const Expr* x = b.sym(64);
        const Expr* eq42 = b.eq(x, b.k(42, 64));
        PathCondition pc;
        CHECK(s.sat(pc, eq42) == SolveResult::Sat, "Z3: x == 42 is SAT");
        // x == 42 AND x == 7 → unsat
        pc.push(eq42);
        const Expr* eq7  = b.eq(x, b.k(7, 64));
        CHECK(s.sat(pc, eq7) == SolveResult::Unsat, "Z3: x == 42 AND x == 7 is UNSAT");
        // get_value should pin x to 42 under pc.
        auto v = s.get_value(pc, x);
        CHECK(v.has_value() && *v == 42, "Z3: get_value pins x to 42");
    }
    {
        Builder b;
        Solver s;
        // (x & 1) == 1 enumerates to many odd values; restrict to 4 bits.
        const Expr* x = b.sym(4);
        const Expr* odd = b.eq(b.band(x, b.k(1, 4)), b.k(1, 4));
        PathCondition pc;
        pc.push(odd);
        auto vals = s.enumerate(pc, x, 8);
        // 4-bit odd values: 1, 3, 5, 7, 9, 11, 13, 15 — exactly 8.
        CHECK(vals.size() == 8, "Z3: enumerate finds all 8 odd 4-bit values");
        bool all_odd = true;
        for (auto v : vals) if ((v & 1) == 0) all_odd = false;
        CHECK(all_odd, "Z3: enumerated values are all odd");
    }
#endif

    std::printf("\n%d/%d symx tests passed\n", g_pass, g_pass + g_fail);
    return g_fail > 0 ? 1 : 0;
}
