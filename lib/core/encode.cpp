// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/core.hpp"
#include <cstring>

namespace vedx64 {

size_t encode(const DecodedInstr& instr, uint8_t* out, size_t max_len) {
    if (!instr.desc || !out || max_len == 0) return 0;
    size_t pos = 0;

    auto emit = [&](uint8_t b) -> bool {
        if (pos >= max_len) return false;
        out[pos++] = b;
        return true;
    };

    if (instr.has_vex) {
        // VEX-encoded instruction
        // Emit non-VEX legacy prefixes (LOCK, segment override) but NOT 66/F2/F3
        for (uint8_t i = 0; i < instr.num_prefixes; ++i) {
            uint8_t p = instr.legacy_prefix[i];
            if (p != 0x66 && p != 0xF2 && p != 0xF3)
                if (!emit(p)) return 0;
        }

        uint8_t map = 1; // default: 0F map
        if (instr.desc->opcode == 0x0F38) map = 2;
        else if (instr.desc->opcode == 0x0F3A) map = 3;

        uint8_t pp = 0;
        if (instr.desc->prefix == 0x66) pp = 1;
        else if (instr.desc->prefix == 0xF3) pp = 2;
        else if (instr.desc->prefix == 0xF2) pp = 3;

        bool R = (instr.rex & 0x04) != 0;
        bool X = (instr.rex & 0x02) != 0;
        bool B = (instr.rex & 0x01) != 0;

        if (instr.has_evex) {
            // EVEX: 62 [P0] [P1] [P2] opcode
            // P0: ~R(7) ~X(6) ~B(5) ~R'(4) 0(3) mm(2:0)
            uint8_t p0 = (R ? 0 : 0x80) | (X ? 0 : 0x40) | (B ? 0 : 0x20) | 0x10 | (map & 0x07);
            // P1: W(7) ~vvvv(6:3) 1(2) pp(1:0)
            uint8_t p1 = (instr.vex_W ? 0x80 : 0) | ((~instr.vex_vvvv & 0xF) << 3) | 0x04 | pp;
            // P2: z(7) L'L(6:5) b(4) ~V'(3) aaa(2:0)
            uint8_t p2 = (instr.evex_z ? 0x80 : 0) | ((instr.vex_L & 3) << 5) | (instr.evex_b ? 0x10 : 0) | 0x08 | (instr.evex_aaa & 7);
            if (instr.vex_vvvv & 0x10) p2 &= ~0x08; // V' inverted: vvvv bit4=1 → ~V'=0
            if (!emit(0x62) || !emit(p0) || !emit(p1) || !emit(p2)) return 0;
        } else if (map == 1 && !X && !B && !instr.vex_W) {
            // 2-byte VEX: C5 [R.vvvv.L.pp]
            uint8_t b1 = (R ? 0 : 0x80) | ((~instr.vex_vvvv & 0xF) << 3) | (instr.vex_L << 2) | pp;
            if (!emit(0xC5) || !emit(b1)) return 0;
        } else {
            // 3-byte VEX: C4 [RXB.mmmmm] [W.vvvv.L.pp]
            uint8_t b1 = (R ? 0 : 0x80) | (X ? 0 : 0x40) | (B ? 0 : 0x20) | (map & 0x1F);
            uint8_t b2 = (instr.vex_W ? 0x80 : 0) | ((~instr.vex_vvvv & 0xF) << 3) | (instr.vex_L << 2) | pp;
            if (!emit(0xC4) || !emit(b1) || !emit(b2)) return 0;
        }

        uint8_t opreg_bits = has_opcode_reg(*instr.desc) ? (instr.opcode_reg & 7) : 0;
        if (map >= 2 && instr.desc->sec_opcd != 0xFF) {
            if (!emit(instr.desc->sec_opcd | opreg_bits)) return 0;
        } else {
            if (!emit((uint8_t)((instr.desc->opcode & 0xFF) | opreg_bits))) return 0;
        }
    } else {
        // Legacy (non-VEX) encoding
        for (uint8_t i = 0; i < instr.num_prefixes; ++i) {
            if (!emit(instr.legacy_prefix[i])) return 0;
        }

        if (instr.desc->prefix != 0) {
            bool already = false;
            for (uint8_t i = 0; i < instr.num_prefixes; ++i)
                if (instr.legacy_prefix[i] == instr.desc->prefix) already = true;
            if (!already) { if (!emit(instr.desc->prefix)) return 0; }
        }

        if (instr.has_rex2) {
            uint8_t payload = (instr.rex2_b4 << 6) | (instr.rex2_x4 << 5) | (instr.rex2_r4 << 4)
                | ((instr.rex & 0x08) ? 0x08 : 0) // W
                | ((instr.rex & 0x04) ? 0x04 : 0) // R
                | ((instr.rex & 0x02) ? 0x02 : 0) // X
                | ((instr.rex & 0x01) ? 0x01 : 0); // B
            bool m0 = (instr.desc->opcode & 0xFF00) == 0x0F00;
            if (m0) payload |= 0x80;
            if (!emit(0xD5) || !emit(payload)) return 0;
        } else if (instr.rex != 0) {
            if (!emit(instr.rex)) return 0;
        }

        uint8_t opreg_bits = has_opcode_reg(*instr.desc) ? (instr.opcode_reg & 7) : 0;
        if (instr.desc->opcode > 0xFF) {
            if (!emit(0x0F)) return 0;
            if (!emit((uint8_t)((instr.desc->opcode & 0xFF) | opreg_bits))) return 0;
            if ((instr.desc->opcode == 0x0F38 || instr.desc->opcode == 0x0F3A) && instr.desc->sec_opcd != 0xFF) {
                if (!emit(instr.desc->sec_opcd)) return 0;
            }
        } else {
            if (!emit((uint8_t)(instr.desc->opcode | opreg_bits))) return 0;
        }
    }

    if (instr.desc->has_modrm) {
        if (!emit(instr.modrm)) return 0;

        uint8_t mod_ = instr.modrm >> 6;
        uint8_t rm = instr.modrm & 7;
        if (mod_ != 3 && rm == 4) {
            if (!emit(instr.sib)) return 0;
        }

        int dsize = 0;
        if (mod_ == 0 && rm == 5) dsize = 4;
        else if (mod_ == 0 && rm == 4 && (instr.sib & 7) == 5) dsize = 4;
        else if (mod_ == 1) dsize = 1;
        else if (mod_ == 2) dsize = 4;
        if (pos + dsize > max_len) return 0;
        if (dsize > 0) { memcpy(&out[pos], &instr.displacement, dsize); pos += dsize; }
    }

    // Immediate
    bool has_66 = false;
    for (uint8_t i = 0; i < instr.num_prefixes; ++i) if (instr.legacy_prefix[i] == 0x66) has_66 = true;
    bool rex_w = (instr.rex & 0x08) != 0;
    for (uint8_t i = 0; i < instr.desc->num_operands; ++i) {
        AddrMode a = instr.desc->operands[i].addr;
        if (a == AddrMode::Immediate || a == AddrMode::RelOffset) {
            int isz = 0;
            switch (instr.desc->operands[i].size) {
            case OpSize::Byte: case OpSize::ByteSignExt: isz = 1; break;
            case OpSize::Word: isz = 2; break;
            case OpSize::Dword: isz = 4; break;
            case OpSize::Qword: isz = 8; break;
            case OpSize::OpSz: isz = has_66 ? 2 : 4; break;
            case OpSize::OpSzQ: isz = rex_w ? 8 : (has_66 ? 2 : 4); break;
            case OpSize::OpSzDS: isz = has_66 ? 2 : 4; break;
            default: break;
            }
            if (pos + isz > max_len) return 0;
            if (isz > 0) { memcpy(&out[pos], &instr.immediate, isz); pos += isz; }
            break;
        }
        if (a == AddrMode::Moffset) {
            if (pos + 8 > max_len) return 0;
            memcpy(&out[pos], &instr.displacement, 8); pos += 8;
            break;
        }
    }

    if (instr.desc->opcode == 0x0F3A && instr.immediate != 0) {
        bool has_imm_op = false;
        for (uint8_t i = 0; i < instr.desc->num_operands; ++i)
            if (instr.desc->operands[i].addr == AddrMode::Immediate) has_imm_op = true;
        if (!has_imm_op) {
            if (pos >= max_len) return 0;
            out[pos++] = (uint8_t)instr.immediate;
        }
    }

    return pos;
}

} // namespace vedx64
