// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <unordered_map>
#include "vedx64/core.hpp"
#include "vedx64/mnemonic.hpp"
#include "vedx64/instruction.hpp"
#include "vedx64/operand.hpp"

namespace vedx64::analysis {


/// x86 condition code (4-bit tttn field).
enum class CondCode : uint8_t {
    O   = 0x0, NO  = 0x1,
    B   = 0x2, NB  = 0x3,
    Z   = 0x4, NZ  = 0x5,
    BE  = 0x6, NBE = 0x7,
    S   = 0x8, NS  = 0x9,
    P   = 0xA, NP  = 0xB,
    L   = 0xC, NL  = 0xD,
    LE  = 0xE, NLE = 0xF,
};

/// Short mnemonic suffix for a condition code ("o", "b", "z", "le", ...).
inline const char* cond_name(CondCode c) {
    static const char* n[16] = {
        "o","no","b","nb","z","nz","be","nbe",
        "s","ns","p","np","l","nl","le","nle"
    };
    return n[static_cast<uint8_t>(c) & 0xF];
}


/// EFLAGS bitmask used by sets_eflags / reads_eflags.
/// Bit positions match the order used in vedx64::semantics (CF, PF, AF, ZF, SF, OF, DF, IF).
enum EFlag : uint8_t {
    EF_CF = 1u << 0,
    EF_PF = 1u << 1,
    EF_AF = 1u << 2,
    EF_ZF = 1u << 3,
    EF_SF = 1u << 4,
    EF_OF = 1u << 5,
    EF_DF = 1u << 6,
    EF_IF = 1u << 7,
};
using EFlagMask = uint8_t;

constexpr EFlagMask EF_STATUS = EF_CF|EF_PF|EF_AF|EF_ZF|EF_SF|EF_OF; // arithmetic flags


// ============================================================
//  Mnemonic-level queries (cheap, no DecodedInstr needed)
// ============================================================

/// True if `m` is a conditional branch (Jcc) or LOOP*/JCXZ-family.
inline bool is_jcc(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::JO:case M::JNO:
    case M::JB:case M::JC:case M::JNAE:
    case M::JNB:case M::JNC:case M::JAE:
    case M::JZ:case M::JE:
    case M::JNZ:case M::JNE:
    case M::JBE:case M::JNA:
    case M::JNBE:case M::JA:
    case M::JS:case M::JNS:
    case M::JP:case M::JPE:
    case M::JNP:case M::JPO:
    case M::JL:case M::JNGE:
    case M::JNL:case M::JGE:
    case M::JLE:case M::JNG:
    case M::JNLE:case M::JG:
    case M::JCXZ:case M::JECXZ:case M::JRCXZ:
    case M::LOOP:case M::LOOPE:case M::LOOPZ:case M::LOOPNE:case M::LOOPNZ:
        return true;
    default: return false;
    }
}

/// Map a Jcc mnemonic to its CondCode. Returns CondCode::O if not a recognized
/// conditional branch (caller should check is_jcc first). LOOP/JCXZ family
/// have no plain CondCode and return CondCode::O as well.
inline CondCode jcc_condition(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::JO:                                   return CondCode::O;
    case M::JNO:                                  return CondCode::NO;
    case M::JB:case M::JC:case M::JNAE:           return CondCode::B;
    case M::JNB:case M::JNC:case M::JAE:          return CondCode::NB;
    case M::JZ:case M::JE:                        return CondCode::Z;
    case M::JNZ:case M::JNE:                      return CondCode::NZ;
    case M::JBE:case M::JNA:                      return CondCode::BE;
    case M::JNBE:case M::JA:                      return CondCode::NBE;
    case M::JS:                                   return CondCode::S;
    case M::JNS:                                  return CondCode::NS;
    case M::JP:case M::JPE:                       return CondCode::P;
    case M::JNP:case M::JPO:                      return CondCode::NP;
    case M::JL:case M::JNGE:                      return CondCode::L;
    case M::JNL:case M::JGE:                      return CondCode::NL;
    case M::JLE:case M::JNG:                      return CondCode::LE;
    case M::JNLE:case M::JG:                      return CondCode::NLE;
    default:                                      return CondCode::O;
    }
}

/// Canonical Jcc mnemonic for a given condition code (canonical = the
/// short name: O,NO,B,NB,Z,NZ,BE,A,S,NS,P,NP,L,GE,LE,G).
inline Mnemonic jcc_for_condition(CondCode c) {
    using M = Mnemonic;
    switch (c) {
    case CondCode::O:   return M::JO;
    case CondCode::NO:  return M::JNO;
    case CondCode::B:   return M::JB;
    case CondCode::NB:  return M::JNB;
    case CondCode::Z:   return M::JZ;
    case CondCode::NZ:  return M::JNZ;
    case CondCode::BE:  return M::JBE;
    case CondCode::NBE: return M::JA;
    case CondCode::S:   return M::JS;
    case CondCode::NS:  return M::JNS;
    case CondCode::P:   return M::JP;
    case CondCode::NP:  return M::JNP;
    case CondCode::L:   return M::JL;
    case CondCode::NL:  return M::JGE;
    case CondCode::LE:  return M::JLE;
    case CondCode::NLE: return M::JG;
    }
    return M::INVALID;
}

/// True if `m` is a CMOVcc.
inline bool is_cmov(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::CMOVO:case M::CMOVNO:
    case M::CMOVB:case M::CMOVC:case M::CMOVNAE:
    case M::CMOVNB:case M::CMOVNC:case M::CMOVAE:
    case M::CMOVZ:case M::CMOVE:
    case M::CMOVNZ:case M::CMOVNE:
    case M::CMOVBE:case M::CMOVNA:
    case M::CMOVNBE:case M::CMOVA:
    case M::CMOVS:case M::CMOVNS:
    case M::CMOVP:case M::CMOVPE:
    case M::CMOVNP:case M::CMOVPO:
    case M::CMOVL:case M::CMOVNGE:
    case M::CMOVNL:case M::CMOVGE:
    case M::CMOVLE:case M::CMOVNG:
    case M::CMOVNLE:case M::CMOVG:
        return true;
    default: return false;
    }
}

