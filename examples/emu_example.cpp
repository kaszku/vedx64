// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/emu.hpp"
#include <cstdio>
#include <cstring>

using namespace vedx64;

int main() {
    // Compute fibonacci(10) = 55
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rsp() = 0x800;

    uint8_t code[] = {
        0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0
        0xBB, 0x01, 0x00, 0x00, 0x00,  // mov ebx, 1
        0xB9, 0x0A, 0x00, 0x00, 0x00,  // mov ecx, 10
        // loop:
        0x89, 0xC2,                      // mov edx, eax
        0x01, 0xD8,                      // add eax, ebx
        0x89, 0xD3,                      // mov ebx, edx
        0xFF, 0xC9,                      // dec ecx
        0x75, 0xF6,                      // jnz loop (-10)
        0xF4,                            // hlt
    };

    memcpy(mem + 0x100, code, sizeof(code));
    cpu.rip = 0x100;

    printf("=== Fibonacci(10) Emulation ===\n\n");

    size_t steps = 0;
    while (true) {
        char disasm[128];
#ifdef VEDX64_STRINGS
        disassemble(cpu.mem + cpu.rip, 15, disasm, sizeof(disasm), cpu.rip);
#else
        disasm[0] = '\0';
#endif // VEDX64_STRINGS
        printf("  RIP=%04llX  RAX=%-10llu RBX=%-10llu RCX=%-10llu  %s\n",
               (unsigned long long)cpu.rip,
               (unsigned long long)cpu.rax(),
               (unsigned long long)cpu.rbx(),
               (unsigned long long)cpu.rcx(),
               disasm);

        StepResult r = emu_step(cpu);
        steps++;
        if (r != StepResult::OK) break;
    }

    printf("\nResult: fibonacci(10) = %llu (expected 55)\n", (unsigned long long)cpu.rax());
    printf("Steps executed: %zu\n", steps);

    return (cpu.rax() == 55) ? 0 : 1;
}
