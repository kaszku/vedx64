// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include "vedx64/core.hpp"

namespace vedx64 {

/// Check if an instruction uses RIP-relative addressing.
bool is_rip_relative(const uint8_t* code, size_t len);

/// Check if an instruction uses PC-relative addressing (RIP-relative or relative branch/call).
bool is_pc_relative(const uint8_t* code, size_t len);

/// Check if a single instruction can be safely relocated.
/// Returns false for rel8 branches (need expansion) or decode failure.
bool can_relocate(const uint8_t* code, size_t len);

/// Relocate a single instruction from old_addr to new_addr.
/// Patches RIP-relative addressing and rel32 jumps/calls.
/// Returns true on success; out_buf receives the relocated bytes, out_len the byte count.
/// out_buf must be at least 15 bytes.
bool relocate_instruction(
    const uint8_t* insn, size_t insn_len,
    uintptr_t old_addr, uintptr_t new_addr,
    uint8_t* out_buf, size_t* out_len);

/// Relocate a sequence of instructions (stolen bytes) to a new address.
/// Returns true on success; out_count receives the number of instructions relocated.
bool relocate_block(
    uint8_t* dest, size_t byte_count,
    uintptr_t src_addr, size_t* out_count);

/// Calculate minimum number of bytes that must be stolen for a hook of min_bytes size.
/// Returns 0 if instructions cannot be decoded.
size_t calc_stolen_bytes(const uint8_t* code, size_t min_bytes, size_t max_stolen = 256);

/// Emit a conditional jump to an absolute target, with automatic fallback for out-of-range.
/// cc is the Jcc condition opcode byte for the 0F 8x form (e.g. 0x84 = JE, 0x85 = JNE).
/// If target fits in rel32 from src_addr: emits `0F 8x rel32` (6 bytes).
/// Otherwise: emits inverted short Jcc over an indirect JMP [rip+0] + qword target (16 bytes).
/// Returns bytes written into out_buf (must be >=16 bytes). Returns 0 on error.
size_t emit_long_jcc(uint8_t cc, uintptr_t src_addr, uintptr_t target,
                     uint8_t* out_buf, size_t out_cap);

/// Detect a jump-table dispatch pattern at `code` of length `len`.
/// Recognizes the common form: `LEA base, [rip+disp]; JMP [base+idx*8]` or `JMP [rip+disp+idx*scale]`.
/// On success, fills in `table_addr` (address of the jump table) and `entry_size` (4 or 8).
/// Returns true if pattern recognized.
bool detect_jump_table(const uint8_t* code, size_t len, uintptr_t addr,
                       uintptr_t* table_addr, uint8_t* entry_size);

} // namespace vedx64
