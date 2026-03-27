// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include "vedx64/mnemonic.hpp"
#include "vedx64/operand.hpp"
#include "vedx64/encoding_id.hpp"
#include "vedx64/core.hpp"

namespace vedx64 {

/// Legacy prefix bitfield packed into a single byte.
///
/// | Bits | Field       | Values                                              |
/// |------|-------------|----------------------------------------------------|
/// | 0    | LOCK        | 0/1                                                |
/// | 1    | REP/REPE    | 0/1                                                |
/// | 2    | REPNE       | 0/1                                                |
/// | 3    | data16 0x66 | 0/1                                                |
/// | 4    | addr32 0x67 | 0/1                                                |
/// | 5-7  | segment     | 0=none 1=ES 2=CS 3=SS 4=DS 5=FS 6=GS              |
enum InstrPrefix : uint8_t {
    PFX_NONE   = 0,
    PFX_LOCK   = 1 << 0,  ///< 0xF0
    PFX_REP    = 1 << 1,  ///< 0xF3 (REP/REPE/REPZ)
    PFX_REPNE  = 1 << 2,  ///< 0xF2 (REPNE/REPNZ)
    PFX_DATA16 = 1 << 3,  ///< 0x66 operand-size override
    PFX_ADDR32 = 1 << 4,  ///< 0x67 address-size override
    // Segment override in bits [7:5]
    PFX_SEG_ES = 1 << 5,  ///< ES (0x26)
    PFX_SEG_CS = 2 << 5,  ///< CS (0x2E)
    PFX_SEG_SS = 3 << 5,  ///< SS (0x36)
    PFX_SEG_DS = 4 << 5,  ///< DS (0x3E)
    PFX_SEG_FS = 5 << 5,  ///< FS (0x64)
    PFX_SEG_GS = 6 << 5,  ///< GS (0x65)
    PFX_SEG_MASK = 7 << 5
};

/// High-level instruction representation with typed operands.
struct Instruction {
    Mnemonic mnemonic = Mnemonic::INVALID;    ///< 2 bytes
    EncodingId encoding_id = EncodingId::UNKNOWN; ///< 2 bytes
    uint8_t length = 0;                       ///< Instruction length (1-15)
    uint8_t prefixes = PFX_NONE;              ///< Legacy prefix bitfield
    uint64_t address = 0;                     ///< Instruction address
    std::vector<Operand> operands;            ///< Typed operand list

    /// Build an Instruction from a low-level DecodedInstr.
    /// Populates mnemonic, operands, encoding_id, length, and address.
    static Instruction from_decoded(const DecodedInstr& di, const uint8_t* code, uint64_t addr = 0);

    /// Decode a single instruction from a byte stream.
    /// Returns the number of bytes consumed (0 on failure).
    /// On success, `out` is populated with full operand information.
    static size_t decode_instruction(const uint8_t* code, size_t len, Instruction& out, uint64_t rip = 0);

    size_t operand_count() const { return operands.size(); }

    /// Get the instruction descriptor from the table (nullptr if unknown).
    const InstrDesc* descriptor() const {
        auto idx = static_cast<uint16_t>(encoding_id);
        return (idx > 0 && idx <= g_instr_table_size) ? &g_instr_table[idx - 1] : nullptr;
    }

    // Prefix queries
    bool has_lock() const { return (prefixes & PFX_LOCK) != 0; }
    bool has_rep() const { return (prefixes & PFX_REP) != 0; }
    bool has_repne() const { return (prefixes & PFX_REPNE) != 0; }
    bool has_data16() const { return (prefixes & PFX_DATA16) != 0; }
    bool has_addr32() const { return (prefixes & PFX_ADDR32) != 0; }
    uint8_t segment() const { return (prefixes & PFX_SEG_MASK) >> 5; }
    bool has_segment() const { return segment() != 0; }

#ifdef VEDX64_STRINGS
    /// Format instruction as disassembly text.
    const char* mnemonic_str() const { return mnemonic_name(mnemonic); }
#endif
};

} // namespace vedx64
