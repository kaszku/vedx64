// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <string>
#include "vedx64/mnemonic.hpp"

namespace vedx64 {

enum class OpSize : uint8_t {
    None,
    Byte,
    Word,
    Dword,
    Qword,
    OpSz,
    OpSzQ,
    OpSzDS,
    ByteSignExt,
    PackedSingle,
    PackedDouble,
    ScalarSingle,
    ScalarDouble,
    Xmm128,
    PseudoDesc,
    BoundPair,
    FarPtr,
    Mmx64,
};

enum class AddrMode : uint8_t {
    None,
    Fixed,
    ModRM_RM,
    ModRM_Reg,
    Immediate,
    RelOffset,
    MemOnly,
    RegOnly,
    XmmReg,
    XmmRM,
    OpcodeReg,
    Moffset,
    StringSrc,
    StringDst,
    SegReg,
    CtrlReg,
    DbgReg,
    Flags,
    MmxReg,
    MmxRM,
    MmxRegOnly,
    XmmRegOnly,
    Stack,
    VexReg,
    XmmVvvv,
};

struct OperandDesc {
    AddrMode addr;
    OpSize size;
    uint8_t fixed_reg; // register number for Fixed addressing
};

struct InstrDesc {
    Mnemonic mnemonic;
    uint16_t opcode;      // full opcode (0x0F__ for two-byte)
    uint8_t opcd_ext;     // /r extension (0xFF if none)
    uint8_t prefix;       // mandatory prefix (0 if none)
    uint8_t sec_opcd;     // secondary opcode (0xFF if none)
    bool has_modrm;
    bool lock_valid;
    int8_t mod_filter;     // 0=any, 1=mem only (mod!=3), -1=reg only (mod==3)
    uint8_t num_operands;
    int8_t vex_w;          // -1=must be W0, 1=must be W1, 0=don't care
    OperandDesc operands[4];
};

// Instruction table
extern const InstrDesc g_instr_table[];
extern const size_t g_instr_table_size;

struct DecodedInstr {
    const InstrDesc* desc;
    uint8_t legacy_prefix[4]; // up to 4 legacy prefix bytes (0-terminated)
    uint8_t num_prefixes;
    uint8_t rex;          // REX byte (0 if none)
    uint8_t modrm;
    uint8_t sib;
    uint8_t opcode_reg;   // register from opcode low 3 bits (for +rb/+rw/+rd encodings)
    int64_t displacement;
    int64_t immediate;
    uint8_t length;
    // VEX prefix fields
    bool has_vex;         // true if VEX or EVEX-encoded instruction
    uint8_t vex_vvvv;     // additional register operand (inverted in encoding, stored normal here)
    uint8_t vex_L;        // vector length: 0=128, 1=256, 2=512 (EVEX)
    bool vex_W;           // like REX.W
    // REX2 prefix fields (APX)
    bool has_rex2;        // true if REX2-encoded (0xD5 prefix)
    uint8_t rex2_r4;      // 5th bit of modrm.reg (regs 16-31)
    uint8_t rex2_x4;      // 5th bit of SIB.index (regs 16-31)
    uint8_t rex2_b4;      // 5th bit of modrm.rm/SIB.base (regs 16-31)
    // EVEX-specific fields
    bool has_evex;        // true if EVEX-encoded (subset of has_vex)
    uint8_t evex_aaa;     // opmask register k0-k7 (3 bits)
    bool evex_z;          // zeroing-masking (merge=0, zero=1)
    bool evex_b;          // broadcast / SAE / rounding control
    uint8_t evex_rc;      // rounding control (2 bits, valid when evex_b && reg-reg)
};

// Decode a single instruction from the byte stream.
// Returns number of bytes consumed, or 0 on failure.
size_t decode(const uint8_t* code, size_t len, DecodedInstr& out);

// Encode an instruction into the output buffer.
// Returns number of bytes written, or 0 on failure.
size_t encode(const DecodedInstr& instr, uint8_t* out, size_t max_len);

#ifdef VEDX64_STRINGS
// Disassemble a single instruction into a text buffer.
// Returns number of bytes consumed, or 0 on failure.
size_t disassemble(const uint8_t* code, size_t len, char* buf, size_t buf_len, uint64_t rip);

/// Disassemble a single instruction, returning the text as std::string.
/// Returns empty string on failure.
inline std::string disassemble(const uint8_t* code, size_t len, uint64_t rip = 0) {
    char buf[256];
    size_t n = disassemble(code, len, buf, sizeof(buf), rip);
    return n > 0 ? std::string(buf) : std::string();
}

const char* mnemonic_name(Mnemonic m);
#endif // VEDX64_STRINGS

inline bool has_opcode_reg(const InstrDesc& d) {
    for (uint8_t i = 0; i < d.num_operands; ++i)
        if (d.operands[i].addr == AddrMode::OpcodeReg) return true;
    return false;
}

} // namespace vedx64
