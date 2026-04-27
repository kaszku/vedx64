// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

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

/// Number of explicit operands in the encoding.
inline uint8_t operand_count(const DecodedInstr& di) {
    if (!di.desc) return 0;
    return di.desc->num_operands;
}

/// True if operand `idx` is read (Access::Read or Access::ReadWrite).
inline bool is_operand_read(const Semantics& s, uint8_t idx) {
    if (idx >= 4) return false;
    return s.operand_access[idx] == Access::Read || s.operand_access[idx] == Access::ReadWrite;
}
/// True if operand `idx` is written (Access::Write or Access::ReadWrite).
inline bool is_operand_written(const Semantics& s, uint8_t idx) {
    if (idx >= 4) return false;
    return s.operand_access[idx] == Access::Write || s.operand_access[idx] == Access::ReadWrite;
}

/// Bitmask of GPR indices (0=RAX..15=R15) implicitly read by this instruction.
/// Covers: MUL/IMUL → RAX; DIV/IDIV → RAX,RDX; string ops → RSI,RDI,RCX;
/// CPUID → RAX,RCX; RDTSC → none (writes only); shifts → RCX (if CL form).
inline uint16_t implicit_gpr_reads(const DecodedInstr& di) {
    if (!di.desc) return 0;
    uint16_t mask = 0;
    switch (di.desc->mnemonic) {
    case Mnemonic::MUL: case Mnemonic::IMUL:
        if (di.desc->num_operands == 1) mask |= (1u << 0); // RAX
        break;
    case Mnemonic::DIV: case Mnemonic::IDIV:
        mask |= (1u << 0) | (1u << 2); // RAX, RDX
        break;
    case Mnemonic::MOVSB: case Mnemonic::MOVSW: case Mnemonic::MOVSD: case Mnemonic::MOVSQ:
        mask |= (1u << 6) | (1u << 7); break; // RSI, RDI
    case Mnemonic::STOSB: case Mnemonic::STOSW: case Mnemonic::STOSD: case Mnemonic::STOSQ:
        mask |= (1u << 0) | (1u << 7); break; // RAX, RDI
    case Mnemonic::LODSB: case Mnemonic::LODSW: case Mnemonic::LODSD: case Mnemonic::LODSQ:
        mask |= (1u << 6); break; // RSI
    case Mnemonic::CMPSB: case Mnemonic::CMPSW: case Mnemonic::CMPSD: case Mnemonic::CMPSQ:
    case Mnemonic::SCASB: case Mnemonic::SCASW: case Mnemonic::SCASD: case Mnemonic::SCASQ:
        mask |= (1u << 0) | (1u << 6) | (1u << 7); break; // RAX, RSI, RDI
    case Mnemonic::CPUID:
        mask |= (1u << 0) | (1u << 1); break; // RAX, RCX
    case Mnemonic::SYSCALL:
        mask |= (1u << 0); break; // RAX (syscall number)
    case Mnemonic::CWD: case Mnemonic::CDQ: case Mnemonic::CQO:
        mask |= (1u << 0); break; // RAX → RDX:RAX sign-extend
    case Mnemonic::CBW: case Mnemonic::CWDE: case Mnemonic::CDQE:
        mask |= (1u << 0); break; // RAX
    default: break;
    }
    return mask;
}

/// Bitmask of GPR indices implicitly written by this instruction.
inline uint16_t implicit_gpr_writes(const DecodedInstr& di) {
    if (!di.desc) return 0;
    uint16_t mask = 0;
    switch (di.desc->mnemonic) {
    case Mnemonic::MUL: case Mnemonic::IMUL:
        if (di.desc->num_operands == 1) mask |= (1u << 0) | (1u << 2); // RAX, RDX
        break;
    case Mnemonic::DIV: case Mnemonic::IDIV:
        mask |= (1u << 0) | (1u << 2); break; // RAX (quotient), RDX (remainder)
    case Mnemonic::MOVSB: case Mnemonic::MOVSW: case Mnemonic::MOVSD: case Mnemonic::MOVSQ:
        mask |= (1u << 6) | (1u << 7); break; // RSI++, RDI++
    case Mnemonic::STOSB: case Mnemonic::STOSW: case Mnemonic::STOSD: case Mnemonic::STOSQ:
        mask |= (1u << 7); break; // RDI++
    case Mnemonic::LODSB: case Mnemonic::LODSW: case Mnemonic::LODSD: case Mnemonic::LODSQ:
        mask |= (1u << 0) | (1u << 6); break; // RAX ← [RSI], RSI++
    case Mnemonic::CPUID:
        mask |= (1u << 0) | (1u << 1) | (1u << 2) | (1u << 3); break; // RAX,RCX,RDX,RBX
    case Mnemonic::RDTSC: case Mnemonic::RDTSCP:
        mask |= (1u << 0) | (1u << 2); break; // RAX, RDX
    case Mnemonic::CWD: case Mnemonic::CDQ: case Mnemonic::CQO:
        mask |= (1u << 2); break; // RDX
    case Mnemonic::CBW: case Mnemonic::CWDE: case Mnemonic::CDQE:
        mask |= (1u << 0); break; // RAX
    case Mnemonic::SYSCALL:
        mask |= (1u << 0) | (1u << 1); break; // RAX (return), RCX (saved RIP)
    default: break;
    }
    return mask;
}

