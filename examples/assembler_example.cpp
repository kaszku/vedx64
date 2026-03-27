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
    printf("--- Single instructions ---\n");
    const char* insns[] = {"nop", "ret", "push rbp", "mov rbp, rsp", "sub rsp, 0x20",
        "mov rax, rcx", "add rax, 42", "xor eax, eax", "int3",
        "mov rax, [rbx]", "mov [rax], rcx", "add dword [rsp+8], 1",
        "movaps xmm0, [rax]"};
    for (auto text : insns) {
        auto bytes = assemble(text);
        if (bytes) {
            printf("  %-30s -> ", text);
            for (auto b : *bytes) printf("%02X ", b);
#ifdef VEDX64_STRINGS
            char dis[128]; disassemble(bytes->data(), bytes->size(), dis, sizeof(dis), 0);
            printf(" (%s)", dis);
#endif
            printf("\n");
        } else printf("  %-30s -> FAILED\n", text);
    }
    printf("\n--- Block assembly ---\n");
    auto block = assemble_block("push rbp\nmov rbp, rsp\nsub rsp, 0x20\nmov rax, [rbp+16]\nadd rsp, 0x20\npop rbp\nret");
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