/// Map a CMOVcc mnemonic to its CondCode (returns CondCode::O if not CMOV).
inline CondCode cmov_condition(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::CMOVO:                                   return CondCode::O;
    case M::CMOVNO:                                  return CondCode::NO;
    case M::CMOVB:case M::CMOVC:case M::CMOVNAE:     return CondCode::B;
    case M::CMOVNB:case M::CMOVNC:case M::CMOVAE:    return CondCode::NB;
    case M::CMOVZ:case M::CMOVE:                     return CondCode::Z;
    case M::CMOVNZ:case M::CMOVNE:                   return CondCode::NZ;
    case M::CMOVBE:case M::CMOVNA:                   return CondCode::BE;
    case M::CMOVNBE:case M::CMOVA:                   return CondCode::NBE;
    case M::CMOVS:                                   return CondCode::S;
    case M::CMOVNS:                                  return CondCode::NS;
    case M::CMOVP:case M::CMOVPE:                    return CondCode::P;
    case M::CMOVNP:case M::CMOVPO:                   return CondCode::NP;
    case M::CMOVL:case M::CMOVNGE:                   return CondCode::L;
    case M::CMOVNL:case M::CMOVGE:                   return CondCode::NL;
    case M::CMOVLE:case M::CMOVNG:                   return CondCode::LE;
    case M::CMOVNLE:case M::CMOVG:                   return CondCode::NLE;
    default:                                         return CondCode::O;
    }
}

/// True for unconditional jump instructions (JMP, JMPF). Does NOT include
/// conditional branches — use is_jcc for those.
inline bool is_unconditional_branch(Mnemonic m) {
    return m == Mnemonic::JMP || m == Mnemonic::JMPF;
}

/// True for any branch (conditional or unconditional jump).
inline bool is_relative_branch(Mnemonic m) {
    return is_unconditional_branch(m) || is_jcc(m);
}

/// True for CALL / CALLF.
inline bool is_call(Mnemonic m) {
    return m == Mnemonic::CALL || m == Mnemonic::CALLF;
}

/// True for return-from-procedure / interrupt-return instructions.
inline bool is_ret(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::RETN:case M::RETF:
    case M::IRET:case M::IRETD:case M::IRETQ:
        return true;
    default: return false;
    }
}

/// True for instructions that change RIP non-sequentially (any branch,
/// call, return, syscall, interrupt, halt).
inline bool changes_rip(Mnemonic m) {
    using M = Mnemonic;
    if (is_relative_branch(m) || is_call(m) || is_ret(m)) return true;
    switch (m) {
    case M::SYSCALL:case M::SYSENTER:case M::SYSEXIT:
    case M::INT:case M::INT1:case M::INTO:
    case M::HLT:case M::UD:case M::UD2:
        return true;
    default: return false;
    }
}

/// True for the boolean-logic family: AND, OR, XOR, NOT, TEST.
inline bool is_logical(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::AND:case M::OR:case M::XOR:case M::NOT:case M::TEST:
        return true;
    default: return false;
    }
}

/// True for the integer-arithmetic family: ADD, SUB, ADC, SBB, INC, DEC,
/// NEG, MUL, IMUL, DIV, IDIV, CMP.
inline bool is_arith(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::ADD:case M::SUB:case M::ADC:case M::SBB:
    case M::INC:case M::DEC:case M::NEG:
    case M::MUL:case M::IMUL:case M::DIV:case M::IDIV:
    case M::CMP:
        return true;
    default: return false;
    }
}

/// Canonical operand size implied by a mnemonic (1/2/4/8 bytes), or 0 if
/// the size is variable / not implied. Covers the size-suffixed string and
/// stack mnemonics: MOVSB/W/D/Q, STOSB/W/D/Q, LODSB/W/D/Q, SCASB/W/D/Q,
/// CMPSB/W/D/Q, plus CBW/CWD/CDQ/CQO/CWDE/CDQE convert pairs.
inline uint8_t canonical_size(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::MOVSB:case M::STOSB:case M::LODSB:case M::SCASB:case M::CMPSB:return 1;
    case M::MOVSW:case M::STOSW:case M::LODSW:case M::SCASW:case M::CMPSW:case M::CBW:return 2;
    case M::MOVSD:case M::STOSD:case M::LODSD:case M::SCASD:case M::CMPSD:case M::CWDE:case M::CWD:return 4;
    case M::MOVSQ:case M::STOSQ:case M::LODSQ:case M::SCASQ:case M::CMPSQ:case M::CDQE:case M::CDQ:case M::CQO:return 8;
    default: return 0;
    }
}

/// True for shift / rotate family: SHL, SHR, SAR, SAL, ROL, ROR, RCL, RCR,
/// SHLD, SHRD, plus BMI2 SARX/SHLX/SHRX/RORX.
inline bool is_shift(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    case M::SHL:case M::SHR:case M::SAR:
    case M::ROL:case M::ROR:case M::RCL:case M::RCR:
    case M::SHLD:case M::SHRD:
    case M::SARX:case M::SHLX:case M::SHRX:case M::RORX:
        return true;
    default: return false;
    }
}

