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
    { uint8_t c[] = {0x0F,0x57,0xC0}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0x0F,0x58,0xC1}; lift_ok(c, sizeof(c)); }
    { uint8_t c[] = {0xF3,0x0F,0x58,0xC1}; lift_ok(c, sizeof(c)); }
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

    printf("All IR tests passed!\n");
    return 0;
}
#else
#include <cstdio>
int main() { printf("VEDX64_IR not enabled, skipping.\n"); return 0; }
#endif
