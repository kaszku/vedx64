// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/core.hpp"
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>

using namespace vedx64;

static bool has_vex_operand(const InstrDesc& d) {
    for (uint8_t i = 0; i < d.num_operands; ++i)
        if (d.operands[i].addr == AddrMode::VexReg || d.operands[i].addr == AddrMode::XmmVvvv) return true;
    return false;
}

static uint8_t make_modrm(uint8_t mod_, uint8_t reg, uint8_t rm) {
    return (mod_ << 6) | ((reg & 7) << 3) | (rm & 7);
}

static uint8_t make_sib(uint8_t scale, uint8_t index, uint8_t base) {
    return (scale << 6) | ((index & 7) << 3) | (base & 7);
}

static int get_imm_size(OpSize sz) {
    switch (sz) {
    case OpSize::Byte: case OpSize::ByteSignExt: return 1;
    case OpSize::Word: return 2;
    case OpSize::Dword: return 4;
    case OpSize::Qword: return 8;
    case OpSize::OpSz: case OpSize::OpSzDS: return 4;
    case OpSize::OpSzQ: return 4;
    default: return 0;
    }
}

struct GenInstr {
    uint8_t bytes[16];
    uint8_t len;
    const char* variant;
};

static bool needs_modrm_rm(const InstrDesc& d) {
    for (uint8_t i = 0; i < d.num_operands; ++i) {
        switch (d.operands[i].addr) {
        case AddrMode::ModRM_RM: case AddrMode::MemOnly:
        case AddrMode::RegOnly: case AddrMode::XmmRM:
        case AddrMode::XmmRegOnly: case AddrMode::MmxRM:
        case AddrMode::MmxRegOnly:
            return true;
        default: break;
        }
    }
    return false;
}

static bool is_mem_only(const InstrDesc& d) {
    for (uint8_t i = 0; i < d.num_operands; ++i)
        if (d.operands[i].addr == AddrMode::MemOnly) return true;
    return false;
}

static bool is_reg_only(const InstrDesc& d) {
    for (uint8_t i = 0; i < d.num_operands; ++i) {
        AddrMode a = d.operands[i].addr;
        if (a == AddrMode::RegOnly || a == AddrMode::XmmRegOnly || a == AddrMode::MmxRegOnly) return true;
    }
    return false;
}