/// True if the instruction's only register operand reference is through ModRM.RM
/// with mod=3 (register-direct) or no modrm at all.
inline bool is_indirect_branch(const DecodedInstr& di) {
    if (!di.desc) return false;
    if (!is_call(di.desc->mnemonic) && !is_unconditional_branch(di.desc->mnemonic))
        return false;
    // Direct branch carries a J-form (rel) or A-form (far ptr) operand;
    // indirect branch uses E (modrm) or M (mem-only).
    for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
        AddrMode a = di.desc->operands[i].addr;
        if (a == AddrMode::ModRM_RM || a == AddrMode::MemOnly || a == AddrMode::RegOnly)
            return true;
    }
    return false;
}


// ============================================================
//  EFLAGS read/write masks
//
//  These are derived from instruction semantics (the same data used by
//  vedx64::semantics); the table below covers the flag-affecting integer
//  instructions an analysis pass typically cares about. Anything not listed
//  returns 0 — callers should treat 0 as "no information" and either
//  conservatively assume worst-case or fall back to a richer table.
// ============================================================

/// EFLAGS bits this mnemonic writes (defines with a new value).
inline EFlagMask sets_eflags(Mnemonic m) {
    using M = Mnemonic;
    switch (m) {
    // Arithmetic — CF,PF,AF,ZF,SF,OF
    case M::ADD:case M::SUB:case M::ADC:case M::SBB:
    case M::CMP:case M::NEG:case M::SUBPD:
        return EF_STATUS;
    case M::INC:case M::DEC:
        return EF_PF|EF_AF|EF_ZF|EF_SF|EF_OF; // CF unchanged
    case M::MUL:case M::IMUL:
        return EF_CF|EF_OF|EF_PF|EF_AF|EF_ZF|EF_SF; // PF/AF/ZF/SF undefined per spec but we mark written
    case M::DIV:case M::IDIV:
        return EF_CF|EF_PF|EF_AF|EF_ZF|EF_SF|EF_OF; // all undefined
    // Logic — clears CF,OF; sets PF,ZF,SF; AF undefined
    case M::AND:case M::OR:case M::XOR:case M::TEST:
        return EF_CF|EF_PF|EF_AF|EF_ZF|EF_SF|EF_OF;
    // Shifts — CF defined; OF defined for count==1; PF/ZF/SF defined; AF undefined
    case M::SHL:case M::SHR:case M::SAR:
    case M::ROL:case M::ROR:
        return EF_CF|EF_OF|EF_PF|EF_ZF|EF_SF|EF_AF;
    case M::RCL:case M::RCR:
        return EF_CF|EF_OF;
    case M::SHLD:case M::SHRD:
        return EF_CF|EF_OF|EF_PF|EF_ZF|EF_SF|EF_AF;
    // Bit ops — write CF; OF/SF/AF/PF undefined
    case M::BT:case M::BTS:case M::BTR:case M::BTC:
        return EF_CF|EF_OF|EF_SF|EF_AF|EF_PF;
    case M::BSF:case M::BSR:
        return EF_ZF|EF_CF|EF_OF|EF_SF|EF_AF|EF_PF;
    case M::TZCNT:case M::LZCNT:
        return EF_CF|EF_ZF|EF_OF|EF_SF|EF_AF|EF_PF;
    case M::POPCNT:
        return EF_CF|EF_OF|EF_SF|EF_AF|EF_PF|EF_ZF;
    // Flag-control
    case M::CLC:case M::STC:case M::CMC:                       return EF_CF;
    case M::CLD:case M::STD:                                   return EF_DF;
    case M::CLI:case M::STI:                                   return EF_IF;
    case M::SAHF:                                              return EF_CF|EF_PF|EF_AF|EF_ZF|EF_SF;
    // BMI1 — write CF,ZF (SF/OF defined per spec for ANDN; rest undefined)
    case M::ANDN:
        return EF_ZF|EF_SF|EF_CF|EF_OF|EF_PF|EF_AF;
    case M::BLSI:case M::BLSMSK:case M::BLSR:case M::BEXTR:
        return EF_CF|EF_ZF|EF_SF|EF_OF|EF_PF|EF_AF;
    // BMI2 — most don't touch flags; BZHI does
    case M::BZHI:
        return EF_CF|EF_ZF|EF_SF|EF_OF|EF_PF|EF_AF;
    // ADX
    case M::ADCX:                                              return EF_CF;
    case M::ADOX:                                              return EF_OF;
    // RDRAND/RDSEED — write CF,ZF; clear OF/SF/AF/PF
    case M::RDRAND:case M::RDSEED:
        return EF_CF|EF_ZF|EF_OF|EF_SF|EF_AF|EF_PF;
    default: return 0;
    }
}

