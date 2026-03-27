// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#pragma once
#include "vedx64/core.hpp"
#include "vedx64/encoding_id.hpp"
#include <cstdint>

namespace vedx64 {

// RFLAGS bit positions for semantic flags
enum Flag : uint8_t {
    FLAG_CF = 0,  // Carry
    FLAG_PF = 1,  // Parity
    FLAG_AF = 2,  // Auxiliary Carry
    FLAG_ZF = 3,  // Zero
    FLAG_SF = 4,  // Sign
    FLAG_OF = 5,  // Overflow
    FLAG_DF = 6,  // Direction
    FLAG_IF = 7,  // Interrupt
};
using FlagSet = uint8_t; // bitmask of Flag values

enum class FlowType : uint8_t {
    Sequential = 0,  // falls through to next instruction
    Branch,          // conditional branch (Jcc)
    Jump,            // unconditional jump
    Call,            // function call
    Return,          // function return
    Syscall,         // system call / interrupt
    Halt,            // stops execution (HLT)
};

enum class Category : uint8_t {
    Unknown = 0,
    Arithmetic,      // ADD, SUB, MUL, DIV, INC, DEC, NEG, ADC, SBB
    Logic,           // AND, OR, XOR, NOT, TEST
    Shift,           // SHL, SHR, SAR, ROL, ROR, RCL, RCR, SHLD, SHRD
    DataMove,        // MOV, MOVSX, MOVZX, LEA, XCHG, CMOVcc, PUSH, POP
    Compare,         // CMP, TEST
    Branch,          // Jcc, JMP, LOOP
    Call,            // CALL, RET
    Stack,           // PUSH, POP, ENTER, LEAVE
    String,          // MOVS, STOS, LODS, CMPS, SCAS
    IO,              // IN, OUT, INS, OUTS
    Flag,            // CLC, STC, CLD, STD, CLI, STI, CMC, LAHF, SAHF
    System,          // SYSCALL, SYSRET, INT, HLT, CPUID, RDTSC, WRMSR
    Nop,             // NOP, PAUSE, HINT_NOP
    Bit,             // BT, BTS, BTR, BTC, BSF, BSR, POPCNT
    Convert,         // CBW, CWD, CDQ, CQO, CWDE, CDQE
    SimdArith,       // ADDPS, MULPD, etc.
    SimdLogic,       // ANDPS, ORPS, XORPS, etc.
    SimdMove,        // MOVAPS, MOVDQA, etc.
    SimdCompare,     // CMPPS, COMISS, etc.
    SimdConvert,     // CVTSS2SD, etc.
    SimdShuffle,     // SHUFPS, PSHUFB, etc.
    SimdOther,       // EMMS, LDMXCSR, etc.
    Crypto,          // CRC32
    FPU,             // x87 FPU instructions
    VMX,             // VMX instructions
    Sync,            // LOCK, FENCE, MFENCE, etc.
};

enum class Access : uint8_t {
    None = 0,
    Read = 1,
    Write = 2,
    ReadWrite = 3,
};

struct Semantics {
    FlagSet flags_read;      // flags tested before execution
    FlagSet flags_written;    // flags with defined values after
    FlagSet flags_undefined;  // flags left undefined after
    FlagSet flags_set;        // flags forced to 0 or 1
    FlowType flow;
    Category category;
    Access operand_access[4]; // per-operand read/write
    uint8_t ring;             // 0=any, 3=ring0 required
    bool lock_valid;          // LOCK prefix allowed
    bool is_privileged;       // requires ring 0
};

extern const Semantics g_semantics_table[];

// Get semantics for a decoded instruction
inline const Semantics* get_semantics(const DecodedInstr& di) {
    if (!di.desc) return nullptr;
    ptrdiff_t idx = di.desc - g_instr_table;
    if (idx < 0 || idx >= (ptrdiff_t)g_instr_table_size) return nullptr;
    return &g_semantics_table[idx];
}

inline const Semantics* get_semantics(EncodingId eid) {
    auto idx = static_cast<uint16_t>(eid);
    if (idx == 0 || idx > g_instr_table_size) return nullptr;
    return &g_semantics_table[idx - 1];
}

inline bool is_branch(const Semantics& s) { return s.flow == FlowType::Branch; }
inline bool is_jump(const Semantics& s) { return s.flow == FlowType::Jump; }
inline bool is_call(const Semantics& s) { return s.flow == FlowType::Call; }
inline bool is_return(const Semantics& s) { return s.flow == FlowType::Return; }
inline bool is_control_flow(const Semantics& s) { return s.flow != FlowType::Sequential; }
inline bool reads_flags(const Semantics& s) { return s.flags_read != 0; }
inline bool writes_flags(const Semantics& s) { return s.flags_written != 0; }
inline bool reads_flag(const Semantics& s, Flag f) { return (s.flags_read >> f) & 1; }
inline bool writes_flag(const Semantics& s, Flag f) { return (s.flags_written >> f) & 1; }

} // namespace vedx64