static GenInstr emit(const InstrDesc& d, uint8_t modrm_mod, uint8_t modrm_rm, bool use_sib, const char* variant) {
    GenInstr gi;
    memset(&gi, 0, sizeof(gi));
    gi.variant = variant;
    uint8_t* p = gi.bytes;
    uint8_t reg_val = d.opcd_ext != 0xFF ? d.opcd_ext : 1;

    const char* mn = mnemonic_name(d.mnemonic);
    bool needs_vex = false;
    if (d.opcode == 0x0F38 || d.opcode == 0x0F3A) {
        // Check if instruction is VEX-encoded (AVX/FMA/BMI), not legacy SSE/VMX
        // BMI1: ANDN(F2), BEXTR(F7), BLSI/BLSMSK/BLSR(F3)
        // BMI2: BZHI(F5), MULX(F6), PDEP(F5), PEXT(F5), SARX/SHLX/SHRX(F7), RORX(F0)
        // AVX: broadcast, permute, FMA, etc. (start with 'v')
        if (mn[0] == 'v' && mn[1] != 'm' && mn[1] != 'e'
            && strncmp(mn, "vscalef", 7) != 0 && strncmp(mn, "vgetexp", 7) != 0
            && strncmp(mn, "vcvtudq", 7) != 0 && strncmp(mn, "vcvtuqq", 7) != 0
            && d.vex_w == 0) needs_vex = true; // AVX/FMA (skip EVEX-only)
        else if (d.mnemonic == Mnemonic::ANDN || d.mnemonic == Mnemonic::BEXTR || d.mnemonic == Mnemonic::BLSI ||
            d.mnemonic == Mnemonic::BLSMSK || d.mnemonic == Mnemonic::BLSR || d.mnemonic == Mnemonic::BZHI ||
            d.mnemonic == Mnemonic::MULX || d.mnemonic == Mnemonic::PDEP || d.mnemonic == Mnemonic::PEXT ||
            d.mnemonic == Mnemonic::SARX || d.mnemonic == Mnemonic::SHLX || d.mnemonic == Mnemonic::SHRX ||
            d.mnemonic == Mnemonic::RORX) needs_vex = true; // BMI1/2
        // AMX / VMASKMOV
        else if (mn[0] == 't' && (strncmp(mn, "tdp", 3) == 0 || strncmp(mn, "tile", 4) == 0)) needs_vex = true;
        else if (strncmp(mn, "ldtilecfg", 9) == 0 || strncmp(mn, "sttilecfg", 9) == 0) needs_vex = true;
        else if (strncmp(mn, "vmaskmov", 8) == 0) needs_vex = true;
        else if (mn[0] == 'k') needs_vex = true; // mask ops
    }

    if (needs_vex && d.sec_opcd != 0xFF) {
        // VEX encoding: 3-byte VEX (C4) for 0F38/0F3A, 2-byte VEX (C5) for 0F
        uint8_t pp = 0; // 00=none, 01=66, 10=F3, 11=F2
        if (d.prefix == 0x66) pp = 1;
        else if (d.prefix == 0xF3) pp = 2;
        else if (d.prefix == 0xF2) pp = 3;
        uint8_t mmmmm = 1; // 0F map
        if (d.opcode == 0x0F38) mmmmm = 2;
        else if (d.opcode == 0x0F3A) mmmmm = 3;
        // 3-byte VEX: C4 [RXB.mmmmm] [W.vvvv.L.pp]
        *p++ = 0xC4;
        *p++ = 0xE0 | mmmmm; // R=1,X=1,B=1 (inverted: all clear)
        bool vex_l1 = (d.mnemonic == Mnemonic::VBROADCASTSD || d.mnemonic == Mnemonic::VBROADCASTF128 ||
            d.mnemonic == Mnemonic::VPERM2F128 || d.mnemonic == Mnemonic::VPERM2I128 ||
            d.mnemonic == Mnemonic::VPERMD || d.mnemonic == Mnemonic::VPERMQ ||
            d.mnemonic == Mnemonic::VINSERTF128 || d.mnemonic == Mnemonic::VINSERTI128 ||
            d.mnemonic == Mnemonic::VEXTRACTF128 || d.mnemonic == Mnemonic::VEXTRACTI128);
        *p++ = 0x78 | pp | (vex_l1 ? 0x04 : 0x00); // W=0, vvvv=1111, L, pp
        *p++ = d.sec_opcd; // the actual opcode byte
        goto emit_modrm; // VEX encoding done, skip EVEX/legacy
    }
    { // EVEX or legacy scope
    bool needs_evex = d.vex_w != 0;
    if (!needs_evex && mn[0] == 'v' && has_vex_operand(d)) {
        // Check if this is EVEX-only (vscalef, vgetexp, etc.) vs VEX (vaddps, etc.)
        if (strncmp(mn, "vscalef", 7) == 0 || strncmp(mn, "vgetexp", 7) == 0 ||
            strncmp(mn, "vblendm", 7) == 0 || strncmp(mn, "vpblendm", 8) == 0 ||
            strncmp(mn, "vpermi2", 7) == 0 || strncmp(mn, "vpermt2", 7) == 0 ||
            strncmp(mn, "vpternlog", 9) == 0 || strncmp(mn, "vpxord", 6) == 0 ||
            strncmp(mn, "vpxorq", 6) == 0 || strncmp(mn, "vpandd", 6) == 0 ||
            strncmp(mn, "vpandq", 6) == 0 || strncmp(mn, "vpord", 5) == 0 ||
            strncmp(mn, "vporq", 5) == 0 || strncmp(mn, "vpandnd", 7) == 0 ||
            strncmp(mn, "vpandnq", 7) == 0 || strncmp(mn, "vpdpbusd", 8) == 0 ||
            strncmp(mn, "vpdpwssd", 8) == 0 || strncmp(mn, "vpmadd52", 8) == 0 ||
            strncmp(mn, "vcompress", 9) == 0 || strncmp(mn, "vpcompress", 10) == 0 ||
            strncmp(mn, "vexpand", 7) == 0 || strncmp(mn, "vpexpand", 8) == 0 ||
            strncmp(mn, "vpconflict", 10) == 0 || strncmp(mn, "vplzcnt", 7) == 0 ||
            strncmp(mn, "vfixupimm", 9) == 0 || strncmp(mn, "vgetmant", 8) == 0 ||
            strncmp(mn, "vrndscale", 9) == 0 || strncmp(mn, "vrange", 6) == 0 ||
            strncmp(mn, "vreduce", 7) == 0 || strncmp(mn, "vfpclass", 8) == 0 ||
            strncmp(mn, "vdbpsadbw", 9) == 0 || strncmp(mn, "vprol", 5) == 0 ||
            strncmp(mn, "vpror", 5) == 0 || strncmp(mn, "vptestm", 7) == 0 ||
            strncmp(mn, "vptestnm", 8) == 0 ||
            strncmp(mn, "vcvtudq", 7) == 0 || strncmp(mn, "vcvtuqq", 7) == 0 ||
            strncmp(mn, "vcvtps2udq", 10) == 0 || strncmp(mn, "vcvtpd2udq", 10) == 0 ||
            strncmp(mn, "vcvttps2udq", 11) == 0 || strncmp(mn, "vcvttpd2udq", 11) == 0) needs_evex = true;
    }
    if (!needs_evex && !needs_vex && has_vex_operand(d)) needs_evex = true;
    if (!needs_evex && !needs_vex && mn[0] == 'v') {
        if (strstr(mn, "usi") || strncmp(mn, "vcvtne", 6) == 0 ||
            strncmp(mn, "vcvtudq", 7) == 0 || strncmp(mn, "vcvtuqq", 7) == 0) needs_evex = true;
    }
    if (needs_evex) {
        // EVEX encoding for EVEX-only instructions
        uint8_t pp = 0;
        if (d.prefix == 0x66) pp = 1; else if (d.prefix == 0xF3) pp = 2; else if (d.prefix == 0xF2) pp = 3;
        uint8_t mmm = 1;
        if (d.opcode == 0x0F38) mmm = 2; else if (d.opcode == 0x0F3A) mmm = 3;
        else if ((d.opcode & 0xFF00) == 0x0F00) mmm = 1;
        uint8_t w = (d.vex_w == 1) ? 1 : 0;
        *p++ = 0x62; // EVEX prefix
        *p++ = 0xF0 | mmm; // R=1,X=1,B=1,R'=1 (inverted)
        *p++ = (w << 7) | 0x7C | pp; // W, vvvv=1111, 1, pp
        *p++ = 0x08; // z=0, L'L=00(128), b=0, V'=1, aaa=000
        if (d.sec_opcd != 0xFF) *p++ = d.sec_opcd;
        else *p++ = (uint8_t)(d.opcode & 0xFF);
    } else {
        // Legacy encoding
        if (d.prefix != 0) *p++ = d.prefix;

        if (d.opcode > 0xFF) {
            *p++ = 0x0F;
            uint8_t ob = (uint8_t)(d.opcode & 0xFF);
            if (has_opcode_reg(d)) ob |= 1; // pick register 1
            *p++ = ob;
        } else {
            uint8_t ob = (uint8_t)d.opcode;
            if (has_opcode_reg(d)) ob |= 1; // pick register 1
            *p++ = ob;
        }

        if (d.sec_opcd != 0xFF) {
            if (d.opcode == 0x0F38 || d.opcode == 0x0F3A) {
                // Three-byte opcode: sec_opcd is 3rd byte, ModR/M follows
                *p++ = d.sec_opcd;
            } else {
                // Two-byte + fixed byte (e.g. 0F 01 C1 = VMCALL, C7 F8 = XBEGIN)
                *p++ = d.sec_opcd;
                // Check if instruction has RelOffset or Immediate — need to emit those too
                bool has_rel_op = false;
                for (uint8_t i = 0; i < d.num_operands; ++i)
                    if (d.operands[i].addr == AddrMode::RelOffset) has_rel_op = true;
                if (!has_rel_op && !d.has_modrm) {
                    bool has_imm = false;
                    for (uint8_t i = 0; i < d.num_operands; ++i)
                        if (d.operands[i].addr == AddrMode::Immediate) has_imm = true;
                    if (!has_imm) { gi.len = (uint8_t)(p - gi.bytes); return gi; }
                }
            }
        }
    }

    } // end EVEX/legacy scope
    emit_modrm:
    if (d.has_modrm) {
        uint8_t rm = modrm_rm;
        if (use_sib) rm = 4; // SIB follows
        *p++ = make_modrm(modrm_mod, reg_val, rm);

        if (modrm_mod != 3 && use_sib) {
            *p++ = make_sib(1, 1, 3); // [rbx + rcx*2]
        }

        if (modrm_mod == 0 && (rm == 5 || (use_sib && (p[-1] & 7) == 5))) {
            // RIP-relative or SIB disp32
            int32_t disp = 0x100; *p++ = (uint8_t)disp; *p++ = (uint8_t)(disp>>8);
            *p++ = (uint8_t)(disp>>16); *p++ = (uint8_t)(disp>>24);
        } else if (modrm_mod == 1) {
            *p++ = 0x10; // disp8 = +16
        } else if (modrm_mod == 2) {
            int32_t disp = 0x200; *p++ = (uint8_t)disp; *p++ = (uint8_t)(disp>>8);
            *p++ = (uint8_t)(disp>>16); *p++ = (uint8_t)(disp>>24);
        }
    }

    for (uint8_t i = 0; i < d.num_operands; ++i) {
        AddrMode a = d.operands[i].addr;
        if (a == AddrMode::Immediate) {
            int isz = get_imm_size(d.operands[i].size);
            uint64_t val = 0x42;
            if (isz >= 2) val = 0x1234;
            if (isz >= 4) val = 0x12345678;
            if (isz >= 8) val = 0x123456789ABCULL;
            memcpy(p, &val, isz); p += isz;
            break;
        }
        if (a == AddrMode::RelOffset) {
            // value will be set per-variant outside
            int isz = get_imm_size(d.operands[i].size);
            int32_t val = 0x10; // positive by default
            memcpy(p, &val, isz); p += isz;
            break;
        }
        if (a == AddrMode::Moffset) {
            uint64_t addr = 0x00007FFF00001000ULL;
            memcpy(p, &addr, 8); p += 8;
            break;
        }
    }

    if (d.opcode == 0x0F3A) {
        bool has_imm = false;
        for (uint8_t i = 0; i < d.num_operands; ++i)
            if (d.operands[i].addr == AddrMode::Immediate) has_imm = true;
        if (!has_imm) *p++ = 0x42; // trailing imm8
    }

    gi.len = (uint8_t)(p - gi.bytes);
    return gi;
}