/// EFLAGS bits this mnemonic reads before executing.
inline EFlagMask reads_eflags(Mnemonic m) {
    using M = Mnemonic;
    if (is_jcc(m)) {
        // condition decode
        switch (m) {
        case M::JO:case M::JNO:                                return EF_OF;
        case M::JB:case M::JC:case M::JNAE:
        case M::JNB:case M::JNC:case M::JAE:                   return EF_CF;
        case M::JZ:case M::JE:case M::JNZ:case M::JNE:         return EF_ZF;
        case M::JBE:case M::JNA:case M::JNBE:case M::JA:       return EF_CF|EF_ZF;
        case M::JS:case M::JNS:                                return EF_SF;
        case M::JP:case M::JPE:case M::JNP:case M::JPO:        return EF_PF;
        case M::JL:case M::JNGE:case M::JNL:case M::JGE:       return EF_SF|EF_OF;
        case M::JLE:case M::JNG:case M::JNLE:case M::JG:       return EF_ZF|EF_SF|EF_OF;
        default: break;
        }
    }
    if (is_cmov(m)) {
        CondCode c = cmov_condition(m);
        switch (c) {
        case CondCode::O:case CondCode::NO:                    return EF_OF;
        case CondCode::B:case CondCode::NB:                    return EF_CF;
        case CondCode::Z:case CondCode::NZ:                    return EF_ZF;
        case CondCode::BE:case CondCode::NBE:                  return EF_CF|EF_ZF;
        case CondCode::S:case CondCode::NS:                    return EF_SF;
        case CondCode::P:case CondCode::NP:                    return EF_PF;
        case CondCode::L:case CondCode::NL:                    return EF_SF|EF_OF;
        case CondCode::LE:case CondCode::NLE:                  return EF_ZF|EF_SF|EF_OF;
        }
    }
    switch (m) {
    case M::ADC:case M::SBB:                                   return EF_CF;
    case M::ADCX:                                              return EF_CF;
    case M::ADOX:                                              return EF_OF;
    case M::RCL:case M::RCR:                                   return EF_CF;
    case M::PUSHF:case M::PUSHFD:case M::PUSHFQ:               return 0xFF;
    case M::LAHF:                                              return EF_CF|EF_PF|EF_AF|EF_ZF|EF_SF;
    case M::CMPSB:case M::CMPSW:case M::CMPSD:case M::CMPSQ:
    case M::SCASB:case M::SCASW:case M::SCASD:case M::SCASQ:
    case M::MOVSB:case M::MOVSW:case M::MOVSD:case M::MOVSQ:
    case M::STOSB:case M::STOSW:case M::STOSD:case M::STOSQ:
    case M::LODSB:case M::LODSW:case M::LODSD:case M::LODSQ:
        return EF_DF;
    default: return 0;
    }
}


// ============================================================
//  Fast decode helpers
// ============================================================

/// Decode just the length of a single instruction (0 on failure).
/// Cheaper than the full decode if you only want to skip past it.
inline size_t decode_instruction_length(const uint8_t* code, size_t n) {
    DecodedInstr di;
    return decode(code, n, di);
}


// ============================================================
//  In-place byte patchers (no allocation, no decode unless noted)
// ============================================================

/// Single-byte 0x90 NOP.
constexpr uint8_t nop_byte = 0x90;
/// Single-byte 0xCC INT3.
constexpr uint8_t int3_byte = 0xCC;

/// Fill `n` bytes at `p` with 0x90 (NOP). Caller is responsible for ensuring
/// no instruction crosses the patched region.
inline void fill_nop(uint8_t* p, size_t n) { std::memset(p, nop_byte, n); }

/// Fill `n` bytes at `p` with 0xCC (INT3 / debugger trap).
inline void fill_int3(uint8_t* p, size_t n) { std::memset(p, int3_byte, n); }

/// Patch a 5-byte rel32 JMP at `p` (E9 disp32). Does NOT verify the original
/// opcode — caller must already know the slot is a 5-byte JMP/CALL placeholder.
inline void patch_jmp_rel32(uint8_t* p, int32_t disp) {
    p[0] = 0xE9;
    std::memcpy(p + 1, &disp, 4);
}

/// Patch a 6-byte rel32 Jcc at `p` (0F 8x disp32).
inline void patch_jcc_rel32(uint8_t* p, CondCode cc, int32_t disp) {
    p[0] = 0x0F;
    p[1] = static_cast<uint8_t>(0x80u | static_cast<uint8_t>(cc));
    std::memcpy(p + 2, &disp, 4);
}

/// Patch a 2-byte rel8 JMP at `p` (EB disp8).
inline void patch_jmp_rel8(uint8_t* p, int8_t disp) {
    p[0] = 0xEB;
    p[1] = static_cast<uint8_t>(disp);
}

/// Patch a 5-byte rel32 CALL at `p` (E8 disp32).
inline void patch_call_rel32(uint8_t* p, int32_t disp) {
    p[0] = 0xE8;
    std::memcpy(p + 1, &disp, 4);
}

/// Patch a 10-byte MOV reg64, imm64 at `p` (REX.W + B8+r imm64).
inline void patch_mov_imm64(uint8_t* p, uint8_t reg_id, uint64_t imm) {
    uint8_t rex = 0x48 | ((reg_id >> 3) & 1); // REX.W + REX.B
    p[0] = rex;
    p[1] = static_cast<uint8_t>(0xB8 + (reg_id & 7));
    std::memcpy(p + 2, &imm, 8);
}

/// Read the rel32 displacement from a 5-byte rel32 JMP/CALL or 6-byte rel32 Jcc.
/// `m` is used to pick the offset of the disp field (E9/E8 → +1, 0F 8x → +2).
inline int32_t read_rel32_disp(const uint8_t* p, Mnemonic m) {
    int32_t d = 0;
    size_t off = is_jcc(m) ? 2 : 1;
    std::memcpy(&d, p + off, 4);
    return d;
}


// ============================================================
//  Common-prologue / epilogue pattern matchers
//
//  Each matcher decodes a single instruction at `p` (with `n` bytes
//  available) and returns true iff the decoded form matches the pattern.
// ============================================================

namespace detail {
inline bool _decode_one(const uint8_t* p, size_t n, DecodedInstr& di) {
    return decode(p, n, di) > 0 && di.desc;
}
inline bool _is_rsp(const DecodedInstr& di, uint8_t expected_op_idx) {
    if (di.desc->num_operands <= expected_op_idx) return false;
    const auto& op = di.desc->operands[expected_op_idx];
    if (op.addr != AddrMode::ModRM_RM && op.addr != AddrMode::ModRM_Reg) return false;
    // RSP encoding = 4 in modrm.rm/reg with mod=3 and no REX.B/R.
    uint8_t mod = di.modrm >> 6;
    uint8_t rm = di.modrm & 0x7;
    uint8_t reg = (di.modrm >> 3) & 0x7;
    if (op.addr == AddrMode::ModRM_RM)  return mod == 3 && rm == 4 && (di.rex & 0x1) == 0;
    if (op.addr == AddrMode::ModRM_Reg) return reg == 4 && (di.rex & 0x4) == 0;
    return false;
}
inline bool _is_rbp_rm(const DecodedInstr& di) {
    if (di.desc->num_operands < 1) return false;
    const auto& op = di.desc->operands[0];
    if (op.addr != AddrMode::ModRM_RM) return false;
    uint8_t mod = di.modrm >> 6;
    uint8_t rm = di.modrm & 0x7;
    return mod == 3 && rm == 5 && (di.rex & 0x1) == 0;
}
}

