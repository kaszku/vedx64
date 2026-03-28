// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#ifdef VEDX64_IR
#include "vedx64/ir.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
using namespace vedx64;
int main() {
    printf("=== IR Lifting & Interpreter Example ===\n\n");
    printf("--- Lifting instructions ---\n");
    struct { const char* name; uint8_t bytes[8]; size_t len; } tests[] = {
        {"nop", {0x90}, 1},
        {"mov rax,rcx", {0x48,0x89,0xC8}, 3},
        {"add rax,rcx", {0x48,0x01,0xC8}, 3},
        {"sub rax,rcx", {0x48,0x29,0xC8}, 3},
        {"xor rax,rax", {0x48,0x31,0xC0}, 3},
        {"push rbp", {0x55}, 1},
        {"pop rbp", {0x5D}, 1},
        {"ret", {0xC3}, 1},
    };
    for (auto& t : tests) {
        auto l = ir::lift(t.bytes, t.len);
        if (l) printf("  %-16s -> %zu IR ops\n", t.name, l->ops.size());
        else printf("  %-16s -> FAILED\n", t.name);
    }
    printf("\n--- Interpreter: mov rax,10; add rax,rcx (rcx=32) ---\n");
    ir::Context ctx;
    ctx.gpr[1] = 32; // RCX = 32
    // mov rax, 10 — we'll set RAX directly since MOV reg,imm needs opcode-reg form
    ctx.gpr[0] = 10; // RAX = 10
    // add rax, rcx
    uint8_t add_code[] = {0x48, 0x01, 0xC8};
    auto add_lifted = ir::lift(add_code, sizeof(add_code));
    if (add_lifted) {
        ir::execute(ctx, *add_lifted);
        printf("  RAX = %llu (expected 42)\n", (unsigned long long)ctx.gpr[0]);
        printf("  ZF  = %d\n", ctx.flags[2]);
        printf("  SF  = %d\n", ctx.flags[3]);
    }
    printf("\n--- xor rax,rax ---\n");
    { ir::Context c2; c2.gpr[0]=42;
      uint8_t xor_c[]={0x48,0x31,0xC0}; auto l=ir::lift(xor_c,3);
      if(l) { ir::execute(c2,*l); printf("  RAX=%llu ZF=%d\n",(unsigned long long)c2.gpr[0],c2.flags[2]); } }
    return 0;
}
#else
#include <cstdio>
int main() { printf("VEDX64_IR not enabled.\n"); return 0; }
#endif
