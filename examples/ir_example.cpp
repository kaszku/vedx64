// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#if defined(VEDX64_IR) && defined(VEDX64_STRINGS)
#include "vedx64/ir.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstring>
using namespace vedx64;

static void show(const char* text, const uint8_t* code, size_t len) {
    auto lifted = ir::lift(code, len);
    printf("  %-32s", text);
    if (!lifted) { printf("(lift failed)\n"); return; }
    printf("[%zu ops]\n", lifted->ops.size());
    ir::dump(*lifted);
    printf("\n");
}

int main() {
    printf("=== IR Instruction Semantics ===\n\n");
    printf("--- Data Movement ---\n");
    { uint8_t c[] = {0x48,0x89,0xC8}; show("mov rax, rcx", c, 3); }
    { uint8_t c[] = {0x8B,0x03}; show("mov eax, [rbx]", c, 2); }
    { uint8_t c[] = {0x89,0x03}; show("mov [rbx], eax", c, 2); }
    { uint8_t c[] = {0x48,0x87,0xC8}; show("xchg rax, rcx", c, 3); }
    { uint8_t c[] = {0x55}; show("push rbp", c, 1); }
    { uint8_t c[] = {0x5D}; show("pop rbp", c, 1); }
    { uint8_t c[] = {0x48,0x8D,0x44,0x91,0x10}; show("lea rax, [rcx+rdx*4+0x10]", c, 5); }
    printf("--- Arithmetic ---\n");
    { uint8_t c[] = {0x48,0x01,0xC8}; show("add rax, rcx", c, 3); }
    { uint8_t c[] = {0x48,0x29,0xC8}; show("sub rax, rcx", c, 3); }
    { uint8_t c[] = {0x48,0x0F,0xAF,0xC1}; show("imul rax, rcx", c, 4); }
    { uint8_t c[] = {0x48,0xF7,0xD8}; show("neg rax", c, 3); }
    { uint8_t c[] = {0x48,0xFF,0xC0}; show("inc rax", c, 3); }
    { uint8_t c[] = {0x48,0x11,0xC8}; show("adc rax, rcx", c, 3); }
    printf("--- Bitwise Logic ---\n");
    { uint8_t c[] = {0x48,0x21,0xC8}; show("and rax, rcx", c, 3); }
    { uint8_t c[] = {0x48,0x09,0xC8}; show("or rax, rcx", c, 3); }
    { uint8_t c[] = {0x48,0x31,0xC0}; show("xor rax, rax", c, 3); }
    { uint8_t c[] = {0x48,0xD3,0xE0}; show("shl rax, cl", c, 3); }
    { uint8_t c[] = {0x48,0xF7,0xD0}; show("not rax", c, 3); }
    { uint8_t c[] = {0x48,0x0F,0xC8}; show("bswap rax", c, 3); }
    { uint8_t c[] = {0x48,0x0F,0xA3,0xC8}; show("bt rax, rcx", c, 4); }
    printf("--- Bit Manipulation ---\n");
    { uint8_t c[] = {0xF3,0x48,0x0F,0xB8,0xC1}; show("popcnt rax, rcx", c, 5); }
    { uint8_t c[] = {0xF3,0x48,0x0F,0xBC,0xC1}; show("tzcnt rax, rcx", c, 5); }
    { uint8_t c[] = {0xF3,0x48,0x0F,0xBD,0xC1}; show("lzcnt rax, rcx", c, 5); }
    printf("--- Control Flow ---\n");
    { uint8_t c[] = {0xE8,0x00,0x00,0x00,0x00}; show("call [rel+0]", c, 5); }
    { uint8_t c[] = {0xC3}; show("ret", c, 1); }
    printf("--- Comparison/Flags ---\n");
    { uint8_t c[] = {0x48,0x39,0xC8}; show("cmp rax, rcx", c, 3); }
    { uint8_t c[] = {0x48,0x85,0xC0}; show("test rax, rax", c, 3); }
    { uint8_t c[] = {0x0F,0x94,0xC0}; show("sete al", c, 3); }
    { uint8_t c[] = {0x48,0x0F,0x45,0xC1}; show("cmovne rax, rcx", c, 4); }
    printf("--- Extension ---\n");
    { uint8_t c[] = {0x48,0x0F,0xB6,0xC1}; show("movzx rax, cl", c, 4); }
    { uint8_t c[] = {0x48,0x0F,0xBE,0xC1}; show("movsx rax, cl", c, 4); }
    { uint8_t c[] = {0x48,0x98}; show("cdqe", c, 2); }
    printf("--- String Ops ---\n");
    { uint8_t c[] = {0xAA}; show("stosb", c, 1); }
    { uint8_t c[] = {0xAC}; show("lodsb", c, 1); }
    { uint8_t c[] = {0xA4}; show("movsb", c, 1); }
    { uint8_t c[] = {0xA6}; show("cmpsb", c, 1); }
    { uint8_t c[] = {0xAE}; show("scasb", c, 1); }
    printf("--- Stack Frame ---\n");
    { uint8_t c[] = {0xC8,0x20,0x00,0x00}; show("enter 0x20, 0", c, 4); }
    { uint8_t c[] = {0xC9}; show("leave", c, 1); }
    { uint8_t c[] = {0x9C}; show("pushfq", c, 1); }
    { uint8_t c[] = {0x9D}; show("popfq", c, 1); }
    printf("--- SSE Arithmetic ---\n");
    { uint8_t c[] = {0x0F,0x58,0xC1}; show("addps xmm0, xmm1", c, 3); }
    { uint8_t c[] = {0x0F,0x59,0xC1}; show("mulps xmm0, xmm1", c, 3); }
    { uint8_t c[] = {0x0F,0x57,0xC0}; show("xorps xmm0, xmm0", c, 3); }
    { uint8_t c[] = {0x0F,0x28,0xC1}; show("movaps xmm0, xmm1", c, 3); }
    printf("--- Atomic ---\n");
    { uint8_t c[] = {0x48,0x0F,0xB1,0xD1}; show("cmpxchg rcx, rdx", c, 4); }
    { uint8_t c[] = {0x48,0x0F,0xC1,0xD1}; show("xadd rcx, rdx", c, 4); }
    printf("=== IR Interpreter Demo ===\n\n");
    ir::Context ctx = {};
    uint8_t mem[256] = {};
    ctx.memory = mem; ctx.memory_size = sizeof(mem);

    printf("--- Compute (10 + 32) XOR 7 = %d ---\n", (10+32)^7);
    ctx.gpr[0] = 10; ctx.gpr[1] = 32; ctx.gpr[2] = 7;
    printf("  Initial: RAX=%llu RCX=%llu RDX=%llu\n", (unsigned long long)ctx.gpr[0], (unsigned long long)ctx.gpr[1], (unsigned long long)ctx.gpr[2]);
    { uint8_t c[]={0x48,0x01,0xC8}; auto l=ir::lift(c,3); if(l) ir::execute(ctx,*l); }
    printf("  After ADD RAX,RCX: RAX=%llu\n", (unsigned long long)ctx.gpr[0]);
    { uint8_t c[]={0x48,0x31,0xD0}; auto l=ir::lift(c,3); if(l) ir::execute(ctx,*l); }
    printf("  After XOR RAX,RDX: RAX=%llu (ZF=%d SF=%d)\n\n", (unsigned long long)ctx.gpr[0], ctx.flags[2], ctx.flags[3]);
    printf("--- String: STOSB (write 0x41 to mem[100]) ---\n");
    ctx.gpr[0] = 0x41; ctx.gpr[7] = 100;
    { uint8_t c[]={0xAA}; auto l=ir::lift(c,1);
      printf("  IR decomposition:\n"); if(l) { ir::dump(*l); ir::execute(ctx,*l); }
      printf("  Result: mem[100]=0x%02X RDI=%llu\n\n", mem[100], (unsigned long long)ctx.gpr[7]); }
    printf("--- Push/Pop roundtrip ---\n");
    ctx.gpr[5] = 0xDEADBEEF; ctx.gpr[4] = 128;
    { uint8_t c[]={0x55}; auto l=ir::lift(c,1);
      printf("  PUSH RBP (RBP=0xDEADBEEF):\n"); if(l) { ir::dump(*l); ir::execute(ctx,*l); }
      printf("  RSP=%llu\n", (unsigned long long)ctx.gpr[4]); }
    ctx.gpr[5] = 0;
    { uint8_t c[]={0x5D}; auto l=ir::lift(c,1);
      printf("  POP RBP:\n"); if(l) { ir::dump(*l); ir::execute(ctx,*l); }
      printf("  RBP=0x%llX RSP=%llu\n\n", (unsigned long long)ctx.gpr[5], (unsigned long long)ctx.gpr[4]); }
    printf("--- Flags: CMP + SETE ---\n");
    ctx.gpr[0] = 42; ctx.gpr[1] = 42;
    { uint8_t c[]={0x48,0x39,0xC8}; auto l=ir::lift(c,3); if(l) ir::execute(ctx,*l); }
    printf("  CMP RAX(42), RCX(42): ZF=%d CF=%d SF=%d OF=%d\n", ctx.flags[2], ctx.flags[0], ctx.flags[3], ctx.flags[4]);
    { uint8_t c[]={0x0F,0x94,0xC0}; auto l=ir::lift(c,3); if(l) ir::execute(ctx,*l); }
    printf("  SETE AL: AL=%d\n\n", (int)(ctx.gpr[0] & 0xFF));
    printf("--- Bit manipulation: POPCNT ---\n");
    ctx.gpr[1] = 0xFF00FF; // 16 bits set
    { uint8_t c[]={0xF3,0x48,0x0F,0xB8,0xC1}; auto l=ir::lift(c,5);
      printf("  POPCNT RAX, RCX (RCX=0xFF00FF):\n"); if(l) { ir::dump(*l); ir::execute(ctx,*l); }
      printf("  RAX=%llu (expected 16)\n\n", (unsigned long long)ctx.gpr[0]); }
    return 0;
}
#else
#include <cstdio>
int main() { printf("VEDX64_IR + VEDX64_STRINGS required.\n"); return 0; }
#endif