/// True if `p` decodes as `SUB rsp, imm` (8 or 32 bit imm). On match,
/// `*out_imm` (if non-null) receives the immediate.
inline bool is_sub_rsp_imm(const uint8_t* p, size_t n, int64_t* out_imm = nullptr) {
    DecodedInstr di;
    if (!detail::_decode_one(p, n, di)) return false;
    if (di.desc->mnemonic != Mnemonic::SUB) return false;
    if (!detail::_is_rsp(di, 0)) return false;
    if (di.desc->num_operands < 2 || di.desc->operands[1].addr != AddrMode::Immediate) return false;
    if (out_imm) *out_imm = di.immediate;
    return true;
}

/// True if `p` decodes as `ADD rsp, imm`.
inline bool is_add_rsp_imm(const uint8_t* p, size_t n, int64_t* out_imm = nullptr) {
    DecodedInstr di;
    if (!detail::_decode_one(p, n, di)) return false;
    if (di.desc->mnemonic != Mnemonic::ADD) return false;
    if (!detail::_is_rsp(di, 0)) return false;
    if (di.desc->num_operands < 2 || di.desc->operands[1].addr != AddrMode::Immediate) return false;
    if (out_imm) *out_imm = di.immediate;
    return true;
}

/// True if `p` decodes as `MOV rbp, rsp`.
inline bool is_mov_rbp_rsp(const uint8_t* p, size_t n) {
    DecodedInstr di;
    if (!detail::_decode_one(p, n, di)) return false;
    if (di.desc->mnemonic != Mnemonic::MOV) return false;
    if (!detail::_is_rbp_rm(di)) return false;
    uint8_t reg = (di.modrm >> 3) & 0x7;
    return reg == 4 && (di.rex & 0x4) == 0;
}

/// True if `p` decodes as `LEA rbp, [rsp + disp]`. On match, `*out_disp` receives the displacement.
inline bool is_lea_rbp_disp(const uint8_t* p, size_t n, int64_t* out_disp = nullptr) {
    DecodedInstr di;
    if (!detail::_decode_one(p, n, di)) return false;
    if (di.desc->mnemonic != Mnemonic::LEA) return false;
    if (di.desc->num_operands < 2) return false;
    uint8_t reg = (di.modrm >> 3) & 0x7;
    if (reg != 5 || (di.rex & 0x4) != 0) return false; // RBP destination
    if (out_disp) *out_disp = di.displacement;
    return true;
}

/// True if `p` decodes as `MOV [rbp + disp], reg` (slot save). On match,
/// `*out_reg` (0..15) gets the register, `*out_disp` gets the displacement.
inline bool is_mov_rbp_slot_save(const uint8_t* p, size_t n,
                                 uint8_t* out_reg = nullptr, int64_t* out_disp = nullptr) {
    DecodedInstr di;
    if (!detail::_decode_one(p, n, di)) return false;
    if (di.desc->mnemonic != Mnemonic::MOV) return false;
    if (di.desc->num_operands < 2) return false;
    if (di.desc->operands[0].addr != AddrMode::ModRM_RM) return false;
    if (di.desc->operands[1].addr != AddrMode::ModRM_Reg) return false;
    uint8_t mod = di.modrm >> 6;
    uint8_t rm  = di.modrm & 0x7;
    if (rm != 5 || mod == 3) return false;
    uint8_t reg = ((di.modrm >> 3) & 0x7) | (((di.rex >> 2) & 1) << 3);
    if (out_reg) *out_reg = reg;
    if (out_disp) *out_disp = di.displacement;
    return true;
}

/// True if `p` decodes as `MOV reg, [rbp + disp]` (slot load).
inline bool is_mov_rbp_slot_load(const uint8_t* p, size_t n,
                                 uint8_t* out_reg = nullptr, int64_t* out_disp = nullptr) {
    DecodedInstr di;
    if (!detail::_decode_one(p, n, di)) return false;
    if (di.desc->mnemonic != Mnemonic::MOV) return false;
    if (di.desc->num_operands < 2) return false;
    if (di.desc->operands[0].addr != AddrMode::ModRM_Reg) return false;
    if (di.desc->operands[1].addr != AddrMode::ModRM_RM) return false;
    uint8_t mod = di.modrm >> 6;
    uint8_t rm  = di.modrm & 0x7;
    if (rm != 5 || mod == 3) return false;
    uint8_t reg = ((di.modrm >> 3) & 0x7) | (((di.rex >> 2) & 1) << 3);
    if (out_reg) *out_reg = reg;
    if (out_disp) *out_disp = di.displacement;
    return true;
}

