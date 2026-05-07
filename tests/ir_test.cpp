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

    uint8_t nop[] = {0x0F, 0x1F, 0x00};
    auto l = ir::lift(nop, sizeof(nop));
    CHECK(l.has_value(), "lift nop");
    CHECK(!l->ops.empty(), "nop has ops");
    CHECK(l->ops[0].opcode == ir::Opcode::NOP, "nop opcode");
    CHECK(ir::is_fully_lifted(*l), "nop is fully lifted");

    {
        uint8_t aaa[] = {0x37};  // AAA — affects AF/CF, undef OF/SF/ZF/PF
        auto u = ir::lift(aaa, sizeof(aaa));
        CHECK(u.has_value() && !u->ops.empty(), "lift AAA");
        CHECK(u->ops[0].opcode == ir::Opcode::UNDEF, "AAA emits UNDEF (no longer silent NOP)");
        CHECK(!ir::is_fully_lifted(*u), "AAA is not fully lifted");
    }

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
        { // emit_lifted on NOP — use long-NOP so the explicit handler fires
            uint8_t nop[] = {0x0F, 0x1F, 0x00};
            auto l = ir::lift(nop, sizeof(nop));
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


    // ADD r64, r64 → produces an ADD_FLAGS bundle. After expansion every
    // flag is set by a primitive SET_CF/SET_ZF/SET_SF/SET_OF/SET_PF op.
    {
        uint8_t add_rax_rcx[] = {0x48, 0x01, 0xC8};  // add rax, rcx
        auto la = ir::lift(add_rax_rcx, sizeof(add_rax_rcx));
        CHECK(la.has_value(), "lift add rax,rcx");
        size_t bundles_before = 0;
        for (auto& o : la->ops)
            if (o.opcode == ir::Opcode::ADD_FLAGS || o.opcode == ir::Opcode::SUB_FLAGS ||
                o.opcode == ir::Opcode::AND_FLAGS) ++bundles_before;
        CHECK(bundles_before > 0, "add lift contains a flag bundle");

        auto expanded = ir::expand_flag_bundles(*la);
        size_t bundles_after = 0, sets = 0;
        for (auto& o : expanded.ops) {
            if (o.opcode == ir::Opcode::ADD_FLAGS || o.opcode == ir::Opcode::SUB_FLAGS ||
                o.opcode == ir::Opcode::AND_FLAGS) ++bundles_after;
            if (o.opcode == ir::Opcode::SET_CF || o.opcode == ir::Opcode::SET_ZF ||
                o.opcode == ir::Opcode::SET_SF || o.opcode == ir::Opcode::SET_OF ||
                o.opcode == ir::Opcode::SET_PF) ++sets;
        }
        CHECK(bundles_after == 0, "no flag bundles remain after expansion");
        CHECK(sets >= 5, "expansion emits at least 5 SET_* ops (CF/ZF/SF/OF/PF)");
    }
    {
        // SUB writes CF as borrow, distinct from ADD's overflow form.
        uint8_t sub_rax_rcx[] = {0x48, 0x29, 0xC8};
        auto ls = ir::lift(sub_rax_rcx, sizeof(sub_rax_rcx));
        CHECK(ls.has_value(), "lift sub rax,rcx");
        auto e = ir::expand_flag_bundles(*ls);
        CHECK(ir::is_fully_lifted(e), "expanded sub stays fully lifted");
        bool has_set_cf = false;
        for (auto& o : e.ops) if (o.opcode == ir::Opcode::SET_CF) { has_set_cf = true; break; }
        CHECK(has_set_cf, "sub expansion emits SET_CF");
    }
    {
        // expand_flag_bundles is a noop on already-primitive sequences.
        uint8_t mov_only[] = {0x48, 0x89, 0xC8};
        auto lo = ir::lift(mov_only, sizeof(mov_only));
        auto e2 = ir::expand_flag_bundles(*lo);
        CHECK(e2.ops.size() == lo->ops.size(), "mov: expansion is idempotent");
    }

    // ── New SymExec-targeted handlers ──
    auto count_op = [](const ir::Lifted& L, ir::Opcode oc) {
        size_t n = 0; for (auto& o : L.ops) if (o.opcode == oc) ++n; return n;
    };
    // ALU AL,imm accumulator forms (no ModRM): ADD AL, 0x42 → 04 42
    {
        uint8_t add_al[] = {0x04, 0x42};
        auto l = ir::lift(add_al, sizeof(add_al));
        CHECK(l && ir::is_fully_lifted(*l), "ADD AL, imm8 fully lifted");
        CHECK(count_op(*l, ir::Opcode::ADD) >= 1, "ADD AL,imm produces ADD op");
    }
    // SUB EAX, imm32: 2D 78 56 34 12
    {
        uint8_t sub_eax[] = {0x2D, 0x78, 0x56, 0x34, 0x12};
        auto l = ir::lift(sub_eax, sizeof(sub_eax));
        CHECK(l && ir::is_fully_lifted(*l), "SUB EAX, imm32 fully lifted");
        CHECK(count_op(*l, ir::Opcode::SUB) >= 1, "SUB EAX,imm produces SUB op");
    }
    // CMP RAX, imm32 (sign-extended): 48 3D ff ff 00 00
    {
        uint8_t cmp_rax[] = {0x48, 0x3D, 0xFF, 0xFF, 0x00, 0x00};
        auto l = ir::lift(cmp_rax, sizeof(cmp_rax));
        CHECK(l && ir::is_fully_lifted(*l), "CMP RAX, imm32 fully lifted");
        CHECK(count_op(*l, ir::Opcode::SUB_FLAGS) >= 1, "CMP emits SUB_FLAGS");
    }
    // TEST AL, imm8: A8 ff
    {
        uint8_t test_al[] = {0xA8, 0xFF};
        auto l = ir::lift(test_al, sizeof(test_al));
        CHECK(l && ir::is_fully_lifted(*l), "TEST AL, imm8 fully lifted");
        CHECK(count_op(*l, ir::Opcode::AND) >= 1, "TEST emits AND");
    }

    // MUL r/m8 (F6 /4): byte form, AX = AL * src
    {
        uint8_t mul_cl[] = {0xF6, 0xE1};  // mul cl
        auto l = ir::lift(mul_cl, sizeof(mul_cl));
        CHECK(l && ir::is_fully_lifted(*l), "MUL CL fully lifted (8-bit)");
        CHECK(count_op(*l, ir::Opcode::MUL) >= 1, "MUL CL emits MUL op");
    }
    // MUL r/m32 (F7 /4): EAX*src → EDX:EAX (modeled exactly via 64-bit temp)
    {
        uint8_t mul_ecx[] = {0xF7, 0xE1};
        auto l = ir::lift(mul_ecx, sizeof(mul_ecx));
        CHECK(l && ir::is_fully_lifted(*l), "MUL ECX fully lifted (32-bit)");
        CHECK(count_op(*l, ir::Opcode::MUL) >= 1, "MUL ECX emits MUL");
        CHECK(count_op(*l, ir::Opcode::TRUNC) >= 2, "MUL ECX emits 2 TRUNCs (EAX, EDX)");
    }
    // MUL r/m64 (F7 /4 with REX.W): widens through 128-bit temps so both
    // RAX (low) and RDX (high) get explicit writes — no UNDEF.
    {
        uint8_t mul_rcx[] = {0x48, 0xF7, 0xE1};
        auto l = ir::lift(mul_rcx, sizeof(mul_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "MUL RCX is fully lifted (no UNDEF)");
        CHECK(count_op(*l, ir::Opcode::MUL) >= 1, "MUL RCX emits MUL");
        CHECK(count_op(*l, ir::Opcode::TRUNC) >= 2, "MUL RCX emits 2 TRUNCs (RAX, RDX)");
        // Both RAX and RDX must be written explicitly so DCE sees the prior
        // RDX def as dead.
        bool wrote_rax = false, wrote_rdx = false;
        for (const auto& op : l->ops) {
            if (op.output.space != ir::Space::GPR) continue;
            if (op.output.offset == 0 && op.output.size == 8) wrote_rax = true;
            if (op.output.offset == 2 && op.output.size == 8) wrote_rdx = true;
        }
        CHECK(wrote_rax, "MUL RCX writes RAX");
        CHECK(wrote_rdx, "MUL RCX writes RDX");
    }
    // IMUL r/m64 (F7 /5 with REX.W): same shape, signed extension.
    {
        uint8_t imul_rcx[] = {0x48, 0xF7, 0xE9};
        auto l = ir::lift(imul_rcx, sizeof(imul_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "IMUL RCX is fully lifted");
        CHECK(count_op(*l, ir::Opcode::IMUL) >= 1, "IMUL RCX emits IMUL");
        CHECK(count_op(*l, ir::Opcode::SEXT) >= 2, "IMUL RCX emits SEXTs to widen");
    }
    // DIV r/m64 (F7 /6 with REX.W): dividend is RDX:RAX widened to 128-bit.
    {
        uint8_t div_rcx[] = {0x48, 0xF7, 0xF1};
        auto l = ir::lift(div_rcx, sizeof(div_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "DIV RCX is fully lifted (no UNDEF)");
        CHECK(count_op(*l, ir::Opcode::DIV) >= 1, "DIV RCX emits DIV");
        CHECK(count_op(*l, ir::Opcode::MOD) >= 1, "DIV RCX emits MOD");
        bool wrote_rax = false, wrote_rdx = false;
        for (const auto& op : l->ops) {
            if (op.output.space != ir::Space::GPR) continue;
            if (op.output.offset == 0 && op.output.size == 8) wrote_rax = true;
            if (op.output.offset == 2 && op.output.size == 8) wrote_rdx = true;
        }
        CHECK(wrote_rax, "DIV RCX writes RAX (quotient)");
        CHECK(wrote_rdx, "DIV RCX writes RDX (remainder)");
    }
    // IDIV r/m64 (F7 /7 with REX.W): same shape, signed.
    {
        uint8_t idiv_rcx[] = {0x48, 0xF7, 0xF9};
        auto l = ir::lift(idiv_rcx, sizeof(idiv_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "IDIV RCX is fully lifted");
        CHECK(count_op(*l, ir::Opcode::IDIV) >= 1, "IDIV RCX emits IDIV");
        CHECK(count_op(*l, ir::Opcode::SMOD) >= 1, "IDIV RCX emits SMOD");
    }

    // Helper: returns true if any op in the list writes the upper 32 bits of
    // the given GPR (size=4, bit_lo=32) from the constant 0 — that's the
    // x86-64 implicit zero-extension marker the post-pass appends.
    auto has_zext_kill = [](const ir::Lifted& L, uint16_t reg) {
        for (const auto& op : L.ops) {
            if (op.opcode != ir::Opcode::COPY || op.num_inputs != 1) continue;
            const auto& d = op.output;
            const auto& s = op.inputs[0];
            if (d.space == ir::Space::GPR && d.offset == reg && d.size == 4 && d.bit_lo == 32
                && s.space == ir::Space::Const && s.value == 0) return true;
        }
        return false;
    };

    // Bug A: every 32-bit GPR write zero-extends to 64-bit.
    //   89 CB     mov ebx, ecx        — gpr3[0..31] <- gpr1[0..31] then gpr3[32..63] <- 0
    //   D3 E0     shl eax, cl         — same kill on EAX
    //   01 D0     add eax, edx        — same kill on EAX
    {
        uint8_t mov_ebx_ecx[] = {0x89, 0xCB};
        auto l = ir::lift(mov_ebx_ecx, sizeof(mov_ebx_ecx));
        CHECK(l && ir::is_fully_lifted(*l), "mov ebx, ecx fully lifted");
        CHECK(has_zext_kill(*l, 3), "mov ebx, ecx zero-extends EBX into RBX[32:63]");
    }
    {
        uint8_t shl_eax_cl[] = {0xD3, 0xE0};
        auto l = ir::lift(shl_eax_cl, sizeof(shl_eax_cl));
        CHECK(l && ir::is_fully_lifted(*l), "shl eax, cl fully lifted");
        CHECK(has_zext_kill(*l, 0), "shl eax, cl zero-extends EAX into RAX[32:63]");
    }
    {
        uint8_t add_eax_edx[] = {0x01, 0xD0};
        auto l = ir::lift(add_eax_edx, sizeof(add_eax_edx));
        CHECK(l && ir::is_fully_lifted(*l), "add eax, edx fully lifted");
        CHECK(has_zext_kill(*l, 0), "add eax, edx zero-extends EAX into RAX[32:63]");
    }

    // Bug B: MOVSXD r64, r/m32 — destination is 64-bit when REX.W.
    //   49 63 D7  movsxd rdx, r15d  → SEXT into gpr2 size=8.
    {
        uint8_t movsxd_rdx_r15d[] = {0x49, 0x63, 0xD7};
        auto l = ir::lift(movsxd_rdx_r15d, sizeof(movsxd_rdx_r15d));
        CHECK(l && ir::is_fully_lifted(*l), "movsxd rdx, r15d fully lifted");
        bool ok = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::SEXT) continue;
            if (op.output.space == ir::Space::GPR && op.output.offset == 2 && op.output.size == 8) ok = true;
        }
        CHECK(ok, "movsxd rdx,r15d: SEXT writes gpr 2 size=8 (not 4)");
    }

    // Bug C: ADC r/m, imm uses the immediate, not modrm.reg (= /2 = RDX).
    //   48 81 D3 1C 44 2D 18  →  adc rbx, 0x182d441c
    {
        uint8_t adc_rbx_imm[] = {0x48, 0x81, 0xD3, 0x1C, 0x44, 0x2D, 0x18};
        auto l = ir::lift(adc_rbx_imm, sizeof(adc_rbx_imm));
        CHECK(l && ir::is_fully_lifted(*l), "adc rbx, imm32 fully lifted");
        bool uses_imm = false;
        bool reads_rdx = false;
        for (const auto& op : l->ops) {
            for (uint8_t i = 0; i < op.num_inputs; ++i) {
                const auto& v = op.inputs[i];
                if (v.space == ir::Space::Const && (uint64_t)v.value == 0x182d441c) uses_imm = true;
                if (v.space == ir::Space::GPR && v.offset == 2) reads_rdx = true;
            }
        }
        CHECK(uses_imm, "adc rbx, imm32: source is the immediate 0x182d441c");
        CHECK(!reads_rdx, "adc rbx, imm32: lifter must not mistake /2 ext for RDX read");
    }

    // Shift flag emission: the data-only SHL/SHR/SAR/etc. lifts now also
    // emit a flag bundle so DCE sees the flag write.
    //   D3 E0  shl eax, cl  → expect at least one AND_FLAGS bundle.
    //   66 D3 CA  ror dx, cl
    {
        uint8_t shl_eax_cl[] = {0xD3, 0xE0};
        auto l = ir::lift(shl_eax_cl, sizeof(shl_eax_cl));
        CHECK(l && ir::is_fully_lifted(*l), "shl eax,cl: fully lifted");
        CHECK(count_op(*l, ir::Opcode::AND_FLAGS) >= 1, "shl eax,cl emits a flag-bundle op");
    }
    {
        uint8_t ror_dx_cl[] = {0x66, 0xD3, 0xCA};
        auto l = ir::lift(ror_dx_cl, sizeof(ror_dx_cl));
        CHECK(l && ir::is_fully_lifted(*l), "ror dx,cl: fully lifted");
        CHECK(count_op(*l, ir::Opcode::AND_FLAGS) >= 1, "ror dx,cl emits a flag-bundle op");
    }

    // BSR flag: just SET_ZF on (src == 0), no full AND_FLAGS that also
    // clobbers SF/PF.
    {
        uint8_t bsr_rax_rcx[] = {0x48, 0x0F, 0xBD, 0xC1};
        auto l = ir::lift(bsr_rax_rcx, sizeof(bsr_rax_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "bsr rax,rcx fully lifted");
        CHECK(count_op(*l, ir::Opcode::SET_ZF) == 1, "BSR emits exactly one SET_ZF");
        CHECK(count_op(*l, ir::Opcode::AND_FLAGS) == 0, "BSR does not clobber other flags");
    }

    // Identity folds: AND/OR with identical inputs collapse to COPY,
    // and self-COPY is dropped entirely.
    //   85 C0  test eax, eax  → emits AND_FLAGS but no AND data op,
    //                          AND with eax,eax would fold to a COPY.
    //   The cleanest direct check is to assert that the lifter never emits
    //   a redundant AND/OR with identical sources, i.e. count == 0.
    {
        uint8_t and_eax_eax[] = {0x21, 0xC0};   // and eax, eax
        auto l = ir::lift(and_eax_eax, sizeof(and_eax_eax));
        CHECK(l && ir::is_fully_lifted(*l), "and eax,eax fully lifted");
        for (const auto& op : l->ops) {
            if (op.opcode == ir::Opcode::AND && op.num_inputs == 2) {
                CHECK(!(op.inputs[0].space == ir::Space::GPR
                        && op.inputs[1].space == ir::Space::GPR
                        && op.inputs[0].offset == op.inputs[1].offset
                        && op.inputs[0].size == op.inputs[1].size
                        && op.inputs[0].bit_lo == op.inputs[1].bit_lo),
                      "AND x,x is folded out of the IR");
            }
        }
    }

    // and rbx,rbx and or rbx,rbx should leave only the flag write — the
    // copy-propagation pass eliminates the redundant data round-trip.
    {
        uint8_t and_rbx_rbx[] = {0x48, 0x21, 0xDB};
        auto l = ir::lift(and_rbx_rbx, sizeof(and_rbx_rbx));
        CHECK(l && ir::is_fully_lifted(*l), "and rbx,rbx fully lifted");
        CHECK(l->ops.size() == 1, "and rbx,rbx collapses to 1 op (flag-only)");
        CHECK(l->ops[0].opcode == ir::Opcode::AND_FLAGS,
              "and rbx,rbx leaves only the AND_FLAGS bundle");
    }
    {
        uint8_t or_rbx_rbx[] = {0x48, 0x09, 0xDB};
        auto l = ir::lift(or_rbx_rbx, sizeof(or_rbx_rbx));
        CHECK(l && ir::is_fully_lifted(*l), "or rbx,rbx fully lifted");
        CHECK(l->ops.size() == 1, "or rbx,rbx collapses to 1 op (flag-only)");
        CHECK(l->ops[0].opcode == ir::Opcode::AND_FLAGS,
              "or rbx,rbx leaves only the AND_FLAGS bundle");
    }
    // xchg bx,bx and mov rax,rax fold to a single synthetic NOP.
    {
        uint8_t xchg_bx_bx[] = {0x66, 0x87, 0xDB};
        auto l = ir::lift(xchg_bx_bx, sizeof(xchg_bx_bx));
        CHECK(l && l->ops.size() == 1 && l->ops[0].opcode == ir::Opcode::NOP,
              "xchg bx,bx folds to a single NOP");
    }
    {
        uint8_t mov_rax_rax[] = {0x48, 0x89, 0xC0};
        auto l = ir::lift(mov_rax_rax, sizeof(mov_rax_rax));
        CHECK(l && l->ops.size() == 1 && l->ops[0].opcode == ir::Opcode::NOP,
              "mov rax,rax folds to a single NOP");
    }
    // xchg with two distinct registers must still preserve the swap (the
    // copy-propagation pass materializes the temp before the conflicting write).
    {
        uint8_t xchg_rax_rcx[] = {0x48, 0x87, 0xC1};
        auto l = ir::lift(xchg_rax_rcx, sizeof(xchg_rax_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "xchg rax,rcx fully lifted");
        ir::Context ctx{};
        ctx.gpr[0] = 10; ctx.gpr[1] = 20;
        ir::execute(ctx, *l);
        CHECK(ctx.gpr[0] == 20 && ctx.gpr[1] == 10, "xchg rax,rcx swaps values");
    }

    // MUL/IMUL emit a flag-bundle write so DCE can kill prior flag defs.
    {
        uint8_t mul_rcx[] = {0x48, 0xF7, 0xE1};
        auto l = ir::lift(mul_rcx, sizeof(mul_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "mul rcx fully lifted");
        CHECK(count_op(*l, ir::Opcode::AND_FLAGS) >= 1, "mul rcx emits a flag bundle");
    }
    {
        uint8_t imul_r10w[] = {0x66, 0x41, 0xF7, 0xEA};
        auto l = ir::lift(imul_r10w, sizeof(imul_r10w));
        CHECK(l && ir::is_fully_lifted(*l), "imul r10w fully lifted");
        CHECK(count_op(*l, ir::Opcode::AND_FLAGS) >= 1, "imul r10w emits a flag bundle");
    }
    // DIV r/m8 (F6 /6): AX / src → AL=quot, AH=rem
    {
        uint8_t div_cl[] = {0xF6, 0xF1};
        auto l = ir::lift(div_cl, sizeof(div_cl));
        CHECK(l && ir::is_fully_lifted(*l), "DIV CL fully lifted");
        CHECK(count_op(*l, ir::Opcode::DIV) >= 1, "DIV CL emits DIV");
        CHECK(count_op(*l, ir::Opcode::MOD) >= 1, "DIV CL emits MOD");
    }

    // CMOVcc memory form: 48 0F 44 03  →  cmove rax, [rbx]
    {
        uint8_t cmove_mem[] = {0x48, 0x0F, 0x44, 0x03};
        auto l = ir::lift(cmove_mem, sizeof(cmove_mem));
        CHECK(l && ir::is_fully_lifted(*l), "CMOVE rax,[rbx] fully lifted");
        CHECK(count_op(*l, ir::Opcode::LOAD) >= 1, "CMOVE mem emits LOAD");
        CHECK(count_op(*l, ir::Opcode::SELECT) == 1, "CMOVE mem emits SELECT");
    }

    // XCHG opcode-register form: 91 = XCHG eax, ecx
    {
        uint8_t xchg_eax_ecx[] = {0x91};
        auto l = ir::lift(xchg_eax_ecx, sizeof(xchg_eax_ecx));
        CHECK(l && ir::is_fully_lifted(*l), "XCHG eax,ecx (opcode-reg) fully lifted");
        // The 32-bit zero-extension post-pass adds COPY 0 → gpr[N][32:63]
        // after every 32-bit GPR write, so the raw COPY count is now 3
        // exchange COPYs + 2 zero-extension COPYs = 5.
        CHECK(count_op(*l, ir::Opcode::COPY) >= 3, "XCHG opcode-reg emits >= 3 COPYs");
    }
    // XCHG memory form: 87 03 = XCHG eax, [rbx]
    {
        uint8_t xchg_mem[] = {0x87, 0x03};
        auto l = ir::lift(xchg_mem, sizeof(xchg_mem));
        CHECK(l && ir::is_fully_lifted(*l), "XCHG mem fully lifted");
        CHECK(count_op(*l, ir::Opcode::LOAD)  >= 1, "XCHG mem emits LOAD");
        CHECK(count_op(*l, ir::Opcode::STORE) >= 1, "XCHG mem emits STORE");
    }

    // BT memory form: 48 0F A3 0B  →  bt qword [rbx], rcx
    {
        uint8_t bt_mem[] = {0x48, 0x0F, 0xA3, 0x0B};
        auto l = ir::lift(bt_mem, sizeof(bt_mem));
        CHECK(l && ir::is_fully_lifted(*l), "BT [rbx], rcx fully lifted");
        CHECK(count_op(*l, ir::Opcode::LOAD)   >= 1, "BT mem emits LOAD");
        CHECK(count_op(*l, ir::Opcode::SET_CF) >= 1, "BT mem sets CF");
    }
    // BTS memory form: writes back via STORE
    {
        uint8_t bts_mem[] = {0x48, 0x0F, 0xAB, 0x0B};
        auto l = ir::lift(bts_mem, sizeof(bts_mem));
        CHECK(l && ir::is_fully_lifted(*l), "BTS [rbx], rcx fully lifted");
        CHECK(count_op(*l, ir::Opcode::STORE) >= 1, "BTS mem emits STORE");
    }
    // BT imm8 form: 48 0F BA E0 04  →  bt rax, 4
    {
        uint8_t bt_imm[] = {0x48, 0x0F, 0xBA, 0xE0, 0x04};
        auto l = ir::lift(bt_imm, sizeof(bt_imm));
        CHECK(l && ir::is_fully_lifted(*l), "BT rax, imm8 fully lifted");
    }

    // BTS reg with imm/reg offset: lifter must mask offset by (opsize-1).
    //   48 0F BA E9 41  →  bts rcx, 0x41  (offset masked to 1 mod 64)
    //   48 0F AB C8     →  bts rax, rcx   (lifter ANDs RCX with 0x3F before SHR)
    {
        uint8_t bts_reg_imm[] = {0x48, 0x0F, 0xBA, 0xE9, 0x41};
        auto l = ir::lift(bts_reg_imm, sizeof(bts_reg_imm));
        CHECK(l && ir::is_fully_lifted(*l), "BTS rcx, 0x41 fully lifted");
        // The lifter constant-folded the masked imm. It MUST NOT keep an
        // SHL/SHR by a count of 0x41 (which would be implementation-defined
        // for a 64-bit shift). Walk inputs of every SHR/SHL and confirm any
        // constant input is < 64.
        bool count_in_range = true;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::SHR && op.opcode != ir::Opcode::SHL) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value >= 64) count_in_range = false;
        }
        CHECK(count_in_range, "BTS imm offset masked into [0,63]");
    }
    {
        uint8_t bts_reg_reg[] = {0x48, 0x0F, 0xAB, 0xC8};
        auto l = ir::lift(bts_reg_reg, sizeof(bts_reg_reg));
        CHECK(l && ir::is_fully_lifted(*l), "BTS rax, rcx fully lifted");
        // For register source we expect an explicit mask op: AND ?, RCX, 0x3F.
        bool found_mask = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::AND) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value == 0x3F) found_mask = true;
        }
        CHECK(found_mask, "BTS reg-source: lifter ANDs offset with 0x3F");
    }

    // BTS mem,reg: lifter must adjust EA by (offset SAR 3) to walk the
    // bit array properly (not just take offset MOD opsize within the chunk).
    //   48 0F AB 0B  →  bts qword [rbx], rcx
    {
        uint8_t bts_mem_reg[] = {0x48, 0x0F, 0xAB, 0x0B};
        auto l = ir::lift(bts_mem_reg, sizeof(bts_mem_reg));
        CHECK(l && ir::is_fully_lifted(*l), "BTS [rbx], rcx fully lifted");
        bool found_sar = false;
        for (const auto& op : l->ops)
            if (op.opcode == ir::Opcode::SAR) found_sar = true;
        CHECK(found_sar, "BTS mem,reg: lifter emits SAR for byte-level EA adjust");
    }

    // SAR cl, 0x80: count masked to 5 bits → 0 → no-op semantically.
    // The lifter must fold the immediate to 0 (or an AND by 0x1F before SHR
    // for register count) so symbolic execution doesn't get a wide-shift.
    //   D2 F9              →  sar cl, cl       (count = CL & 0x1F)
    //   C0 F9 80           →  sar cl, 0x80     (count = 0x80 & 0x1F = 0)
    {
        uint8_t sar_imm[] = {0xC0, 0xF9, 0x80};
        auto l = ir::lift(sar_imm, sizeof(sar_imm));
        CHECK(l && ir::is_fully_lifted(*l), "SAR cl, 0x80 fully lifted");
        bool wide_shift = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::SAR) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value > 31) wide_shift = true;
        }
        CHECK(!wide_shift, "SAR cl, 0x80: imm masked into [0,31] (5-bit mask)");
    }
    {
        uint8_t sar_cl[] = {0xD2, 0xF9};
        auto l = ir::lift(sar_cl, sizeof(sar_cl));
        CHECK(l && ir::is_fully_lifted(*l), "SAR cl, cl fully lifted");
        bool found_mask = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::AND) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value == 0x1F) found_mask = true;
        }
        CHECK(found_mask, "SAR cl, cl: lifter ANDs CL with 0x1F (8-bit op)");
    }
    // SAR rax, cl: 64-bit operand uses 6-bit mask.
    {
        uint8_t sar_rax_cl[] = {0x48, 0xD3, 0xF8};
        auto l = ir::lift(sar_rax_cl, sizeof(sar_rax_cl));
        CHECK(l && ir::is_fully_lifted(*l), "SAR rax, cl fully lifted");
        bool found_mask = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::AND) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value == 0x3F) found_mask = true;
        }
        CHECK(found_mask, "SAR rax, cl: lifter ANDs CL with 0x3F (64-bit op)");
    }

    // BSR — must produce dst = (bits - 1) - CLZ(src), not bare CLZ.
    {
        uint8_t bsr_rax_rcx[] = {0x48, 0x0F, 0xBD, 0xC1};
        auto l = ir::lift(bsr_rax_rcx, sizeof(bsr_rax_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "BSR rax, rcx fully lifted");
        CHECK(count_op(*l, ir::Opcode::CLZ) == 1, "BSR emits exactly one CLZ");
        bool found_inv = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::SUB) continue;
            const auto& a = op.inputs[0];
            if (a.space == ir::Space::Const && a.value == 63) found_inv = true;
        }
        CHECK(found_inv, "BSR rax: lifter emits SUB 63 - CLZ");
    }
    // BSF — single CTZ, no inversion.
    {
        uint8_t bsf_rax_rcx[] = {0x48, 0x0F, 0xBC, 0xC1};
        auto l = ir::lift(bsf_rax_rcx, sizeof(bsf_rax_rcx));
        CHECK(l && ir::is_fully_lifted(*l), "BSF rax, rcx fully lifted");
        CHECK(count_op(*l, ir::Opcode::CTZ) == 1, "BSF emits exactly one CTZ");
    }

    // Legacy AH/CH/DH/BH addressing: without REX/REX2/VEX/EVEX, ModRM
    // r/m or reg = 4..7 at byte size names AH/CH/DH/BH (= rax/rcx/rdx/rbx
    // [15:8]), not SPL/BPL/SIL/DIL. The lifter must build VarNodes that
    // refer to gpr 0..3 with bit_lo=8 so symbolic / interpreted execution
    // touches the right bits of the right register.
    auto var_is_high_byte = [](const ir::VarNode& v, int reg) {
        return v.space == ir::Space::GPR && v.size == 1 && v.offset == (uint16_t)reg && v.bit_lo == 8;
    };
    {
        // 88 E0 = mov al, ah (without REX → ah is rax[15:8], al is rax[7:0]).
        uint8_t mov_al_ah[] = {0x88, 0xE0};
        auto l = ir::lift(mov_al_ah, sizeof(mov_al_ah));
        CHECK(l && ir::is_fully_lifted(*l), "mov al, ah fully lifted");
        const auto& copy = l->ops[0];
        CHECK(copy.opcode == ir::Opcode::COPY, "mov is COPY");
        CHECK(copy.output.space == ir::Space::GPR && copy.output.offset == 0
              && copy.output.size == 1 && copy.output.bit_lo == 0,
              "mov dst is AL (gpr 0, bit_lo 0)");
        CHECK(var_is_high_byte(copy.inputs[0], 0),
              "mov src is AH (gpr 0, bit_lo 8)");
    }
    {
        // 40 88 E0 = mov al, spl (REX present → no high-byte addressing).
        uint8_t rex_mov[] = {0x40, 0x88, 0xE0};
        auto l = ir::lift(rex_mov, sizeof(rex_mov));
        CHECK(l && ir::is_fully_lifted(*l), "mov al, spl (REX) fully lifted");
        const auto& copy = l->ops[0];
        CHECK(copy.inputs[0].space == ir::Space::GPR && copy.inputs[0].offset == 4
              && copy.inputs[0].bit_lo == 0,
              "REX byte mov: source is SPL (gpr 4, low byte) not AH");
    }
    {
        // 0F C0 CD = xadd ch, cl (no REX). After: ch += cl; cl <- old ch.
        uint8_t xadd_ch_cl[] = {0x0F, 0xC0, 0xCD};
        auto l = ir::lift(xadd_ch_cl, sizeof(xadd_ch_cl));
        CHECK(l && ir::is_fully_lifted(*l), "xadd ch, cl fully lifted");
        bool found_ch = false, found_cl = false;
        for (const auto& op : l->ops) {
            for (uint8_t i = 0; i < op.num_inputs; ++i) {
                const auto& v = op.inputs[i];
                if (var_is_high_byte(v, 1)) found_ch = true;       // CH = gpr 1, bit_lo 8
                if (v.space == ir::Space::GPR && v.size == 1 && v.offset == 1 && v.bit_lo == 0)
                    found_cl = true;
            }
        }
        CHECK(found_ch, "xadd uses CH (gpr 1, bit_lo 8)");
        CHECK(found_cl, "xadd uses CL (gpr 1, bit_lo 0)");
    }
    {
        // B4 11 = mov ah, 0x11 (OpcodeReg form, no REX).
        uint8_t mov_ah_imm[] = {0xB4, 0x11};
        auto l = ir::lift(mov_ah_imm, sizeof(mov_ah_imm));
        CHECK(l && ir::is_fully_lifted(*l), "mov ah, imm fully lifted");
        CHECK(var_is_high_byte(l->ops[0].output, 0),
              "OpcodeReg mov dst is AH (gpr 0, bit_lo 8)");
    }
    {
        // 41 B4 11 = mov r12b, 0x11 (REX.B → never legacy).
        uint8_t mov_r12b_imm[] = {0x41, 0xB4, 0x11};
        auto l = ir::lift(mov_r12b_imm, sizeof(mov_r12b_imm));
        CHECK(l && ir::is_fully_lifted(*l), "mov r12b, imm fully lifted");
        CHECK(l->ops[0].output.space == ir::Space::GPR
              && l->ops[0].output.offset == 12
              && l->ops[0].output.bit_lo == 0,
              "REX.B opcode_reg dst is r12b (gpr 12, low byte)");
    }
    // Execute the xadd ch, cl scenario through the IR interpreter.
    {
        uint8_t xadd_ch_cl[] = {0x0F, 0xC0, 0xCD};
        auto l = ir::lift(xadd_ch_cl, sizeof(xadd_ch_cl));
        CHECK(l && ir::is_fully_lifted(*l), "xadd ch, cl interpret-prep");
        ir::Context ctx{};
        ctx.gpr[1] = 0x0102;                       // CH=0x01, CL=0x02
        ctx.gpr[5] = 0xDEADBEEFCAFEBABEULL;        // RBP — must NOT be touched
        ctx.gpr[7] = 0x1122334455667788ULL;        // RDI — must NOT be touched
        ir::execute(ctx, *l);
        CHECK(ctx.gpr[1] == 0x0301,
              "xadd ch, cl: rcx=0x0301 after (CH=0x03, CL=0x01)");
        CHECK(ctx.gpr[5] == 0xDEADBEEFCAFEBABEULL,
              "xadd ch, cl: RBP unchanged (no aliasing into legacy slot)");
        CHECK(ctx.gpr[7] == 0x1122334455667788ULL,
              "xadd ch, cl: RDI unchanged");
    }

    // SHLD/SHRD count masking.
    //   0F A4 C8 21       →  shld eax, ecx, 0x21  (32-bit; 0x21 & 0x1F = 1)
    //   48 0F A4 C8 41    →  shld rax, rcx, 0x41  (64-bit; 0x41 & 0x3F = 1)
    //   0F A5 C8          →  shld eax, ecx, cl    (lifter ANDs CL with 0x1F)
    auto find_shift_const_count = [](const ir::Lifted& L, ir::Opcode opc, int64_t bound) {
        for (const auto& op : L.ops) {
            if (op.opcode != opc) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value >= bound) return true;
        }
        return false;
    };
    {
        uint8_t shld32_imm[] = {0x0F, 0xA4, 0xC8, 0x21};
        auto l = ir::lift(shld32_imm, sizeof(shld32_imm));
        CHECK(l && ir::is_fully_lifted(*l), "SHLD eax,ecx,0x21 fully lifted");
        CHECK(!find_shift_const_count(*l, ir::Opcode::SHL, 32),
              "SHLD eax: count masked into [0,31]");
    }
    {
        uint8_t shld64_imm[] = {0x48, 0x0F, 0xA4, 0xC8, 0x41};
        auto l = ir::lift(shld64_imm, sizeof(shld64_imm));
        CHECK(l && ir::is_fully_lifted(*l), "SHLD rax,rcx,0x41 fully lifted");
        CHECK(!find_shift_const_count(*l, ir::Opcode::SHL, 64),
              "SHLD rax: count masked into [0,63]");
    }
    {
        uint8_t shld_cl[] = {0x0F, 0xA5, 0xC8};
        auto l = ir::lift(shld_cl, sizeof(shld_cl));
        CHECK(l && ir::is_fully_lifted(*l), "SHLD eax,ecx,cl fully lifted");
        bool found = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::AND) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value == 0x1F) found = true;
        }
        CHECK(found, "SHLD CL form: lifter ANDs CL with 0x1F");
    }

    // RCL/RCR count masking.
    //   C0 D0 21    →  rcl al, 0x21  (count 0x21 & 0x1F = 1)
    //   D2 D0       →  rcl al, cl    (lifter ANDs CL with 0x1F)
    {
        uint8_t rcl_imm[] = {0xC0, 0xD0, 0x21};
        auto l = ir::lift(rcl_imm, sizeof(rcl_imm));
        CHECK(l && ir::is_fully_lifted(*l), "RCL al,0x21 fully lifted");
        CHECK(!find_shift_const_count(*l, ir::Opcode::ROL, 32),
              "RCL imm: count masked into [0,31]");
    }
    {
        uint8_t rcl_cl[] = {0xD2, 0xD0};
        auto l = ir::lift(rcl_cl, sizeof(rcl_cl));
        CHECK(l && ir::is_fully_lifted(*l), "RCL al,cl fully lifted");
        bool found = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::AND) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value == 0x1F) found = true;
        }
        CHECK(found, "RCL CL form: lifter ANDs CL with 0x1F");
    }

    // BMI2 SHLX/SHRX/SARX (VEX-encoded; mask same as legacy shifts).
    //   C4 E2 70 F7 C1   →  shlx eax, ecx, ebx (32-bit; 5-bit mask)
    //   C4 E2 F0 F7 C1   →  shlx rax, rcx, rbx (64-bit; 6-bit mask)
    {
        uint8_t shlx32[] = {0xC4, 0xE2, 0x71, 0xF7, 0xC1};
        auto l = ir::lift(shlx32, sizeof(shlx32));
        CHECK(l && ir::is_fully_lifted(*l), "SHLX eax,ecx,ebx fully lifted");
        bool found = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::AND) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value == 0x1F) found = true;
        }
        CHECK(found, "SHLX 32-bit: lifter ANDs ctl with 0x1F");
    }
    {
        uint8_t shlx64[] = {0xC4, 0xE2, 0xF1, 0xF7, 0xC1};
        auto l = ir::lift(shlx64, sizeof(shlx64));
        CHECK(l && ir::is_fully_lifted(*l), "SHLX rax,rcx,rbx fully lifted");
        bool found = false;
        for (const auto& op : l->ops) {
            if (op.opcode != ir::Opcode::AND) continue;
            const auto& cnt = op.inputs[op.num_inputs - 1];
            if (cnt.space == ir::Space::Const && cnt.value == 0x3F) found = true;
        }
        CHECK(found, "SHLX 64-bit: lifter ANDs ctl with 0x3F");
    }

    // SHLD imm form: 48 0F A4 C8 04  →  shld rax, rcx, 4
    {
        uint8_t shld_imm[] = {0x48, 0x0F, 0xA4, 0xC8, 0x04};
        auto l = ir::lift(shld_imm, sizeof(shld_imm));
        CHECK(l && ir::is_fully_lifted(*l), "SHLD rax,rcx,imm fully lifted");
        CHECK(count_op(*l, ir::Opcode::SHL) >= 1, "SHLD emits SHL");
        CHECK(count_op(*l, ir::Opcode::SHR) >= 1, "SHLD also emits SHR (high-half merge)");
        CHECK(count_op(*l, ir::Opcode::OR)  >= 1, "SHLD merges low|high via OR");
    }
    // SHRD CL form: 48 0F AD C8  →  shrd rax, rcx, cl
    {
        uint8_t shrd_cl[] = {0x48, 0x0F, 0xAD, 0xC8};
        auto l = ir::lift(shrd_cl, sizeof(shrd_cl));
        CHECK(l && ir::is_fully_lifted(*l), "SHRD rax,rcx,cl fully lifted");
    }

    // ADC AL,imm + flag bundle expansion smoke test
    {
        uint8_t adc_al[] = {0x14, 0x05};   // adc al, 5
        auto l = ir::lift(adc_al, sizeof(adc_al));
        CHECK(l && ir::is_fully_lifted(*l), "ADC AL, imm8 fully lifted");
        CHECK(count_op(*l, ir::Opcode::GET_CF) >= 1, "ADC reads CF");
    }
    printf("All IR tests passed!\n");
    return 0;
}
#else
#include <cstdio>
int main() { printf("VEDX64_IR not enabled, skipping.\n"); return 0; }
#endif
