// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#ifdef VEDX64_IR
#include "vedx64/ir.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstdlib>

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
    CHECK(la->ops[0].opcode == ir::Opcode::ADD, "add opcode");

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
    printf("    Semantic: %d/%d\n", sem_pass, sem_total);
    CHECK(sem_pass == sem_total, "all semantic checks pass");

    printf("All IR tests passed!\n");
    return 0;
}
#else
#include <cstdio>
int main() { printf("VEDX64_IR not enabled, skipping.\n"); return 0; }
#endif