/// Skip a typical Win64 prologue: any combination of PUSH r64, MOV rbp,rsp,
/// SUB rsp,imm, MOV [rsp+imm],reg. Returns the number of bytes skipped
/// (0 if no prologue recognized). Stops at the first non-prologue instruction.
inline size_t skip_prologue(const uint8_t* p, size_t n) {
    size_t off = 0;
    while (off < n) {
        DecodedInstr di;
        size_t len = decode(p + off, n - off, di);
        if (len == 0 || !di.desc) break;
        Mnemonic m = di.desc->mnemonic;
        bool is_push_r64 = (m == Mnemonic::PUSH);
        bool is_sub_rsp = (m == Mnemonic::SUB && detail::_is_rsp(di, 0));
        bool is_mov_rbp_rsp_ = (m == Mnemonic::MOV && detail::_is_rbp_rm(di));
        bool is_mov_rsp_save = false;
        if (m == Mnemonic::MOV && di.desc->num_operands >= 2 &&
            di.desc->operands[0].addr == AddrMode::ModRM_RM &&
            di.desc->operands[1].addr == AddrMode::ModRM_Reg) {
            uint8_t mod = di.modrm >> 6, rm = di.modrm & 7;
            if (mod != 3 && rm == 4) is_mov_rsp_save = true; // SIB-based [rsp+...]
        }
        if (!(is_push_r64 || is_sub_rsp || is_mov_rbp_rsp_ || is_mov_rsp_save)) break;
        off += len;
    }
    return off;
}

/// Walk backward from `end` to find the start of an epilogue: ADD rsp,imm /
/// LEAVE / POP r64 / RETN. Best-effort — we decode forward up to `end` and
/// pick the earliest position whose tail-suffix is all epilogue.
/// Returns the offset *into the buffer (0..end)* where the epilogue begins.
/// `end` is an exclusive end position.
inline size_t skip_epilogue_before(const uint8_t* p, size_t end) {
    // Scan forward; track the offset of the first "always-epilogue" instr that
    // is followed only by other epilogue instrs.
    size_t off = 0;
    size_t epilogue_start = end;
    while (off < end) {
        DecodedInstr di;
        size_t len = decode(p + off, end - off, di);
        if (len == 0 || !di.desc) { epilogue_start = end; break; }
        Mnemonic m = di.desc->mnemonic;
        bool is_pop = (m == Mnemonic::POP);
        bool is_leave = (m == Mnemonic::LEAVE);
        bool is_add_rsp_ = (m == Mnemonic::ADD && detail::_is_rsp(di, 0));
        bool is_ret = (m == Mnemonic::RETN || m == Mnemonic::RETF);
        if (is_pop || is_leave || is_add_rsp_ || is_ret) {
            if (epilogue_start == end) epilogue_start = off;
        } else {
            epilogue_start = end;
        }
        off += len;
    }
    return epilogue_start;
}

/// Try to recognize the `PUSH imm32 ; CALL rel32` IAT-stub pattern.
/// `p` must cover at least the two instructions.  On match, `*out_imm` gets
/// the pushed value and `*out_target` gets the absolute call target,
/// computed from `va_after_push + rel32` where `va_after_push` is the
/// runtime address that `p` will live at, plus 5 (size of the push).
inline bool match_push_imm_call(const uint8_t* p, size_t n, uint64_t va_at_push,
                                int64_t* out_imm, uint64_t* out_target) {
    auto has_op = [](const DecodedInstr& di, AddrMode a) {
        if (!di.desc) return false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == a) return true;
        return false;
    };
    DecodedInstr a, b;
    size_t la = decode(p, n, a);
    if (la == 0 || !a.desc || a.desc->mnemonic != Mnemonic::PUSH ||
        !has_op(a, AddrMode::Immediate))
        return false;
    size_t lb = decode(p + la, n - la, b);
    if (lb == 0 || !b.desc || b.desc->mnemonic != Mnemonic::CALL ||
        !has_op(b, AddrMode::RelOffset))
        return false;
    if (out_imm)    *out_imm = a.immediate;
    if (out_target) *out_target = va_at_push + la + lb + static_cast<uint64_t>(b.immediate);
    return true;
}


// ============================================================
//  Operand introspection (works on Instruction)
// ============================================================

inline bool is_reg_op(const Instruction& i, size_t idx) {
    return idx < i.operands.size() && i.operands[idx].is_register();
}
inline bool is_imm_op(const Instruction& i, size_t idx) {
    return idx < i.operands.size() && i.operands[idx].is_immediate();
}
inline bool is_mem_op(const Instruction& i, size_t idx) {
    return idx < i.operands.size() && i.operands[idx].is_memory();
}
inline bool is_rel_op(const Instruction& i, size_t idx) {
    return idx < i.operands.size() && i.operands[idx].is_relative();
}

/// Read register id (0..15), 0xFF if not a register operand.
inline uint8_t reg_op_id(const Instruction& i, size_t idx) {
    return is_reg_op(i, idx) ? i.operands[idx].reg.reg_id : 0xFF;
}
/// Read the immediate value, 0 if not an immediate operand.
inline int64_t imm_value(const Instruction& i, size_t idx) {
    return is_imm_op(i, idx) ? i.operands[idx].imm.value : 0;
}
/// Read the base register id of a memory operand (0xFF if none).
inline uint8_t mem_base(const Instruction& i, size_t idx) {
    return is_mem_op(i, idx) ? i.operands[idx].mem.base_reg : 0xFF;
}
/// Read the index register id of a memory operand (0xFF if none).
inline uint8_t mem_index(const Instruction& i, size_t idx) {
    return is_mem_op(i, idx) ? i.operands[idx].mem.index_reg : 0xFF;
}
/// Read the displacement of a memory operand (0 if not a memory operand).
inline int64_t mem_disp(const Instruction& i, size_t idx) {
    return is_mem_op(i, idx) ? i.operands[idx].mem.disp : 0;
}

/// Conservative "does this instruction modify GPR `reg_id` (0..15)?" query.
/// Looks only at the explicit destination operand (operands[0]) for register-
/// modifying mnemonics. Returns false for instructions that only write memory.
inline bool modifies_reg(const Instruction& i, uint8_t reg_id) {
    if (i.operands.empty()) return false;
    const auto& dst = i.operands[0];
    if (!dst.is_register()) return false;
    Mnemonic m = i.mnemonic;
    // Conservative: any mnemonic whose dst is a register and is not a pure
    // compare/test counts as a write.
    if (m == Mnemonic::CMP || m == Mnemonic::TEST) return false;
    return dst.reg.reg_id == reg_id;
}

