// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/relocation.hpp"
#include <cstring>

namespace vedx64 {

bool is_rip_relative(const uint8_t* code, size_t len) {
    DecodedInstr di;
    size_t n = decode(code, len, di);
    if (n == 0 || !di.desc || !di.desc->has_modrm) return false;
    uint8_t mod_ = (di.modrm >> 6) & 3;
    uint8_t rm = di.modrm & 7;
    return mod_ == 0 && rm == 5;
}

bool is_pc_relative(const uint8_t* code, size_t len) {
    if (is_rip_relative(code, len)) return true;
    DecodedInstr di;
    size_t n = decode(code, len, di);
    if (n == 0 || !di.desc) return false;
    for (uint8_t i = 0; i < di.desc->num_operands; ++i)
        if (di.desc->operands[i].addr == AddrMode::RelOffset)
            return true;
    return false;
}

bool can_relocate(const uint8_t* code, size_t len) {
    DecodedInstr di;
    size_t n = decode(code, len, di);
    if (n == 0) return false;
    if (di.desc) {
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset &&
                di.desc->operands[i].size == OpSize::ByteSignExt)
                return false; // rel8 cannot be relocated in-place
        }
    }
    return true;
}

bool relocate_block(
    uint8_t* dest, size_t byte_count,
    uintptr_t src_addr, size_t* out_count) {
    size_t off = 0;
    size_t count = 0;
    while (off < byte_count) {
        DecodedInstr di;
        size_t n = decode(dest + off, byte_count - off, di);
        if (n == 0) return false;
        count++;

        if (di.desc && di.desc->has_modrm) {
            size_t modrm_off = off + di.num_prefixes + (di.rex ? 1 : 0);
            if (di.desc->opcode > 0xFF) modrm_off += 2; else modrm_off += 1;
            if ((di.desc->opcode == 0x0F38 || di.desc->opcode == 0x0F3A) && di.desc->sec_opcd != 0xFF) modrm_off += 1;
            if (modrm_off < off + n) {
                uint8_t mb = dest[modrm_off];
                if ((mb >> 6) == 0 && (mb & 7) == 5) {
                    size_t disp_off = modrm_off + 1;
                    int32_t old_disp;
                    memcpy(&old_disp, dest + disp_off, 4);
                    int64_t abs_target = (int64_t)(src_addr + off + n) + old_disp;
                    int32_t new_disp = (int32_t)(abs_target - (int64_t)((uintptr_t)(dest + off + n)));
                    memcpy(dest + disp_off, &new_disp, 4);
                }
            }
        }

        if (di.desc) {
            for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
                if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                    if (di.desc->operands[i].size == OpSize::ByteSignExt) break;
                    size_t imm_off = off + n - 4;
                    int32_t old_rel;
                    memcpy(&old_rel, dest + imm_off, 4);
                    int64_t abs_target = (int64_t)(src_addr + off + n) + old_rel;
                    int32_t new_rel = (int32_t)(abs_target - (int64_t)((uintptr_t)(dest + off + n)));
                    memcpy(dest + imm_off, &new_rel, 4);
                    break;
                }
            }
        }
        off += n;
    }
    if (out_count) *out_count = count;
    return true;
}

bool relocate_instruction(
    const uint8_t* insn, size_t insn_len,
    uintptr_t old_addr, uintptr_t new_addr,
    uint8_t* out_buf, size_t* out_len) {
    (void)new_addr;
    memcpy(out_buf, insn, insn_len);
    *out_len = insn_len;
    size_t dummy_count = 0;
    return relocate_block(out_buf, insn_len, old_addr, &dummy_count);
}

size_t calc_stolen_bytes(const uint8_t* code, size_t min_bytes, size_t max_stolen) {
    size_t total = 0;
    while (total < min_bytes) {
        DecodedInstr di;
        size_t n = decode(code + total, 15, di);
        if (n == 0) return 0;
        total += n;
        if (total > max_stolen) return 0;
    }
    return total;
}

} // namespace vedx64