/// True if the instruction is "pure" — only writes explicit register operands
/// and flags, with no memory stores, I/O, control flow, or system effects.
inline bool is_pure(const DecodedInstr& di) {
    auto* s = get_semantics(di);
    if (!s) return false;
    if (s->flow != FlowType::Sequential) return false;
    if (s->is_privileged) return false;
    if (s->category == Category::IO || s->category == Category::System ||
        s->category == Category::VMX || s->category == Category::String) return false;
    // Check no memory write operands
    if (!di.desc) return false;
    for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
        if (is_operand_written(*s, i) &&
            (di.desc->operands[i].addr == AddrMode::MemOnly ||
             (di.desc->operands[i].addr == AddrMode::ModRM_RM && (di.modrm >> 6) != 3)))
            return false;
    }
    return true;
}

/// True if the instruction's flag writes are potentially dead — i.e., all flags
/// it defines are also defined (without being read) by `next`.
/// Useful for dead-flag elimination: if dead_flags(curr, next), curr's flag write is unused.
inline bool dead_flags(const Semantics& curr, const Semantics& next) {
    if (!curr.flags_written) return false; // curr doesn't write flags
    if (next.flags_read & curr.flags_written) return false; // next reads some of curr's flags
    return (curr.flags_written & ~next.flags_written) == 0; // next overwrites all of curr's flags
}

// Jcc condition kinds. OTHER = JCXZ/JECXZ/JRCXZ/LOOP.
enum class JccKind : uint8_t {
    None = 0,
    O, NO, B, NB, Z, NZ, BE, NBE, S, NS, P, NP, L, NL, LE, NLE,
    CXZ, ECXZ, RCXZ, LOOP, LOOPE, LOOPNE,
};

// Map a DecodedInstr to its Jcc kind (None if not a conditional branch).
inline JccKind jcc_condition(const DecodedInstr& di) {
    if (!di.desc) return JccKind::None;
    switch (di.desc->mnemonic) {
    case Mnemonic::JO: return JccKind::O;
    case Mnemonic::JNO: return JccKind::NO;
    case Mnemonic::JB: case Mnemonic::JC: case Mnemonic::JNAE: return JccKind::B;
    case Mnemonic::JNB: case Mnemonic::JNC: case Mnemonic::JAE: return JccKind::NB;
    case Mnemonic::JZ: case Mnemonic::JE: return JccKind::Z;
    case Mnemonic::JNZ: case Mnemonic::JNE: return JccKind::NZ;
    case Mnemonic::JBE: case Mnemonic::JNA: return JccKind::BE;
    case Mnemonic::JNBE: case Mnemonic::JA: return JccKind::NBE;
    case Mnemonic::JS: return JccKind::S;
    case Mnemonic::JNS: return JccKind::NS;
    case Mnemonic::JP: case Mnemonic::JPE: return JccKind::P;
    case Mnemonic::JNP: case Mnemonic::JPO: return JccKind::NP;
    case Mnemonic::JL: case Mnemonic::JNGE: return JccKind::L;
    case Mnemonic::JNL: case Mnemonic::JGE: return JccKind::NL;
    case Mnemonic::JLE: case Mnemonic::JNG: return JccKind::LE;
    case Mnemonic::JNLE: case Mnemonic::JG: return JccKind::NLE;
    case Mnemonic::JCXZ: return JccKind::CXZ;
    case Mnemonic::JECXZ: return JccKind::ECXZ;
    case Mnemonic::JRCXZ: return JccKind::RCXZ;
    case Mnemonic::LOOP: return JccKind::LOOP;
    case Mnemonic::LOOPE: case Mnemonic::LOOPZ: return JccKind::LOOPE;
    case Mnemonic::LOOPNE: case Mnemonic::LOOPNZ: return JccKind::LOOPNE;
    default: return JccKind::None;
    }
}

// Terminators stop straight-line flow (unconditional jump, return, halt, syscall, ud2).
inline bool is_terminator(const DecodedInstr& di) {
    auto* s = get_semantics(di);
    if (!s) return false;
    return s->flow == FlowType::Jump || s->flow == FlowType::Return ||
           s->flow == FlowType::Halt || s->flow == FlowType::Syscall;
}

// True if the instruction can fall through to the next address.
inline bool has_fallthrough(const DecodedInstr& di) {
    auto* s = get_semantics(di);
    if (!s) return true;
    return s->flow == FlowType::Sequential || s->flow == FlowType::Branch ||
           s->flow == FlowType::Call;
}

// Flags this instruction defines (written with known value).
inline FlagSet flags_defined(const DecodedInstr& di) {
    auto* s = get_semantics(di); return s ? s->flags_written : 0;
}
// Flags this instruction reads before executing.
inline FlagSet flags_used(const DecodedInstr& di) {
    auto* s = get_semantics(di); return s ? s->flags_read : 0;
}

// Register class for a GPR encoding (by size/variant).
enum class GprClass : uint8_t {
    None = 0, GPR8, GPR8H, GPR16, GPR32, GPR64,
};

// Classify a register number + size. `has_rex` distinguishes SPL/BPL/SIL/DIL (with REX)
// from AH/CH/DH/BH (without REX) when size == 1 and reg >= 4.
inline GprClass gpr_class(uint8_t reg, uint8_t size, bool has_rex) {
    switch (size) {
    case 1: return (reg >= 4 && reg <= 7 && !has_rex) ? GprClass::GPR8H : GprClass::GPR8;
    case 2: return GprClass::GPR16;
    case 4: return GprClass::GPR32;
    case 8: return GprClass::GPR64;
    default: return GprClass::None;
    }
}

// Canonical 64-bit base register index (0..15) for a sub-register access.
// For AH/CH/DH/BH (reg 4..7, size 1, no REX), returns the low-4 sibling (0..3).
inline uint8_t gpr_canonical(uint8_t reg, uint8_t size, bool has_rex) {
    if (size == 1 && reg >= 4 && reg <= 7 && !has_rex) return reg - 4;
    return reg;
}

} // namespace vedx64