/// Conservative "does this instruction read GPR `reg_id` (0..15)?" query.
/// Considers any source register operand and the base/index of any memory
/// operand. Read-modify-write destinations (ADD, SUB, …) also count as reads.
inline bool reads_reg(const Instruction& i, uint8_t reg_id) {
    Mnemonic m = i.mnemonic;
    bool dst_is_rmw = !(m == Mnemonic::MOV || m == Mnemonic::LEA ||
                        m == Mnemonic::MOVSX || m == Mnemonic::MOVSXD ||
                        m == Mnemonic::MOVZX || m == Mnemonic::POP);
    for (size_t k = 0; k < i.operands.size(); ++k) {
        const auto& op = i.operands[k];
        if (op.is_register()) {
            if (k == 0 && !dst_is_rmw) continue;
            if (op.reg.reg_id == reg_id) return true;
        } else if (op.is_memory()) {
            if (op.mem.base_reg == reg_id || op.mem.index_reg == reg_id) return true;
        }
    }
    return false;
}


// ============================================================
//  Concrete execution helpers (emu integration is via callbacks
//  supplied by the user; we don't take a hard dependency here).
//
//  The user passes a `step` callback that runs one instruction at the
//  current `rip` and updates the CPU + memory accordingly.
// ============================================================

/// Page-granularity sparse memory. Allocates 4 KiB pages on demand and
/// caches them in a hash map. Cheap when the working set is sparse;
/// avoids the multi-GB flat allocations that a typical x86-64 emulator
/// would otherwise need.
class DemandPagedMemory {
public:
    static constexpr size_t kPageBits = 12;
    static constexpr size_t kPageSize = size_t{1} << kPageBits;
    static constexpr uint64_t kPageMask = kPageSize - 1;

    /// True if `addr..addr+n` is fully backed by previously-touched pages.
    bool is_committed(uint64_t addr, size_t n) const {
        uint64_t first = addr >> kPageBits;
        uint64_t last  = (addr + n - 1) >> kPageBits;
        for (uint64_t p = first; p <= last; ++p) {
            if (pages_.find(p) == pages_.end()) return false;
        }
        return true;
    }

    /// Read `n` bytes from `addr` into `dst`. Pages that have never been
    /// written are read as 0xCC (so unmapped reads stand out in dumps).
    void read(uint64_t addr, void* dst, size_t n) {
        auto* d = static_cast<uint8_t*>(dst);
        for (size_t i = 0; i < n; ) {
            uint64_t pgn = (addr + i) >> kPageBits;
            size_t   off = static_cast<size_t>((addr + i) & kPageMask);
            size_t   chunk = kPageSize - off;
            if (chunk > n - i) chunk = n - i;
            auto it = pages_.find(pgn);
            if (it == pages_.end()) std::memset(d + i, 0xCC, chunk);
            else                    std::memcpy(d + i, it->second.data() + off, chunk);
            i += chunk;
        }
    }

    /// Write `n` bytes from `src` into `addr`. Pages are allocated on first
    /// touch and zero-initialized.
    void write(uint64_t addr, const void* src, size_t n) {
        const auto* s = static_cast<const uint8_t*>(src);
        for (size_t i = 0; i < n; ) {
            uint64_t pgn = (addr + i) >> kPageBits;
            size_t   off = static_cast<size_t>((addr + i) & kPageMask);
            size_t   chunk = kPageSize - off;
            if (chunk > n - i) chunk = n - i;
            auto& page = pages_[pgn];
            if (page.empty()) page.assign(kPageSize, 0);
            std::memcpy(page.data() + off, s + i, chunk);
            i += chunk;
        }
    }

    /// Convenience accessors.
    uint8_t  read_u8 (uint64_t a) { uint8_t  v; read(a, &v, 1); return v; }
    uint16_t read_u16(uint64_t a) { uint16_t v; read(a, &v, 2); return v; }
    uint32_t read_u32(uint64_t a) { uint32_t v; read(a, &v, 4); return v; }
    uint64_t read_u64(uint64_t a) { uint64_t v; read(a, &v, 8); return v; }
    void write_u8 (uint64_t a, uint8_t  v) { write(a, &v, 1); }
    void write_u16(uint64_t a, uint16_t v) { write(a, &v, 2); }
    void write_u32(uint64_t a, uint32_t v) { write(a, &v, 4); }
    void write_u64(uint64_t a, uint64_t v) { write(a, &v, 8); }

    size_t page_count() const { return pages_.size(); }
    void clear() { pages_.clear(); }

private:
    std::unordered_map<uint64_t, std::vector<uint8_t>> pages_;
};

// ============================================================
//  DecodedInstr operand introspection (no allocation)
// ============================================================

/// True if operand `k` of `di` carries a register encoding (ModRM.reg/rm
/// reg-direct, opcode register, or a fixed register).
inline bool is_reg_op(const DecodedInstr& di, size_t k) {
    if (!di.desc || k >= di.desc->num_operands) return false;
    AddrMode a = di.desc->operands[k].addr;
    if (a == AddrMode::ModRM_Reg) return true;
    if (a == AddrMode::ModRM_RM)  return (di.modrm >> 6) == 3;
    if (a == AddrMode::OpcodeReg) return true;
    if (a == AddrMode::RegOnly)   return true;
    if (a == AddrMode::Fixed)     return true;
    return false;
}

