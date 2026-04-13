// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/relocation.hpp"
#include <cstring>
#include <climits>
#include "vedx64/mnemonic.hpp"

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

size_t emit_long_jcc(uint8_t cc, uintptr_t src_addr, uintptr_t target,
                     uint8_t* out_buf, size_t out_cap) {
    if (!out_buf || out_cap < 6) return 0;
    int64_t rel = (int64_t)target - (int64_t)(src_addr + 6);
    if (rel >= INT32_MIN && rel <= INT32_MAX) {
        out_buf[0] = 0x0F; out_buf[1] = cc;
        int32_t r32 = (int32_t)rel;
        memcpy(out_buf + 2, &r32, 4);
        return 6;
    }
    if (out_cap < 16) return 0;
    uint8_t inv = (uint8_t)(cc ^ 0x01);
    out_buf[0] = (uint8_t)(0x70 | (inv & 0x0F));
    out_buf[1] = 0x0E;
    out_buf[2] = 0xFF; out_buf[3] = 0x25;
    uint32_t disp = 0; memcpy(out_buf + 4, &disp, 4);
    uint64_t abs_tgt = (uint64_t)target; memcpy(out_buf + 8, &abs_tgt, 8);
    return 16;
}

bool detect_jump_table(const uint8_t* code, size_t len, uintptr_t addr,
                       uintptr_t* table_addr, uint8_t* entry_size) {
    if (!code || len == 0) return false;
    DecodedInstr di1; size_t n1 = decode(code, len, di1); if (n1 == 0) return false;
    if (di1.desc && di1.desc->mnemonic == Mnemonic::JMP && di1.desc->has_modrm) {
        uint8_t mb = di1.modrm;
        size_t modrm_off = di1.num_prefixes + (di1.rex ? 1 : 0) + (di1.desc->opcode > 0xFF ? 2 : 1);
        if ((mb >> 6) == 0 && (mb & 7) == 4 && modrm_off + 1 < n1) {
            uint8_t sib = code[modrm_off + 1];
            if (entry_size) *entry_size = (uint8_t)(1 << ((sib >> 6) & 3));
            if (table_addr) *table_addr = 0;
            return true;
        }
        if ((mb >> 6) == 0 && (mb & 7) == 5 && modrm_off + 5 <= n1) {
            int32_t d; memcpy(&d, code + modrm_off + 1, 4);
            if (table_addr) *table_addr = (uintptr_t)((int64_t)(addr + n1) + d);
            if (entry_size) *entry_size = 8;
            return true;
        }
    }
    return false;
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
