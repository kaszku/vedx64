// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#ifdef VEDX64_ASSEMBLER
#include "vedx64/assembler.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
using namespace vedx64;
int main() {
    printf("=== Text Assembler Example ===\n\n");
    auto show = [](const char* text) {
        auto bytes = assemble(text);
        if (bytes) {
            printf("  %-35s -> ", text);
            for (auto b : *bytes) printf("%02X ", b);
#ifdef VEDX64_STRINGS
            char dis[128]; disassemble(bytes->data(), bytes->size(), dis, sizeof(dis), 0);
            printf(" (%s)", dis);
#endif
            printf("\n");
        } else printf("  %-35s -> FAILED\n", text);
    };

    printf("--- Basic instructions ---\n");
    show("nop");
    show("ret");
    show("push rbp");
    show("mov rbp, rsp");
    show("sub rsp, 0x20");
    show("mov rax, rcx");
    show("add rax, 42");
    show("xor eax, eax");
    show("int3");
    printf("\n--- Memory operands ---\n");
    show("mov rax, [rbx]");
    show("mov rax, [rbx+8]");
    show("mov rax, [rbx+rcx*4+16]");
    show("mov [rax], rcx");
    show("add dword [rsp+8], 1");
    show("lea rax, [rip+0x1000]");
    show("movaps xmm0, [rax]");
    show("movaps [rbx], xmm1");
    printf("\n--- Segment overrides ---\n");
    show("mov rax, fs:[rbx]");
    show("mov rax, gs:[0x28]");
    printf("\n--- Prefixes ---\n");
    show("lock add [rax], rcx");
    show("lock cmpxchg [rax], rcx");
    show("rep movsb");
    show("rep stosq");
    show("repe cmpsb");
    printf("\n--- Data directives ---\n");
    show("db 0x90");
    show("db 0xCC, 0xCC, 0x90");
    show("dw 0x1234");
    show("dd 0xDEADBEEF");
    printf("\n--- Block with labels ---\n");
    auto block = assemble_block(
        "    xor eax, eax\n"
        "    cmp ecx, 0\n"
        "    je done\n"
        "loop:\n"
        "    add eax, 1\n"
        "    dec ecx\n"
        "    jnz loop\n"
        "done:\n"
        "    ret\n"
    );
    if (block) {
        printf("  %zu bytes: ", block->size());
        for (auto b : *block) printf("%02X ", b);
        printf("\n");
#ifdef VEDX64_STRINGS
        size_t off=0; while(off<block->size()) {
            char dis[128]; size_t n=disassemble(block->data()+off,block->size()-off,dis,sizeof(dis),off);
            if(!n) break; printf("    %04zx: %s\n",off,dis); off+=n;
        }
#endif
    }
    return 0;
}
#else
#include <cstdio>
int main() { printf("VEDX64_ASSEMBLER not enabled.\n"); return 0; }
#endif