static bool has_rel(const InstrDesc& d) {
    for (uint8_t i = 0; i < d.num_operands; ++i)
        if (d.operands[i].addr == AddrMode::RelOffset) return true;
    return false;
}

static int get_rel_size(const InstrDesc& d) {
    for (uint8_t i = 0; i < d.num_operands; ++i)
        if (d.operands[i].addr == AddrMode::RelOffset) return get_imm_size(d.operands[i].size);
    return 0;
}

static void generate_variants(const InstrDesc& d, size_t idx, std::vector<GenInstr>& out) {
    bool mem_only = is_mem_only(d) || d.mod_filter == 1;
    bool reg_only_flag = is_reg_only(d) || d.mod_filter == -1;
    bool has_rm = needs_modrm_rm(d);

    if (!d.has_modrm && !has_opcode_reg(d) && !has_rel(d)) {
        // No modrm, no opcode-reg, no rel: single encoding (e.g. NOP, RET, HLT)
        out.push_back(emit(d, 0, 0, false, "plain"));
        return;
    }

    if (has_opcode_reg(d) && !d.has_modrm) {
        // Register encoded in opcode (PUSH r, POP r, MOV r,imm, XCHG, BSWAP)
        out.push_back(emit(d, 0, 0, false, "opcreg"));
        return;
    }

    if (has_rel(d)) {
        GenInstr pos = emit(d, 3, 0, false, "rel_pos");
        out.push_back(pos);
        // Generate negative variant: patch the immediate
        GenInstr neg = pos;
        neg.variant = "rel_neg";
        int isz = get_rel_size(d);
        if (isz > 0 && neg.len >= (uint8_t)isz) {
            int32_t nval = -0x10;
            memcpy(&neg.bytes[neg.len - isz], &nval, isz);
        }
        out.push_back(neg);
        return;
    }

    if (!d.has_modrm) {
        out.push_back(emit(d, 0, 0, false, "no_modrm"));
        return;
    }

    if (!mem_only) {
        out.push_back(emit(d, 3, 0, false, "reg"));
    }

    if (!reg_only_flag) {
        out.push_back(emit(d, 2, 4, true, "sib_disp"));
    }
}

