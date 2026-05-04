// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/symx.hpp"
#include "vedx64/ir.hpp"
#include "vedx64/codegen.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

using namespace vedx64;
using namespace vedx64::symx;

static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { if (cond) ++g_pass; else { ++g_fail; std::printf("FAIL: %s\n", msg); } } while (0)

// Build a snippet via the CodeGen emit API and return (bytes, anchor map).
// Tests use `add_anchor("name")` between instructions to capture offsets
// for later reference; the second component maps name → byte offset.
struct Snippet {
    std::vector<uint8_t> bytes;
    std::unordered_map<std::string, size_t> anchors;
};

static Snippet emit(std::function<void(CodeGen&)> build) {
    CodeGen g;
    build(g);
    Snippet s;
    s.bytes.assign(g.data(), g.data() + g.size());
    for (const auto& a : g.anchors()) s.anchors[a.name] = a.offset;
    return s;
}

// Wrap a byte vector in the read_code lambda the engine expects.
static std::function<bool(uint64_t, uint8_t*, size_t)> code_reader(
    uint64_t base, const std::vector<uint8_t>& bytes)
{
    return [base, &bytes](uint64_t addr, uint8_t* out, size_t n) {
        if (addr < base || addr + n > base + bytes.size()) return false;
        std::memcpy(out, bytes.data() + (addr - base), n);
        return true;
    };
}

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
        auto snip = emit([](CodeGen& g) {
            g.mov(rax, (int64_t)5);
            g.mov(rcx, (int64_t)7);
            g.add(rax, rcx);
            g.add_anchor("ret");
            g.ret();
        });
        constexpr uint64_t base = 0x1000;
        Config cfg{}; cfg.max_steps_per_path = 32;
        Engine eng(cfg, code_reader(base, snip.bytes));
        auto results = eng.run(base, [&](const State& s) { return s.rip == base + snip.anchors["ret"]; });
        CHECK(results.size() == 1, "engine produced one finished path");
        if (!results.empty()) {
            const Expr* rax = results[0].gpr[0];
            CHECK(is_const(rax) && rax->k == 12, "RAX = 5 + 7 = 12");
        }
    }

    // --- Engine: symbolic input flows through ---
    {
        auto snip = emit([](CodeGen& g) {
            g.mov(rax, rdi);
            g.add(rax, (int64_t)1);
            g.add_anchor("ret");
            g.ret();
        });
        constexpr uint64_t base = 0x2000;
        Config cfg{}; cfg.max_steps_per_path = 16;
        Engine eng(cfg, code_reader(base, snip.bytes));
        eng.seed_state().gpr[7] = eng.builder().k(41, 64);
        auto results = eng.run(base, [&](const State& s) { return s.rip == base + snip.anchors["ret"]; });
        CHECK(results.size() == 1, "symbolic-RDI engine produced one path");
        if (!results.empty()) {
            const Expr* rax = results[0].gpr[0];
            CHECK(is_const(rax) && rax->k == 42, "RAX = 41 + 1 = 42 (concretized)");
        }
    }

    // --- Engine: symbolic branch forks (merging disabled) ---
    {
        auto snip = emit([](CodeGen& g) {
            auto taken = g.label();
            g.cmp(rdi, (int64_t)0);
            g.jz(taken);
            g.mov(rax, (int64_t)1);
            g.add_anchor("ret_fall");
            g.ret();
            g.bind(taken);
            g.mov(rax, (int64_t)2);
            g.add_anchor("ret_taken");
            g.ret();
        });
        constexpr uint64_t base = 0x3000;
        Config cfg{}; cfg.max_steps_per_path = 16; cfg.enable_merging = false;
        Engine eng(cfg, code_reader(base, snip.bytes));
        uint64_t fall = base + snip.anchors["ret_fall"];
        uint64_t take = base + snip.anchors["ret_taken"];
        auto results = eng.run(base, [&](const State& s) { return s.rip == fall || s.rip == take; });
        CHECK(results.size() == 2, "symbolic compare → two paths");
        if (results.size() == 2) {
            CHECK(results[0].rip != results[1].rip, "two paths reach different RIPs");
            const Expr* a = results[0].gpr[0];
            const Expr* b = results[1].gpr[0];
            CHECK(is_const(a) && is_const(b) && a->k != b->k, "RAX differs across the two paths");
        }
    }

    // --- Builder: algebraic peepholes ---
    {
        Builder b;
        const Expr* x = b.sym(64);
        // (x + 5) + 7 → x + 12
        const Expr* lhs = b.add(b.add(x, b.k(5, 64)), b.k(7, 64));
        CHECK(lhs->op == ExprOp::Add && lhs->a == x && is_const(lhs->b) && lhs->b->k == 12,
            "(x+5)+7 collapses to x+12");
        // (x + a) - a → x
        const Expr* a = b.sym(64);
        CHECK(b.sub(b.add(x, a), a) == x, "(x + a) - a → x");
        // (x - 3) + 10 → x + 7
        const Expr* l2 = b.add(b.sub(x, b.k(3, 64)), b.k(10, 64));
        CHECK(l2->op == ExprOp::Add && l2->a == x && is_const(l2->b) && l2->b->k == 7,
            "(x-3)+10 → x+7");
        // x & ~x → 0,  x | ~x → all-ones
        CHECK(is_const(b.band(x, b.bnot(x))) && b.band(x, b.bnot(x))->k == 0, "x & ~x → 0");
        const Expr* allones = b.bor(x, b.bnot(x));
        CHECK(is_const(allones) && allones->k == ~0ULL, "x | ~x → all-ones");
        // ult/sle reflexive
        CHECK(is_const(b.ult(x, x)) && b.ult(x, x)->k == 0, "x <u x → false");
        CHECK(is_const(b.sle(x, x)) && b.sle(x, x)->k == 1, "x <=s x → true");
        // (shl (shl x 4) 8) → shl x 12
        const Expr* shifted = b.shl(b.shl(x, b.k(4, 8)), b.k(8, 8));
        CHECK(shifted->op == ExprOp::Shl && shifted->a == x &&
              is_const(shifted->b) && shifted->b->k == 12, "(shl (shl x 4) 8) coalesces");
    }

    // --- Engine: step ---
    {
        auto snip = emit([](CodeGen& g) {
            g.mov(rax, (int64_t)1);
            g.add(rax, (int64_t)2);
            g.add(rax, (int64_t)3);
        });
        constexpr uint64_t base = 0x4000;
        Config cfg{};
        Engine eng(cfg, code_reader(base, snip.bytes));
        State s = eng.seed_state().fork();
        s.rip = base;
        CHECK(eng.step(s), "step #1 (mov)");
        CHECK(eng.step(s), "step #2 (add)");
        CHECK(eng.step(s), "step #3 (add)");
        const Expr* rax = s.gpr[0];
        CHECK(is_const(rax) && rax->k == 6, "rax = 1 + 2 + 3 = 6 after 3 steps");
    }
    // --- Engine: run_block ---
    {
        auto snip = emit([](CodeGen& g) {
            g.mov(rax, (int64_t)10);
            g.inc(rax);
            g.inc(rax);
            g.inc(rax);
            g.ret();
        });
        constexpr uint64_t base = 0x5000;
        Config cfg{};
        Engine eng(cfg, code_reader(base, snip.bytes));
        State s = eng.seed_state().fork();
        s.rip = base;
        size_t n = eng.run_block(s, 4);
        CHECK(n == 4, "run_block executed 4 instructions");
        const Expr* rax = s.gpr[0];
        CHECK(is_const(rax) && rax->k == 13, "rax = 10 + 1 + 1 + 1 = 13");
    }

    // --- Loop visit cap kills runaway loops ---
    {
        auto snip = emit([](CodeGen& g) {
            auto self = g.label();
            g.bind(self);
            g.jmp(self);
        });
        constexpr uint64_t base = 0x6000;
        Config cfg{}; cfg.max_visits_per_rip = 5; cfg.max_steps_per_path = 1000;
        Engine eng(cfg, code_reader(base, snip.bytes));
        State s = eng.seed_state().fork();
        s.rip = base;
        size_t executed = 0;
        while (eng.step(s)) ++executed;
        CHECK(s.dead, "infinite loop killed by visit cap");
        CHECK(executed <= cfg.max_visits_per_rip + 1, "stopped within visit cap budget");
    }

    // --- CallStack discipline: CALL → target, RET → caller ---
    {
        auto snip = emit([](CodeGen& g) {
            auto callee = g.label();
            g.call(callee);
            g.mov(rcx, (int64_t)1);
            g.add_anchor("outer_ret");
            g.ret();
            g.bind(callee);
            g.mov(rax, (int64_t)42);
            g.ret();
        });
        constexpr uint64_t base = 0x7000;
        Config cfg{};
        Engine eng(cfg, code_reader(base, snip.bytes));
        State s = eng.seed_state().fork();
        s.rip = base;
        // Run until just before the outer ret (where the shadow stack would
        // empty and the path would die on a symbolic-memory pop).
        uint64_t outer_ret = base + snip.anchors["outer_ret"];
        size_t executed = 0;
        while (executed < 8 && s.rip != outer_ret) { eng.step(s); ++executed; }
        CHECK(!s.dead, "callstack-guided run is alive at outer ret");
        CHECK(s.rip == outer_ret, "returned to caller's RIP after callee ret");
        const Expr* rax = s.gpr[0];
        const Expr* rcx = s.gpr[1];
        CHECK(is_const(rax) && rax->k == 42, "RAX = 42 from callee");
        CHECK(is_const(rcx) && rcx->k == 1,  "RCX = 1 from caller after return");
    }

    // --- Memory chunk coalescing: store-then-load returns the source Expr ---
    {
        Builder b;
        Memory m(&b);
        const Expr* addr = b.k(0x4000, 64);
        const Expr* val  = b.add(b.sym(64), b.k(7, 64));   // symbolic, non-const
        m.store(addr, val, 8);
        const Expr* loaded = m.load(addr, 8);
        CHECK(loaded == val, "8B store + 8B load returns the same Expr (no byte chain)");
        // Chunk hit only on exact width: 4B load doesn't short-circuit.
        const Expr* loaded4 = m.load(addr, 4);
        CHECK(loaded4 != val, "4B load of 8B-stored chunk takes the byte path");
    }
    {
        // Eviction: a partial overwrite invalidates the chunk.
        Builder b;
        Memory m(&b);
        const Expr* a8 = b.k(0x5000, 64);
        const Expr* v  = b.add(b.sym(64), b.k(1, 64));
        m.store(a8, v, 8);
        // Overwrite byte 0.
        m.store(a8, b.k(0xAA, 8), 1);
        const Expr* loaded = m.load(a8, 8);
        CHECK(loaded != v, "partial overwrite evicts the matching chunk");
    }

    // --- State merging at convergence ---
    auto build_diamond = [](){
        return emit([](CodeGen& g) {
            auto taken = g.label();
            auto done  = g.label();
            g.cmp(rdi, (int64_t)0);
            g.jz(taken);
            g.mov(rax, (int64_t)1);
            g.jmp(done);
            g.bind(taken);
            g.mov(rax, (int64_t)2);
            g.bind(done);
            g.add_anchor("conv");
            g.nop();   // landing pad for the stop predicate
        });
    };
    {
        auto snip = build_diamond();
        constexpr uint64_t base = 0x8000;
        Config cfg{}; cfg.enable_merging = true;
        Engine eng(cfg, code_reader(base, snip.bytes));
        uint64_t conv = base + snip.anchors["conv"];
        auto results = eng.run(base, [&](const State& s) { return s.rip == conv; });
        CHECK(results.size() == 1, "diamond: two converging paths merged into one");
        if (results.size() == 1) {
            const Expr* rax = results[0].gpr[0];
            CHECK(rax->op == ExprOp::Ite, "merged RAX is an ite");
            CHECK(is_const(rax->b) && is_const(rax->c) &&
                  ((rax->b->k == 1 && rax->c->k == 2) || (rax->b->k == 2 && rax->c->k == 1)),
                  "merged RAX arms are 1 and 2");
        }
    }
    // Same diamond with merging disabled — should produce two separate paths.
    {
        auto snip = build_diamond();
        constexpr uint64_t base = 0x9000;
        Config cfg{}; cfg.enable_merging = false;
        Engine eng(cfg, code_reader(base, snip.bytes));
        uint64_t conv = base + snip.anchors["conv"];
        auto results = eng.run(base, [&](const State& s) { return s.rip == conv; });
        CHECK(results.size() == 2, "without merging: two separate paths");
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
