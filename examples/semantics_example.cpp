// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/semantics.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstring>

using namespace vedx64;

void example_cfg() {
    printf("=== Control Flow Analysis ===\n\n");
    // A function with branches: compares, conditional jump, loop
    uint8_t code[] = {
        0x55,                         // push rbp
        0x48, 0x89, 0xE5,             // mov rbp, rsp
        0x89, 0x7D, 0xFC,             // mov [rbp-4], edi
        0x83, 0x7D, 0xFC, 0x00,       // cmp dword [rbp-4], 0
        0x7E, 0x07,                   // jle $+7
        0x8B, 0x45, 0xFC,             // mov eax, [rbp-4]
        0x0F, 0xAF, 0xC0,             // imul eax, eax
        0xEB, 0x05,                   // jmp $+5
        0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, 0
        0x5D,                         // pop rbp
        0xC3,                         // ret
    };

    uint64_t rip = 0x401000;
    size_t offset = 0;
    int block_id = 0;
    printf("  Block %d (0x%llx):\n", block_id, (unsigned long long)rip);

    while (offset < sizeof(code)) {
        DecodedInstr di;
        size_t n = decode(&code[offset], sizeof(code) - offset, di);
        if (n == 0) break;

        const Semantics* sem = get_semantics(di);
        char disasm[128];
#ifdef VEDX64_STRINGS
        disassemble(&code[offset], sizeof(code) - offset, disasm, sizeof(disasm), rip + offset);
#else
        disasm[0] = '\0';
#endif // VEDX64_STRINGS

        const char* flow_str = "seq";
        if (sem) {
            switch (sem->flow) {
            case FlowType::Branch: flow_str = "BRANCH"; break;
            case FlowType::Jump: flow_str = "JUMP"; break;
            case FlowType::Call: flow_str = "CALL"; break;
            case FlowType::Return: flow_str = "RET"; break;
            case FlowType::Syscall: flow_str = "SYSCALL"; break;
            default: break;
            }
        }
        printf("    %04llx: %-30s [%s]\n", (unsigned long long)(rip + offset), disasm, flow_str);

        if (sem && sem->flow != FlowType::Sequential && sem->flow != FlowType::Return) {
            block_id++;
            printf("  Block %d (0x%llx):\n", block_id, (unsigned long long)(rip + offset + n));
        }
        offset += n;
    }
}

void example_flag_liveness() {
    printf("\n=== Flag Liveness Analysis ===\n\n");
    // Analyze which instructions produce flags that are actually consumed
    uint8_t code[] = {
        0x01, 0xC8,                   // add eax, ecx    (writes OSZAPC)
        0x89, 0xC2,                   // mov edx, eax    (no flags)
        0x83, 0xF8, 0x0A,             // cmp eax, 10     (writes OSZAPC)
        0x7C, 0x02,                   // jl $+2          (reads SF,OF)
        0x31, 0xC0,                   // xor eax, eax    (writes OSZAPC)
        0x11, 0xD0,                   // adc eax, edx    (reads CF, writes OSZAPC)
        0xC3,                         // ret
    };

    const char* flag_names[] = {"CF","PF","AF","ZF","SF","OF","DF","IF"};
    uint64_t rip = 0;
    size_t offset = 0;

    while (offset < sizeof(code)) {
        DecodedInstr di;
        size_t n = decode(&code[offset], sizeof(code) - offset, di);
        if (n == 0) break;
        const Semantics* sem = get_semantics(di);
        char disasm[128];
#ifdef VEDX64_STRINGS
        disassemble(&code[offset], sizeof(code) - offset, disasm, sizeof(disasm), rip + offset);
#else
        disasm[0] = '\0';
#endif // VEDX64_STRINGS

        printf("    %-30s  ", disasm);
        if (sem) {
            if (sem->flags_read) {
                printf("reads:");
                for (int f = 0; f < 8; f++) if ((sem->flags_read >> f) & 1) printf(" %s", flag_names[f]);
                printf("  ");
            }
            if (sem->flags_written) {
                printf("writes:");
                for (int f = 0; f < 8; f++) if ((sem->flags_written >> f) & 1) printf(" %s", flag_names[f]);
            }
            if (sem->flags_undefined) {
                printf("  undef:");
                for (int f = 0; f < 8; f++) if ((sem->flags_undefined >> f) & 1) printf(" %s", flag_names[f]);
            }
            if (!sem->flags_read && !sem->flags_written)
                printf("(no flag effects)");
        }
        printf("\n");
        offset += n;
    }
}

void example_classification() {
    printf("\n=== Instruction Classification ===\n\n");
    size_t cat_counts[32] = {};
    size_t flow_counts[8] = {};
    size_t privileged = 0, lockable = 0, flag_writers = 0;

    for (size_t i = 0; i < g_instr_table_size; ++i) {
        const auto& sem = g_semantics_table[i];
        cat_counts[(int)sem.category]++;
        flow_counts[(int)sem.flow]++;
        if (sem.is_privileged) privileged++;
        if (sem.lock_valid) lockable++;
        if (sem.flags_written) flag_writers++;
    }

    const char* cat_names[] = {"Unknown","Arithmetic","Logic","Shift","DataMove","Compare","Branch","Call","Stack","String","IO","Flag","System","Nop","Bit","Convert","SimdArith","SimdLogic","SimdMove","SimdCompare","SimdConvert","SimdShuffle","SimdOther","Crypto","FPU","VMX","Sync"};
    const char* flow_names[] = {"Sequential","Branch","Jump","Call","Return","Syscall","Halt"};

    printf("  Categories:\n");
    for (int i = 0; i < 27; ++i)
        if (cat_counts[i] > 0)
            printf("    %-15s %zu\n", cat_names[i], cat_counts[i]);

    printf("\n  Control flow:\n");
    for (int i = 0; i < 7; ++i)
        if (flow_counts[i] > 0)
            printf("    %-15s %zu\n", flow_names[i], flow_counts[i]);

    printf("\n  Privileged:    %zu\n", privileged);
    printf("  LOCK-able:     %zu\n", lockable);
    printf("  Flag writers:  %zu\n", flag_writers);
}

void example_operand_access() {
    printf("\n=== Operand Access Patterns ===\n\n");
    uint8_t code[] = {
        0x8B, 0x45, 0xFC,             // mov eax, [rbp-4]   (write, read)
        0x03, 0x45, 0xF8,             // add eax, [rbp-8]   (rw, read)
        0x89, 0x45, 0xF4,             // mov [rbp-12], eax  (write, read)
        0xF7, 0xD8,                   // neg eax            (rw)
        0x87, 0xC1,                   // xchg ecx, eax      (rw, rw)
    };
    const char* acc_names[] = {"none","R","W","RW"};
    size_t offset = 0;
    while (offset < sizeof(code)) {
        DecodedInstr di;
        size_t n = decode(&code[offset], sizeof(code) - offset, di);
        if (n == 0) break;
        const Semantics* sem = get_semantics(di);
        char disasm[128];
#ifdef VEDX64_STRINGS
        disassemble(&code[offset], sizeof(code) - offset, disasm, sizeof(disasm), 0);
#else
        disasm[0] = '\0';
#endif // VEDX64_STRINGS
        printf("    %-25s  ops: ", disasm);
        if (sem) for (int i = 0; i < 4 && di.desc->operands[i].addr != AddrMode::None; ++i)
            printf("%s%s", i?",":"", acc_names[(int)sem->operand_access[i]]);
        printf("\n");
        offset += n;
    }
}

int main() {
    example_cfg();
    example_flag_liveness();
    example_classification();
    example_operand_access();
    return 0;
}
