// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/codegen.hpp"
#include "vedx64/core.hpp"
#include <cstdio>

using namespace vedx64;

void example_basic() {
    printf("=== Assembler: Basic Code Generation ===\n\n");
    CodeGen g;

    // Generate: int square(int x) { return x * x; }
    // All instruction methods return CodeGen& for chaining
    g.push(rbp)
     .mov(rbp, rsp)
     .mov(eax, ptr(rbp, 16))  // first arg (Windows x64 shadow space)
     .imul(eax, eax)
     .pop(rbp)
     .ret();

#ifdef VEDX64_STRINGS
    // Disassemble the generated code
    size_t off = 0;
    while (off < g.size()) {
        char buf[128];
        size_t n = disassemble(g.data() + off, g.size() - off, buf, sizeof(buf), off);
        if (n == 0) break;
        printf("  %04zx: ", off);
        for (size_t i = 0; i < n; i++) printf("%02X ", g.data()[off + i]);
        for (size_t i = n; i < 8; i++) printf("   ");
        printf(" %s\n", buf);
        off += n;
    }
    printf("  Total: %zu bytes\n", g.size());
#endif // VEDX64_STRINGS
}

void example_labels() {
    printf("\n=== Assembler: Labels and Branches ===\n\n");
    CodeGen g;

    // Generate: int abs(int x) { return x < 0 ? -x : x; }
    auto negative = g.label();
    auto done = g.label();

    g.mov(eax, ecx);         // x in ecx (SysV)
    g.test(eax, eax);
    g.jl(negative);           // if x < 0, goto negative
    g.jmp(done);              // else skip
    g.bind(negative);
    g.neg(eax);               // eax = -eax
    g.bind(done);
    g.ret();

#ifdef VEDX64_STRINGS
    size_t off = 0;
    while (off < g.size()) {
        char buf[128];
        size_t n = disassemble(g.data() + off, g.size() - off, buf, sizeof(buf), off);
        if (n == 0) break;
        printf("  %04zx: ", off);
        for (size_t i = 0; i < n; i++) printf("%02X ", g.data()[off + i]);
        for (size_t i = n; i < 8; i++) printf("   ");
        printf(" %s\n", buf);
        off += n;
    }
#endif // VEDX64_STRINGS
}

void example_loop() {
    printf("\n=== Assembler: Loop ===\n\n");
    CodeGen g;

    // Generate: int sum(int n) { int s=0; while(n>0) { s+=n; n--; } return s; }
    auto loop_top = g.label();
    auto loop_end = g.label();

    g.xor_(eax, eax);         // s = 0
    g.bind(loop_top);
    g.test(ecx, ecx);          // test n
    g.jle(loop_end);            // if n <= 0, exit
    g.add(eax, ecx);            // s += n
    g.dec(ecx);                 // n--
    g.jmp(loop_top);            // goto top
    g.bind(loop_end);
    g.ret();

#ifdef VEDX64_STRINGS
    size_t off = 0;
    while (off < g.size()) {
        char buf[128];
        size_t n = disassemble(g.data() + off, g.size() - off, buf, sizeof(buf), off);
        if (n == 0) break;
        printf("  %04zx: ", off);
        for (size_t i = 0; i < n; i++) printf("%02X ", g.data()[off + i]);
        for (size_t i = n; i < 8; i++) printf("   ");
        printf(" %s\n", buf);
        off += n;
    }
#endif // VEDX64_STRINGS
}

void example_sse() {
    printf("\n=== Assembler: SSE Code ===\n\n");
    CodeGen g;

    // Generate: dot product of two floats
    g.movss(xmm0, ptr(rcx));       // load a
    g.movss(xmm1, ptr(rdx));       // load b
    g.mulss(xmm0, xmm1);           // a * b
    g.addss(xmm0, ptr(rcx, 4));    // + a[1]... (simplified)
    g.ret();

#ifdef VEDX64_STRINGS
    size_t off = 0;
    while (off < g.size()) {
        char buf[128];
        size_t n = disassemble(g.data() + off, g.size() - off, buf, sizeof(buf), off);
        if (n == 0) break;
        printf("  %04zx: ", off);
        for (size_t i = 0; i < n; i++) printf("%02X ", g.data()[off + i]);
        for (size_t i = n; i < 10; i++) printf("   ");
        printf(" %s\n", buf);
        off += n;
    }
#endif // VEDX64_STRINGS
}

int main() {
    example_basic();
    example_labels();
    example_loop();
    example_sse();
    return 0;
}
