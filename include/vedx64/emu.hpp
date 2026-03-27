// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include "vedx64/core.hpp"

namespace vedx64 {

struct CpuState {
    uint64_t gpr[16];      // RAX=0, RCX=1, RDX=2, RBX=3, RSP=4, RBP=5, RSI=6, RDI=7, R8-R15=8-15
    uint64_t rip;
    uint64_t rflags;
    uint8_t* mem;
    size_t mem_size;

    // ZMM registers (512-bit as 8 doubles; ymm is lower half, xmm is lower quarter)
    double zmm[32][8];
    uint32_t mxcsr;
    uint64_t opmask[8]; // k0-k7 opmask registers

    // x87 FPU
    double fpu_stack[8];
    int fpu_top;
    uint16_t fpu_status;
    uint16_t fpu_control;

    // Segment registers
    uint16_t seg[6]; // ES=0, CS=1, SS=2, DS=3, FS=4, GS=5
    uint64_t fs_base;
    uint64_t gs_base;

    // Convenience accessors
    uint64_t& rax() { return gpr[0]; }
    uint64_t& rcx() { return gpr[1]; }
    uint64_t& rdx() { return gpr[2]; }
    uint64_t& rbx() { return gpr[3]; }
    uint64_t& rsp() { return gpr[4]; }
    uint64_t& rbp() { return gpr[5]; }
    uint64_t& rsi() { return gpr[6]; }
    uint64_t& rdi() { return gpr[7]; }
};

enum class StepResult : uint8_t {
    OK,
    Halt,
    Unsupported,
    DecodeError,
    MemFault,
    DivByZero,
};

// Flag bit positions in RFLAGS
constexpr uint64_t RFLAG_CF = 1ULL << 0;
constexpr uint64_t RFLAG_PF = 1ULL << 2;
constexpr uint64_t RFLAG_AF = 1ULL << 4;
constexpr uint64_t RFLAG_ZF = 1ULL << 6;
constexpr uint64_t RFLAG_SF = 1ULL << 7;
constexpr uint64_t RFLAG_DF = 1ULL << 10;
constexpr uint64_t RFLAG_OF = 1ULL << 11;

// Initialize CPU state
void emu_init(CpuState& cpu, uint8_t* mem, size_t mem_size);

// Step a single instruction. Returns result code.
StepResult emu_step(CpuState& cpu);

// Run until halt/error/max_steps. Returns number of steps executed.
size_t emu_run(CpuState& cpu, size_t max_steps = 1000000);

} // namespace vedx64