int main(int argc, char** argv) {
    bool bin_mode = false;
    const char* out_bin = nullptr;
    const char* out_txt = nullptr;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--bin") == 0 && i+1 < argc) out_bin = argv[++i];
        else if (strcmp(argv[i], "--txt") == 0 && i+1 < argc) out_txt = argv[++i];
    }

    FILE* fbin = out_bin ? fopen(out_bin, "wb") : nullptr;
    FILE* ftxt = out_txt ? fopen(out_txt, "w") : stdout;

    size_t total = 0, decode_ok = 0, roundtrip_ok = 0;
    uint64_t rip = 0;

    auto is_invalid_64 = [](const InstrDesc& d) -> bool {
        switch (d.opcode) {
        // One-byte: invalid in 64-bit long mode
        case 0x06: case 0x07: case 0x0E: case 0x16: case 0x17: case 0x1E: case 0x1F:
        case 0x27: case 0x2F: case 0x37: case 0x3F:
        case 0x60: case 0x61: case 0x62: case 0x82:
        case 0x9A: case 0xCE: case 0xEA:
        case 0xD4: case 0xD5: case 0xD6:
        // Two-byte: MOV TR (removed in 64-bit), JMPE (IA-64), LES/LDS (VEX conflict)
        case 0xC4: case 0xC5: // LES/LDS — conflict with VEX prefix in 64-bit
        case 0x0F24: case 0x0F26: // MOV TR — test registers removed
        case 0x0FB8: // JMPE — IA-64 transition
            return true;
        default:
            // Skip JMPE (0F 00 /6)
            if (d.opcode == 0x0F00 && d.opcd_ext == 6) return true;
            return false;
        }
    };

    for (size_t idx = 0; idx < g_instr_table_size; ++idx) {
        const auto& d = g_instr_table[idx];
        if (is_invalid_64(d)) continue;
        std::vector<GenInstr> variants;
        generate_variants(d, idx, variants);

        for (const auto& gi : variants) {
            if (gi.len == 0) continue;
            total++;

            DecodedInstr di;
            size_t n = decode(gi.bytes, gi.len, di);
            char disasm_buf[256] = {};
            bool decoded = (n > 0 && di.desc != nullptr);
            if (decoded) {
                decode_ok++;
                disassemble(gi.bytes, gi.len, disasm_buf, sizeof(disasm_buf), rip);

                // Roundtrip check
                uint8_t rebuf[16];
                size_t rn = encode(di, rebuf, sizeof(rebuf));
                if (rn == n && memcmp(gi.bytes, rebuf, n) == 0) roundtrip_ok++;
            }

            fprintf(ftxt, "%08llx  ", (unsigned long long)rip);
            for (uint8_t b = 0; b < gi.len; ++b)
                fprintf(ftxt, "%02X ", gi.bytes[b]);
            for (int pad = gi.len; pad < 12; ++pad) fprintf(ftxt, "   ");
            if (decoded) fprintf(ftxt, " %s", disasm_buf);
            else fprintf(ftxt, " ; ??? (decode failed)");
            fprintf(ftxt, "  ; [%zu] %s %s\n", idx, mnemonic_name(d.mnemonic), gi.variant);

            if (fbin) fwrite(gi.bytes, 1, gi.len, fbin);
            rip += gi.len;
        }
    }

    fprintf(stderr, "Generated %zu instruction variants\n", total);
    fprintf(stderr, "Decoded:    %zu / %zu (%.1f%%)\n", decode_ok, total, 100.0*decode_ok/total);
    fprintf(stderr, "Roundtrip:  %zu / %zu (%.1f%%)\n", roundtrip_ok, total, 100.0*roundtrip_ok/total);

    if (fbin) fclose(fbin);
    if (ftxt != stdout) fclose(ftxt);
    return 0;
}