/// Resolve the GPR id (0..15) for operand `k` of `di`. Returns 0xFF if the
/// operand is not a register.
inline uint8_t reg_op_id(const DecodedInstr& di, size_t k) {
    if (!di.desc || k >= di.desc->num_operands) return 0xFF;
    AddrMode a = di.desc->operands[k].addr;
    auto rex_bit = [&](uint8_t mask) { return (di.rex & mask) ? 8u : 0u; };
    switch (a) {
    case AddrMode::ModRM_Reg: return ((di.modrm >> 3) & 7) | rex_bit(0x4);
    case AddrMode::ModRM_RM:
        if ((di.modrm >> 6) != 3) return 0xFF;
        return (di.modrm & 7) | rex_bit(0x1);
    case AddrMode::OpcodeReg: return di.opcode_reg;
    case AddrMode::Fixed:     return di.desc->operands[k].fixed_reg;
    default: return 0xFF;
    }
}

/// True if operand `k` is an immediate.
inline bool is_imm_op(const DecodedInstr& di, size_t k) {
    if (!di.desc || k >= di.desc->num_operands) return false;
    return di.desc->operands[k].addr == AddrMode::Immediate;
}

/// Read the immediate value (di.immediate). Returns 0 if no operand is an immediate.
inline int64_t imm_value(const DecodedInstr& di) {
    if (!di.desc) return 0;
    for (uint8_t k = 0; k < di.desc->num_operands; ++k) {
        if (di.desc->operands[k].addr == AddrMode::Immediate) return di.immediate;
    }
    return 0;
}

/// True if operand `k` references a memory location (ModRM.RM with mod!=3,
/// or M/X/Y addressing).
inline bool is_mem_op(const DecodedInstr& di, size_t k) {
    if (!di.desc || k >= di.desc->num_operands) return false;
    AddrMode a = di.desc->operands[k].addr;
    if (a == AddrMode::ModRM_RM)  return (di.modrm >> 6) != 3;
    if (a == AddrMode::MemOnly || a == AddrMode::Moffset ||
        a == AddrMode::StringSrc || a == AddrMode::StringDst)
        return true;
    return false;
}

/// Memory-operand base register id (0..15, 0xFF if none / not a memory op).
/// Decodes ModRM+SIB the way the encoder does: SIB.base==5 with mod==0 means
/// no base (RIP-relative or pure disp).
inline uint8_t mem_base(const DecodedInstr& di) {
    if (!di.desc) return 0xFF;
    if ((di.modrm >> 6) == 3) return 0xFF;
    uint8_t rm = di.modrm & 7;
    auto rex_b = (di.rex & 0x1) ? 8u : 0u;
    if (rm != 4) {
        // No SIB; base is rm itself, but mod==0 && rm==5 is RIP-relative.
        if ((di.modrm >> 6) == 0 && rm == 5) return 0xFF;
        return rm | rex_b;
    }
    // SIB present.
    uint8_t base = di.sib & 7;
    if ((di.modrm >> 6) == 0 && base == 5) return 0xFF;
    return base | rex_b;
}

/// Memory-operand index register id (0..15, 0xFF if none).
inline uint8_t mem_index(const DecodedInstr& di) {
    if (!di.desc) return 0xFF;
    if ((di.modrm >> 6) == 3) return 0xFF;
    uint8_t rm = di.modrm & 7;
    if (rm != 4) return 0xFF; // no SIB → no scaled index
    uint8_t idx = (di.sib >> 3) & 7;
    if (idx == 4 && (di.rex & 0x2) == 0) return 0xFF; // no index encoding
    return idx | ((di.rex & 0x2) ? 8u : 0u);
}

/// Memory-operand displacement (returns di.displacement; 0 for non-memory).
inline int64_t mem_disp(const DecodedInstr& di) {
    return di.displacement;
}

// ============================================================

/// Trace a register's source by walking backward through a list of decoded
/// `Instruction`s until either an immediate definition or a non-trivial
/// instruction is hit. Recognized "forwarders" are MOV reg,reg / MOVZX /
/// MOVSXD / XCHG (treated as a swap).
///
/// Returns the index of the first instruction `j <= i` that defines
/// `reg_id` with something other than a register-to-register move
/// (or `i + 1` if no such defining instruction is found within the window).
/// On return, `*final_reg_id` (if non-null) holds the original source register
/// (which differs from `reg_id` if the chain went through MOV/XCHG).
inline size_t backward_chain(const std::vector<Instruction>& insns, size_t i,
                             uint8_t reg_id, uint8_t* final_reg_id = nullptr) {
    uint8_t cur = reg_id;
    for (size_t k = i + 1; k-- > 0; ) {
        const Instruction& ins = insns[k];
        if (!modifies_reg(ins, cur)) continue;
        Mnemonic m = ins.mnemonic;
        bool is_forwarder = (m == Mnemonic::MOV || m == Mnemonic::MOVZX ||
                             m == Mnemonic::MOVSX || m == Mnemonic::MOVSXD ||
                             m == Mnemonic::XCHG);
        if (!is_forwarder || ins.operands.size() < 2 || !ins.operands[1].is_register()) {
            if (final_reg_id) *final_reg_id = cur;
            return k;
        }
        // Forward through the source register.
        cur = ins.operands[1].reg.reg_id;
    }
    if (final_reg_id) *final_reg_id = cur;
    return i + 1;
}

/// Apply a sparse {reg_id → value} mapping to a 16-element GPR array.
/// Useful for caller-state-aware concrete execution (e.g. setting RCX/RDX
/// for known IAT-stub arguments).
inline void apply_sparse_gprs(uint64_t (&gpr)[16],
                              const std::unordered_map<uint8_t, uint64_t>& seed) {
    for (auto& kv : seed) {
        if (kv.first < 16) gpr[kv.first] = kv.second;
    }
}

} // namespace vedx64::analysis
