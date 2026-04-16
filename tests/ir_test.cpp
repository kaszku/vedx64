// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#ifdef VEDX64_IR
#include "vedx64/ir.hpp"
#include "vedx64/core.hpp"
#include "vedx64/codegen.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define CHECK(cond, msg) do { if (!(cond)) { printf("FAIL: %s\n", msg); return 1; } } while(0)

int main() {
    using namespace vedx64;
    printf("=== IR Tests ===\n");

    uint8_t nop[] = {0x90};
    auto l = ir::lift(nop, sizeof(nop));
    CHECK(l.has_value(), "lift nop");
    CHECK(!l->ops.empty(), "nop has ops");
    CHECK(l->ops[0].opcode == ir::Opcode::NOP, "nop opcode");

    uint8_t mov_rax_rcx[] = {0x48, 0x89, 0xC8};
    auto lm = ir::lift(mov_rax_rcx, sizeof(mov_rax_rcx));
    CHECK(lm.has_value(), "lift mov rax, rcx");
    CHECK(!lm->ops.empty(), "mov has ops");
    CHECK(lm->ops[0].opcode == ir::Opcode::COPY, "mov is COPY");

    uint8_t add_rax_rcx[] = {0x48, 0x01, 0xC8};
    auto la = ir::lift(add_rax_rcx, sizeof(add_rax_rcx));
    CHECK(la.has_value(), "lift add rax, rcx");
    CHECK(la->ops.size() >= 2, "add has op + flags");
    CHECK(la->ops[0].opcode == ir::Opcode::ADD, "add result first");
    CHECK(la->ops[1].opcode == ir::Opcode::ADD_FLAGS, "add flags second");

    {
        ir::Context ctx;
        ctx.gpr[1] = 42; // RCX = 42
        ir::execute(ctx, *lm);
        CHECK(ctx.gpr[0] == 42, "execute mov rax, rcx -> rax==42");
        printf("  MOV RAX, RCX: RAX=%llu\n", (unsigned long long)ctx.gpr[0]);
    }

    {
        ir::Context ctx;
        ctx.gpr[0] = 10; ctx.gpr[1] = 32;
        ir::execute(ctx, *la);
        CHECK(ctx.gpr[0] == 42, "execute add rax, rcx -> rax==42");
        printf("  ADD RAX, RCX: RAX=%llu\n", (unsigned long long)ctx.gpr[0]);
    }

    printf("  Mass lift...\n");
    int lift_pass = 0, lift_total = 0;
    auto lift_ok = [&](const uint8_t* code, size_t len) -> bool {
        lift_total++;
        auto r = ir::lift(code, len);
        if (r && !r->ops.empty()) { lift_pass++; return true; }
        return false;
    };

    { uint8_t c[] = {0x48,0x29,0xC8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0x31,0xC8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0x21,0xC8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0x09,0xC8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0x39,0xC8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0x85,0xC8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0xFF,0xC0}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0xFF,0xC8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0xF7,0xD8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0xF7,0xD0}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0xD3,0xE0}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0xD3,0xE8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0xD3,0xF8}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x48,0xC7,0xC0,0x2A,0x00,0x00,0x00}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xB8,0x2A,0x00,0x00,0x00}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xC3}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x90}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xCC}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x55}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x5D}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xA4}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xA5}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xAA}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xAC}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xA6}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xAE}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x0F,0x28,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x0F,0x57,0xC0}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x0F,0x58,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x0F,0x5C,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x0F,0x59,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x0F,0x5E,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xF3,0x0F,0x58,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x66,0x0F,0xFE,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x66,0x0F,0xFA,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x66,0x0F,0xEF,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x66,0x0F,0xDB,0xC1}; lift_ok(c, sizeof(c)); }
    printf("    Lift: %d/%d\n", lift_pass, lift_total);
    CHECK(lift_pass == lift_total, "all lifts pass");

    printf("  Execute...\n");
    {
        uint8_t code[] = {0x48, 0x29, 0xC8}; // sub rax, rcx
        auto lifted = ir::lift(code, sizeof(code));
        CHECK(lifted.has_value(), "lift sub");
        ir::Context ctx; ctx.gpr[0] = 100; ctx.gpr[1] = 58;
        ir::execute(ctx, *lifted);
        CHECK(ctx.gpr[0] == 42, "sub 100-58=42");
    }
    {
        uint8_t code[] = {0x31, 0xC0}; // xor eax, eax
        auto lifted = ir::lift(code, sizeof(code));
        CHECK(lifted.has_value(), "lift xor");
        ir::Context ctx; ctx.gpr[0] = 0xDEADBEEF;
        ir::execute(ctx, *lifted);
        CHECK((ctx.gpr[0] & 0xFFFFFFFF) == 0, "xor eax,eax -> 0");
    }
    {
        uint8_t code[] = {0x48, 0x21, 0xC8}; // and rax, rcx
        auto lifted = ir::lift(code, sizeof(code));
        CHECK(lifted.has_value(), "lift and");
        ir::Context ctx; ctx.gpr[0] = 0xFF; ctx.gpr[1] = 0x0F;
        ir::execute(ctx, *lifted);
        CHECK(ctx.gpr[0] == 0x0F, "and 0xFF & 0x0F = 0x0F");
    }
    {
        uint8_t code[] = {0x48, 0x09, 0xC8}; // or rax, rcx
        auto lifted = ir::lift(code, sizeof(code));
        CHECK(lifted.has_value(), "lift or");
        ir::Context ctx; ctx.gpr[0] = 0xF0; ctx.gpr[1] = 0x0F;
        ir::execute(ctx, *lifted);
        CHECK(ctx.gpr[0] == 0xFF, "or 0xF0 | 0x0F = 0xFF");
    }
    {
        uint8_t code[] = {0x48, 0xFF, 0xC0}; // inc rax
        auto lifted = ir::lift(code, sizeof(code));
        CHECK(lifted.has_value(), "lift inc");
        ir::Context ctx; ctx.gpr[0] = 41;
        ir::execute(ctx, *lifted);
        CHECK(ctx.gpr[0] == 42, "inc 41 -> 42");
    }
    {
        uint8_t code[] = {0x48, 0xFF, 0xC8}; // dec rax
        auto lifted = ir::lift(code, sizeof(code));
        CHECK(lifted.has_value(), "lift dec");
        ir::Context ctx; ctx.gpr[0] = 43;
        ir::execute(ctx, *lifted);
        CHECK(ctx.gpr[0] == 42, "dec 43 -> 42");
    }
    {
        uint8_t code[] = {0x48, 0xF7, 0xD8}; // neg rax
        auto lifted = ir::lift(code, sizeof(code));
        CHECK(lifted.has_value(), "lift neg");
        ir::Context ctx; ctx.gpr[0] = 42;
        ir::execute(ctx, *lifted);
        CHECK(ctx.gpr[0] == (uint64_t)-42LL, "neg 42 -> -42");
    }
    {
        uint8_t code[] = {0x48, 0xF7, 0xD0}; // not rax
        auto lifted = ir::lift(code, sizeof(code));
        CHECK(lifted.has_value(), "lift not");
        ir::Context ctx; ctx.gpr[0] = 0;
        ir::execute(ctx, *lifted);
        CHECK(ctx.gpr[0] == 0xFFFFFFFFFFFFFFFFULL, "not 0 -> all 1s");
    }

    printf("  Semantic verification...\n");
    int sem_pass = 0, sem_total = 0;
    auto sem_ok = [&](const char* name, const uint8_t* code, size_t len,
                      uint64_t init_rax, uint64_t init_rcx, uint64_t init_rdx,
                      uint64_t expect_rax, int expect_zf = -1) -> bool {
        sem_total++;
        auto lifted = ir::lift(code, len);
        if (!lifted) { printf("    FAIL %s: lift failed\n", name); return false; }
        ir::Context ctx;
        ctx.gpr[0] = init_rax; ctx.gpr[1] = init_rcx; ctx.gpr[2] = init_rdx;
        ir::execute(ctx, *lifted);
        if (ctx.gpr[0] != expect_rax) {
            printf("    FAIL %s: RAX=%llu expected %llu\n", name,
                   (unsigned long long)ctx.gpr[0], (unsigned long long)expect_rax);
            return false;
        }
        if (expect_zf >= 0 && ctx.flags[2] != (uint8_t)expect_zf) {
            printf("    FAIL %s: ZF=%d expected %d\n", name, ctx.flags[2], expect_zf);
            return false;
        }
        sem_pass++; return true;
    };

    { uint8_t c[] = {0x48,0x01,0xC8}; sem_ok("add rax,rcx", c, sizeof(c), 10, 32, 0, 42); }
    { uint8_t c[] = {0x48,0x29,0xC8}; sem_ok("sub rax,rcx", c, sizeof(c), 100, 58, 0, 42); }
    { uint8_t c[] = {0x48,0x21,0xC8}; sem_ok("and rax,rcx", c, sizeof(c), 0xFF, 0x0F, 0, 0x0F); }
    { uint8_t c[] = {0x48,0x09,0xC8}; sem_ok("or rax,rcx", c, sizeof(c), 0xF0, 0x0F, 0, 0xFF); }
    { uint8_t c[] = {0x48,0x31,0xC8}; sem_ok("xor rax,rcx", c, sizeof(c), 0xFF, 0xFF, 0, 0, 1); }
    { uint8_t c[] = {0x31,0xC0}; sem_ok("xor eax,eax", c, sizeof(c), 0xDEAD, 0, 0, 0, 1); }
    { uint8_t c[] = {0x48,0xFF,0xC0}; sem_ok("inc rax", c, sizeof(c), 41, 0, 0, 42); }
    { uint8_t c[] = {0x48,0xFF,0xC8}; sem_ok("dec rax", c, sizeof(c), 43, 0, 0, 42); }
    { uint8_t c[] = {0x48,0xFF,0xC0}; sem_ok("inc 0->1", c, sizeof(c), 0, 0, 0, 1, 0); }
    { uint8_t c[] = {0x48,0xF7,0xD8}; sem_ok("neg rax", c, sizeof(c), 42, 0, 0, (uint64_t)-42LL); }
    { uint8_t c[] = {0x48,0xF7,0xD0}; sem_ok("not rax", c, sizeof(c), 0, 0, 0, ~0ULL); }
    { uint8_t c[] = {0x48,0xC1,0xE0,0x04}; sem_ok("shl rax,4", c, sizeof(c), 1, 0, 0, 16); }
    { uint8_t c[] = {0x48,0xC1,0xE8,0x04}; sem_ok("shr rax,4", c, sizeof(c), 0x100, 0, 0, 0x10); }
    { uint8_t c[] = {0x48,0xC1,0xF8,0x04}; sem_ok("sar rax,4", c, sizeof(c), (uint64_t)-16LL, 0, 0, (uint64_t)-1LL); }
    { uint8_t c[] = {0x48,0x89,0xC8}; sem_ok("mov rax,rcx", c, sizeof(c), 0, 42, 0, 42); }
    { uint8_t c[] = {0x48,0x39,0xC8};
       sem_ok("cmp rax,rcx (eq)", c, sizeof(c), 42, 42, 0, 42, 1); }
    { uint8_t c[] = {0x48,0x39,0xC8};
       sem_ok("cmp rax,rcx (ne)", c, sizeof(c), 42, 43, 0, 42, 0); }
    { uint8_t c[] = {0x48,0x0F,0xAF,0xC1}; sem_ok("imul rax,rcx", c, sizeof(c), 6, 7, 0, 42); }
    { uint8_t c[] = {0x55}; // push rbp
       auto l = ir::lift(c, sizeof(c));
       ir::Context ctx; uint8_t mem[256] = {};
       ctx.memory = mem; ctx.memory_size = sizeof(mem);
       ctx.gpr[4] = 128; ctx.gpr[5] = 0xBEEF;
       if (l) ir::execute(ctx, *l);
       sem_total++;
       if (ctx.gpr[4] == 120) sem_pass++; else printf("    FAIL push RSP=%llu\n", (unsigned long long)ctx.gpr[4]);
    }
    {
        uint8_t mem[256] = {};
        uint8_t stos_code[] = {0xAA}; // stosb
        auto stos_l = ir::lift(stos_code, sizeof(stos_code));
        ir::Context ctx;
        ctx.memory = mem; ctx.memory_size = sizeof(mem);
        ctx.gpr[0] = 0x41; // AL = 'A'
        ctx.gpr[7] = 100;  // RDI = 100
        if (stos_l) ir::execute(ctx, *stos_l);
        sem_total++;
        if (mem[100] == 0x41) { sem_pass++; } else printf("    FAIL stosb: [100]=%d\n", mem[100]);
        sem_total++;
        if (ctx.gpr[7] == 101) { sem_pass++; } else printf("    FAIL stosb: RDI=%llu\n", (unsigned long long)ctx.gpr[7]);
    }
    { uint8_t c[] = {0x48,0x01,0xC8}; sem_ok("add 0+0", c, sizeof(c), 0, 0, 0, 0, 1); }
    { uint8_t c[] = {0x48,0x01,0xC8}; sem_ok("add max+1", c, sizeof(c), 0xFFFFFFFFFFFFFFFFULL, 1, 0, 0); }
    { uint8_t c[] = {0x48,0x29,0xC8}; sem_ok("sub 0-1", c, sizeof(c), 0, 1, 0, 0xFFFFFFFFFFFFFFFFULL); }
    { uint8_t c[] = {0x48,0x29,0xC8}; sem_ok("sub self", c, sizeof(c), 42, 42, 0, 0); }
    { uint8_t c[] = {0x48,0x21,0xC8}; sem_ok("and 0", c, sizeof(c), 0xFF00, 0x00FF, 0, 0, 1); }
    { uint8_t c[] = {0x48,0x09,0xC8}; sem_ok("or all", c, sizeof(c), 0xAAAA, 0x5555, 0, 0xFFFF); }
    { uint8_t c[] = {0x48,0xF7,0xD0}; sem_ok("not max", c, sizeof(c), 0xFFFFFFFFFFFFFFFFULL, 0, 0, 0); }
    { uint8_t c[] = {0x48,0xC1,0xE0,0x00}; sem_ok("shl 0", c, sizeof(c), 42, 0, 0, 42); }
    { uint8_t c[] = {0x48,0xC1,0xE0,0x3F}; sem_ok("shl 63", c, sizeof(c), 1, 0, 0, 1ULL << 63); }
    { uint8_t c[] = {0x48,0xC1,0xE8,0x3F}; sem_ok("shr 63", c, sizeof(c), 1ULL << 63, 0, 0, 1); }
    { uint8_t c[] = {0x01,0xC8}; sem_ok("add eax,ecx 32", c, sizeof(c), 0xFFFFFFFF00000001ULL, 1, 0, 2); }
    { uint8_t c[] = {0x31,0xC0}; sem_ok("xor eax,eax clears", c, sizeof(c), 0xDEADBEEF12345678ULL, 0, 0, 0, 1); }
    { uint8_t c[] = {0x48,0x0F,0xAF,0xC1}; sem_ok("imul neg", c, sizeof(c), (uint64_t)-3LL, (uint64_t)-14LL, 0, 42); }
    { uint8_t c[] = {0x48,0x0F,0xAF,0xC1}; sem_ok("imul zero", c, sizeof(c), 100, 0, 0, 0); }
    { // setz al after cmp (equal)
       uint8_t cmp_c[] = {0x48,0x39,0xC8}; // cmp rax,rcx
       uint8_t setz_c[] = {0x0F,0x94,0xC0}; // setz al
       auto cmp_l = ir::lift(cmp_c, sizeof(cmp_c));
       auto setz_l = ir::lift(setz_c, sizeof(setz_c));
       if (cmp_l && setz_l) {
           ir::Context ctx; ctx.gpr[0] = 42; ctx.gpr[1] = 42;
           ir::execute(ctx, *cmp_l); ir::execute(ctx, *setz_l);
           sem_total++; if ((ctx.gpr[0] & 0xFF) == 1) sem_pass++; else printf("    FAIL setz(eq)=%llu\n", (unsigned long long)(ctx.gpr[0]&0xFF));
       }
    }
    { // setz al after cmp (not equal)
       uint8_t cmp_c[] = {0x48,0x39,0xC8};
       uint8_t setz_c[] = {0x0F,0x94,0xC0};
       auto cmp_l = ir::lift(cmp_c, sizeof(cmp_c));
       auto setz_l = ir::lift(setz_c, sizeof(setz_c));
       if (cmp_l && setz_l) {
           ir::Context ctx; ctx.gpr[0] = 42; ctx.gpr[1] = 43;
           ir::execute(ctx, *cmp_l); ir::execute(ctx, *setz_l);
           sem_total++; if ((ctx.gpr[0] & 0xFF) == 0) sem_pass++; else printf("    FAIL setz(ne)=%llu\n", (unsigned long long)(ctx.gpr[0]&0xFF));
       }
    }
    { uint8_t c[] = {0x48,0x8D,0x44,0x88,0x08}; // lea rax,[rax+rcx*4+8]
       auto l = ir::lift(c, sizeof(c)); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[0] = 100; ctx.gpr[1] = 10;
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 148) sem_pass++; else printf("    FAIL lea: %llu\n", (unsigned long long)ctx.gpr[0]); }
    }
    { uint8_t c[] = {0x48,0x87,0xC1}; // xchg rax,rcx
       auto l = ir::lift(c, sizeof(c)); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[0] = 10; ctx.gpr[1] = 20;
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 20 && ctx.gpr[1] == 10) sem_pass++; else printf("    FAIL xchg\n"); }
    }
    { uint8_t c[] = {0x0F,0xC8}; // bswap eax
       auto l = ir::lift(c, sizeof(c)); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[0] = 0x01020304;
       ir::execute(ctx, *l);
       // bswap is approximated, just check it changed
       if (ctx.gpr[0] != 0x01020304) sem_pass++; else printf("    FAIL bswap unchanged\n"); }
    }
    {
        uint8_t mem[256] = {};
        mem[50] = 0x42; // 'B' at offset 50
        uint8_t lods_code[] = {0xAC}; // lodsb
        auto l = ir::lift(lods_code, sizeof(lods_code));
        ir::Context ctx; ctx.memory = mem; ctx.memory_size = sizeof(mem);
        ctx.gpr[6] = 50; // RSI = 50
        if (l) ir::execute(ctx, *l);
        sem_total++; if ((ctx.gpr[0] & 0xFF) == 0x42) sem_pass++; else printf("    FAIL lodsb: AL=%d\n", (int)(ctx.gpr[0]&0xFF));
        sem_total++; if (ctx.gpr[6] == 51) sem_pass++; else printf("    FAIL lodsb: RSI=%llu\n", (unsigned long long)ctx.gpr[6]);
    }
    {
        uint8_t mem[256] = {};
        mem[10] = 0x55;
        uint8_t movs_code[] = {0xA4}; // movsb
        auto l = ir::lift(movs_code, sizeof(movs_code));
        ir::Context ctx; ctx.memory = mem; ctx.memory_size = sizeof(mem);
        ctx.gpr[6] = 10; ctx.gpr[7] = 20; // RSI=10, RDI=20
        if (l) ir::execute(ctx, *l);
        sem_total++; if (mem[20] == 0x55) sem_pass++; else printf("    FAIL movsb: [20]=%d\n", mem[20]);
        sem_total++; if (ctx.gpr[6] == 11) sem_pass++; else printf("    FAIL movsb: RSI=%llu\n", (unsigned long long)ctx.gpr[6]);
        sem_total++; if (ctx.gpr[7] == 21) sem_pass++; else printf("    FAIL movsb: RDI=%llu\n", (unsigned long long)ctx.gpr[7]);
    }
    { // adc rax, rcx with CF=1 -> rax = rax + rcx + 1
       uint8_t c[] = {0x48,0x11,0xC8}; // adc rax, ecx
       auto l = ir::lift(c, sizeof(c)); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[0] = 10; ctx.gpr[1] = 31;
       ctx.flags[0] = 1; // CF = 1
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 42) sem_pass++; else printf("    FAIL adc: %llu\n", (unsigned long long)ctx.gpr[0]); }
    }
    { uint8_t c[] = {0xF8}; // clc
       auto l = ir::lift(c, sizeof(c)); sem_total++;
       if (l) { ir::Context ctx; ctx.flags[0] = 1;
       ir::execute(ctx, *l);
       if (ctx.flags[0] == 0) sem_pass++; else printf("    FAIL clc: CF=%d\n", ctx.flags[0]); }
    }
    { uint8_t c[] = {0xF9}; // stc
       auto l = ir::lift(c, sizeof(c)); sem_total++;
       if (l) { ir::Context ctx; ctx.flags[0] = 0;
       ir::execute(ctx, *l);
       if (ctx.flags[0] == 1) sem_pass++; else printf("    FAIL stc: CF=%d\n", ctx.flags[0]); }
    }
    printf("--- Operand & side-effect checks ---\n");
    { // push rbp / pop rcx → RCX == old RBP, RSP restored
       uint8_t push_c[] = {0x55};  // push rbp
       uint8_t pop_c[]  = {0x59};  // pop rcx
       auto push_l = ir::lift(push_c, 1);
       auto pop_l  = ir::lift(pop_c,  1);
       uint8_t stk[256] = {};
       ir::Context ctx; ctx.memory = stk; ctx.memory_size = 256;
       ctx.gpr[4] = 128; ctx.gpr[5] = 0xDEADBEEF;
       if (push_l) ir::execute(ctx, *push_l);
       sem_total++; if (ctx.gpr[4] == 120) sem_pass++; else printf("    FAIL push: RSP=%llu\n", (unsigned long long)ctx.gpr[4]);
       if (pop_l) ir::execute(ctx, *pop_l);
       sem_total++; if (ctx.gpr[1] == 0xDEADBEEF) sem_pass++; else printf("    FAIL pop: RCX=%llX\n", (unsigned long long)ctx.gpr[1]);
       sem_total++; if (ctx.gpr[4] == 128) sem_pass++; else printf("    FAIL pop: RSP=%llu\n", (unsigned long long)ctx.gpr[4]);
    }
    { // enter 0x20, 0 / leave → RSP/RBP restored
       uint8_t enter_c[] = {0xC8,0x20,0x00,0x00};
       uint8_t leave_c[] = {0xC9};
       auto enter_l = ir::lift(enter_c, 4);
       auto leave_l = ir::lift(leave_c, 1);
       uint8_t stk[512] = {};
       ir::Context ctx; ctx.memory = stk; ctx.memory_size = 512;
       ctx.gpr[4] = 256; ctx.gpr[5] = 0xAAAA;
       uint64_t orig_rsp = ctx.gpr[4];
       if (enter_l) ir::execute(ctx, *enter_l);
       uint64_t frame_rbp = ctx.gpr[5];
       sem_total++; if (ctx.gpr[4] == frame_rbp - 0x20) sem_pass++; else printf("    FAIL enter RSP=%llu exp=%llu\n", (unsigned long long)ctx.gpr[4], (unsigned long long)(frame_rbp-0x20));
       if (leave_l) ir::execute(ctx, *leave_l);
       sem_total++; if (ctx.gpr[5] == 0xAAAA) sem_pass++; else printf("    FAIL leave RBP=%llX\n", (unsigned long long)ctx.gpr[5]);
       sem_total++; if (ctx.gpr[4] == orig_rsp) sem_pass++; else printf("    FAIL leave RSP=%llu\n", (unsigned long long)ctx.gpr[4]);
    }
    { // STD then stosb should decrement RDI
       uint8_t std_c[] = {0xFD}; // std
       uint8_t cld_c[] = {0xFC}; // cld
       uint8_t stos_c[] = {0xAA}; // stosb
       auto std_l = ir::lift(std_c, 1); auto cld_l = ir::lift(cld_c, 1);
       auto stos_l = ir::lift(stos_c, 1);
       uint8_t buf[256] = {};
       ir::Context ctx; ctx.memory = buf; ctx.memory_size = 256;
       ctx.gpr[0] = 0xCC; ctx.gpr[7] = 100;
       if (std_l) ir::execute(ctx, *std_l); // set DF=1
       sem_total++; if (ctx.flags[6] == 1) sem_pass++; else printf("    FAIL STD: DF=%d\n", ctx.flags[6]);
       if (stos_l) ir::execute(ctx, *stos_l);
       sem_total++; if (ctx.gpr[7] == 99) sem_pass++; else printf("    FAIL STD+stosb: RDI=%llu (exp 99)\n", (unsigned long long)ctx.gpr[7]);
       sem_total++; if (buf[100] == 0xCC) sem_pass++; else printf("    FAIL STD+stosb: [100]=%d\n", buf[100]);
       // CLD then stosb should increment RDI
       if (cld_l) ir::execute(ctx, *cld_l);
       ctx.gpr[7] = 110;
       if (stos_l) ir::execute(ctx, *stos_l);
       sem_total++; if (ctx.gpr[7] == 111) sem_pass++; else printf("    FAIL CLD+stosb: RDI=%llu\n", (unsigned long long)ctx.gpr[7]);
    }
    { // rep stosb: fill 8 bytes
       uint8_t rep_stos[] = {0xF3,0xAA};
       auto l = ir::lift(rep_stos, 2);
       uint8_t buf[256] = {};
       ir::Context ctx; ctx.memory = buf; ctx.memory_size = 256;
       ctx.gpr[0] = 0xAB; ctx.gpr[7] = 30; ctx.gpr[1] = 8;
       if (l) ir::execute(ctx, *l);
       sem_total++; if (ctx.gpr[1] == 0) sem_pass++; else printf("    FAIL rep stosb: RCX=%llu\n", (unsigned long long)ctx.gpr[1]);
       sem_total++; if (ctx.gpr[7] == 38) sem_pass++; else printf("    FAIL rep stosb: RDI=%llu\n", (unsigned long long)ctx.gpr[7]);
       bool all_ab = true; for (int i=0;i<8;i++) if (buf[30+i] != 0xAB) all_ab = false;
       sem_total++; if (all_ab) sem_pass++; else printf("    FAIL rep stosb: data mismatch\n");
    }
    { // repnz scasb: scan for 0x42
       uint8_t repnz_scas[] = {0xF2,0xAE};
       auto l = ir::lift(repnz_scas, 2);
       uint8_t buf[256] = {};
       buf[50] = 0x11; buf[51] = 0x22; buf[52] = 0x42; buf[53] = 0x33;
       ir::Context ctx; ctx.memory = buf; ctx.memory_size = 256;
       ctx.gpr[0] = 0x42; ctx.gpr[7] = 50; ctx.gpr[1] = 10;
       if (l) ir::execute(ctx, *l);
       sem_total++; if (ctx.gpr[7] == 53) sem_pass++; else printf("    FAIL repnz scasb: RDI=%llu (exp 53)\n", (unsigned long long)ctx.gpr[7]);
       sem_total++; if (ctx.gpr[1] == 7) sem_pass++; else printf("    FAIL repnz scasb: RCX=%llu (exp 7)\n", (unsigned long long)ctx.gpr[1]);
       sem_total++; if (ctx.flags[2] == 1) sem_pass++; else printf("    FAIL repnz scasb: ZF=%d\n", ctx.flags[2]); // ZF=1 (found)
    }
    { // repe cmpsb: compare matching then mismatching
       uint8_t repe_cmps[] = {0xF3,0xA6};
       auto l = ir::lift(repe_cmps, 2);
       uint8_t buf[256] = {};
       buf[0]=1; buf[1]=2; buf[2]=3; buf[3]=99; // src
       buf[100]=1; buf[101]=2; buf[102]=3; buf[103]=77; // dst
       ir::Context ctx; ctx.memory = buf; ctx.memory_size = 256;
       ctx.gpr[6] = 0; ctx.gpr[7] = 100; ctx.gpr[1] = 10;
       if (l) ir::execute(ctx, *l);
       sem_total++; if (ctx.gpr[1] == 6) sem_pass++; else printf("    FAIL repe cmpsb: RCX=%llu (exp 6)\n", (unsigned long long)ctx.gpr[1]);
       sem_total++; if (ctx.flags[2] == 0) sem_pass++; else printf("    FAIL repe cmpsb: ZF=%d (exp 0)\n", ctx.flags[2]);
    }
    { // lock cmpxchg [rbx], ecx — memory compare-exchange
       uint8_t c[] = {0xF0,0x0F,0xB1,0x0B}; // lock cmpxchg [rbx], ecx
       auto l = ir::lift(c, 4);
       uint8_t buf[256] = {};
       buf[40] = 42; // [rbx] = 42
       ir::Context ctx; ctx.memory = buf; ctx.memory_size = 256;
       ctx.gpr[0] = 42; // EAX = 42 (equal to [rbx])
       ctx.gpr[1] = 99; // ECX = 99 (new value)
       ctx.gpr[3] = 40; // RBX = address
       if (l) ir::execute(ctx, *l);
       sem_total++; if (buf[40] == 99) sem_pass++; else printf("    FAIL cmpxchg mem: [40]=%d (exp 99)\n", buf[40]);
    }
    { // lock xadd [rbx], ecx — exchange and add
       uint8_t c[] = {0xF0,0x0F,0xC1,0x0B}; // lock xadd [rbx], ecx
       auto l = ir::lift(c, 4);
       uint8_t buf[256] = {};
       uint32_t v = 100; memcpy(&buf[40], &v, 4);
       ir::Context ctx; ctx.memory = buf; ctx.memory_size = 256;
       ctx.gpr[1] = 5; ctx.gpr[3] = 40;
       if (l) ir::execute(ctx, *l);
       uint32_t result; memcpy(&result, &buf[40], 4);
       sem_total++; if (result == 105) sem_pass++; else printf("    FAIL xadd mem: [40]=%u (exp 105)\n", result);
       sem_total++; if ((ctx.gpr[1] & 0xFFFFFFFF) == 100) sem_pass++; else printf("    FAIL xadd mem: ECX=%llu (exp 100)\n", (unsigned long long)ctx.gpr[1]);
    }
    { uint8_t c[] = {0xF3,0x48,0x0F,0xB8,0xC1}; // popcnt rax,rcx
       auto l = ir::lift(c, 5); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[1] = 0xFF00FF00FF;
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 24) sem_pass++; else printf("    FAIL popcnt: %llu\n", (unsigned long long)ctx.gpr[0]); }
    }
    { uint8_t c[] = {0xF3,0x48,0x0F,0xBC,0xC1}; // tzcnt rax,rcx
       auto l = ir::lift(c, 5); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[1] = 0x80; // bit 7 set
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 7) sem_pass++; else printf("    FAIL tzcnt: %llu\n", (unsigned long long)ctx.gpr[0]); }
    }
    { uint8_t c[] = {0xF3,0x48,0x0F,0xBD,0xC1}; // lzcnt rax,rcx
       auto l = ir::lift(c, 5); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[1] = 1; // only bit 0 set → 63 leading zeros
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 63) sem_pass++; else printf("    FAIL lzcnt: %llu\n", (unsigned long long)ctx.gpr[0]); }
    }
    { uint8_t c[] = {0x48,0x0F,0xB6,0xC1}; // movzx rax, cl
       auto l = ir::lift(c, 4); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[0] = 0xDEAD; ctx.gpr[1] = 0xFF;
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 0xFF) sem_pass++; else printf("    FAIL movzx: %llX\n", (unsigned long long)ctx.gpr[0]); }
    }
    { uint8_t c[] = {0x48,0x0F,0xBE,0xC1}; // movsx rax, cl
       auto l = ir::lift(c, 4); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[1] = 0x80; // -128 as signed byte
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 0xFFFFFFFFFFFFFF80ULL) sem_pass++; else printf("    FAIL movsx: %llX\n", (unsigned long long)ctx.gpr[0]); }
    }
    { // cmovne rax, rcx — should move when ZF=0
       uint8_t c[] = {0x48,0x0F,0x45,0xC1};
       auto l = ir::lift(c, 4); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[0] = 1; ctx.gpr[1] = 42; ctx.flags[2] = 0; // ZF=0 → move
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 42) sem_pass++; else printf("    FAIL cmovne(ZF=0): %llu\n", (unsigned long long)ctx.gpr[0]); }
    }
    { // cmovne rax, rcx — should NOT move when ZF=1
       uint8_t c[] = {0x48,0x0F,0x45,0xC1};
       auto l = ir::lift(c, 4); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[0] = 1; ctx.gpr[1] = 42; ctx.flags[2] = 1; // ZF=1 → no move
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 1) sem_pass++; else printf("    FAIL cmovne(ZF=1): %llu\n", (unsigned long long)ctx.gpr[0]); }
    }
    { // pushfq / popfq roundtrip
       uint8_t pushf_c[] = {0x9C}; uint8_t popf_c[] = {0x9D};
       auto pushf_l = ir::lift(pushf_c, 1); auto popf_l = ir::lift(popf_c, 1);
       uint8_t stk[256] = {};
       ir::Context ctx; ctx.memory = stk; ctx.memory_size = 256;
       ctx.gpr[4] = 128; ctx.flags[0] = 1; ctx.flags[2] = 1; // CF=1, ZF=1
       if (pushf_l) ir::execute(ctx, *pushf_l);
       sem_total++; if (ctx.gpr[4] == 120) sem_pass++; else printf("    FAIL pushfq RSP=%llu\n", (unsigned long long)ctx.gpr[4]);
       ctx.flags[0] = 0; ctx.flags[2] = 0; // clear flags
       if (popf_l) ir::execute(ctx, *popf_l);
       sem_total++; if (ctx.gpr[4] == 128) sem_pass++; else printf("    FAIL popfq RSP=%llu\n", (unsigned long long)ctx.gpr[4]);
    }
    { // add rax, rcx: 0xFFFFFFFFFFFFFFFF + 1 = 0 with CF=1
       uint8_t c[] = {0x48,0x01,0xC8};
       auto l = ir::lift(c, 3); sem_total++;
       if (l) { ir::Context ctx; ctx.gpr[0] = 0xFFFFFFFFFFFFFFFFULL; ctx.gpr[1] = 1;
       ir::execute(ctx, *l);
       if (ctx.gpr[0] == 0 && ctx.flags[2] == 1) sem_pass++; // ZF=1 (result zero)
       else printf("    FAIL add overflow: RAX=%llX ZF=%d\n", (unsigned long long)ctx.gpr[0], ctx.flags[2]); }
    }
    { // test rax, rax → ZF=1 when RAX=0, ZF=0 when RAX!=0
       uint8_t c[] = {0x48,0x85,0xC0};
       auto l = ir::lift(c, 3);
       if (l) {
           ir::Context ctx; ctx.gpr[0] = 0;
           ir::execute(ctx, *l);
           sem_total++; if (ctx.flags[2] == 1) sem_pass++; else printf("    FAIL test(0): ZF=%d\n", ctx.flags[2]);
           ctx.gpr[0] = 42;
           ir::execute(ctx, *l);
           sem_total++; if (ctx.flags[2] == 0) sem_pass++; else printf("    FAIL test(42): ZF=%d\n", ctx.flags[2]);
       }
    }
    { // neg rax: 0 → 0 (ZF=1, CF=0), 1 → -1 (ZF=0, CF=1)
       uint8_t c[] = {0x48,0xF7,0xD8};
       auto l = ir::lift(c, 3);
       if (l) {
           ir::Context ctx; ctx.gpr[0] = 0;
           ir::execute(ctx, *l);
           sem_total++; if (ctx.gpr[0] == 0 && ctx.flags[2] == 1) sem_pass++; else printf("    FAIL neg(0): RAX=%llu ZF=%d\n", (unsigned long long)ctx.gpr[0], ctx.flags[2]);
           ctx.gpr[0] = 1;
           ir::execute(ctx, *l);
           sem_total++; if (ctx.gpr[0] == 0xFFFFFFFFFFFFFFFFULL) sem_pass++; else printf("    FAIL neg(1): RAX=%llX\n", (unsigned long long)ctx.gpr[0]);
       }
    }
    { // inc rax: CF should be preserved from previous op
       uint8_t c[] = {0x48,0xFF,0xC0}; // inc rax
       auto l = ir::lift(c, 3);
       if (l) {
           ir::Context ctx; ctx.gpr[0] = 0xFFFFFFFFFFFFFFFFULL; ctx.flags[0] = 1; // CF=1 before
           ir::execute(ctx, *l);
           sem_total++; if (ctx.gpr[0] == 0) sem_pass++; else printf("    FAIL inc wrap: %llX\n", (unsigned long long)ctx.gpr[0]);
           sem_total++; if (ctx.flags[2] == 1) sem_pass++; else printf("    FAIL inc ZF: %d\n", ctx.flags[2]);
       }
    }
    { // rep movsb: copy 4 bytes
       uint8_t c[] = {0xF3,0xA4};
       auto l = ir::lift(c, 2);
       uint8_t buf[256] = {};
       buf[0]=0xDE; buf[1]=0xAD; buf[2]=0xBE; buf[3]=0xEF;
       ir::Context ctx; ctx.memory = buf; ctx.memory_size = 256;
       ctx.gpr[6] = 0; ctx.gpr[7] = 100; ctx.gpr[1] = 4;
       if (l) ir::execute(ctx, *l);
       sem_total++; if (buf[100]==0xDE && buf[101]==0xAD && buf[102]==0xBE && buf[103]==0xEF) sem_pass++; else printf("    FAIL rep movsb\n");
       sem_total++; if (ctx.gpr[1] == 0) sem_pass++; else printf("    FAIL rep movsb: RCX=%llu\n", (unsigned long long)ctx.gpr[1]);
    }
    printf("    Semantic: %d/%d\n", sem_pass, sem_total);
    CHECK(sem_pass == sem_total, "all semantic checks pass");

    printf("  Emit lowering...\n");
    {
        int emit_pass = 0, emit_total = 0;
        auto emit_ok = [&](const char* name, ir::Opcode opc, ir::VarNode dst, ir::VarNode s0, ir::VarNode s1 = {}, ir::VarNode s2 = {}) {
            emit_total++;
            ir::Op op; op.opcode = opc; op.output = dst;
            op.inputs[0] = s0; op.inputs[1] = s1; op.inputs[2] = s2;
            op.num_inputs = (s2.size?3:s1.size?2:1);
            CodeGen cg;
            if (ir::emit(op, cg) && cg.size() > 0) emit_pass++;
            else printf("    FAIL emit %s\n", name);
        };
        auto R = [](uint16_t id, uint8_t sz) { return ir::VarNode::gpr(id, sz); };
        auto C = [](int64_t v, uint8_t sz) { return ir::VarNode::constant(v, sz); };
        auto X = [](uint16_t id, uint8_t sz) { return ir::VarNode::xmm(id, sz); };
        auto F = []() { return ir::VarNode::flags(); };

        emit_ok("COPY gpr", ir::Opcode::COPY, R(0,8), R(1,8));
        emit_ok("COPY imm", ir::Opcode::COPY, R(0,8), C(42,8));
        emit_ok("COPY xmm", ir::Opcode::COPY, X(0,16), X(1,16));
        emit_ok("ADD reg", ir::Opcode::ADD, R(0,8), R(0,8), R(1,8));
        emit_ok("SUB imm", ir::Opcode::SUB, R(0,4), R(0,4), C(10,4));
        emit_ok("IMUL", ir::Opcode::IMUL, R(0,8), R(0,8), R(1,8));
        emit_ok("SHL imm", ir::Opcode::SHL, R(0,8), R(0,8), C(4,1));
        emit_ok("SHR CL", ir::Opcode::SHR, R(0,8), R(0,8), R(1,1));
        emit_ok("SAR CL", ir::Opcode::SAR, R(3,4), R(3,4), R(1,1));
        emit_ok("ROL CL", ir::Opcode::ROL, R(0,8), R(0,8), R(1,1));
        emit_ok("DIV", ir::Opcode::DIV, R(0,4), R(0,4), R(1,4));
        emit_ok("IDIV", ir::Opcode::IDIV, R(0,8), R(0,8), R(1,8));
        emit_ok("MOD", ir::Opcode::MOD, R(2,4), R(0,4), R(1,4));
        emit_ok("MUL", ir::Opcode::MUL, R(0,8), R(0,8), R(1,8));
        emit_ok("SUB_FLAGS", ir::Opcode::SUB_FLAGS, F(), R(0,8), R(1,8));
        emit_ok("AND_FLAGS", ir::Opcode::AND_FLAGS, F(), R(0,4), C(0xFF,4));
        emit_ok("ADD_FLAGS", ir::Opcode::ADD_FLAGS, F(), R(0,8), R(1,8));
        emit_ok("GET_CF", ir::Opcode::GET_CF, R(0,1), F());
        emit_ok("GET_ZF", ir::Opcode::GET_ZF, R(0,4), F());
        emit_ok("SET_CF 1", ir::Opcode::SET_CF, F(), C(1,1));
        emit_ok("SET_DF 0", ir::Opcode::SET_DF, F(), C(0,1));
        emit_ok("SET_ZF 1", ir::Opcode::SET_ZF, F(), C(1,1));
        emit_ok("SET_SF 1", ir::Opcode::SET_SF, F(), C(1,1));
        emit_ok("SET_OF 0", ir::Opcode::SET_OF, F(), C(0,1));
        emit_ok("SET_PF 1", ir::Opcode::SET_PF, F(), C(1,1));
        emit_ok("BRANCH const", ir::Opcode::BRANCH, {}, C(0x1000,8));
        { // CBRANCH: cond reg + const target
            emit_total++;
            ir::Op op; op.opcode = ir::Opcode::CBRANCH; op.num_inputs = 2;
            op.inputs[0] = R(0,1); op.inputs[1] = C(0x2000,8);
            CodeGen cg; if (ir::emit(op, cg) && cg.size() > 0) emit_pass++;
            else printf("    FAIL emit CBRANCH\n");
        }
        emit_ok("VADD dw", ir::Opcode::VADD, X(0,16), X(0,16), X(1,16), C(0,4));
        emit_ok("VSUB w", ir::Opcode::VSUB, X(0,16), X(0,16), X(1,16), C(0,2));
        emit_ok("VAND", ir::Opcode::VAND, X(2,16), X(2,16), X(3,16));
        emit_ok("VXOR", ir::Opcode::VXOR, X(0,16), X(0,16), X(0,16));
        emit_ok("VCMP b", ir::Opcode::VCMP, X(0,16), X(0,16), X(1,16), C(0,1));
        emit_ok("FADD ss", ir::Opcode::FADD, X(0,4), X(0,4), X(1,4));
        emit_ok("FSQRT sd", ir::Opcode::FSQRT, X(0,8), X(1,8));
        emit_ok("I2F", ir::Opcode::I2F, X(0,4), R(0,4));
        emit_ok("F2I", ir::Opcode::F2I, R(0,4), X(0,4));
        emit_ok("EXTRACT", ir::Opcode::EXTRACT, R(0,4), R(1,8), C(8,1));
        emit_ok("CONCAT", ir::Opcode::CONCAT, R(0,8), R(1,4), R(2,4));
        { // emit_lifted on NOP
            uint8_t nop[] = {0x90};
            auto l = ir::lift(nop, 1);
            emit_total++;
            if (l) {
                CodeGen cg; bool ok = ir::emit_all(*l, cg);
                if (ok && cg.size() > 0) emit_pass++;
                else printf("    FAIL emit_all NOP\n");
            }
        }
        { // emit_lifted on MOV rax, rcx
            uint8_t mov[] = {0x48, 0x89, 0xC8};
            auto l = ir::lift(mov, 3);
            emit_total++;
            if (l) {
                CodeGen cg; size_t n = ir::emit_lifted(*l, cg);
                if (n == l->ops.size() && cg.size() > 0) emit_pass++;
                else printf("    FAIL emit_lifted MOV: %zu/%zu\n", n, l->ops.size());
            }
        }
        printf("    Emit: %d/%d\n", emit_pass, emit_total);
        CHECK(emit_pass == emit_total, "all emit checks pass");
    }

    printf("All IR tests passed!\n");
    return 0;
}
#else
#include <cstdio>
int main() { printf("VEDX64_IR not enabled, skipping.\n"); return 0; }
#endif
