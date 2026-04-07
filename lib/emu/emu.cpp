// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/emu.hpp"
#include <cstring>
#include <cmath>
#if defined(_MSC_VER)
#include <intrin.h>
#endif

namespace vedx64 {

void emu_init(CpuState& cpu, uint8_t* mem, size_t mem_size) {
    memset(&cpu, 0, sizeof(cpu));
    cpu.mem = mem;
    cpu.mem_size = mem_size;
    cpu.rflags = 0x202;
    cpu.fpu_control = 0x037F; // default FPU control word
    cpu.mxcsr = 0x1F80; // default MXCSR
}

static bool mem_check(const CpuState& cpu, uint64_t addr, size_t size) {
    return addr + size <= cpu.mem_size;
}

static uint64_t mem_read(const CpuState& cpu, uint64_t addr, int bytes) {
    uint64_t val = 0;
    memcpy(&val, cpu.mem + addr, bytes);
    return val;
}

static void mem_write(CpuState& cpu, uint64_t addr, uint64_t val, int bytes) {
    memcpy(cpu.mem + addr, &val, bytes);
}

static uint64_t compute_pf(uint64_t val) {
    uint8_t b = (uint8_t)val;
    b ^= b >> 4; b ^= b >> 2; b ^= b >> 1;
    return (b & 1) ? 0 : RFLAG_PF;
}

static uint64_t sign_bit(int bits) { return 1ULL << (bits - 1); }
static uint64_t mask(int bits) { return (bits == 64) ? ~0ULL : (1ULL << bits) - 1; }

static void update_flags_add(CpuState& cpu, uint64_t a, uint64_t b, uint64_t res, int bits) {
    uint64_t m = mask(bits);
    uint64_t sb = sign_bit(bits);
    res &= m; a &= m; b &= m;
    cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_OF | RFLAG_PF | RFLAG_AF);
    if (bits < 64 && (a + b) > m) cpu.rflags |= RFLAG_CF;
    else if (bits == 64) { if (res < a) cpu.rflags |= RFLAG_CF; }
    if (res == 0) cpu.rflags |= RFLAG_ZF;
    if (res & sb) cpu.rflags |= RFLAG_SF;
    if (((a ^ res) & (b ^ res)) & sb) cpu.rflags |= RFLAG_OF;
    cpu.rflags |= compute_pf(res);
    if (((a & 0xf) + (b & 0xf)) > 0xf) cpu.rflags |= RFLAG_AF;
}

static void update_flags_sub(CpuState& cpu, uint64_t a, uint64_t b, uint64_t res, int bits) {
    uint64_t m = mask(bits);
    uint64_t sb = sign_bit(bits);
    res &= m; a &= m; b &= m;
    cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_OF | RFLAG_PF | RFLAG_AF);
    if (b > a) cpu.rflags |= RFLAG_CF;
    if (res == 0) cpu.rflags |= RFLAG_ZF;
    if (res & sb) cpu.rflags |= RFLAG_SF;
    if (((a ^ b) & (a ^ res)) & sb) cpu.rflags |= RFLAG_OF;
    cpu.rflags |= compute_pf(res);
    if ((b & 0xf) > (a & 0xf)) cpu.rflags |= RFLAG_AF;
}

static void update_flags_logic(CpuState& cpu, uint64_t res, int bits) {
    uint64_t m = mask(bits);
    uint64_t sb = sign_bit(bits);
    res &= m;
    cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_OF | RFLAG_PF | RFLAG_AF);
    if (res == 0) cpu.rflags |= RFLAG_ZF;
    if (res & sb) cpu.rflags |= RFLAG_SF;
    cpu.rflags |= compute_pf(res);
}

static bool eval_cc(const CpuState& cpu, uint8_t cc) {
    uint64_t f = cpu.rflags;
    switch (cc & 0xF) {
    case 0x0: return (f & RFLAG_OF) != 0;
    case 0x1: return (f & RFLAG_OF) == 0;
    case 0x2: return (f & RFLAG_CF) != 0;
    case 0x3: return (f & RFLAG_CF) == 0;
    case 0x4: return (f & RFLAG_ZF) != 0;
    case 0x5: return (f & RFLAG_ZF) == 0;
    case 0x6: return (f & (RFLAG_CF | RFLAG_ZF)) != 0;
    case 0x7: return (f & (RFLAG_CF | RFLAG_ZF)) == 0;
    case 0x8: return (f & RFLAG_SF) != 0;
    case 0x9: return (f & RFLAG_SF) == 0;
    case 0xA: return (f & RFLAG_PF) != 0;
    case 0xB: return (f & RFLAG_PF) == 0;
    case 0xC: return !!(f & RFLAG_SF) != !!(f & RFLAG_OF);
    case 0xD: return !!(f & RFLAG_SF) == !!(f & RFLAG_OF);
    case 0xE: return (f & RFLAG_ZF) || (!!(f & RFLAG_SF) != !!(f & RFLAG_OF));
    case 0xF: return !(f & RFLAG_ZF) && (!!(f & RFLAG_SF) == !!(f & RFLAG_OF));
    default: return false;
    }
}

static int resolve_opsize(const DecodedInstr& di, const OperandDesc& op) {
    switch (op.size) {
    case OpSize::Byte: case OpSize::ByteSignExt: return 8;
    case OpSize::Word: return 16;
    case OpSize::Dword: return 32;
    case OpSize::Qword: return 64;
    case OpSize::OpSz:
        if (di.rex & 0x08) return 64;
        for (int i = 0; i < di.num_prefixes; ++i)
            if (di.legacy_prefix[i] == 0x66) return 16;
        return 32;
    case OpSize::OpSzQ:
        if (di.rex & 0x08) return 64;
        for (int i = 0; i < di.num_prefixes; ++i)
            if (di.legacy_prefix[i] == 0x66) return 16;
        return 64;
    default: return 64;
    }
}

static int reg_index(const DecodedInstr& di, const OperandDesc& op) {
    switch (op.addr) {
    case AddrMode::ModRM_Reg: {
        int r = (di.modrm >> 3) & 7;
        if (di.rex & 0x04) r |= 8;
        return r;
    }
    case AddrMode::ModRM_RM: case AddrMode::MemOnly: case AddrMode::RegOnly: {
        int r = di.modrm & 7;
        if (di.rex & 0x01) r |= 8;
        return r;
    }
    case AddrMode::OpcodeReg: {
        int r = di.opcode_reg;
        if (di.rex & 0x01) r |= 8;
        return r;
    }
    case AddrMode::Fixed: return op.fixed_reg;
    case AddrMode::VexReg: case AddrMode::XmmVvvv: return (int)(di.vex_vvvv & 15);
    default: return 0;
    }
}

static bool is_mem_operand(const DecodedInstr& di, const OperandDesc& op) {
    if (op.addr == AddrMode::MemOnly || op.addr == AddrMode::Moffset) return true;
    if (op.addr == AddrMode::ModRM_RM) {
        uint8_t mod_field = (di.modrm >> 6) & 3;
        return mod_field != 3;
    }
    return false;
}

static uint64_t compute_ea(const CpuState& cpu, const DecodedInstr& di) {
    uint8_t mod_field = (di.modrm >> 6) & 3;
    uint8_t rm = di.modrm & 7;
    bool rex_b = (di.rex & 0x01) != 0;
    bool rex_x = (di.rex & 0x02) != 0;
    uint64_t addr = 0;

    if (rm == 4 && mod_field != 3) {
        uint8_t scale = (di.sib >> 6) & 3;
        uint8_t index = (di.sib >> 3) & 7;
        uint8_t base = di.sib & 7;
        if (rex_x) index |= 8;
        if (rex_b) base |= 8;

        if ((base & 7) == 5 && mod_field == 0) {
            addr = (uint64_t)(int64_t)(int32_t)di.displacement;
        } else {
            addr = cpu.gpr[base];
        }
        if (index != 4) {
            addr += cpu.gpr[index] * (uint64_t(1) << scale);
        }
        if (mod_field == 1) addr += (int8_t)di.displacement;
        else if (mod_field == 2) addr += (int32_t)di.displacement;
    } else if (rm == 5 && mod_field == 0) {
        addr = cpu.rip + di.length + di.displacement;
    } else {
        int r = rm;
        if (rex_b) r |= 8;
        addr = cpu.gpr[r];
        if (mod_field == 1) addr += (int8_t)di.displacement;
        else if (mod_field == 2) addr += (int32_t)di.displacement;
    }
    return addr;
}

static uint64_t read_gpr(const CpuState& cpu, int reg, int bits, uint8_t rex) {
    if (bits == 8 && !rex && reg >= 4 && reg <= 7)
        return (cpu.gpr[reg - 4] >> 8) & 0xFF;
    uint64_t val = cpu.gpr[reg & 15];
    switch (bits) {
    case 8:  return val & 0xFF;
    case 16: return val & 0xFFFF;
    case 32: return val & 0xFFFFFFFF;
    default: return val;
    }
}

static void write_gpr(CpuState& cpu, int reg, uint64_t val, int bits, uint8_t rex) {
    if (bits == 8 && !rex && reg >= 4 && reg <= 7) {
        cpu.gpr[reg - 4] = (cpu.gpr[reg - 4] & ~0xFF00ULL) | ((val & 0xFF) << 8);
        return;
    }
    switch (bits) {
    case 8:  cpu.gpr[reg & 15] = (cpu.gpr[reg & 15] & ~0xFFULL) | (val & 0xFF); break;
    case 16: cpu.gpr[reg & 15] = (cpu.gpr[reg & 15] & ~0xFFFFULL) | (val & 0xFFFF); break;
    case 32: cpu.gpr[reg & 15] = val & 0xFFFFFFFF; break;
    default: cpu.gpr[reg & 15] = val; break;
    }
}

static uint64_t read_operand(const CpuState& cpu, const DecodedInstr& di, int op_idx, int bits) {
    const OperandDesc& op = di.desc->operands[op_idx];
    if (op.addr == AddrMode::Immediate || op.addr == AddrMode::RelOffset) {
        return (uint64_t)di.immediate & mask(bits);
    }
    if (is_mem_operand(di, op)) {
        uint64_t ea = (op.addr == AddrMode::Moffset) ? (uint64_t)di.displacement : compute_ea(cpu, di);
        return mem_read(cpu, ea, bits / 8);
    }
    return read_gpr(cpu, reg_index(di, op), bits, di.rex);
}

static void write_operand(CpuState& cpu, const DecodedInstr& di, int op_idx, uint64_t val, int bits) {
    const OperandDesc& op = di.desc->operands[op_idx];
    if (is_mem_operand(di, op)) {
        uint64_t ea = (op.addr == AddrMode::Moffset) ? (uint64_t)di.displacement : compute_ea(cpu, di);
        mem_write(cpu, ea, val, bits / 8);
        return;
    }
    write_gpr(cpu, reg_index(di, op), val, bits, di.rex);
}

static uint64_t operand_ea(const CpuState& cpu, const DecodedInstr& di, int op_idx) {
    const OperandDesc& op = di.desc->operands[op_idx];
    if (op.addr == AddrMode::Moffset) return (uint64_t)di.displacement;
    return compute_ea(cpu, di);
}

static int xmm_reg_index(const DecodedInstr& di, const OperandDesc& op) {
    switch (op.addr) {
    case AddrMode::XmmReg: {
        int r = (di.modrm >> 3) & 7;
        if (di.rex & 0x04) r |= 8;
        return r;
    }
    case AddrMode::XmmRM: case AddrMode::XmmRegOnly: {
        int r = di.modrm & 7;
        if (di.rex & 0x01) r |= 8;
        return r;
    }
    case AddrMode::XmmVvvv: case AddrMode::VexReg:
        return (int)(di.vex_vvvv & 15);
    default: return 0;
    }
}

static bool is_xmm_mem(const DecodedInstr& di, const OperandDesc& op) {
    if (op.addr == AddrMode::XmmRM) {
        uint8_t mod_field = (di.modrm >> 6) & 3;
        return mod_field != 3;
    }
    return false;
}

static double& fpu_st(CpuState& cpu, int i) {
    return cpu.fpu_stack[(cpu.fpu_top + i) & 7];
}

static void fpu_push(CpuState& cpu, double val) {
    cpu.fpu_top = (cpu.fpu_top - 1) & 7;
    cpu.fpu_stack[cpu.fpu_top] = val;
}

static double fpu_pop(CpuState& cpu) {
    double val = cpu.fpu_stack[cpu.fpu_top];
    cpu.fpu_top = (cpu.fpu_top + 1) & 7;
    return val;
}

static void fpu_set_c0c1c2c3(CpuState& cpu, bool c0, bool c1, bool c2, bool c3) {
    cpu.fpu_status &= ~((1<<8)|(1<<9)|(1<<10)|(1<<14));
    if (c0) cpu.fpu_status |= (1 << 8);
    if (c1) cpu.fpu_status |= (1 << 9);
    if (c2) cpu.fpu_status |= (1 << 10);
    if (c3) cpu.fpu_status |= (1 << 14);
}

static int vex_src1(const DecodedInstr& di, int default_reg) {
    return di.has_vex ? (int)di.vex_vvvv : default_reg;
}

static void vex_zero_upper(CpuState& cpu, const DecodedInstr& di, int reg) {
    if (di.has_vex) {
        int vl = di.vex_L; // 0=128, 1=256, 2=512 (EVEX)
        if (vl == 0) { cpu.zmm[reg][2]=0; cpu.zmm[reg][3]=0; cpu.zmm[reg][4]=0; cpu.zmm[reg][5]=0; cpu.zmm[reg][6]=0; cpu.zmm[reg][7]=0; }
        else if (vl == 1) { cpu.zmm[reg][4]=0; cpu.zmm[reg][5]=0; cpu.zmm[reg][6]=0; cpu.zmm[reg][7]=0; }
    }
}

static bool has_prefix(const DecodedInstr& di, uint8_t p) {
    for (int i = 0; i < di.num_prefixes; ++i)
        if (di.legacy_prefix[i] == p) return true;
    return false;
}

using EmuHandler = StepResult(*)(CpuState&, const DecodedInstr&, int);

static StepResult emu_exec_switch(CpuState& cpu, const DecodedInstr& di, int bits) {
    uint16_t opcode = di.desc->opcode;
    Mnemonic mn = di.desc->mnemonic;

    bool is_rep = has_prefix(di, 0xF3);
    bool is_repnz = has_prefix(di, 0xF2);
    bool is_string_op = (mn == Mnemonic::STOS || mn == Mnemonic::STOSB || mn == Mnemonic::STOSW || mn == Mnemonic::STOSD || mn == Mnemonic::STOSQ ||
                         mn == Mnemonic::LODS || mn == Mnemonic::LODSB || mn == Mnemonic::LODSW || mn == Mnemonic::LODSD || mn == Mnemonic::LODSQ ||
                         mn == Mnemonic::MOVS || mn == Mnemonic::MOVSB || mn == Mnemonic::MOVSW || mn == Mnemonic::MOVSQ ||
                         mn == Mnemonic::SCAS || mn == Mnemonic::SCASB || mn == Mnemonic::SCASW || mn == Mnemonic::SCASD || mn == Mnemonic::SCASQ ||
                         mn == Mnemonic::CMPS || mn == Mnemonic::CMPSB || mn == Mnemonic::CMPSW || mn == Mnemonic::CMPSQ);
    bool is_scan_cmp = (mn == Mnemonic::SCAS || mn == Mnemonic::SCASB || mn == Mnemonic::SCASW || mn == Mnemonic::SCASD || mn == Mnemonic::SCASQ ||
                        mn == Mnemonic::CMPS || mn == Mnemonic::CMPSB || mn == Mnemonic::CMPSW || mn == Mnemonic::CMPSQ);
    Mnemonic base_mn = mn;
    int forced_str_sz = 0;
    if (mn == Mnemonic::STOSB || mn == Mnemonic::MOVSB || mn == Mnemonic::LODSB || mn == Mnemonic::SCASB || mn == Mnemonic::CMPSB) { base_mn = (mn == Mnemonic::STOSB ? Mnemonic::STOS : mn == Mnemonic::MOVSB ? Mnemonic::MOVS : mn == Mnemonic::LODSB ? Mnemonic::LODS : mn == Mnemonic::SCASB ? Mnemonic::SCAS : Mnemonic::CMPS); forced_str_sz = 1; }
    if (mn == Mnemonic::STOSW || mn == Mnemonic::MOVSW || mn == Mnemonic::LODSW || mn == Mnemonic::SCASW || mn == Mnemonic::CMPSW) { base_mn = (mn == Mnemonic::STOSW ? Mnemonic::STOS : mn == Mnemonic::MOVSW ? Mnemonic::MOVS : mn == Mnemonic::LODSW ? Mnemonic::LODS : mn == Mnemonic::SCASW ? Mnemonic::SCAS : Mnemonic::CMPS); forced_str_sz = 2; }
    if (mn == Mnemonic::STOSD || mn == Mnemonic::LODSD || mn == Mnemonic::SCASD) { base_mn = (mn == Mnemonic::STOSD ? Mnemonic::STOS : mn == Mnemonic::LODSD ? Mnemonic::LODS : Mnemonic::SCAS); forced_str_sz = 4; }
    if (mn == Mnemonic::STOSQ || mn == Mnemonic::MOVSQ || mn == Mnemonic::LODSQ || mn == Mnemonic::SCASQ || mn == Mnemonic::CMPSQ) { base_mn = (mn == Mnemonic::STOSQ ? Mnemonic::STOS : mn == Mnemonic::MOVSQ ? Mnemonic::MOVS : mn == Mnemonic::LODSQ ? Mnemonic::LODS : mn == Mnemonic::SCASQ ? Mnemonic::SCAS : Mnemonic::CMPS); forced_str_sz = 8; }

    if ((is_rep || is_repnz) && is_string_op) {
        int str_sz;
        if (forced_str_sz > 0) str_sz = forced_str_sz;
        else {
            uint8_t base_op = opcode & 0xFF;
            if ((base_op & 1) == 0) str_sz = 1;
            else { str_sz = 4; if (di.rex & 0x08) str_sz = 8; for (int i=0;i<di.num_prefixes;i++) if(di.legacy_prefix[i]==0x66) str_sz=2; }
        }
        while (cpu.gpr[1] != 0) {
            int64_t delta = (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
            if (base_mn == Mnemonic::STOS) {
                if (!mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
                mem_write(cpu, cpu.gpr[7], cpu.gpr[0], str_sz);
                cpu.gpr[7] += delta;
            } else if (base_mn == Mnemonic::LODS) {
                if (!mem_check(cpu, cpu.gpr[6], str_sz)) return StepResult::MemFault;
                uint64_t val = mem_read(cpu, cpu.gpr[6], str_sz);
                if (str_sz == 4) cpu.gpr[0] = val & 0xFFFFFFFF;
                else if (str_sz == 8) cpu.gpr[0] = val;
                else cpu.gpr[0] = (cpu.gpr[0] & ~mask(str_sz*8)) | val;
                cpu.gpr[6] += delta;
            } else if (base_mn == Mnemonic::MOVS) {
                if (!mem_check(cpu, cpu.gpr[6], str_sz) || !mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
                mem_write(cpu, cpu.gpr[7], mem_read(cpu, cpu.gpr[6], str_sz), str_sz);
                cpu.gpr[6] += delta; cpu.gpr[7] += delta;
            } else if (base_mn == Mnemonic::SCAS) {
                if (!mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
                uint64_t a = cpu.gpr[0] & mask(str_sz*8);
                uint64_t b = mem_read(cpu, cpu.gpr[7], str_sz);
                update_flags_sub(cpu, a, b, (a - b) & mask(str_sz*8), str_sz*8);
                cpu.gpr[7] += delta;
            } else if (base_mn == Mnemonic::CMPS) {
                if (!mem_check(cpu, cpu.gpr[6], str_sz) || !mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
                uint64_t a = mem_read(cpu, cpu.gpr[6], str_sz);
                uint64_t b = mem_read(cpu, cpu.gpr[7], str_sz);
                update_flags_sub(cpu, a, b, (a - b) & mask(str_sz*8), str_sz*8);
                cpu.gpr[6] += delta; cpu.gpr[7] += delta;
            }
            cpu.gpr[1]--;
            if (is_scan_cmp) {
                if (is_rep && !(cpu.rflags & RFLAG_ZF)) break;
                if (is_repnz && (cpu.rflags & RFLAG_ZF)) break;
            }
        }
        return StepResult::OK;
    }

    switch (mn) {

    case Mnemonic::ADD: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        if (di.desc->operands[1].size == OpSize::ByteSignExt) {
            int64_t sb = (int64_t)(int8_t)(b & 0xFF);
            b = (uint64_t)sb & mask(bits);
        }
        uint64_t res = (a + b) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        update_flags_add(cpu, a, b, res, bits);
        break;
    }
    case Mnemonic::ADC: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        if (di.desc->operands[1].size == OpSize::ByteSignExt) {
            int64_t sb = (int64_t)(int8_t)(b & 0xFF);
            b = (uint64_t)sb & mask(bits);
        }
        uint64_t cf = (cpu.rflags & RFLAG_CF) ? 1 : 0;
        uint64_t res = (a + b + cf) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        update_flags_add(cpu, a, b + cf, res, bits);
        break;
    }
    case Mnemonic::SUB: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        if (di.desc->operands[1].size == OpSize::ByteSignExt) {
            int64_t sb = (int64_t)(int8_t)(b & 0xFF);
            b = (uint64_t)sb & mask(bits);
        }
        uint64_t res = (a - b) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        update_flags_sub(cpu, a, b, res, bits);
        break;
    }
    case Mnemonic::SBB: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        if (di.desc->operands[1].size == OpSize::ByteSignExt) {
            int64_t sb = (int64_t)(int8_t)(b & 0xFF);
            b = (uint64_t)sb & mask(bits);
        }
        uint64_t cf = (cpu.rflags & RFLAG_CF) ? 1 : 0;
        uint64_t res = (a - b - cf) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        update_flags_sub(cpu, a, b + cf, res, bits);
        break;
    }
    case Mnemonic::CMP: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        if (di.desc->operands[1].size == OpSize::ByteSignExt) {
            int64_t sb = (int64_t)(int8_t)(b & 0xFF);
            b = (uint64_t)sb & mask(bits);
        }
        uint64_t res = (a - b) & mask(bits);
        update_flags_sub(cpu, a, b, res, bits);
        break;
    }
    case Mnemonic::INC: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        uint64_t res = (a + 1) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        uint64_t saved_cf = cpu.rflags & RFLAG_CF;
        update_flags_add(cpu, a, 1, res, bits);
        cpu.rflags = (cpu.rflags & ~RFLAG_CF) | saved_cf;
        break;
    }
    case Mnemonic::DEC: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        uint64_t res = (a - 1) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        uint64_t saved_cf = cpu.rflags & RFLAG_CF;
        update_flags_sub(cpu, a, 1, res, bits);
        cpu.rflags = (cpu.rflags & ~RFLAG_CF) | saved_cf;
        break;
    }
    case Mnemonic::NEG: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        uint64_t res = (0 - a) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        update_flags_sub(cpu, 0, a, res, bits);
        if (a != 0) cpu.rflags |= RFLAG_CF; else cpu.rflags &= ~RFLAG_CF;
        break;
    }
    case Mnemonic::NOT: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        write_operand(cpu, di, 0, ~a & mask(bits), bits);
        break;
    }
    case Mnemonic::AND: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        if (di.desc->operands[1].size == OpSize::ByteSignExt) {
            int64_t sb = (int64_t)(int8_t)(b & 0xFF);
            b = (uint64_t)sb & mask(bits);
        }
        uint64_t res = a & b;
        write_operand(cpu, di, 0, res, bits);
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::OR: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        if (di.desc->operands[1].size == OpSize::ByteSignExt) {
            int64_t sb = (int64_t)(int8_t)(b & 0xFF);
            b = (uint64_t)sb & mask(bits);
        }
        uint64_t res = a | b;
        write_operand(cpu, di, 0, res, bits);
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::XOR: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        if (di.desc->operands[1].size == OpSize::ByteSignExt) {
            int64_t sb = (int64_t)(int8_t)(b & 0xFF);
            b = (uint64_t)sb & mask(bits);
        }
        uint64_t res = a ^ b;
        write_operand(cpu, di, 0, res, bits);
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::TEST: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t b = read_operand(cpu, di, 1, sbits);
        uint64_t res = a & b;
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::MUL: {
        uint64_t a = cpu.gpr[0] & mask(bits);
        uint64_t b = read_operand(cpu, di, 0, bits);
        if (bits == 8) {
            uint16_t r = (uint16_t)a * (uint16_t)b;
            cpu.gpr[0] = (cpu.gpr[0] & ~0xFFFFULL) | r;
            cpu.rflags &= ~(RFLAG_CF | RFLAG_OF);
            if (r >> 8) cpu.rflags |= RFLAG_CF | RFLAG_OF;
        } else if (bits == 16) {
            uint32_t r = (uint32_t)(uint16_t)a * (uint32_t)(uint16_t)b;
            cpu.gpr[0] = (cpu.gpr[0] & ~0xFFFFULL) | (r & 0xFFFF);
            cpu.gpr[2] = (cpu.gpr[2] & ~0xFFFFULL) | (r >> 16);
            cpu.rflags &= ~(RFLAG_CF | RFLAG_OF);
            if (r >> 16) cpu.rflags |= RFLAG_CF | RFLAG_OF;
        } else if (bits == 32) {
            uint64_t r = (uint64_t)(uint32_t)a * (uint64_t)(uint32_t)b;
            cpu.gpr[0] = r & 0xFFFFFFFF;
            cpu.gpr[2] = (r >> 32) & 0xFFFFFFFF;
            cpu.rflags &= ~(RFLAG_CF | RFLAG_OF);
            if (r >> 32) cpu.rflags |= RFLAG_CF | RFLAG_OF;
        } else {
#if defined(_MSC_VER) && defined(_M_X64)
            uint64_t hi = 0;
            cpu.gpr[0] = _umul128(a, b, &hi);
            cpu.gpr[2] = hi;
#elif defined(_MSC_VER)
            // Portable 64x64->128 multiply for ARM64 MSVC
            uint64_t al = (uint32_t)a, ah = a >> 32;
            uint64_t bl = (uint32_t)b, bh = b >> 32;
            uint64_t p0 = al*bl, p1 = al*bh, p2 = ah*bl, p3 = ah*bh;
            uint64_t mid = p1 + (p0 >> 32);
            uint64_t carry = ((uint64_t)(uint32_t)mid + p2) >> 32;
            cpu.gpr[0] = a * b;
            cpu.gpr[2] = p3 + (mid >> 32) + (p2 >> 32) + carry;
#else
            __uint128_t r = (__uint128_t)a * b;
            cpu.gpr[0] = (uint64_t)r;
            cpu.gpr[2] = (uint64_t)(r >> 64);
#endif
            cpu.rflags &= ~(RFLAG_CF | RFLAG_OF);
            if (cpu.gpr[2]) cpu.rflags |= RFLAG_CF | RFLAG_OF;
        }
        break;
    }
    case Mnemonic::IMUL: {
        if (di.desc->num_operands == 1) {
            int64_t a = (int64_t)(cpu.gpr[0] & mask(bits));
            if (bits < 64 && (a & sign_bit(bits))) a |= ~mask(bits);
            int64_t b = (int64_t)(read_operand(cpu, di, 0, bits));
            if (bits < 64 && (b & sign_bit(bits))) b |= ~(int64_t)mask(bits);
            if (bits == 32) {
                int64_t r = a * b;
                cpu.gpr[0] = (uint64_t)r & 0xFFFFFFFF;
                cpu.gpr[2] = ((uint64_t)r >> 32) & 0xFFFFFFFF;
                cpu.rflags &= ~(RFLAG_CF | RFLAG_OF);
                if ((int32_t)cpu.gpr[0] != r) cpu.rflags |= RFLAG_CF | RFLAG_OF;
            } else {
                // Simplified: just do the multiply in the lower register
                cpu.gpr[0] = (uint64_t)(a * b);
                cpu.gpr[2] = 0;
            }
        } else if (di.desc->num_operands == 2) {
            int64_t a = (int64_t)read_operand(cpu, di, 0, bits);
            if (bits < 64 && (a & sign_bit(bits))) a |= ~(int64_t)mask(bits);
            int sbits2 = resolve_opsize(di, di.desc->operands[1]);
            int64_t b = (int64_t)read_operand(cpu, di, 1, sbits2);
            if (di.desc->operands[1].size == OpSize::ByteSignExt) {
                b = (int64_t)(int8_t)(b & 0xFF);
            } else if (sbits2 < 64 && (b & sign_bit(sbits2))) {
                b |= ~(int64_t)mask(sbits2);
            }
            uint64_t res = (uint64_t)(a * b) & mask(bits);
            write_operand(cpu, di, 0, res, bits);
            cpu.rflags &= ~(RFLAG_CF | RFLAG_OF);
        } else {
            // 3-operand: dst = src1 * imm
            int sbits1 = resolve_opsize(di, di.desc->operands[1]);
            int64_t a = (int64_t)read_operand(cpu, di, 1, sbits1);
            if (sbits1 < 64 && (a & sign_bit(sbits1))) a |= ~(int64_t)mask(sbits1);
            int64_t b = di.immediate;
            uint64_t res = (uint64_t)(a * b) & mask(bits);
            write_operand(cpu, di, 0, res, bits);
            cpu.rflags &= ~(RFLAG_CF | RFLAG_OF);
        }
        break;
    }
    case Mnemonic::DIV: {
        uint64_t divisor = read_operand(cpu, di, 0, bits);
        if (divisor == 0) return StepResult::DivByZero;
        if (bits == 32) {
            uint64_t dividend = ((cpu.gpr[2] & 0xFFFFFFFF) << 32) | (cpu.gpr[0] & 0xFFFFFFFF);
            cpu.gpr[0] = (dividend / divisor) & 0xFFFFFFFF;
            cpu.gpr[2] = (dividend % divisor) & 0xFFFFFFFF;
        } else if (bits == 64) {
            cpu.gpr[0] = cpu.gpr[0] / divisor;
            cpu.gpr[2] = cpu.gpr[0] % divisor;
        } else if (bits == 16) {
            uint32_t dividend = ((cpu.gpr[2] & 0xFFFF) << 16) | (cpu.gpr[0] & 0xFFFF);
            uint32_t d = (uint32_t)(divisor & 0xFFFF);
            cpu.gpr[0] = (cpu.gpr[0] & ~0xFFFFULL) | ((dividend / d) & 0xFFFF);
            cpu.gpr[2] = (cpu.gpr[2] & ~0xFFFFULL) | ((dividend % d) & 0xFFFF);
        } else {
            uint16_t dividend = (uint16_t)(cpu.gpr[0] & 0xFFFF);
            uint8_t d = (uint8_t)divisor;
            cpu.gpr[0] = (cpu.gpr[0] & ~0xFFFFULL) | ((dividend / d) & 0xFF) | (((dividend % d) & 0xFF) << 8);
        }
        break;
    }
    case Mnemonic::IDIV: {
        int64_t divisor = (int64_t)read_operand(cpu, di, 0, bits);
        if (bits < 64 && (divisor & sign_bit(bits))) divisor |= ~(int64_t)mask(bits);
        if (divisor == 0) return StepResult::DivByZero;
        if (bits == 32) {
            int64_t dividend = (int64_t)(((uint64_t)(cpu.gpr[2] & 0xFFFFFFFF) << 32) | (cpu.gpr[0] & 0xFFFFFFFF));
            cpu.gpr[0] = (uint64_t)(dividend / divisor) & 0xFFFFFFFF;
            cpu.gpr[2] = (uint64_t)(dividend % divisor) & 0xFFFFFFFF;
        } else {
            int64_t dividend = (int64_t)cpu.gpr[0];
            cpu.gpr[0] = (uint64_t)(dividend / divisor);
            cpu.gpr[2] = (uint64_t)(dividend % divisor);
        }
        break;
    }
    case Mnemonic::SHL: case Mnemonic::SAL: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        uint64_t count = (di.desc->num_operands > 1) ? read_operand(cpu, di, 1, 8) : 1;
        count &= (bits == 64) ? 63 : 31;
        if (count == 0) break;
        uint64_t res = (a << count) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_PF);
        if ((a >> (bits - count)) & 1) cpu.rflags |= RFLAG_CF;
        if (res == 0) cpu.rflags |= RFLAG_ZF;
        if (res & sign_bit(bits)) cpu.rflags |= RFLAG_SF;
        cpu.rflags |= compute_pf(res);
        if (count == 1) {
            cpu.rflags &= ~RFLAG_OF;
            if (!!(res & sign_bit(bits)) != !!(cpu.rflags & RFLAG_CF)) cpu.rflags |= RFLAG_OF;
        }
        break;
    }
    case Mnemonic::SHR: {
        uint64_t a = read_operand(cpu, di, 0, bits) & mask(bits);
        uint64_t count = (di.desc->num_operands > 1) ? read_operand(cpu, di, 1, 8) : 1;
        count &= (bits == 64) ? 63 : 31;
        if (count == 0) break;
        uint64_t res = a >> count;
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_PF | RFLAG_OF);
        if ((a >> (count - 1)) & 1) cpu.rflags |= RFLAG_CF;
        if (res == 0) cpu.rflags |= RFLAG_ZF;
        if (res & sign_bit(bits)) cpu.rflags |= RFLAG_SF;
        cpu.rflags |= compute_pf(res);
        if (count == 1 && (a & sign_bit(bits))) cpu.rflags |= RFLAG_OF;
        break;
    }
    case Mnemonic::SAR: {
        int64_t a = (int64_t)(read_operand(cpu, di, 0, bits) & mask(bits));
        if (bits < 64 && (a & sign_bit(bits))) a |= ~(int64_t)mask(bits);
        uint64_t count = (di.desc->num_operands > 1) ? read_operand(cpu, di, 1, 8) : 1;
        count &= (bits == 64) ? 63 : 31;
        if (count == 0) break;
        int64_t res = a >> count;
        write_operand(cpu, di, 0, (uint64_t)res & mask(bits), bits);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_PF | RFLAG_OF);
        if ((a >> (count - 1)) & 1) cpu.rflags |= RFLAG_CF;
        if (((uint64_t)res & mask(bits)) == 0) cpu.rflags |= RFLAG_ZF;
        if ((uint64_t)res & sign_bit(bits)) cpu.rflags |= RFLAG_SF;
        cpu.rflags |= compute_pf((uint64_t)res);
        break;
    }
    case Mnemonic::ROL: {
        uint64_t a = read_operand(cpu, di, 0, bits) & mask(bits);
        uint64_t count = (di.desc->num_operands > 1) ? read_operand(cpu, di, 1, 8) : 1;
        count &= (bits == 64) ? 63 : 31;
        if (count == 0) break;
        count %= bits;
        uint64_t res = ((a << count) | (a >> (bits - count))) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~RFLAG_CF;
        if (res & 1) cpu.rflags |= RFLAG_CF;
        break;
    }
    case Mnemonic::ROR: {
        uint64_t a = read_operand(cpu, di, 0, bits) & mask(bits);
        uint64_t count = (di.desc->num_operands > 1) ? read_operand(cpu, di, 1, 8) : 1;
        count &= (bits == 64) ? 63 : 31;
        if (count == 0) break;
        count %= bits;
        uint64_t res = ((a >> count) | (a << (bits - count))) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~RFLAG_CF;
        if (res & sign_bit(bits)) cpu.rflags |= RFLAG_CF;
        break;
    }
    case Mnemonic::MOV: {
        int sbits = (di.desc->num_operands > 1) ? resolve_opsize(di, di.desc->operands[1]) : bits;
        uint64_t val = read_operand(cpu, di, 1, sbits);
        write_operand(cpu, di, 0, val, bits);
        break;
    }
    case Mnemonic::MOVSX: {
        int sbits = resolve_opsize(di, di.desc->operands[1]);
        uint64_t val = read_operand(cpu, di, 1, sbits);
        if (val & sign_bit(sbits)) val |= ~mask(sbits);
        write_operand(cpu, di, 0, val & mask(bits), bits);
        break;
    }
    case Mnemonic::MOVSXD: {
        int sbits = resolve_opsize(di, di.desc->operands[1]);
        uint64_t val = read_operand(cpu, di, 1, sbits);
        if (val & sign_bit(sbits)) val |= ~mask(sbits);
        write_operand(cpu, di, 0, val & mask(bits), bits);
        break;
    }
    case Mnemonic::MOVZX: {
        int sbits = resolve_opsize(di, di.desc->operands[1]);
        uint64_t val = read_operand(cpu, di, 1, sbits);
        write_operand(cpu, di, 0, val & mask(sbits), bits);
        break;
    }
    case Mnemonic::LEA: {
        uint64_t ea = operand_ea(cpu, di, 1);
        write_operand(cpu, di, 0, ea & mask(bits), bits);
        break;
    }
    case Mnemonic::XCHG: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        uint64_t b = read_operand(cpu, di, 1, bits);
        write_operand(cpu, di, 0, b, bits);
        write_operand(cpu, di, 1, a, bits);
        break;
    }
    case Mnemonic::BSWAP: {
        uint64_t val = read_operand(cpu, di, 0, bits);
        if (bits == 32) {
            val = ((val >> 24) & 0xFF) | ((val >> 8) & 0xFF00) | ((val << 8) & 0xFF0000) | ((val << 24) & 0xFF000000);
        } else if (bits == 64) {
#if defined(_MSC_VER)
            val = _byteswap_uint64(val);
#else
            val = __builtin_bswap64(val);
#endif
        }
        write_operand(cpu, di, 0, val, bits);
        break;
    }
    case Mnemonic::PUSH: {
        // Find the source operand (skip None operands)
        int src_idx = 0;
        for (int i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr != AddrMode::None) { src_idx = i; break; }
        }
        int push_bits = resolve_opsize(di, di.desc->operands[src_idx]);
        if (push_bits < 64) push_bits = 64; // PUSH defaults to 64-bit in long mode
        uint64_t val = read_operand(cpu, di, src_idx, push_bits);
        if (di.desc->operands[src_idx].size == OpSize::ByteSignExt) {
            int64_t sv = (int64_t)(int8_t)(val & 0xFF);
            val = (uint64_t)sv;
        }
        cpu.gpr[4] -= 8;
        mem_write(cpu, cpu.gpr[4], val, 8);
        break;
    }
    case Mnemonic::POP: {
        // Find the destination operand (skip None operands)
        int dst_idx = 0;
        for (int i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr != AddrMode::None) { dst_idx = i; break; }
        }
        uint64_t val = mem_read(cpu, cpu.gpr[4], 8);
        cpu.gpr[4] += 8;
        write_operand(cpu, di, dst_idx, val, 64);
        break;
    }
    case Mnemonic::JMP: {
        // Find the target operand (skip None operands)
        int jmp_idx = 0;
        for (int i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr != AddrMode::None) { jmp_idx = i; break; }
        if (di.desc->operands[jmp_idx].addr == AddrMode::RelOffset) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip = read_operand(cpu, di, jmp_idx, 64);
        }
        return StepResult::OK;
    }
    case Mnemonic::CALL: {
        uint64_t ret_addr = cpu.rip + di.length;
        cpu.gpr[4] -= 8;
        mem_write(cpu, cpu.gpr[4], ret_addr, 8);
        // Find the target operand (skip None operands)
        int call_idx = 0;
        for (int i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr != AddrMode::None) { call_idx = i; break; }
        if (di.desc->operands[call_idx].addr == AddrMode::RelOffset) {
            cpu.rip = ret_addr + di.immediate;
        } else {
            cpu.rip = read_operand(cpu, di, call_idx, 64);
        }
        return StepResult::OK;
    }
    case Mnemonic::RETN: {
        cpu.rip = mem_read(cpu, cpu.gpr[4], 8);
        cpu.gpr[4] += 8;
        if (di.desc->num_operands > 0 && di.desc->operands[0].addr == AddrMode::Immediate) {
            cpu.gpr[4] += (uint16_t)di.immediate;
        }
        return StepResult::OK;
    }
    case Mnemonic::JO: {
        if (eval_cc(cpu, 0)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNO: {
        if (eval_cc(cpu, 1)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JB: {
        if (eval_cc(cpu, 2)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNB: {
        if (eval_cc(cpu, 3)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JZ: {
        if (eval_cc(cpu, 4)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNZ: {
        if (eval_cc(cpu, 5)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JBE: {
        if (eval_cc(cpu, 6)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNBE: {
        if (eval_cc(cpu, 7)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JS: {
        if (eval_cc(cpu, 8)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNS: {
        if (eval_cc(cpu, 9)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JP: {
        if (eval_cc(cpu, 10)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNP: {
        if (eval_cc(cpu, 11)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JL: {
        if (eval_cc(cpu, 12)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNL: {
        if (eval_cc(cpu, 13)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JLE: {
        if (eval_cc(cpu, 14)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNLE: {
        if (eval_cc(cpu, 15)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::CMOVO: {
        if (eval_cc(cpu, 0)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNO: {
        if (eval_cc(cpu, 1)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVB: {
        if (eval_cc(cpu, 2)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNB: {
        if (eval_cc(cpu, 3)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVZ: {
        if (eval_cc(cpu, 4)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNZ: {
        if (eval_cc(cpu, 5)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVBE: {
        if (eval_cc(cpu, 6)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNBE: {
        if (eval_cc(cpu, 7)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVS: {
        if (eval_cc(cpu, 8)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNS: {
        if (eval_cc(cpu, 9)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVP: {
        if (eval_cc(cpu, 10)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNP: {
        if (eval_cc(cpu, 11)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVL: {
        if (eval_cc(cpu, 12)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNL: {
        if (eval_cc(cpu, 13)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVLE: {
        if (eval_cc(cpu, 14)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNLE: {
        if (eval_cc(cpu, 15)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::SETO: {
        write_operand(cpu, di, 0, eval_cc(cpu, 0) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNO: {
        write_operand(cpu, di, 0, eval_cc(cpu, 1) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETB: {
        write_operand(cpu, di, 0, eval_cc(cpu, 2) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNB: {
        write_operand(cpu, di, 0, eval_cc(cpu, 3) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETZ: {
        write_operand(cpu, di, 0, eval_cc(cpu, 4) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNZ: {
        write_operand(cpu, di, 0, eval_cc(cpu, 5) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETBE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 6) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNBE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 7) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETS: {
        write_operand(cpu, di, 0, eval_cc(cpu, 8) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNS: {
        write_operand(cpu, di, 0, eval_cc(cpu, 9) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETP: {
        write_operand(cpu, di, 0, eval_cc(cpu, 10) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNP: {
        write_operand(cpu, di, 0, eval_cc(cpu, 11) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETL: {
        write_operand(cpu, di, 0, eval_cc(cpu, 12) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNL: {
        write_operand(cpu, di, 0, eval_cc(cpu, 13) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETLE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 14) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNLE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 15) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::LOOP: {
        cpu.gpr[1]--;
        if (cpu.gpr[1] != 0) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::LOOPZ: {
        cpu.gpr[1]--;
        if (cpu.gpr[1] != 0 && (cpu.rflags & RFLAG_ZF)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::LOOPNZ: {
        cpu.gpr[1]--;
        if (cpu.gpr[1] != 0 && !(cpu.rflags & RFLAG_ZF)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::CLC: cpu.rflags &= ~RFLAG_CF; break;
    case Mnemonic::STC: cpu.rflags |= RFLAG_CF; break;
    case Mnemonic::CMC: cpu.rflags ^= RFLAG_CF; break;
    case Mnemonic::CLD: cpu.rflags &= ~RFLAG_DF; break;
    case Mnemonic::STD: cpu.rflags |= RFLAG_DF; break;
    case Mnemonic::LAHF: {
        uint8_t ah = (uint8_t)(cpu.rflags & 0xFF);
        cpu.gpr[0] = (cpu.gpr[0] & ~0xFF00ULL) | ((uint64_t)ah << 8);
        break;
    }
    case Mnemonic::SAHF: {
        uint8_t ah = (uint8_t)(cpu.gpr[0] >> 8);
        cpu.rflags = (cpu.rflags & ~0xFFULL) | ah;
        break;
    }
    case Mnemonic::CBW: {
        int8_t al = (int8_t)(cpu.gpr[0] & 0xFF);
        cpu.gpr[0] = (cpu.gpr[0] & ~0xFFFFULL) | ((uint16_t)(int16_t)al & 0xFFFF);
        break;
    }
    case Mnemonic::CWDE: {
        int16_t ax = (int16_t)(cpu.gpr[0] & 0xFFFF);
        cpu.gpr[0] = (uint32_t)(int32_t)ax;
        break;
    }
    case Mnemonic::CDQE: {
        int32_t eax = (int32_t)(cpu.gpr[0] & 0xFFFFFFFF);
        cpu.gpr[0] = (uint64_t)(int64_t)eax;
        break;
    }
    case Mnemonic::CWD: {
        cpu.gpr[2] = (cpu.gpr[2] & ~0xFFFFULL) | ((cpu.gpr[0] & 0x8000) ? 0xFFFF : 0);
        break;
    }
    case Mnemonic::CDQ: {
        cpu.gpr[2] = (cpu.gpr[0] & 0x80000000) ? 0xFFFFFFFF : 0;
        break;
    }
    case Mnemonic::CQO: {
        cpu.gpr[2] = (cpu.gpr[0] & 0x8000000000000000ULL) ? ~0ULL : 0;
        break;
    }
    case Mnemonic::STOS: {
        int str_sz = 4; uint8_t base_op = opcode & 0xFF;
        if ((base_op & 1) == 0) str_sz = 1;
        else { if (di.rex & 0x08) str_sz = 8; for (int i=0;i<di.num_prefixes;i++) if(di.legacy_prefix[i]==0x66) str_sz=2; }
        mem_write(cpu, cpu.gpr[7], cpu.gpr[0], str_sz);
        cpu.gpr[7] += (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        break;
    }
    case Mnemonic::LODS: {
        int str_sz = 4; uint8_t base_op = opcode & 0xFF;
        if ((base_op & 1) == 0) str_sz = 1;
        else { if (di.rex & 0x08) str_sz = 8; for (int i=0;i<di.num_prefixes;i++) if(di.legacy_prefix[i]==0x66) str_sz=2; }
        uint64_t val = mem_read(cpu, cpu.gpr[6], str_sz);
        if (str_sz == 4) cpu.gpr[0] = val & 0xFFFFFFFF;
        else if (str_sz == 8) cpu.gpr[0] = val;
        else cpu.gpr[0] = (cpu.gpr[0] & ~mask(str_sz*8)) | val;
        cpu.gpr[6] += (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        break;
    }
    case Mnemonic::MOVS: {
        int str_sz = 4; uint8_t base_op = opcode & 0xFF;
        if ((base_op & 1) == 0) str_sz = 1;
        else { if (di.rex & 0x08) str_sz = 8; for (int i=0;i<di.num_prefixes;i++) if(di.legacy_prefix[i]==0x66) str_sz=2; }
        mem_write(cpu, cpu.gpr[7], mem_read(cpu, cpu.gpr[6], str_sz), str_sz);
        int64_t d = (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        cpu.gpr[6] += d; cpu.gpr[7] += d;
        break;
    }
    case Mnemonic::SCAS: {
        int str_sz = 4; uint8_t base_op = opcode & 0xFF;
        if ((base_op & 1) == 0) str_sz = 1;
        else { if (di.rex & 0x08) str_sz = 8; for (int i=0;i<di.num_prefixes;i++) if(di.legacy_prefix[i]==0x66) str_sz=2; }
        uint64_t a = cpu.gpr[0] & mask(str_sz*8);
        uint64_t b = mem_read(cpu, cpu.gpr[7], str_sz);
        update_flags_sub(cpu, a, b, (a - b) & mask(str_sz*8), str_sz*8);
        cpu.gpr[7] += (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        break;
    }
    case Mnemonic::CMPS: {
        int str_sz = 4; uint8_t base_op = opcode & 0xFF;
        if ((base_op & 1) == 0) str_sz = 1;
        else { if (di.rex & 0x08) str_sz = 8; for (int i=0;i<di.num_prefixes;i++) if(di.legacy_prefix[i]==0x66) str_sz=2; }
        uint64_t a = mem_read(cpu, cpu.gpr[6], str_sz);
        uint64_t b = mem_read(cpu, cpu.gpr[7], str_sz);
        update_flags_sub(cpu, a, b, (a - b) & mask(str_sz*8), str_sz*8);
        int64_t d = (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        cpu.gpr[6] += d; cpu.gpr[7] += d;
        break;
    }
    case Mnemonic::BT: {
        uint64_t val = read_operand(cpu, di, 0, bits);
        uint64_t bit = read_operand(cpu, di, 1, resolve_opsize(di, di.desc->operands[1])) % bits;
        cpu.rflags &= ~RFLAG_CF;
        if (val & (1ULL << bit)) cpu.rflags |= RFLAG_CF;
        break;
    }
    case Mnemonic::BTS: {
        uint64_t val = read_operand(cpu, di, 0, bits);
        uint64_t bit = read_operand(cpu, di, 1, resolve_opsize(di, di.desc->operands[1])) % bits;
        cpu.rflags &= ~RFLAG_CF;
        if (val & (1ULL << bit)) cpu.rflags |= RFLAG_CF;
        write_operand(cpu, di, 0, val | (1ULL << bit), bits);
        break;
    }
    case Mnemonic::BTR: {
        uint64_t val = read_operand(cpu, di, 0, bits);
        uint64_t bit = read_operand(cpu, di, 1, resolve_opsize(di, di.desc->operands[1])) % bits;
        cpu.rflags &= ~RFLAG_CF;
        if (val & (1ULL << bit)) cpu.rflags |= RFLAG_CF;
        write_operand(cpu, di, 0, val & ~(1ULL << bit), bits);
        break;
    }
    case Mnemonic::BTC: {
        uint64_t val = read_operand(cpu, di, 0, bits);
        uint64_t bit = read_operand(cpu, di, 1, resolve_opsize(di, di.desc->operands[1])) % bits;
        cpu.rflags &= ~RFLAG_CF;
        if (val & (1ULL << bit)) cpu.rflags |= RFLAG_CF;
        write_operand(cpu, di, 0, val ^ (1ULL << bit), bits);
        break;
    }
    case Mnemonic::BSF: {
        uint64_t val = read_operand(cpu, di, 1, bits) & mask(bits);
        cpu.rflags &= ~RFLAG_ZF;
        if (val == 0) { cpu.rflags |= RFLAG_ZF; }
        else {
            int i = 0;
            while (!(val & (1ULL << i))) i++;
            write_operand(cpu, di, 0, i, bits);
        }
        break;
    }
    case Mnemonic::BSR: {
        uint64_t val = read_operand(cpu, di, 1, bits) & mask(bits);
        cpu.rflags &= ~RFLAG_ZF;
        if (val == 0) { cpu.rflags |= RFLAG_ZF; }
        else {
            int i = bits - 1;
            while (!(val & (1ULL << i))) i--;
            write_operand(cpu, di, 0, i, bits);
        }
        break;
    }
    case Mnemonic::RCL: {
        uint64_t a = read_operand(cpu, di, 0, bits) & mask(bits);
        uint64_t count = (di.desc->num_operands > 1) ? read_operand(cpu, di, 1, 8) : 1;
        count &= (bits == 64) ? 63 : 31;
        count %= (bits + 1);
        if (count == 0) break;
        uint64_t cf = (cpu.rflags & RFLAG_CF) ? 1 : 0;
        for (uint64_t i = 0; i < count; i++) {
            uint64_t top = (a >> (bits - 1)) & 1;
            a = ((a << 1) | cf) & mask(bits);
            cf = top;
        }
        write_operand(cpu, di, 0, a, bits);
        cpu.rflags = (cpu.rflags & ~RFLAG_CF) | (cf ? RFLAG_CF : 0);
        break;
    }
    case Mnemonic::RCR: {
        uint64_t a = read_operand(cpu, di, 0, bits) & mask(bits);
        uint64_t count = (di.desc->num_operands > 1) ? read_operand(cpu, di, 1, 8) : 1;
        count &= (bits == 64) ? 63 : 31;
        count %= (bits + 1);
        if (count == 0) break;
        uint64_t cf = (cpu.rflags & RFLAG_CF) ? 1 : 0;
        for (uint64_t i = 0; i < count; i++) {
            uint64_t bot = a & 1;
            a = ((a >> 1) | (cf << (bits - 1))) & mask(bits);
            cf = bot;
        }
        write_operand(cpu, di, 0, a, bits);
        cpu.rflags = (cpu.rflags & ~RFLAG_CF) | (cf ? RFLAG_CF : 0);
        break;
    }
    case Mnemonic::SHLD: {
        uint64_t dst = read_operand(cpu, di, 0, bits) & mask(bits);
        uint64_t src = read_operand(cpu, di, 1, bits) & mask(bits);
        uint64_t count = (di.desc->num_operands > 2) ? read_operand(cpu, di, 2, 8) : cpu.gpr[1] & 0xFF;
        count &= (bits == 64) ? 63 : 31;
        if (count == 0) break;
        uint64_t res;
        if (count < (uint64_t)bits) res = ((dst << count) | (src >> (bits - count))) & mask(bits);
        else res = 0;
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_PF);
        if ((dst >> (bits - count)) & 1) cpu.rflags |= RFLAG_CF;
        if (res == 0) cpu.rflags |= RFLAG_ZF;
        if (res & sign_bit(bits)) cpu.rflags |= RFLAG_SF;
        cpu.rflags |= compute_pf(res);
        break;
    }
    case Mnemonic::SHRD: {
        uint64_t dst = read_operand(cpu, di, 0, bits) & mask(bits);
        uint64_t src = read_operand(cpu, di, 1, bits) & mask(bits);
        uint64_t count = (di.desc->num_operands > 2) ? read_operand(cpu, di, 2, 8) : cpu.gpr[1] & 0xFF;
        count &= (bits == 64) ? 63 : 31;
        if (count == 0) break;
        uint64_t res;
        if (count < (uint64_t)bits) res = ((dst >> count) | (src << (bits - count))) & mask(bits);
        else res = 0;
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_PF);
        if ((dst >> (count - 1)) & 1) cpu.rflags |= RFLAG_CF;
        if (res == 0) cpu.rflags |= RFLAG_ZF;
        if (res & sign_bit(bits)) cpu.rflags |= RFLAG_SF;
        cpu.rflags |= compute_pf(res);
        break;
    }
    case Mnemonic::CMPXCHG: {
        uint64_t dst = read_operand(cpu, di, 0, bits);
        uint64_t acc = cpu.gpr[0] & mask(bits);
        uint64_t res = (acc - dst) & mask(bits);
        update_flags_sub(cpu, acc, dst, res, bits);
        if (acc == (dst & mask(bits))) {
            uint64_t src = read_operand(cpu, di, 2, bits);
            write_operand(cpu, di, 0, src, bits);
        } else {
            write_gpr(cpu, 0, dst, bits, di.rex);
        }
        break;
    }
    case Mnemonic::CMPXCHG8B: {
        uint64_t ea = compute_ea(cpu, di);
        if (!mem_check(cpu, ea, 8)) return StepResult::MemFault;
        uint32_t lo = (uint32_t)mem_read(cpu, ea, 4);
        uint32_t hi = (uint32_t)mem_read(cpu, ea + 4, 4);
        if ((uint32_t)cpu.gpr[0] == lo && (uint32_t)cpu.gpr[2] == hi) {
            cpu.rflags |= RFLAG_ZF;
            mem_write(cpu, ea, cpu.gpr[3] & 0xFFFFFFFF, 4);
            mem_write(cpu, ea + 4, cpu.gpr[1] & 0xFFFFFFFF, 4);
        } else {
            cpu.rflags &= ~RFLAG_ZF;
            cpu.gpr[0] = lo;
            cpu.gpr[2] = hi;
        }
        break;
    }
    case Mnemonic::XADD: {
        uint64_t dst = read_operand(cpu, di, 0, bits);
        uint64_t src = read_operand(cpu, di, 1, bits);
        uint64_t res = (dst + src) & mask(bits);
        write_operand(cpu, di, 1, dst, bits);
        write_operand(cpu, di, 0, res, bits);
        update_flags_add(cpu, dst, src, res, bits);
        break;
    }
    case Mnemonic::LEAVE: {
        cpu.gpr[4] = cpu.gpr[5]; // MOV RSP, RBP
        cpu.gpr[5] = mem_read(cpu, cpu.gpr[4], 8); // POP RBP
        cpu.gpr[4] += 8;
        break;
    }
    case Mnemonic::ENTER: {
        uint16_t alloc_size = (uint16_t)di.immediate;
        int nesting = (int)(di.displacement & 0x1F);
        cpu.gpr[4] -= 8;
        if (!mem_check(cpu, cpu.gpr[4], 8)) return StepResult::MemFault;
        mem_write(cpu, cpu.gpr[4], cpu.gpr[5], 8);
        uint64_t frame_temp = cpu.gpr[4];
        if (nesting > 0) {
            for (int i = 1; i < nesting; i++) {
                cpu.gpr[5] -= 8;
                if (!mem_check(cpu, cpu.gpr[5], 8)) return StepResult::MemFault;
                uint64_t fp = mem_read(cpu, cpu.gpr[5], 8);
                cpu.gpr[4] -= 8;
                if (!mem_check(cpu, cpu.gpr[4], 8)) return StepResult::MemFault;
                mem_write(cpu, cpu.gpr[4], fp, 8);
            }
            cpu.gpr[4] -= 8;
            if (!mem_check(cpu, cpu.gpr[4], 8)) return StepResult::MemFault;
            mem_write(cpu, cpu.gpr[4], frame_temp, 8);
        }
        cpu.gpr[5] = frame_temp;
        cpu.gpr[4] -= alloc_size;
        break;
    }
    case Mnemonic::XLATB: {
        uint64_t addr = cpu.gpr[3] + (cpu.gpr[0] & 0xFF);
        if (!mem_check(cpu, addr, 1)) return StepResult::MemFault;
        cpu.gpr[0] = (cpu.gpr[0] & ~0xFFULL) | mem_read(cpu, addr, 1);
        break;
    }
    case Mnemonic::POPCNT: {
        uint64_t val = read_operand(cpu, di, 1, bits) & mask(bits);
        uint64_t cnt = 0;
        uint64_t tmp = val;
        while (tmp) { cnt += tmp & 1; tmp >>= 1; }
        write_operand(cpu, di, 0, cnt, bits);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_OF | RFLAG_PF | RFLAG_AF);
        if (val == 0) cpu.rflags |= RFLAG_ZF;
        break;
    }
    case Mnemonic::MOVBE: {
        if (is_mem_operand(di, di.desc->operands[1])) {
            // Load from memory with byte swap
            uint64_t val = read_operand(cpu, di, 1, bits);
            if (bits == 16) val = ((val >> 8) & 0xFF) | ((val & 0xFF) << 8);
            else if (bits == 32) val = ((val>>24)&0xFF)|((val>>8)&0xFF00)|((val<<8)&0xFF0000)|((val<<24)&0xFF000000ULL);
            else if (bits == 64) {
#if defined(_MSC_VER)
                val = _byteswap_uint64(val);
#else
                val = __builtin_bswap64(val);
#endif
            }
            write_operand(cpu, di, 0, val, bits);
        } else {
            // Store to memory with byte swap
            uint64_t val = read_operand(cpu, di, 1, bits);
            if (bits == 16) val = ((val >> 8) & 0xFF) | ((val & 0xFF) << 8);
            else if (bits == 32) val = ((val>>24)&0xFF)|((val>>8)&0xFF00)|((val<<8)&0xFF0000)|((val<<24)&0xFF000000ULL);
            else if (bits == 64) {
#if defined(_MSC_VER)
                val = _byteswap_uint64(val);
#else
                val = __builtin_bswap64(val);
#endif
            }
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::RETF: {
        cpu.rip = mem_read(cpu, cpu.gpr[4], 8);
        cpu.gpr[4] += 8;
        cpu.seg[1] = (uint16_t)mem_read(cpu, cpu.gpr[4], 2); // pop CS
        cpu.gpr[4] += 8;
        return StepResult::OK;
    }
    case Mnemonic::IRETQ: {
        cpu.rip = mem_read(cpu, cpu.gpr[4], 8); cpu.gpr[4] += 8;
        cpu.seg[1] = (uint16_t)mem_read(cpu, cpu.gpr[4], 2); cpu.gpr[4] += 8;
        cpu.rflags = mem_read(cpu, cpu.gpr[4], 8); cpu.gpr[4] += 8;
        return StepResult::OK;
    }
    case Mnemonic::JRCXZ: {
        if (cpu.gpr[1] == 0) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JA: {
        if (eval_cc(cpu, 7)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JAE: {
        if (eval_cc(cpu, 3)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JC: {
        if (eval_cc(cpu, 2)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JE: {
        if (eval_cc(cpu, 4)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JG: {
        if (eval_cc(cpu, 15)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JGE: {
        if (eval_cc(cpu, 13)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNA: {
        if (eval_cc(cpu, 6)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNAE: {
        if (eval_cc(cpu, 2)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNC: {
        if (eval_cc(cpu, 3)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNE: {
        if (eval_cc(cpu, 5)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNG: {
        if (eval_cc(cpu, 14)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JNGE: {
        if (eval_cc(cpu, 12)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JPE: {
        if (eval_cc(cpu, 10)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JPO: {
        if (eval_cc(cpu, 11)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::CMOVA: {
        if (eval_cc(cpu, 7)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVAE: {
        if (eval_cc(cpu, 3)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVC: {
        if (eval_cc(cpu, 2)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVE: {
        if (eval_cc(cpu, 4)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVG: {
        if (eval_cc(cpu, 15)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVGE: {
        if (eval_cc(cpu, 13)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNA: {
        if (eval_cc(cpu, 6)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNAE: {
        if (eval_cc(cpu, 2)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNC: {
        if (eval_cc(cpu, 3)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNE: {
        if (eval_cc(cpu, 5)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNG: {
        if (eval_cc(cpu, 14)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVNGE: {
        if (eval_cc(cpu, 12)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVPE: {
        if (eval_cc(cpu, 10)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::CMOVPO: {
        if (eval_cc(cpu, 11)) {
            int sbits = resolve_opsize(di, di.desc->operands[1]);
            uint64_t val = read_operand(cpu, di, 1, sbits);
            write_operand(cpu, di, 0, val, bits);
        }
        break;
    }
    case Mnemonic::SETA: {
        write_operand(cpu, di, 0, eval_cc(cpu, 7) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETAE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 3) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETC: {
        write_operand(cpu, di, 0, eval_cc(cpu, 2) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 4) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETG: {
        write_operand(cpu, di, 0, eval_cc(cpu, 15) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETGE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 13) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNA: {
        write_operand(cpu, di, 0, eval_cc(cpu, 6) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNAE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 2) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNC: {
        write_operand(cpu, di, 0, eval_cc(cpu, 3) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 5) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNG: {
        write_operand(cpu, di, 0, eval_cc(cpu, 14) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETNGE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 12) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETPE: {
        write_operand(cpu, di, 0, eval_cc(cpu, 10) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SETPO: {
        write_operand(cpu, di, 0, eval_cc(cpu, 11) ? 1 : 0, 8);
        break;
    }
    case Mnemonic::SWAPGS: {
        uint64_t tmp = cpu.gs_base;
        cpu.gs_base = 0; // KernelGSBase not tracked
        (void)tmp;
        break;
    }
    case Mnemonic::LFENCE: case Mnemonic::MFENCE: case Mnemonic::SFENCE: break;
    case Mnemonic::CLFLUSH: break;
    case Mnemonic::PREFETCHNTA: case Mnemonic::PREFETCHT0: case Mnemonic::PREFETCHT1: case Mnemonic::PREFETCHT2: break;
    case Mnemonic::MOVAPS: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        int sz = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 64 : 32) : 16;
        if (is_xmm_mem(di, op1)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op0);
            memcpy(cpu.zmm[r], cpu.mem + ea, sz);
            vex_zero_upper(cpu, di, r);
        } else if (is_xmm_mem(di, op0)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op1);
            memcpy(cpu.mem + ea, cpu.zmm[r], sz);
        } else {
            int dst = xmm_reg_index(di, op0);
            int src = xmm_reg_index(di, op1);
            memcpy(cpu.zmm[dst], cpu.zmm[src], sz);
            vex_zero_upper(cpu, di, dst);
        }
        break;
    }
    case Mnemonic::MOVUPS: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        int sz = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 64 : 32) : 16;
        if (is_xmm_mem(di, op1)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op0);
            memcpy(cpu.zmm[r], cpu.mem + ea, sz);
            vex_zero_upper(cpu, di, r);
        } else if (is_xmm_mem(di, op0)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op1);
            memcpy(cpu.mem + ea, cpu.zmm[r], sz);
        } else {
            int dst = xmm_reg_index(di, op0);
            int src = xmm_reg_index(di, op1);
            memcpy(cpu.zmm[dst], cpu.zmm[src], sz);
            vex_zero_upper(cpu, di, dst);
        }
        break;
    }
    case Mnemonic::MOVAPD: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        int sz = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 64 : 32) : 16;
        if (is_xmm_mem(di, op1)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op0);
            memcpy(cpu.zmm[r], cpu.mem + ea, sz);
            vex_zero_upper(cpu, di, r);
        } else if (is_xmm_mem(di, op0)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op1);
            memcpy(cpu.mem + ea, cpu.zmm[r], sz);
        } else {
            int dst = xmm_reg_index(di, op0);
            int src = xmm_reg_index(di, op1);
            memcpy(cpu.zmm[dst], cpu.zmm[src], sz);
            vex_zero_upper(cpu, di, dst);
        }
        break;
    }
    case Mnemonic::MOVUPD: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        int sz = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 64 : 32) : 16;
        if (is_xmm_mem(di, op1)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op0);
            memcpy(cpu.zmm[r], cpu.mem + ea, sz);
            vex_zero_upper(cpu, di, r);
        } else if (is_xmm_mem(di, op0)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op1);
            memcpy(cpu.mem + ea, cpu.zmm[r], sz);
        } else {
            int dst = xmm_reg_index(di, op0);
            int src = xmm_reg_index(di, op1);
            memcpy(cpu.zmm[dst], cpu.zmm[src], sz);
            vex_zero_upper(cpu, di, dst);
        }
        break;
    }
    case Mnemonic::MOVDQA: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        int sz = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 64 : 32) : 16;
        if (is_xmm_mem(di, op1)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op0);
            memcpy(cpu.zmm[r], cpu.mem + ea, sz);
            vex_zero_upper(cpu, di, r);
        } else if (is_xmm_mem(di, op0)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op1);
            memcpy(cpu.mem + ea, cpu.zmm[r], sz);
        } else {
            int dst = xmm_reg_index(di, op0);
            int src = xmm_reg_index(di, op1);
            memcpy(cpu.zmm[dst], cpu.zmm[src], sz);
            vex_zero_upper(cpu, di, dst);
        }
        break;
    }
    case Mnemonic::MOVDQU: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        int sz = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 64 : 32) : 16;
        if (is_xmm_mem(di, op1)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op0);
            memcpy(cpu.zmm[r], cpu.mem + ea, sz);
            vex_zero_upper(cpu, di, r);
        } else if (is_xmm_mem(di, op0)) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, sz)) return StepResult::MemFault;
            int r = xmm_reg_index(di, op1);
            memcpy(cpu.mem + ea, cpu.zmm[r], sz);
        } else {
            int dst = xmm_reg_index(di, op0);
            int src = xmm_reg_index(di, op1);
            memcpy(cpu.zmm[dst], cpu.zmm[src], sz);
            vex_zero_upper(cpu, di, dst);
        }
        break;
    }
    case Mnemonic::MOVSS: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        if (is_xmm_mem(di, op1)) {
            uint64_t ea = compute_ea(cpu, di); float f; memcpy(&f, cpu.mem + ea, 4);
            int r = xmm_reg_index(di, op0);
            double d = f; uint64_t zero = 0;
            memcpy(&cpu.zmm[r][0], &d, 8); memcpy(&cpu.zmm[r][1], &zero, 8);
        } else if (is_xmm_mem(di, op0)) {
            uint64_t ea = compute_ea(cpu, di);
            int r = xmm_reg_index(di, op1);
            float f = (float)cpu.zmm[r][0]; memcpy(cpu.mem + ea, &f, 4);
        } else {
            int dst = xmm_reg_index(di, op0); int src = xmm_reg_index(di, op1);
            cpu.zmm[dst][0] = cpu.zmm[src][0]; // upper bits unchanged for reg-reg
        }
        break;
    }
    case Mnemonic::ADDPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int count = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 16 : 8) : 4;
        float d[16], s[16];
        memcpy(d, cpu.zmm[src1], count*4); memcpy(s, cpu.zmm[src2], count*4);
        for (int i = 0; i < count; i++) d[i] = d[i] + s[i];
        memcpy(cpu.zmm[dst], d, count*4);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::SUBPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int count = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 16 : 8) : 4;
        float d[16], s[16];
        memcpy(d, cpu.zmm[src1], count*4); memcpy(s, cpu.zmm[src2], count*4);
        for (int i = 0; i < count; i++) d[i] = d[i] - s[i];
        memcpy(cpu.zmm[dst], d, count*4);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::MULPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int count = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 16 : 8) : 4;
        float d[16], s[16];
        memcpy(d, cpu.zmm[src1], count*4); memcpy(s, cpu.zmm[src2], count*4);
        for (int i = 0; i < count; i++) d[i] = d[i] * s[i];
        memcpy(cpu.zmm[dst], d, count*4);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::DIVPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int count = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 16 : 8) : 4;
        float d[16], s[16];
        memcpy(d, cpu.zmm[src1], count*4); memcpy(s, cpu.zmm[src2], count*4);
        for (int i = 0; i < count; i++) d[i] = d[i] / s[i];
        memcpy(cpu.zmm[dst], d, count*4);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::ADDPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int count = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < count; i++) cpu.zmm[dst][i] = cpu.zmm[src1][i] + cpu.zmm[src2][i];
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::SUBPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int count = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < count; i++) cpu.zmm[dst][i] = cpu.zmm[src1][i] - cpu.zmm[src2][i];
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::MULPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int count = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < count; i++) cpu.zmm[dst][i] = cpu.zmm[src1][i] * cpu.zmm[src2][i];
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::DIVPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int count = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < count; i++) cpu.zmm[dst][i] = cpu.zmm[src1][i] / cpu.zmm[src2][i];
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::ADDSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        float d, s;
        memcpy(&d, &cpu.zmm[src1][0], 4); memcpy(&s, &cpu.zmm[src2][0], 4);
        d = d + s;
        if (di.has_vex && dst != src1) memcpy(cpu.zmm[dst], cpu.zmm[src1], 16);
        memcpy(&cpu.zmm[dst][0], &d, 4);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::SUBSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        float d, s;
        memcpy(&d, &cpu.zmm[src1][0], 4); memcpy(&s, &cpu.zmm[src2][0], 4);
        d = d - s;
        if (di.has_vex && dst != src1) memcpy(cpu.zmm[dst], cpu.zmm[src1], 16);
        memcpy(&cpu.zmm[dst][0], &d, 4);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::MULSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        float d, s;
        memcpy(&d, &cpu.zmm[src1][0], 4); memcpy(&s, &cpu.zmm[src2][0], 4);
        d = d * s;
        if (di.has_vex && dst != src1) memcpy(cpu.zmm[dst], cpu.zmm[src1], 16);
        memcpy(&cpu.zmm[dst][0], &d, 4);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::DIVSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        float d, s;
        memcpy(&d, &cpu.zmm[src1][0], 4); memcpy(&s, &cpu.zmm[src2][0], 4);
        d = d / s;
        if (di.has_vex && dst != src1) memcpy(cpu.zmm[dst], cpu.zmm[src1], 16);
        memcpy(&cpu.zmm[dst][0], &d, 4);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::ADDSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        double res = cpu.zmm[src1][0] + cpu.zmm[src2][0];
        if (di.has_vex && dst != src1) { cpu.zmm[dst][1] = cpu.zmm[src1][1]; }
        cpu.zmm[dst][0] = res;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::SUBSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        double res = cpu.zmm[src1][0] - cpu.zmm[src2][0];
        if (di.has_vex && dst != src1) { cpu.zmm[dst][1] = cpu.zmm[src1][1]; }
        cpu.zmm[dst][0] = res;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::MULSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        double res = cpu.zmm[src1][0] * cpu.zmm[src2][0];
        if (di.has_vex && dst != src1) { cpu.zmm[dst][1] = cpu.zmm[src1][1]; }
        cpu.zmm[dst][0] = res;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::DIVSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        double res = cpu.zmm[src1][0] / cpu.zmm[src2][0];
        if (di.has_vex && dst != src1) { cpu.zmm[dst][1] = cpu.zmm[src1][1]; }
        cpu.zmm[dst][0] = res;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::XORPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int qwords = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < qwords; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a ^ b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::XORPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int qwords = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < qwords; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a ^ b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PXOR: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int qwords = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < qwords; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a ^ b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::ANDPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int qwords = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < qwords; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a & b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::ANDPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int qwords = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < qwords; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a & b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::ORPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int qwords = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < qwords; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a | b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::ORPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int qwords = (di.has_vex && di.vex_L >= 1) ? ((di.vex_L >= 2) ? 8 : 4) : 2;
        for (int i = 0; i < qwords; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a | b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::SQRTSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float f; memcpy(&f, &cpu.zmm[src][0], 4);
        f = sqrtf(f); memcpy(&cpu.zmm[dst][0], &f, 4);
        break;
    }
    case Mnemonic::SQRTSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = sqrt(cpu.zmm[src][0]);
        break;
    }
    case Mnemonic::MINPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float d[4], s[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i = 0; i < 4; i++) if (s[i] < d[i]) d[i] = s[i];
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::MAXPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float d[4], s[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i = 0; i < 4; i++) if (s[i] > d[i]) d[i] = s[i];
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::MINPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = fmin(cpu.zmm[dst][0], cpu.zmm[src][0]);
        cpu.zmm[dst][1] = fmin(cpu.zmm[dst][1], cpu.zmm[src][1]);
        break;
    }
    case Mnemonic::MAXPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = fmax(cpu.zmm[dst][0], cpu.zmm[src][0]);
        cpu.zmm[dst][1] = fmax(cpu.zmm[dst][1], cpu.zmm[src][1]);
        break;
    }
    case Mnemonic::MINSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float d, s; memcpy(&d, &cpu.zmm[dst][0], 4); memcpy(&s, &cpu.zmm[src][0], 4);
        if (s < d) d = s; memcpy(&cpu.zmm[dst][0], &d, 4);
        break;
    }
    case Mnemonic::MAXSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float d, s; memcpy(&d, &cpu.zmm[dst][0], 4); memcpy(&s, &cpu.zmm[src][0], 4);
        if (s > d) d = s; memcpy(&cpu.zmm[dst][0], &d, 4);
        break;
    }
    case Mnemonic::MINSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = fmin(cpu.zmm[dst][0], cpu.zmm[src][0]);
        break;
    }
    case Mnemonic::MAXSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = fmax(cpu.zmm[dst][0], cpu.zmm[src][0]);
        break;
    }
    case Mnemonic::UCOMISS: case Mnemonic::COMISS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float a, b; memcpy(&a, &cpu.zmm[dst][0], 4); memcpy(&b, &cpu.zmm[src][0], 4);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_PF | RFLAG_OF | RFLAG_SF | RFLAG_AF);
        if (a != a || b != b) cpu.rflags |= RFLAG_ZF | RFLAG_PF | RFLAG_CF; // unordered
        else if (a < b) cpu.rflags |= RFLAG_CF;
        else if (a == b) cpu.rflags |= RFLAG_ZF;
        break;
    }
    case Mnemonic::UCOMISD: case Mnemonic::COMISD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        double a = cpu.zmm[dst][0], b = cpu.zmm[src][0];
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_PF | RFLAG_OF | RFLAG_SF | RFLAG_AF);
        if (a != a || b != b) cpu.rflags |= RFLAG_ZF | RFLAG_PF | RFLAG_CF;
        else if (a < b) cpu.rflags |= RFLAG_CF;
        else if (a == b) cpu.rflags |= RFLAG_ZF;
        break;
    }
    case Mnemonic::CVTSI2SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int64_t val = (int64_t)read_operand(cpu, di, 1, bits);
        if (bits == 32) val = (int64_t)(int32_t)val;
        float f = (float)val; memcpy(&cpu.zmm[dst][0], &f, 4);
        break;
    }
    case Mnemonic::CVTSI2SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int64_t val = (int64_t)read_operand(cpu, di, 1, bits);
        if (bits == 32) val = (int64_t)(int32_t)val;
        cpu.zmm[dst][0] = (double)val;
        break;
    }
    case Mnemonic::CVTSS2SI: {
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float f; memcpy(&f, &cpu.zmm[src][0], 4);
        write_operand(cpu, di, 0, (uint64_t)(int64_t)f, bits);
        break;
    }
    case Mnemonic::CVTSD2SI: {
        int src = xmm_reg_index(di, di.desc->operands[1]);
        write_operand(cpu, di, 0, (uint64_t)(int64_t)cpu.zmm[src][0], bits);
        break;
    }
    case Mnemonic::CVTTSS2SI: {
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float f; memcpy(&f, &cpu.zmm[src][0], 4);
        write_operand(cpu, di, 0, (uint64_t)(int64_t)f, bits);
        break;
    }
    case Mnemonic::CVTTSD2SI: {
        int src = xmm_reg_index(di, di.desc->operands[1]);
        write_operand(cpu, di, 0, (uint64_t)(int64_t)cpu.zmm[src][0], bits);
        break;
    }
    case Mnemonic::CVTSS2SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float f; memcpy(&f, &cpu.zmm[src][0], 4);
        cpu.zmm[dst][0] = (double)f;
        break;
    }
    case Mnemonic::CVTSD2SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float f = (float)cpu.zmm[src][0]; memcpy(&cpu.zmm[dst][0], &f, 4);
        break;
    }
    case Mnemonic::MOVD: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        if (op0.addr == AddrMode::XmmReg || op0.addr == AddrMode::XmmRM) {
            int r = xmm_reg_index(di, op0);
            uint64_t val = read_operand(cpu, di, 1, 32);
            memset(cpu.zmm[r], 0, 16);
            uint32_t v32 = (uint32_t)val; memcpy(&cpu.zmm[r][0], &v32, 4);
        } else {
            int r = xmm_reg_index(di, op1);
            uint32_t v32; memcpy(&v32, &cpu.zmm[r][0], 4);
            write_operand(cpu, di, 0, v32, 32);
        }
        break;
    }
    case Mnemonic::MOVQ: {
        const OperandDesc& op0 = di.desc->operands[0];
        const OperandDesc& op1 = di.desc->operands[1];
        if (op0.addr == AddrMode::XmmReg || op0.addr == AddrMode::XmmRM || op0.addr == AddrMode::XmmRegOnly) {
            int r = xmm_reg_index(di, op0);
            uint64_t val = read_operand(cpu, di, 1, 64);
            memcpy(&cpu.zmm[r][0], &val, 8); uint64_t zero = 0; memcpy(&cpu.zmm[r][1], &zero, 8);
        } else {
            int r = xmm_reg_index(di, op1);
            uint64_t val; memcpy(&val, &cpu.zmm[r][0], 8);
            write_operand(cpu, di, 0, val, 64);
        }
        break;
    }
    case Mnemonic::MOVLPS: case Mnemonic::MOVLPD: {
        const OperandDesc& op0 = di.desc->operands[0];
        if (is_xmm_mem(di, di.desc->operands[1])) {
            uint64_t ea = compute_ea(cpu, di);
            int r = xmm_reg_index(di, op0);
            memcpy(&cpu.zmm[r][0], cpu.mem + ea, 8);
        } else {
            uint64_t ea = compute_ea(cpu, di);
            int r = xmm_reg_index(di, di.desc->operands[1]);
            memcpy(cpu.mem + ea, &cpu.zmm[r][0], 8);
        }
        break;
    }
    case Mnemonic::MOVHPS: case Mnemonic::MOVHPD: {
        const OperandDesc& op0 = di.desc->operands[0];
        if (is_xmm_mem(di, di.desc->operands[1])) {
            uint64_t ea = compute_ea(cpu, di);
            int r = xmm_reg_index(di, op0);
            memcpy(&cpu.zmm[r][1], cpu.mem + ea, 8);
        } else {
            uint64_t ea = compute_ea(cpu, di);
            int r = xmm_reg_index(di, di.desc->operands[1]);
            memcpy(cpu.mem + ea, &cpu.zmm[r][1], 8);
        }
        break;
    }
    case Mnemonic::MOVMSKPS: {
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float f[4]; memcpy(f, cpu.zmm[src], 16);
        uint32_t msk = 0;
        for (int i = 0; i < 4; i++) { uint32_t u; memcpy(&u, &f[i], 4); if (u >> 31) msk |= (1 << i); }
        write_operand(cpu, di, 0, msk, bits);
        break;
    }
    case Mnemonic::MOVMSKPD: {
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint64_t u0, u1; memcpy(&u0, &cpu.zmm[src][0], 8); memcpy(&u1, &cpu.zmm[src][1], 8);
        uint32_t msk = ((u0 >> 63) & 1) | (((u1 >> 63) & 1) << 1);
        write_operand(cpu, di, 0, msk, bits);
        break;
    }
    case Mnemonic::SHUFPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t imm = (uint8_t)di.immediate;
        float d[4], s[4], r[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        r[0] = d[imm & 3]; r[1] = d[(imm >> 2) & 3]; r[2] = s[(imm >> 4) & 3]; r[3] = s[(imm >> 6) & 3];
        memcpy(cpu.zmm[dst], r, 16);
        break;
    }
    case Mnemonic::SHUFPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t imm = (uint8_t)di.immediate;
        double d0 = cpu.zmm[dst][imm & 1], d1 = cpu.zmm[src][(imm >> 1) & 1];
        cpu.zmm[dst][0] = d0; cpu.zmm[dst][1] = d1;
        break;
    }
    case Mnemonic::UNPCKLPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float d[4], s[4], r[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        r[0] = d[0]; r[1] = s[0]; r[2] = d[1]; r[3] = s[1];
        memcpy(cpu.zmm[dst], r, 16);
        break;
    }
    case Mnemonic::UNPCKHPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float d[4], s[4], r[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        r[0] = d[2]; r[1] = s[2]; r[2] = d[3]; r[3] = s[3];
        memcpy(cpu.zmm[dst], r, 16);
        break;
    }
    case Mnemonic::UNPCKLPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][1] = cpu.zmm[src][0]; // dst[0] unchanged
        break;
    }
    case Mnemonic::UNPCKHPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = cpu.zmm[dst][1]; cpu.zmm[dst][1] = cpu.zmm[src][1];
        break;
    }
    case Mnemonic::PADDB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int vl = (di.has_vex && di.vex_L >= 2) ? 64 : ((di.has_vex && di.vex_L >= 1) ? 32 : 16);
        int count = vl / 1;
        uint8_t d[64], s[64]; memcpy(d, cpu.zmm[src1], vl); memcpy(s, cpu.zmm[src2], vl);
        for (int i = 0; i < count; i++) d[i] = (uint8_t)(d[i] + s[i]);
        memcpy(cpu.zmm[dst], d, vl);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PADDW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int vl = (di.has_vex && di.vex_L >= 2) ? 64 : ((di.has_vex && di.vex_L >= 1) ? 32 : 16);
        int count = vl / 2;
        uint8_t d[64], s[64]; memcpy(d, cpu.zmm[src1], vl); memcpy(s, cpu.zmm[src2], vl);
        uint16_t *dw = (uint16_t*)d, *sw = (uint16_t*)s;
        for (int i = 0; i < count; i++) dw[i] = (uint16_t)(dw[i] + sw[i]);
        memcpy(cpu.zmm[dst], d, vl);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PADDD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int vl = (di.has_vex && di.vex_L >= 2) ? 64 : ((di.has_vex && di.vex_L >= 1) ? 32 : 16);
        int count = vl / 4;
        uint8_t d[64], s[64]; memcpy(d, cpu.zmm[src1], vl); memcpy(s, cpu.zmm[src2], vl);
        uint32_t *dd = (uint32_t*)d, *sd = (uint32_t*)s;
        for (int i = 0; i < count; i++) dd[i] = (uint32_t)(dd[i] + sd[i]);
        memcpy(cpu.zmm[dst], d, vl);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PADDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int vl = (di.has_vex && di.vex_L >= 2) ? 64 : ((di.has_vex && di.vex_L >= 1) ? 32 : 16);
        int count = vl / 8;
        uint8_t d[64], s[64]; memcpy(d, cpu.zmm[src1], vl); memcpy(s, cpu.zmm[src2], vl);
        uint64_t *dq = (uint64_t*)d, *sq = (uint64_t*)s;
        for (int i = 0; i < count; i++) dq[i] = dq[i] + sq[i];
        memcpy(cpu.zmm[dst], d, vl);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PSUBB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int vl = (di.has_vex && di.vex_L >= 2) ? 64 : ((di.has_vex && di.vex_L >= 1) ? 32 : 16);
        int count = vl / 1;
        uint8_t d[64], s[64]; memcpy(d, cpu.zmm[src1], vl); memcpy(s, cpu.zmm[src2], vl);
        for (int i = 0; i < count; i++) d[i] = (uint8_t)(d[i] - s[i]);
        memcpy(cpu.zmm[dst], d, vl);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PSUBW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int vl = (di.has_vex && di.vex_L >= 2) ? 64 : ((di.has_vex && di.vex_L >= 1) ? 32 : 16);
        int count = vl / 2;
        uint8_t d[64], s[64]; memcpy(d, cpu.zmm[src1], vl); memcpy(s, cpu.zmm[src2], vl);
        uint16_t *dw = (uint16_t*)d, *sw = (uint16_t*)s;
        for (int i = 0; i < count; i++) dw[i] = (uint16_t)(dw[i] - sw[i]);
        memcpy(cpu.zmm[dst], d, vl);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PSUBD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int vl = (di.has_vex && di.vex_L >= 2) ? 64 : ((di.has_vex && di.vex_L >= 1) ? 32 : 16);
        int count = vl / 4;
        uint8_t d[64], s[64]; memcpy(d, cpu.zmm[src1], vl); memcpy(s, cpu.zmm[src2], vl);
        uint32_t *dd = (uint32_t*)d, *sd = (uint32_t*)s;
        for (int i = 0; i < count; i++) dd[i] = (uint32_t)(dd[i] - sd[i]);
        memcpy(cpu.zmm[dst], d, vl);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PSUBQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = vex_src1(di, dst);
        int src2 = xmm_reg_index(di, di.desc->operands[1]);
        int vl = (di.has_vex && di.vex_L >= 2) ? 64 : ((di.has_vex && di.vex_L >= 1) ? 32 : 16);
        int count = vl / 8;
        uint8_t d[64], s[64]; memcpy(d, cpu.zmm[src1], vl); memcpy(s, cpu.zmm[src2], vl);
        uint64_t *dq = (uint64_t*)d, *sq = (uint64_t*)s;
        for (int i = 0; i < count; i++) dq[i] = dq[i] - sq[i];
        memcpy(cpu.zmm[dst], d, vl);
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::PADDSB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i = 0; i < 16; i++) { int16_t r = (int8_t)d[i] + (int8_t)s[i]; if(r>127) r=127; if(r<-128) r=-128; d[i]=(uint8_t)(int8_t)r; }
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::PADDSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw = (int16_t*)d, *sw = (int16_t*)s;
        for (int i = 0; i < 8; i++) { int32_t r = (int32_t)dw[i] + sw[i]; if(r>32767) r=32767; if(r<-32768) r=-32768; dw[i]=(int16_t)r; }
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::PADDUSB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i = 0; i < 16; i++) { uint16_t r = (uint16_t)d[i] + s[i]; if(r>255) r=255; d[i]=(uint8_t)r; }
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::PADDUSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint16_t *dw = (uint16_t*)d, *sw = (uint16_t*)s;
        for (int i = 0; i < 8; i++) { uint32_t r = (uint32_t)dw[i] + sw[i]; if(r>65535) r=65535; dw[i]=(uint16_t)r; }
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::PSUBSB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i = 0; i < 16; i++) { int16_t r = (int8_t)d[i] - (int8_t)s[i]; if(r>127) r=127; if(r<-128) r=-128; d[i]=(uint8_t)(int8_t)r; }
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::PSUBSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw = (int16_t*)d, *sw = (int16_t*)s;
        for (int i = 0; i < 8; i++) { int32_t r = (int32_t)dw[i] - sw[i]; if(r>32767) r=32767; if(r<-32768) r=-32768; dw[i]=(int16_t)r; }
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::PSUBUSB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i = 0; i < 16; i++) { int16_t r = (int16_t)d[i] - s[i]; if(r<0) r=0; d[i]=(uint8_t)r; }
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::PSUBUSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint16_t *dw = (uint16_t*)d, *sw = (uint16_t*)s;
        for (int i = 0; i < 8; i++) { int32_t r = (int32_t)dw[i] - sw[i]; if(r<0) r=0; dw[i]=(uint16_t)r; }
        memcpy(cpu.zmm[dst], d, 16);
        break;
    }
    case Mnemonic::PMULLW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw = (int16_t*)d, *sw = (int16_t*)s;
        for (int i = 0; i < 8; i++) dw[i] = (int16_t)(dw[i] * sw[i]);
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMULLD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int32_t *dd = (int32_t*)d, *sd = (int32_t*)s;
        for (int i = 0; i < 4; i++) dd[i] = dd[i] * sd[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMULHW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw = (int16_t*)d, *sw = (int16_t*)s;
        for (int i = 0; i < 8; i++) dw[i] = (int16_t)(((int32_t)dw[i] * sw[i]) >> 16);
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMULHUW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint16_t *dw = (uint16_t*)d, *sw = (uint16_t*)s;
        for (int i = 0; i < 8; i++) dw[i] = (uint16_t)(((uint32_t)dw[i] * sw[i]) >> 16);
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMADDWD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw = (int16_t*)d, *sw = (int16_t*)s; int32_t r[4];
        for (int i = 0; i < 4; i++) r[i] = (int32_t)dw[i*2] * sw[i*2] + (int32_t)dw[i*2+1] * sw[i*2+1];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMULUDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint32_t *dd = (uint32_t*)d, *sd = (uint32_t*)s; uint64_t r[2];
        r[0] = (uint64_t)dd[0] * sd[0]; r[1] = (uint64_t)dd[2] * sd[2];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMULDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int32_t *dd = (int32_t*)d, *sd = (int32_t*)s; int64_t r[2];
        r[0] = (int64_t)dd[0] * sd[0]; r[1] = (int64_t)dd[2] * sd[2];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PCMPEQB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i = 0; i < 16; i++) r[i] = (d[i] == s[i]) ? 0xFF : 0;
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PCMPEQW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw = (int16_t*)d, *sw = (int16_t*)s; int16_t *rw = (int16_t*)r;
        for (int i = 0; i < 8; i++) rw[i] = (dw[i] == sw[i]) ? -1 : 0;
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PCMPEQD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int32_t *dd = (int32_t*)d, *sd = (int32_t*)s; int32_t *rd = (int32_t*)r;
        for (int i = 0; i < 4; i++) rd[i] = (dd[i] == sd[i]) ? -1 : 0;
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PCMPEQQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int64_t *dq = (int64_t*)d, *sq = (int64_t*)s; int64_t *rq = (int64_t*)r;
        for (int i = 0; i < 2; i++) rq[i] = (dq[i] == sq[i]) ? -1LL : 0;
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PCMPGTB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i = 0; i < 16; i++) r[i] = ((int8_t)d[i] > (int8_t)s[i]) ? 0xFF : 0;
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PCMPGTW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw = (int16_t*)d, *sw = (int16_t*)s; int16_t *rw = (int16_t*)r;
        for (int i = 0; i < 8; i++) rw[i] = (dw[i] > sw[i]) ? -1 : 0;
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PCMPGTD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int32_t *dd = (int32_t*)d, *sd = (int32_t*)s; int32_t *rd = (int32_t*)r;
        for (int i = 0; i < 4; i++) rd[i] = (dd[i] > sd[i]) ? -1 : 0;
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PCMPGTQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int64_t *dq = (int64_t*)d, *sq = (int64_t*)s; int64_t *rq = (int64_t*)r;
        for (int i = 0; i < 2; i++) rq[i] = (dq[i] > sq[i]) ? -1LL : 0;
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PAND: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint64_t d0,d1,s0,s1; memcpy(&d0,&cpu.zmm[dst][0],8); memcpy(&d1,&cpu.zmm[dst][1],8); memcpy(&s0,&cpu.zmm[src][0],8); memcpy(&s1,&cpu.zmm[src][1],8);
        d0&=s0; d1&=s1; memcpy(&cpu.zmm[dst][0],&d0,8); memcpy(&cpu.zmm[dst][1],&d1,8); break;
    }
    case Mnemonic::PANDN: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint64_t d0,d1,s0,s1; memcpy(&d0,&cpu.zmm[dst][0],8); memcpy(&d1,&cpu.zmm[dst][1],8); memcpy(&s0,&cpu.zmm[src][0],8); memcpy(&s1,&cpu.zmm[src][1],8);
        d0=(~d0)&s0; d1=(~d1)&s1; memcpy(&cpu.zmm[dst][0],&d0,8); memcpy(&cpu.zmm[dst][1],&d1,8); break;
    }
    case Mnemonic::POR: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint64_t d0,d1,s0,s1; memcpy(&d0,&cpu.zmm[dst][0],8); memcpy(&d1,&cpu.zmm[dst][1],8); memcpy(&s0,&cpu.zmm[src][0],8); memcpy(&s1,&cpu.zmm[src][1],8);
        d0|=s0; d1|=s1; memcpy(&cpu.zmm[dst][0],&d0,8); memcpy(&cpu.zmm[dst][1],&d1,8); break;
    }
    case Mnemonic::ANDNPS: case Mnemonic::ANDNPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint64_t d0,d1,s0,s1; memcpy(&d0,&cpu.zmm[dst][0],8); memcpy(&d1,&cpu.zmm[dst][1],8); memcpy(&s0,&cpu.zmm[src][0],8); memcpy(&s1,&cpu.zmm[src][1],8);
        d0=(~d0)&s0; d1=(~d1)&s1; memcpy(&cpu.zmm[dst][0],&d0,8); memcpy(&cpu.zmm[dst][1],&d1,8); break;
    }
    case Mnemonic::PSHUFD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t imm = (uint8_t)di.immediate;
        uint32_t s[4], r[4]; memcpy(s, cpu.zmm[src], 16);
        r[0] = s[imm & 3]; r[1] = s[(imm>>2) & 3]; r[2] = s[(imm>>4) & 3]; r[3] = s[(imm>>6) & 3];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PSHUFHW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t imm = (uint8_t)di.immediate; uint16_t s[8], r[8]; memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<4;i++) r[i]=s[i]; // low unchanged
        r[4]=s[4+(imm&3)]; r[5]=s[4+((imm>>2)&3)]; r[6]=s[4+((imm>>4)&3)]; r[7]=s[4+((imm>>6)&3)];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PSHUFLW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t imm = (uint8_t)di.immediate; uint16_t s[8], r[8]; memcpy(s, cpu.zmm[src], 16);
        r[0]=s[imm&3]; r[1]=s[(imm>>2)&3]; r[2]=s[(imm>>4)&3]; r[3]=s[(imm>>6)&3];
        for (int i=4;i<8;i++) r[i]=s[i]; // high unchanged
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PSHUFB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<16;i++) r[i] = (s[i] & 0x80) ? 0 : d[s[i] & 0xF];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PUNPCKLBW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<8;i++) { r[i*2]=d[i]; r[i*2+1]=s[i]; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PUNPCKHBW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<8;i++) { r[i*2]=d[8+i]; r[i*2+1]=s[8+i]; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PUNPCKLWD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint16_t d[8], s[8], r[8]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<4;i++) { r[i*2]=d[i]; r[i*2+1]=s[i]; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PUNPCKHWD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint16_t d[8], s[8], r[8]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<4;i++) { r[i*2]=d[4+i]; r[i*2+1]=s[4+i]; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PUNPCKLDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint32_t d[4], s[4], r[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        r[0]=d[0]; r[1]=s[0]; r[2]=d[1]; r[3]=s[1];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PUNPCKHDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint32_t d[4], s[4], r[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        r[0]=d[2]; r[1]=s[2]; r[2]=d[3]; r[3]=s[3];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PUNPCKLQDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][1] = cpu.zmm[src][0]; break;
    }
    case Mnemonic::PUNPCKHQDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = cpu.zmm[dst][1]; cpu.zmm[dst][1] = cpu.zmm[src][1]; break;
    }
    case Mnemonic::PMOVMSKB: {
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint32_t msk = 0;
        for (int i=0;i<16;i++) if (s[i] & 0x80) msk |= (1 << i);
        write_operand(cpu, di, 0, msk, bits); break;
    }
    case Mnemonic::PSLLW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t count;
        if (di.desc->operands[1].addr == AddrMode::Immediate) count = (uint64_t)di.immediate;
        else { int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t v; memcpy(&v, &cpu.zmm[src][0], 8); count = v; }
        uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        uint16_t *dw = (uint16_t*)d;
        if (count >= 16) { memset(d, 0, 16); } else { for (int i=0;i<8;i++) dw[i] = (uint16_t)(dw[i] << count); }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSLLD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t count;
        if (di.desc->operands[1].addr == AddrMode::Immediate) count = (uint64_t)di.immediate;
        else { int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t v; memcpy(&v, &cpu.zmm[src][0], 8); count = v; }
        uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        uint32_t *dd = (uint32_t*)d;
        if (count >= 32) { memset(d, 0, 16); } else { for (int i=0;i<4;i++) dd[i] = (uint32_t)(dd[i] << count); }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSLLQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t count;
        if (di.desc->operands[1].addr == AddrMode::Immediate) count = (uint64_t)di.immediate;
        else { int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t v; memcpy(&v, &cpu.zmm[src][0], 8); count = v; }
        uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        uint64_t *dq = (uint64_t*)d;
        if (count >= 64) { memset(d, 0, 16); } else { for (int i=0;i<2;i++) dq[i] = dq[i] << count; }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSRLW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t count;
        if (di.desc->operands[1].addr == AddrMode::Immediate) count = (uint64_t)di.immediate;
        else { int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t v; memcpy(&v, &cpu.zmm[src][0], 8); count = v; }
        uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        uint16_t *dw = (uint16_t*)d;
        if (count >= 16) { memset(d, 0, 16); } else { for (int i=0;i<8;i++) dw[i] = (uint16_t)(dw[i] >> count); }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSRLD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t count;
        if (di.desc->operands[1].addr == AddrMode::Immediate) count = (uint64_t)di.immediate;
        else { int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t v; memcpy(&v, &cpu.zmm[src][0], 8); count = v; }
        uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        uint32_t *dd = (uint32_t*)d;
        if (count >= 32) { memset(d, 0, 16); } else { for (int i=0;i<4;i++) dd[i] = (uint32_t)(dd[i] >> count); }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSRLQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t count;
        if (di.desc->operands[1].addr == AddrMode::Immediate) count = (uint64_t)di.immediate;
        else { int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t v; memcpy(&v, &cpu.zmm[src][0], 8); count = v; }
        uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        uint64_t *dq = (uint64_t*)d;
        if (count >= 64) { memset(d, 0, 16); } else { for (int i=0;i<2;i++) dq[i] = dq[i] >> count; }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSRAW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t count;
        if (di.desc->operands[1].addr == AddrMode::Immediate) count = (uint64_t)di.immediate;
        else { int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t v; memcpy(&v, &cpu.zmm[src][0], 8); count = v; }
        uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        int16_t *dw = (int16_t*)d;
        if (count >= 16) { for (int i=0;i<8;i++) dw[i] = dw[i] >> 15; } else { for (int i=0;i<8;i++) dw[i] = (int16_t)(dw[i] >> count); }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSRAD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t count;
        if (di.desc->operands[1].addr == AddrMode::Immediate) count = (uint64_t)di.immediate;
        else { int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t v; memcpy(&v, &cpu.zmm[src][0], 8); count = v; }
        uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        int32_t *dd = (int32_t*)d;
        if (count >= 32) { for (int i=0;i<4;i++) dd[i] = dd[i] >> 31; } else { for (int i=0;i<4;i++) dd[i] = dd[i] >> count; }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSLLDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int count = (int)di.immediate; if (count > 16) count = 16;
        uint8_t d[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memset(r, 0, 16);
        for (int i = count; i < 16; i++) r[i] = d[i - count];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PSRLDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int count = (int)di.immediate; if (count > 16) count = 16;
        uint8_t d[16], r[16]; memcpy(d, cpu.zmm[dst], 16); memset(r, 0, 16);
        for (int i = 0; i < 16 - count; i++) r[i] = d[i + count];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PINSRB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); uint8_t d[16]; memcpy(d, cpu.zmm[dst], 16);
        uint8_t val = (uint8_t)read_operand(cpu, di, 1, 32); int idx = (int)di.immediate & 15;
        d[idx] = val; memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PINSRW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); uint16_t d[8]; memcpy(d, cpu.zmm[dst], 16);
        uint16_t val = (uint16_t)read_operand(cpu, di, 1, 32); int idx = (int)di.immediate & 7;
        d[idx] = val; memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PINSRD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); uint32_t d[4]; memcpy(d, cpu.zmm[dst], 16);
        uint32_t val = (uint32_t)read_operand(cpu, di, 1, 32); int idx = (int)di.immediate & 3;
        d[idx] = val; memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PINSRQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); uint64_t d[2]; memcpy(d, cpu.zmm[dst], 16);
        uint64_t val = read_operand(cpu, di, 1, 64); int idx = (int)di.immediate & 1;
        d[idx] = val; memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PEXTRB: {
        int src = xmm_reg_index(di, di.desc->operands[1]); uint8_t s[16]; memcpy(s, cpu.zmm[src], 16);
        write_operand(cpu, di, 0, s[(int)di.immediate & 15], 32); break;
    }
    case Mnemonic::PEXTRW: {
        int src = xmm_reg_index(di, di.desc->operands[1]); uint16_t s[8]; memcpy(s, cpu.zmm[src], 16);
        write_operand(cpu, di, 0, s[(int)di.immediate & 7], 32); break;
    }
    case Mnemonic::PEXTRD: {
        int src = xmm_reg_index(di, di.desc->operands[1]); uint32_t s[4]; memcpy(s, cpu.zmm[src], 16);
        write_operand(cpu, di, 0, s[(int)di.immediate & 3], 32); break;
    }
    case Mnemonic::PEXTRQ: {
        int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t s[2]; memcpy(s, cpu.zmm[src], 16);
        write_operand(cpu, di, 0, s[(int)di.immediate & 1], 64); break;
    }
    case Mnemonic::PABSB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16], r[16]; memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<16;i++) r[i] = (uint8_t)(((int8_t)s[i] < 0) ? -(int8_t)s[i] : s[i]);
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PABSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16], r[16]; memcpy(s, cpu.zmm[src], 16);
        int16_t *sw = (int16_t*)s; uint16_t *rw = (uint16_t*)r;
        for (int i=0;i<8;i++) rw[i] = (uint16_t)((sw[i] < 0) ? -sw[i] : sw[i]);
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PABSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16], r[16]; memcpy(s, cpu.zmm[src], 16);
        int32_t *sd = (int32_t*)s; uint32_t *rd = (uint32_t*)r;
        for (int i=0;i<4;i++) rd[i] = (uint32_t)((sd[i] < 0) ? -sd[i] : sd[i]);
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PAVGB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<16;i++) d[i] = (uint8_t)(((uint16_t)d[i] + s[i] + 1) >> 1);
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PAVGW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint16_t d[8], s[8]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<8;i++) d[i] = (uint16_t)(((uint32_t)d[i] + s[i] + 1) >> 1);
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMINSB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<16;i++) if ((int8_t)d[i] > (int8_t)s[i]) d[i]=s[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMAXSB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<16;i++) if ((int8_t)d[i] < (int8_t)s[i]) d[i]=s[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMINUB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<16;i++) if (d[i] > s[i]) d[i]=s[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMAXUB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<16;i++) if (d[i] < s[i]) d[i]=s[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMINSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw=(int16_t*)d, *sw=(int16_t*)s; for (int i=0;i<8;i++) if (dw[i] > sw[i]) dw[i]=sw[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMAXSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int16_t *dw=(int16_t*)d, *sw=(int16_t*)s; for (int i=0;i<8;i++) if (dw[i] < sw[i]) dw[i]=sw[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMINUW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint16_t *dw=(uint16_t*)d, *sw=(uint16_t*)s; for (int i=0;i<8;i++) if (dw[i] > sw[i]) dw[i]=sw[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMAXUW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint16_t *dw=(uint16_t*)d, *sw=(uint16_t*)s; for (int i=0;i<8;i++) if (dw[i] < sw[i]) dw[i]=sw[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMINSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int32_t *dd=(int32_t*)d, *sd=(int32_t*)s; for (int i=0;i<4;i++) if (dd[i] > sd[i]) dd[i]=sd[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMAXSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        int32_t *dd=(int32_t*)d, *sd=(int32_t*)s; for (int i=0;i<4;i++) if (dd[i] < sd[i]) dd[i]=sd[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMINUD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint32_t *dd=(uint32_t*)d, *sd=(uint32_t*)s; for (int i=0;i<4;i++) if (dd[i] > sd[i]) dd[i]=sd[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMAXUD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint32_t *dd=(uint32_t*)d, *sd=(uint32_t*)s; for (int i=0;i<4;i++) if (dd[i] < sd[i]) dd[i]=sd[i];
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PSADBW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16], s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        uint64_t r[2] = {0,0};
        for (int i=0;i<8;i++) r[0] += (d[i]>s[i]) ? d[i]-s[i] : s[i]-d[i];
        for (int i=8;i<16;i++) r[1] += (d[i]>s[i]) ? d[i]-s[i] : s[i]-d[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PTEST: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint64_t d0,d1,s0,s1; memcpy(&d0,&cpu.zmm[dst][0],8); memcpy(&d1,&cpu.zmm[dst][1],8); memcpy(&s0,&cpu.zmm[src][0],8); memcpy(&s1,&cpu.zmm[src][1],8);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_SF | RFLAG_OF | RFLAG_PF | RFLAG_AF);
        if ((d0 & s0) == 0 && (d1 & s1) == 0) cpu.rflags |= RFLAG_ZF;
        if (((~d0) & s0) == 0 && ((~d1) & s1) == 0) cpu.rflags |= RFLAG_CF;
        break;
    }
    case Mnemonic::PACKSSWB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        int16_t d[8], s[8]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16); int8_t r[16];
        for (int i=0;i<8;i++) { int16_t v=d[i]; if(v>127) v=127; if(v<-128) v=-128; r[i]=(int8_t)v; }
        for (int i=0;i<8;i++) { int16_t v=s[i]; if(v>127) v=127; if(v<-128) v=-128; r[8+i]=(int8_t)v; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PACKSSDW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        int32_t d[4], s[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16); int16_t r[8];
        for (int i=0;i<4;i++) { int32_t v=d[i]; if(v>32767) v=32767; if(v<-32768) v=-32768; r[i]=(int16_t)v; }
        for (int i=0;i<4;i++) { int32_t v=s[i]; if(v>32767) v=32767; if(v<-32768) v=-32768; r[4+i]=(int16_t)v; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PACKUSWB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        int16_t d[8], s[8]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16); uint8_t r[16];
        for (int i=0;i<8;i++) { int16_t v=d[i]; if(v>255) v=255; if(v<0) v=0; r[i]=(uint8_t)v; }
        for (int i=0;i<8;i++) { int16_t v=s[i]; if(v>255) v=255; if(v<0) v=0; r[8+i]=(uint8_t)v; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PACKUSDW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        int32_t d[4], s[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16); uint16_t r[8];
        for (int i=0;i<4;i++) { int32_t v=d[i]; if(v>65535) v=65535; if(v<0) v=0; r[i]=(uint16_t)v; }
        for (int i=0;i<4;i++) { int32_t v=s[i]; if(v>65535) v=65535; if(v<0) v=0; r[4+i]=(uint16_t)v; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PALIGNR: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        int shift = (int)di.immediate; uint8_t d[16], s[16], concat[32], r[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        memcpy(concat, s, 16); memcpy(concat+16, d, 16);
        if (shift >= 32) memset(r, 0, 16);
        else { for (int i=0;i<16;i++) r[i] = (shift+i < 32) ? concat[shift+i] : 0; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMULHRSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        int16_t d[8], s[8]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        for (int i=0;i<8;i++) { int32_t t = ((int32_t)d[i] * s[i] + 0x4000) >> 15; d[i] = (int16_t)t; }
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::PMADDUBSW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t d[16]; int8_t s[16]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16); int16_t r[8];
        for (int i=0;i<8;i++) { int32_t t = (int32_t)d[i*2]*s[i*2] + (int32_t)d[i*2+1]*s[i*2+1]; if(t>32767) t=32767; if(t<-32768) t=-32768; r[i]=(int16_t)t; }
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::HADDPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float d[4], s[4], r[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        r[0]=d[0]+d[1]; r[1]=d[2]+d[3]; r[2]=s[0]+s[1]; r[3]=s[2]+s[3];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::HADDPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = cpu.zmm[dst][0] + cpu.zmm[dst][1]; cpu.zmm[dst][1] = cpu.zmm[src][0] + cpu.zmm[src][1]; break;
    }
    case Mnemonic::HSUBPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float d[4], s[4], r[4]; memcpy(d, cpu.zmm[dst], 16); memcpy(s, cpu.zmm[src], 16);
        r[0]=d[0]-d[1]; r[1]=d[2]-d[3]; r[2]=s[0]-s[1]; r[3]=s[2]-s[3];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::HSUBPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = cpu.zmm[dst][0] - cpu.zmm[dst][1]; cpu.zmm[dst][1] = cpu.zmm[src][0] - cpu.zmm[src][1]; break;
    }
    case Mnemonic::ROUNDSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float f; memcpy(&f, &cpu.zmm[src][0], 4); int mode = (int)di.immediate & 3;
        if (mode==0) f=roundf(f); else if (mode==1) f=floorf(f); else if (mode==2) f=ceilf(f); else f=truncf(f);
        memcpy(&cpu.zmm[dst][0], &f, 4); break;
    }
    case Mnemonic::ROUNDSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        double d = cpu.zmm[src][0]; int mode = (int)di.immediate & 3;
        if (mode==0) d=round(d); else if (mode==1) d=floor(d); else if (mode==2) d=ceil(d); else d=trunc(d);
        cpu.zmm[dst][0] = d; break;
    }
    case Mnemonic::ROUNDPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float f[4]; memcpy(f, cpu.zmm[src], 16); int mode = (int)di.immediate & 3;
        for (int i=0;i<4;i++) { if (mode==0) f[i]=roundf(f[i]); else if (mode==1) f[i]=floorf(f[i]); else if (mode==2) f[i]=ceilf(f[i]); else f[i]=truncf(f[i]); }
        memcpy(cpu.zmm[dst], f, 16); break;
    }
    case Mnemonic::ROUNDPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        double d[2]; d[0]=cpu.zmm[src][0]; d[1]=cpu.zmm[src][1]; int mode = (int)di.immediate & 3;
        for (int i=0;i<2;i++) { if (mode==0) d[i]=round(d[i]); else if (mode==1) d[i]=floor(d[i]); else if (mode==2) d[i]=ceil(d[i]); else d[i]=trunc(d[i]); }
        cpu.zmm[dst][0]=d[0]; cpu.zmm[dst][1]=d[1]; break;
    }
    case Mnemonic::SQRTPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float f[4]; memcpy(f, cpu.zmm[src], 16); for (int i=0;i<4;i++) f[i]=sqrtf(f[i]);
        memcpy(cpu.zmm[dst], f, 16); break;
    }
    case Mnemonic::SQRTPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = sqrt(cpu.zmm[src][0]); cpu.zmm[dst][1] = sqrt(cpu.zmm[src][1]); break;
    }
    case Mnemonic::RCPPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float f[4]; memcpy(f, cpu.zmm[src], 16); for (int i=0;i<4;i++) f[i]=1.0f/f[i];
        memcpy(cpu.zmm[dst], f, 16); break;
    }
    case Mnemonic::RCPSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float f; memcpy(&f, &cpu.zmm[src][0], 4); f=1.0f/f; memcpy(&cpu.zmm[dst][0], &f, 4); break;
    }
    case Mnemonic::RSQRTPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float f[4]; memcpy(f, cpu.zmm[src], 16); for (int i=0;i<4;i++) f[i]=1.0f/sqrtf(f[i]);
        memcpy(cpu.zmm[dst], f, 16); break;
    }
    case Mnemonic::RSQRTSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float f; memcpy(&f, &cpu.zmm[src][0], 4); f=1.0f/sqrtf(f); memcpy(&cpu.zmm[dst][0], &f, 4); break;
    }
    case Mnemonic::MOVDDUP: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        cpu.zmm[dst][0] = cpu.zmm[src][0]; cpu.zmm[dst][1] = cpu.zmm[src][0]; break;
    }
    case Mnemonic::MOVSHDUP: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float s[4], r[4]; memcpy(s, cpu.zmm[src], 16); r[0]=s[1]; r[1]=s[1]; r[2]=s[3]; r[3]=s[3];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::MOVSLDUP: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        float s[4], r[4]; memcpy(s, cpu.zmm[src], 16); r[0]=s[0]; r[1]=s[0]; r[2]=s[2]; r[3]=s[2];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::MOVNTPS: case Mnemonic::MOVNTPD: case Mnemonic::MOVNTDQ: {
        int src = xmm_reg_index(di, di.desc->operands[1]); uint64_t ea = compute_ea(cpu, di);
        if (!mem_check(cpu, ea, 16)) return StepResult::MemFault;
        memcpy(cpu.mem + ea, cpu.zmm[src], 16); break;
    }
    case Mnemonic::MOVNTDQA: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); uint64_t ea = compute_ea(cpu, di);
        if (!mem_check(cpu, ea, 16)) return StepResult::MemFault;
        memcpy(cpu.zmm[dst], cpu.mem + ea, 16); break;
    }
    case Mnemonic::EXTRACTPS: {
        int src = xmm_reg_index(di, di.desc->operands[1]); float f[4]; memcpy(f, cpu.zmm[src], 16);
        uint32_t val; memcpy(&val, &f[(int)di.immediate & 3], 4);
        write_operand(cpu, di, 0, val, 32); break;
    }
    case Mnemonic::INSERTPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint8_t imm = (uint8_t)di.immediate; float d[4]; memcpy(d, cpu.zmm[dst], 16);
        if (is_xmm_mem(di, di.desc->operands[1])) {
            uint64_t ea = compute_ea(cpu, di); float f; memcpy(&f, cpu.mem + ea, 4);
            d[(imm >> 4) & 3] = f;
        } else {
            int src = xmm_reg_index(di, di.desc->operands[1]); float s[4]; memcpy(s, cpu.zmm[src], 16);
            d[(imm >> 4) & 3] = s[(imm >> 6) & 3];
        }
        for (int i=0;i<4;i++) if (imm & (1 << i)) d[i] = 0.0f;
        memcpy(cpu.zmm[dst], d, 16); break;
    }
    case Mnemonic::VFMADD132PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
                cpu.zmm[dst][i] = a*b + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=d[i], b=s[i], c=v[i];
                d[i] = a*b + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD132SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[dst][0]; b = cpu.zmm[src3][0]; c = cpu.zmm[vvvv][0];
            cpu.zmm[dst][0] = a*b + c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[dst][0], 4); memcpy(&b, &cpu.zmm[src3][0], 4); memcpy(&c, &cpu.zmm[vvvv][0], 4);
            float r = a*b + c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD213PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
                cpu.zmm[dst][i] = a*b + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=d[i], c=s[i];
                d[i] = a*b + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD213SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[vvvv][0]; b = cpu.zmm[dst][0]; c = cpu.zmm[src3][0];
            cpu.zmm[dst][0] = a*b + c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[vvvv][0], 4); memcpy(&b, &cpu.zmm[dst][0], 4); memcpy(&c, &cpu.zmm[src3][0], 4);
            float r = a*b + c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD231PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
                cpu.zmm[dst][i] = a*b + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=s[i], c=d[i];
                d[i] = a*b + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD231SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[vvvv][0]; b = cpu.zmm[src3][0]; c = cpu.zmm[dst][0];
            cpu.zmm[dst][0] = a*b + c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[vvvv][0], 4); memcpy(&b, &cpu.zmm[src3][0], 4); memcpy(&c, &cpu.zmm[dst][0], 4);
            float r = a*b + c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB132PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
                cpu.zmm[dst][i] = a*b - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=d[i], b=s[i], c=v[i];
                d[i] = a*b - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB132SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[dst][0]; b = cpu.zmm[src3][0]; c = cpu.zmm[vvvv][0];
            cpu.zmm[dst][0] = a*b - c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[dst][0], 4); memcpy(&b, &cpu.zmm[src3][0], 4); memcpy(&c, &cpu.zmm[vvvv][0], 4);
            float r = a*b - c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB213PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
                cpu.zmm[dst][i] = a*b - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=d[i], c=s[i];
                d[i] = a*b - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB213SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[vvvv][0]; b = cpu.zmm[dst][0]; c = cpu.zmm[src3][0];
            cpu.zmm[dst][0] = a*b - c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[vvvv][0], 4); memcpy(&b, &cpu.zmm[dst][0], 4); memcpy(&c, &cpu.zmm[src3][0], 4);
            float r = a*b - c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB231PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
                cpu.zmm[dst][i] = a*b - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=s[i], c=d[i];
                d[i] = a*b - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB231SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[vvvv][0]; b = cpu.zmm[src3][0]; c = cpu.zmm[dst][0];
            cpu.zmm[dst][0] = a*b - c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[vvvv][0], 4); memcpy(&b, &cpu.zmm[src3][0], 4); memcpy(&c, &cpu.zmm[dst][0], 4);
            float r = a*b - c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD132PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
                cpu.zmm[dst][i] = -(a*b) + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=d[i], b=s[i], c=v[i];
                d[i] = -(a*b) + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD132SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[dst][0]; b = cpu.zmm[src3][0]; c = cpu.zmm[vvvv][0];
            cpu.zmm[dst][0] = -(a*b) + c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[dst][0], 4); memcpy(&b, &cpu.zmm[src3][0], 4); memcpy(&c, &cpu.zmm[vvvv][0], 4);
            float r = -(a*b) + c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD213PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
                cpu.zmm[dst][i] = -(a*b) + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=d[i], c=s[i];
                d[i] = -(a*b) + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD213SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[vvvv][0]; b = cpu.zmm[dst][0]; c = cpu.zmm[src3][0];
            cpu.zmm[dst][0] = -(a*b) + c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[vvvv][0], 4); memcpy(&b, &cpu.zmm[dst][0], 4); memcpy(&c, &cpu.zmm[src3][0], 4);
            float r = -(a*b) + c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD231PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
                cpu.zmm[dst][i] = -(a*b) + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=s[i], c=d[i];
                d[i] = -(a*b) + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD231SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[vvvv][0]; b = cpu.zmm[src3][0]; c = cpu.zmm[dst][0];
            cpu.zmm[dst][0] = -(a*b) + c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[vvvv][0], 4); memcpy(&b, &cpu.zmm[src3][0], 4); memcpy(&c, &cpu.zmm[dst][0], 4);
            float r = -(a*b) + c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB132PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
                cpu.zmm[dst][i] = -(a*b) - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=d[i], b=s[i], c=v[i];
                d[i] = -(a*b) - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB132SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[dst][0]; b = cpu.zmm[src3][0]; c = cpu.zmm[vvvv][0];
            cpu.zmm[dst][0] = -(a*b) - c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[dst][0], 4); memcpy(&b, &cpu.zmm[src3][0], 4); memcpy(&c, &cpu.zmm[vvvv][0], 4);
            float r = -(a*b) - c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB213PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
                cpu.zmm[dst][i] = -(a*b) - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=d[i], c=s[i];
                d[i] = -(a*b) - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB213SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[vvvv][0]; b = cpu.zmm[dst][0]; c = cpu.zmm[src3][0];
            cpu.zmm[dst][0] = -(a*b) - c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[vvvv][0], 4); memcpy(&b, &cpu.zmm[dst][0], 4); memcpy(&c, &cpu.zmm[src3][0], 4);
            float r = -(a*b) - c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB231PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
                cpu.zmm[dst][i] = -(a*b) - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=s[i], c=d[i];
                d[i] = -(a*b) - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB231SS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        bool use_double = di.vex_W; // W=1 → PD/SD
        if (use_double) {
            double a, b, c;
            a = cpu.zmm[vvvv][0]; b = cpu.zmm[src3][0]; c = cpu.zmm[dst][0];
            cpu.zmm[dst][0] = -(a*b) - c;
        } else {
            float a, b, c;
            memcpy(&a, &cpu.zmm[vvvv][0], 4); memcpy(&b, &cpu.zmm[src3][0], 4); memcpy(&c, &cpu.zmm[dst][0], 4);
            float r = -(a*b) - c; memcpy(&cpu.zmm[dst][0], &r, 4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADDSUB132PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        if (di.vex_W) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
                cpu.zmm[dst][i] = (i & 1) ? a*b + c : a*b - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=d[i], b=s[i], c=v[i];
                d[i] = (i & 1) ? a*b + c : a*b - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADDSUB213PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        if (di.vex_W) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
                cpu.zmm[dst][i] = (i & 1) ? a*b + c : a*b - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=d[i], c=s[i];
                d[i] = (i & 1) ? a*b + c : a*b - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADDSUB231PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        if (di.vex_W) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
                cpu.zmm[dst][i] = (i & 1) ? a*b + c : a*b - c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=s[i], c=d[i];
                d[i] = (i & 1) ? a*b + c : a*b - c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUBADD132PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        if (di.vex_W) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
                cpu.zmm[dst][i] = (i & 1) ? a*b - c : a*b + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=d[i], b=s[i], c=v[i];
                d[i] = (i & 1) ? a*b - c : a*b + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUBADD213PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        if (di.vex_W) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
                cpu.zmm[dst][i] = (i & 1) ? a*b - c : a*b + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=d[i], c=s[i];
                d[i] = (i & 1) ? a*b - c : a*b + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUBADD231PS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        if (di.vex_W) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
                cpu.zmm[dst][i] = (i & 1) ? a*b - c : a*b + c;
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            float d[16], v[16], s[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(v, cpu.zmm[vvvv], count*4); memcpy(s, cpu.zmm[src3], count*4);
            for (int i = 0; i < count; i++) {
                float a=v[i], b=s[i], c=d[i];
                d[i] = (i & 1) ? a*b - c : a*b + c;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VBROADCASTSS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        float f; memcpy(&f, &cpu.zmm[src][0], 4);
        int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
        float r[16]; for (int i = 0; i < count; i++) r[i] = f;
        memcpy(cpu.zmm[dst], r, count * 4);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VBROADCASTSD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        double d = cpu.zmm[src][0];
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) cpu.zmm[dst][i] = d;
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VBROADCASTF128: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        uint64_t ea = compute_ea(cpu, di);
        if (!mem_check(cpu, ea, 16)) return StepResult::MemFault;
        memcpy(&cpu.zmm[dst][0], cpu.mem + ea, 16);
        memcpy(&cpu.zmm[dst][2], cpu.mem + ea, 16);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPBROADCASTD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint32_t val; memcpy(&val, &cpu.zmm[src][0], 4);
        int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
        uint32_t r[16]; for (int i = 0; i < count; i++) r[i] = val;
        memcpy(cpu.zmm[dst], r, count * 4);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPBROADCASTQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint64_t val; memcpy(&val, &cpu.zmm[src][0], 8);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        uint64_t r[8]; for (int i = 0; i < count; i++) r[i] = val;
        memcpy(cpu.zmm[dst], r, count * 8);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPBROADCASTB: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t val; memcpy(&val, &cpu.zmm[src][0], 1);
        int count = (di.vex_L >= 2) ? 64 : ((di.vex_L >= 1) ? 32 : 16);
        uint8_t r[64]; for (int i = 0; i < count; i++) r[i] = val;
        memcpy(cpu.zmm[dst], r, count);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPBROADCASTW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint16_t val; memcpy(&val, &cpu.zmm[src][0], 2);
        int count = (di.vex_L >= 2) ? 32 : ((di.vex_L >= 1) ? 16 : 8);
        uint16_t r[32]; for (int i = 0; i < count; i++) r[i] = val;
        memcpy(cpu.zmm[dst], r, count * 2);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VINSERTF128: case Mnemonic::VINSERTI128: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = (int)di.vex_vvvv;
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        memcpy(cpu.zmm[dst], cpu.zmm[src1], 32); // copy full 256-bit from src1
        int lane = (int)di.immediate & 1;
        memcpy(&cpu.zmm[dst][lane * 2], cpu.zmm[src2], 16); // insert 128-bit
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VEXTRACTF128: case Mnemonic::VEXTRACTI128: {
        int src = xmm_reg_index(di, di.desc->operands[1]);
        int lane = (int)di.immediate & 1;
        if (is_xmm_mem(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            if (!mem_check(cpu, ea, 16)) return StepResult::MemFault;
            memcpy(cpu.mem + ea, &cpu.zmm[src][lane * 2], 16);
        } else {
            int dst = xmm_reg_index(di, di.desc->operands[0]);
            memcpy(cpu.zmm[dst], &cpu.zmm[src][lane * 2], 16);
            cpu.zmm[dst][2]=0; cpu.zmm[dst][3]=0; cpu.zmm[dst][4]=0; cpu.zmm[dst][5]=0; cpu.zmm[dst][6]=0; cpu.zmm[dst][7]=0;
        }
        break;
    }
    case Mnemonic::VPERM2F128: case Mnemonic::VPERM2I128: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = (int)di.vex_vvvv;
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        uint8_t imm = (uint8_t)di.immediate;
        double lane[4][2]; memcpy(lane[0], &cpu.zmm[src1][0], 16); memcpy(lane[1], &cpu.zmm[src1][2], 16);
        memcpy(lane[2], &cpu.zmm[src2][0], 16); memcpy(lane[3], &cpu.zmm[src2][2], 16);
        int sel0 = imm & 3, sel1 = (imm >> 4) & 3;
        if (imm & 0x08) memset(&cpu.zmm[dst][0], 0, 16); else memcpy(&cpu.zmm[dst][0], lane[sel0], 16);
        if (imm & 0x80) memset(&cpu.zmm[dst][2], 0, 16); else memcpy(&cpu.zmm[dst][2], lane[sel1], 16);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPERMD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int idx_reg = (int)di.vex_vvvv;
        int src = xmm_reg_index(di, di.desc->operands[2]);
        uint32_t idx[8], s[8], r[8]; memcpy(idx, cpu.zmm[idx_reg], 32); memcpy(s, cpu.zmm[src], 32);
        for (int i = 0; i < 8; i++) r[i] = s[idx[i] & 7];
        memcpy(cpu.zmm[dst], r, 32); vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPERMQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t imm = (uint8_t)di.immediate;
        double s[4]; memcpy(s, cpu.zmm[src], 32);
        cpu.zmm[dst][0]=s[imm&3]; cpu.zmm[dst][1]=s[(imm>>2)&3]; cpu.zmm[dst][2]=s[(imm>>4)&3]; cpu.zmm[dst][3]=s[(imm>>6)&3];
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPERMILPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = (int)di.vex_vvvv;
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 1) ? 8 : 4;
        float s[8]; memcpy(s, cpu.zmm[src1], count*4);
        uint32_t ctrl[8]; memcpy(ctrl, cpu.zmm[src2], count*4);
        float r[8]; for (int i = 0; i < count; i++) r[i] = s[(i & ~3) | (ctrl[i] & 3)];
        memcpy(cpu.zmm[dst], r, count*4); vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPERMILPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = (int)di.vex_vvvv;
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 1) ? 4 : 2;
        double s[4]; memcpy(s, cpu.zmm[src1], count*8);
        uint64_t ctrl[4]; memcpy(ctrl, cpu.zmm[src2], count*8);
        double r[4]; for (int i = 0; i < count; i++) r[i] = s[(i & ~1) | ((ctrl[i] >> 1) & 1)];
        memcpy(cpu.zmm[dst], r, count*8); vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::KMOVB: {
        // Simplified: treat as GPR ↔ opmask move
        int kreg = (di.modrm >> 3) & 7;
        uint64_t val = read_operand(cpu, di, 1, 8);
        cpu.opmask[kreg] = val & mask(8);
        break;
    }
    case Mnemonic::KMOVW: {
        // Simplified: treat as GPR ↔ opmask move
        int kreg = (di.modrm >> 3) & 7;
        uint64_t val = read_operand(cpu, di, 1, 16);
        cpu.opmask[kreg] = val & mask(16);
        break;
    }
    case Mnemonic::KMOVD: {
        // Simplified: treat as GPR ↔ opmask move
        int kreg = (di.modrm >> 3) & 7;
        uint64_t val = read_operand(cpu, di, 1, 32);
        cpu.opmask[kreg] = val & mask(32);
        break;
    }
    case Mnemonic::KMOVQ: {
        // Simplified: treat as GPR ↔ opmask move
        int kreg = (di.modrm >> 3) & 7;
        uint64_t val = read_operand(cpu, di, 1, 64);
        cpu.opmask[kreg] = val & mask(64);
        break;
    }
    case Mnemonic::KANDW: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] & cpu.opmask[k3]; break;
    }
    case Mnemonic::KANDD: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] & cpu.opmask[k3]; break;
    }
    case Mnemonic::KANDQ: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] & cpu.opmask[k3]; break;
    }
    case Mnemonic::KORW: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] | cpu.opmask[k3]; break;
    }
    case Mnemonic::KORD: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] | cpu.opmask[k3]; break;
    }
    case Mnemonic::KORQ: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] | cpu.opmask[k3]; break;
    }
    case Mnemonic::KXORW: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] ^ cpu.opmask[k3]; break;
    }
    case Mnemonic::KXORD: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] ^ cpu.opmask[k3]; break;
    }
    case Mnemonic::KXORQ: {
        int k1 = (di.modrm >> 3) & 7;
        int k2 = (int)di.vex_vvvv;
        int k3 = di.modrm & 7;
        cpu.opmask[k1] = cpu.opmask[k2] ^ cpu.opmask[k3]; break;
    }
    case Mnemonic::KANDNW: {
        int k1 = (di.modrm >> 3) & 7, k2 = (int)di.vex_vvvv, k3 = di.modrm & 7;
        cpu.opmask[k1] = (~cpu.opmask[k2]) & cpu.opmask[k3]; break;
    }
    case Mnemonic::KNOTW: {
        int k1 = (di.modrm >> 3) & 7, k2 = di.modrm & 7;
        cpu.opmask[k1] = ~cpu.opmask[k2]; break;
    }
    case Mnemonic::KNOTD: {
        int k1 = (di.modrm >> 3) & 7, k2 = di.modrm & 7;
        cpu.opmask[k1] = ~cpu.opmask[k2]; break;
    }
    case Mnemonic::KNOTQ: {
        int k1 = (di.modrm >> 3) & 7, k2 = di.modrm & 7;
        cpu.opmask[k1] = ~cpu.opmask[k2]; break;
    }
    case Mnemonic::KORTESTW: {
        int k1 = (di.modrm >> 3) & 7, k2 = di.modrm & 7;
        uint64_t val = cpu.opmask[k1] | cpu.opmask[k2];
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF);
        if (val == 0) cpu.rflags |= RFLAG_ZF;
        if (~val == 0) cpu.rflags |= RFLAG_CF;
        break;
    }
    case Mnemonic::KORTESTD: {
        int k1 = (di.modrm >> 3) & 7, k2 = di.modrm & 7;
        uint64_t val = cpu.opmask[k1] | cpu.opmask[k2];
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF);
        if (val == 0) cpu.rflags |= RFLAG_ZF;
        if (~val == 0) cpu.rflags |= RFLAG_CF;
        break;
    }
    case Mnemonic::KORTESTQ: {
        int k1 = (di.modrm >> 3) & 7, k2 = di.modrm & 7;
        uint64_t val = cpu.opmask[k1] | cpu.opmask[k2];
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF);
        if (val == 0) cpu.rflags |= RFLAG_ZF;
        if (~val == 0) cpu.rflags |= RFLAG_CF;
        break;
    }
    case Mnemonic::EMMS: {
        if (di.has_vex && di.vex_L >= 1) {
            // VZEROALL: zero all YMM/ZMM registers
            for (int r = 0; r < 16; r++) for (int i = 0; i < 8; i++) cpu.zmm[r][i] = 0;
        } else if (di.has_vex) {
            // VZEROUPPER: zero upper 128 bits of all YMM registers
            for (int r = 0; r < 16; r++) { cpu.zmm[r][2]=0; cpu.zmm[r][3]=0; cpu.zmm[r][4]=0; cpu.zmm[r][5]=0; cpu.zmm[r][6]=0; cpu.zmm[r][7]=0; }
        }
        break;
    }
    case Mnemonic::CRC32: {
        // Simplified CRC32C stub - just xor the value in
        uint32_t crc = (uint32_t)read_operand(cpu, di, 0, 32);
        int sbits = resolve_opsize(di, di.desc->operands[1]);
        uint64_t val = read_operand(cpu, di, 1, sbits);
        crc ^= (uint32_t)val;
        write_operand(cpu, di, 0, crc, 32);
        break;
    }
    case Mnemonic::PMOVSXBW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        int16_t *rw = (int16_t*)r; for (int i=0;i<8;i++) rw[i] = (int8_t)s[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVSXBD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        int32_t *rd = (int32_t*)r; for (int i=0;i<4;i++) rd[i] = (int8_t)s[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVSXBQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        int64_t *rq = (int64_t*)r; for (int i=0;i<2;i++) rq[i] = (int8_t)s[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVSXWD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        int16_t *sw = (int16_t*)s; int32_t *rd = (int32_t*)r; for (int i=0;i<4;i++) rd[i] = sw[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVSXWQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        int16_t *sw = (int16_t*)s; int64_t *rq = (int64_t*)r; for (int i=0;i<2;i++) rq[i] = sw[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVSXDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        int32_t *sd = (int32_t*)s; int64_t *rq = (int64_t*)r; for (int i=0;i<2;i++) rq[i] = sd[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVZXBW: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        uint16_t *rw = (uint16_t*)r; for (int i=0;i<8;i++) rw[i] = s[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVZXBD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        uint32_t *rd = (uint32_t*)r; for (int i=0;i<4;i++) rd[i] = s[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVZXBQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        uint64_t *rq = (uint64_t*)r; for (int i=0;i<2;i++) rq[i] = s[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVZXWD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        uint16_t *sw = (uint16_t*)s; uint32_t *rd = (uint32_t*)r; for (int i=0;i<4;i++) rd[i] = sw[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVZXWQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        uint16_t *sw = (uint16_t*)s; uint64_t *rq = (uint64_t*)r; for (int i=0;i<2;i++) rq[i] = sw[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::PMOVZXDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]); int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t s[16]; memcpy(s, cpu.zmm[src], 16); uint8_t r[16]; memset(r, 0, 16);
        uint32_t *sd = (uint32_t*)s; uint64_t *rq = (uint64_t*)r; for (int i=0;i<2;i++) rq[i] = sd[i];
        memcpy(cpu.zmm[dst], r, 16); break;
    }
    case Mnemonic::FLD: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); fpu_push(cpu, (double)f); }
            else { double d; memcpy(&d, cpu.mem + ea, 8); fpu_push(cpu, d); }
        } else {
            int idx = di.modrm & 7;
            double val = fpu_st(cpu, idx);
            fpu_push(cpu, val);
        }
        break;
    }
    case Mnemonic::FST: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            if (sz <= 32) { float f = (float)fpu_st(cpu, 0); memcpy(cpu.mem + ea, &f, 4); }
            else { double d = fpu_st(cpu, 0); memcpy(cpu.mem + ea, &d, 8); }
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, idx) = fpu_st(cpu, 0);
        }
        break;
    }
    case Mnemonic::FSTP: case Mnemonic::FSTP1: case Mnemonic::FSTP8: case Mnemonic::FSTP9: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            if (sz <= 32) { float f = (float)fpu_st(cpu, 0); memcpy(cpu.mem + ea, &f, 4); }
            else { double d = fpu_st(cpu, 0); memcpy(cpu.mem + ea, &d, 8); }
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, idx) = fpu_st(cpu, 0);
        }
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FILD: {
        uint64_t ea = compute_ea(cpu, di);
        int sz = resolve_opsize(di, di.desc->operands[0]);
        int64_t ival;
        if (sz <= 16) ival = (int16_t)mem_read(cpu, ea, 2);
        else if (sz <= 32) ival = (int32_t)mem_read(cpu, ea, 4);
        else ival = (int64_t)mem_read(cpu, ea, 8);
        fpu_push(cpu, (double)ival);
        break;
    }
    case Mnemonic::FIST: {
        uint64_t ea = compute_ea(cpu, di);
        int sz = resolve_opsize(di, di.desc->operands[0]);
        int64_t ival = (int64_t)fpu_st(cpu, 0);
        if (sz <= 16) mem_write(cpu, ea, (uint64_t)(int16_t)ival, 2);
        else mem_write(cpu, ea, (uint64_t)(int32_t)ival, 4);
        break;
    }
    case Mnemonic::FISTP: {
        uint64_t ea = compute_ea(cpu, di);
        int sz = resolve_opsize(di, di.desc->operands[0]);
        int64_t ival = (int64_t)fpu_st(cpu, 0);
        if (sz <= 16) mem_write(cpu, ea, (uint64_t)(int16_t)ival, 2);
        else if (sz <= 32) mem_write(cpu, ea, (uint64_t)(int32_t)ival, 4);
        else mem_write(cpu, ea, (uint64_t)ival, 8);
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FADD: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = fpu_st(cpu, 0) + val;
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, 0) = fpu_st(cpu, 0) + fpu_st(cpu, idx);
        }
        break;
    }
    case Mnemonic::FADDP: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = fpu_st(cpu, 0) + val;
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, idx) = fpu_st(cpu, idx) + fpu_st(cpu, 0);
        }
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FSUB: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = fpu_st(cpu, 0) - val;
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, 0) = fpu_st(cpu, 0) - fpu_st(cpu, idx);
        }
        break;
    }
    case Mnemonic::FSUBP: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = fpu_st(cpu, 0) - val;
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, idx) = fpu_st(cpu, idx) - fpu_st(cpu, 0);
        }
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FSUBR: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = val - fpu_st(cpu, 0);
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, 0) = fpu_st(cpu, idx) - fpu_st(cpu, 0);
        }
        break;
    }
    case Mnemonic::FSUBRP: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = val - fpu_st(cpu, 0);
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, idx) = fpu_st(cpu, 0) - fpu_st(cpu, idx);
        }
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FMUL: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = fpu_st(cpu, 0) * val;
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, 0) = fpu_st(cpu, 0) * fpu_st(cpu, idx);
        }
        break;
    }
    case Mnemonic::FMULP: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = fpu_st(cpu, 0) * val;
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, idx) = fpu_st(cpu, idx) * fpu_st(cpu, 0);
        }
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FDIV: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = fpu_st(cpu, 0) / val;
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, 0) = fpu_st(cpu, 0) / fpu_st(cpu, idx);
        }
        break;
    }
    case Mnemonic::FDIVP: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = fpu_st(cpu, 0) / val;
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, idx) = fpu_st(cpu, idx) / fpu_st(cpu, 0);
        }
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FDIVR: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = val / fpu_st(cpu, 0);
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, 0) = fpu_st(cpu, idx) / fpu_st(cpu, 0);
        }
        break;
    }
    case Mnemonic::FDIVRP: {
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            double val;
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); val = f; }
            else { memcpy(&val, cpu.mem + ea, 8); }
            fpu_st(cpu, 0) = val / fpu_st(cpu, 0);
        } else {
            int idx = di.modrm & 7;
            fpu_st(cpu, idx) = fpu_st(cpu, 0) / fpu_st(cpu, idx);
        }
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FCHS: fpu_st(cpu, 0) = -fpu_st(cpu, 0); break;
    case Mnemonic::FABS: fpu_st(cpu, 0) = fabs(fpu_st(cpu, 0)); break;
    case Mnemonic::FXCH: case Mnemonic::FXCH4: case Mnemonic::FXCH7: {
        int idx = (di.modrm & 7) ? (di.modrm & 7) : 1;
        double tmp = fpu_st(cpu, 0); fpu_st(cpu, 0) = fpu_st(cpu, idx); fpu_st(cpu, idx) = tmp;
        break;
    }
    case Mnemonic::FCOM: case Mnemonic::FCOM2: {
        double a = fpu_st(cpu, 0), b;
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); b = f; }
            else { memcpy(&b, cpu.mem + ea, 8); }
        } else { b = fpu_st(cpu, di.modrm & 7); }
        fpu_set_c0c1c2c3(cpu, a < b, false, a != a || b != b, a == b);
        break;
    }
    case Mnemonic::FCOMP: case Mnemonic::FCOMP3: case Mnemonic::FCOMP5: {
        double a = fpu_st(cpu, 0), b;
        if (is_mem_operand(di, di.desc->operands[0])) {
            uint64_t ea = compute_ea(cpu, di);
            int sz = resolve_opsize(di, di.desc->operands[0]);
            if (sz <= 32) { float f; memcpy(&f, cpu.mem + ea, 4); b = f; }
            else { memcpy(&b, cpu.mem + ea, 8); }
        } else { b = fpu_st(cpu, di.modrm & 7); }
        fpu_set_c0c1c2c3(cpu, a < b, false, a != a || b != b, a == b);
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FCOMPP: {
        double a = fpu_st(cpu, 0), b = fpu_st(cpu, 1);
        fpu_set_c0c1c2c3(cpu, a < b, false, a != a || b != b, a == b);
        fpu_pop(cpu); fpu_pop(cpu);
        break;
    }
    case Mnemonic::FCOMI: case Mnemonic::FUCOMI: {
        double a = fpu_st(cpu, 0), b = fpu_st(cpu, di.modrm & 7);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_PF);
        if (a != a || b != b) cpu.rflags |= RFLAG_ZF | RFLAG_PF | RFLAG_CF;
        else if (a < b) cpu.rflags |= RFLAG_CF;
        else if (a == b) cpu.rflags |= RFLAG_ZF;
        break;
    }
    case Mnemonic::FCOMIP: case Mnemonic::FUCOMIP: {
        double a = fpu_st(cpu, 0), b = fpu_st(cpu, di.modrm & 7);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF | RFLAG_PF);
        if (a != a || b != b) cpu.rflags |= RFLAG_ZF | RFLAG_PF | RFLAG_CF;
        else if (a < b) cpu.rflags |= RFLAG_CF;
        else if (a == b) cpu.rflags |= RFLAG_ZF;
        fpu_pop(cpu);
        break;
    }
    case Mnemonic::FTST: {
        double a = fpu_st(cpu, 0);
        fpu_set_c0c1c2c3(cpu, a < 0.0, false, a != a, a == 0.0);
        break;
    }
    case Mnemonic::FLDZ: fpu_push(cpu, 0.0); break;
    case Mnemonic::FLD1: fpu_push(cpu, 1.0); break;
    case Mnemonic::FLDPI: fpu_push(cpu, 3.14159265358979323846); break;
    case Mnemonic::FLDL2E: fpu_push(cpu, 1.4426950408889634); break;
    case Mnemonic::FLDL2T: fpu_push(cpu, 3.3219280948873626); break;
    case Mnemonic::FLDLG2: fpu_push(cpu, 0.3010299957316528); break;
    case Mnemonic::FLDLN2: fpu_push(cpu, 0.6931471805599453); break;
    case Mnemonic::FINIT: case Mnemonic::FNINIT: {
        cpu.fpu_control = 0x037F; cpu.fpu_status = 0; cpu.fpu_top = 0;
        break;
    }
    case Mnemonic::FNSTCW: case Mnemonic::FSTCW: {
        uint64_t ea = compute_ea(cpu, di);
        mem_write(cpu, ea, cpu.fpu_control, 2);
        break;
    }
    case Mnemonic::FLDCW: {
        uint64_t ea = compute_ea(cpu, di);
        cpu.fpu_control = (uint16_t)mem_read(cpu, ea, 2);
        break;
    }
    case Mnemonic::FNSTSW: case Mnemonic::FSTSW: {
        uint16_t sw = cpu.fpu_status | ((uint16_t)(cpu.fpu_top & 7) << 11);
        if (di.desc->operands[0].addr == AddrMode::Fixed && di.desc->operands[0].fixed_reg == 0) {
            cpu.gpr[0] = (cpu.gpr[0] & ~0xFFFFULL) | sw;
        } else {
            uint64_t ea = compute_ea(cpu, di);
            mem_write(cpu, ea, sw, 2);
        }
        break;
    }
    case Mnemonic::FWAIT: break;
    case Mnemonic::FNOP: break;
    case Mnemonic::NOP: break;
    case Mnemonic::PAUSE: break;
    case Mnemonic::HLT: cpu.rip += di.length; return StepResult::Halt;
    case Mnemonic::INT: cpu.rip += di.length; return StepResult::Halt;
    case Mnemonic::SYSCALL: cpu.rip += di.length; return StepResult::Halt;
    case Mnemonic::CPUID: {
        cpu.gpr[0] = 0; cpu.gpr[3] = 0; cpu.gpr[1] = 0; cpu.gpr[2] = 0;
        break;
    }
    case Mnemonic::RDTSC: {
        cpu.gpr[0] = 0; cpu.gpr[2] = 0;
        break;
    }

    case Mnemonic::ANDN: {
        uint64_t src1 = read_operand(cpu, di, 1, bits); // vvvv
        uint64_t src2 = read_operand(cpu, di, 2, bits);
        uint64_t res = (~src1) & src2;
        write_operand(cpu, di, 0, res, bits);
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::BLSI: {
        uint64_t src = read_operand(cpu, di, 1, bits);
        uint64_t res = src & (uint64_t)(-(int64_t)src);
        write_operand(cpu, di, 0, res, bits); // vvvv
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::BLSMSK: {
        uint64_t src = read_operand(cpu, di, 1, bits);
        uint64_t res = src ^ (src - 1);
        write_operand(cpu, di, 0, res, bits);
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::BLSR: {
        uint64_t src = read_operand(cpu, di, 1, bits);
        uint64_t res = src & (src - 1);
        write_operand(cpu, di, 0, res, bits);
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::TZCNT: {
        uint64_t src = read_operand(cpu, di, 1, bits) & mask(bits);
        uint64_t res = 0;
        if (src == 0) res = bits;
        else { while ((src & 1) == 0) { src >>= 1; res++; } }
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF);
        if (res == (uint64_t)bits) cpu.rflags |= RFLAG_CF;
        if (res == 0) cpu.rflags |= RFLAG_ZF;
        break;
    }
    case Mnemonic::LZCNT: {
        uint64_t src = read_operand(cpu, di, 1, bits) & mask(bits);
        uint64_t res = 0;
        if (src == 0) res = bits;
        else { uint64_t msb = sign_bit(bits); while ((src & msb) == 0) { msb >>= 1; res++; } }
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~(RFLAG_CF | RFLAG_ZF);
        if (res == (uint64_t)bits) cpu.rflags |= RFLAG_CF;
        if (res == 0) cpu.rflags |= RFLAG_ZF;
        break;
    }
    case Mnemonic::BZHI: {
        uint64_t src = read_operand(cpu, di, 1, bits);
        uint64_t idx = read_operand(cpu, di, 2, bits) & 0xFF; // vvvv
        uint64_t res = (idx >= (uint64_t)bits) ? src : (src & ((1ULL << idx) - 1));
        write_operand(cpu, di, 0, res, bits);
        update_flags_logic(cpu, res, bits);
        break;
    }
    case Mnemonic::SARX: {
        uint64_t src = read_operand(cpu, di, 1, bits);
        uint64_t cnt = read_operand(cpu, di, 2, bits) & (bits - 1);
        int64_t res = (int64_t)src >> cnt;
        write_operand(cpu, di, 0, (uint64_t)res & mask(bits), bits);
        break;
    }
    case Mnemonic::SHLX: {
        uint64_t src = read_operand(cpu, di, 1, bits);
        uint64_t cnt = read_operand(cpu, di, 2, bits) & (bits - 1);
        write_operand(cpu, di, 0, (src << cnt) & mask(bits), bits);
        break;
    }
    case Mnemonic::SHRX: {
        uint64_t src = read_operand(cpu, di, 1, bits);
        uint64_t cnt = read_operand(cpu, di, 2, bits) & (bits - 1);
        write_operand(cpu, di, 0, (src >> cnt) & mask(bits), bits);
        break;
    }
    case Mnemonic::RORX: {
        uint64_t src = read_operand(cpu, di, 1, bits) & mask(bits);
        uint64_t cnt = di.immediate & (bits - 1);
        uint64_t res = (src >> cnt) | (src << (bits - cnt));
        write_operand(cpu, di, 0, res & mask(bits), bits);
        break;
    }
    case Mnemonic::ADCX: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        uint64_t b = read_operand(cpu, di, 1, bits);
        uint64_t cf = (cpu.rflags & RFLAG_CF) ? 1 : 0;
        uint64_t res = (a + b + cf) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~RFLAG_CF;
        if (bits < 64 && (a + b + cf) > mask(bits)) cpu.rflags |= RFLAG_CF;
        else if (bits == 64 && res < a) cpu.rflags |= RFLAG_CF;
        break;
    }
    case Mnemonic::ADOX: {
        uint64_t a = read_operand(cpu, di, 0, bits);
        uint64_t b = read_operand(cpu, di, 1, bits);
        uint64_t of = (cpu.rflags & RFLAG_OF) ? 1 : 0;
        uint64_t res = (a + b + of) & mask(bits);
        write_operand(cpu, di, 0, res, bits);
        cpu.rflags &= ~RFLAG_OF;
        if (bits < 64 && (a + b + of) > mask(bits)) cpu.rflags |= RFLAG_OF;
        else if (bits == 64 && res < a) cpu.rflags |= RFLAG_OF;
        break;
    }
    case Mnemonic::RDRAND: case Mnemonic::RDSEED: {
        uint64_t val = 0x42424242ULL; // deterministic for testing
        write_operand(cpu, di, 0, val, bits);
        cpu.rflags |= RFLAG_CF; // success
        break;
    }
    case Mnemonic::ENDBR32: case Mnemonic::ENDBR64:
    case Mnemonic::SERIALIZE: case Mnemonic::CLFLUSHOPT: case Mnemonic::CLWB:
    case Mnemonic::PREFETCHW: case Mnemonic::STAC: case Mnemonic::CLAC:
    case Mnemonic::XEND: case Mnemonic::XTEST:
        break; // no-op in emulation
    case Mnemonic::XBEGIN: {
        // In simplified emulation, always 'abort' — jump to fallback
        int64_t rel = (int64_t)(int32_t)di.immediate;
        cpu.rip = cpu.rip + di.length + rel;
        cpu.rax() = 0; // XABORT code 0
        break;
    }
    case Mnemonic::XABORT: break;
    case Mnemonic::RDPID: {
        write_operand(cpu, di, 0, 0, bits);
        break;
    }
    case Mnemonic::WBNOINVD: break;
    case Mnemonic::CMPPS: case Mnemonic::CMPPD: case Mnemonic::CMPSS: case Mnemonic::CMPSD: {
        bool is_scalar = (mn == Mnemonic::CMPSS || mn == Mnemonic::CMPSD);
        bool is_double = (mn == Mnemonic::CMPPD || mn == Mnemonic::CMPSD);
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        uint8_t pred = (uint8_t)(di.immediate & 7);
        int count = is_scalar ? 1 : (is_double ? 2 : 4);
        for (int i = 0; i < count; i++) {
            double a, b;
            if (is_double) { a = cpu.zmm[dst_r][i]; b = cpu.zmm[src_r][i]; }
            else {
                float fa, fb;
                memcpy(&fa, (char*)&cpu.zmm[dst_r][i/2] + (i%2)*4, 4);
                memcpy(&fb, (char*)&cpu.zmm[src_r][i/2] + (i%2)*4, 4);
                a = fa; b = fb;
            }
            bool result = false;
            switch (pred) {
                case 0: result = (a == b); break; // EQ
                case 1: result = (a < b); break;  // LT
                case 2: result = (a <= b); break; // LE
                case 3: result = (a != a || b != b); break; // UNORD
                case 4: result = (a != b); break; // NEQ
                case 5: result = !(a < b); break; // NLT
                case 6: result = !(a <= b); break; // NLE
                case 7: result = (a == a && b == b); break; // ORD
            }
            if (is_double) {
                uint64_t v = result ? ~0ULL : 0ULL;
                memcpy(&cpu.zmm[dst_r][i], &v, 8);
            } else {
                uint32_t v = result ? ~0U : 0U;
                memcpy((char*)&cpu.zmm[dst_r][i/2] + (i%2)*4, &v, 4);
            }
        }
        break;
    }
    case Mnemonic::BLENDPS: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        uint8_t imm = (uint8_t)(di.immediate & 0xF);
        for (int i = 0; i < 4; i++) {
            if (imm & (1 << i)) {
                float f; memcpy(&f, (char*)&cpu.zmm[src_r][i/2] + (i%2)*4, 4);
                memcpy((char*)&cpu.zmm[dst_r][i/2] + (i%2)*4, &f, 4);
            }
        }
        break;
    }
    case Mnemonic::BLENDPD: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        uint8_t imm = (uint8_t)(di.immediate & 3);
        if (imm & 1) cpu.zmm[dst_r][0] = cpu.zmm[src_r][0];
        if (imm & 2) cpu.zmm[dst_r][1] = cpu.zmm[src_r][1];
        break;
    }
    case Mnemonic::PBLENDW: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        uint8_t imm = (uint8_t)di.immediate;
        uint16_t dst_w[8], src_w[8];
        memcpy(dst_w, &cpu.zmm[dst_r], 16); memcpy(src_w, &cpu.zmm[src_r], 16);
        for (int i = 0; i < 8; i++) if (imm & (1 << i)) dst_w[i] = src_w[i];
        memcpy(&cpu.zmm[dst_r], dst_w, 16);
        break;
    }
    case Mnemonic::PSHUFW:
    case Mnemonic::PBLENDVB: case Mnemonic::BLENDVPS: case Mnemonic::BLENDVPD:
        return StepResult::Unsupported;
    case Mnemonic::CVTPS2PD: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        float f[2]; memcpy(f, &cpu.zmm[src_r], 8);
        cpu.zmm[dst_r][0] = (double)f[0]; cpu.zmm[dst_r][1] = (double)f[1];
        break;
    }
    case Mnemonic::CVTPD2PS: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        float f[4] = {0};
        f[0] = (float)cpu.zmm[src_r][0]; f[1] = (float)cpu.zmm[src_r][1];
        memcpy(&cpu.zmm[dst_r], f, 16);
        break;
    }
    case Mnemonic::CVTDQ2PS: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int32_t dw[4]; memcpy(dw, &cpu.zmm[src_r], 16);
        float f[4]; for (int i=0;i<4;i++) f[i]=(float)dw[i];
        memcpy(&cpu.zmm[dst_r], f, 16);
        break;
    }
    case Mnemonic::CVTPS2DQ: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        float f[4]; memcpy(f, &cpu.zmm[src_r], 16);
        int32_t dw[4]; for (int i=0;i<4;i++) dw[i]=(int32_t)lrintf(f[i]);
        memcpy(&cpu.zmm[dst_r], dw, 16);
        break;
    }
    case Mnemonic::CVTTPS2DQ: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        float f[4]; memcpy(f, &cpu.zmm[src_r], 16);
        int32_t dw[4]; for (int i=0;i<4;i++) dw[i]=(int32_t)f[i];
        memcpy(&cpu.zmm[dst_r], dw, 16);
        break;
    }
    case Mnemonic::CVTDQ2PD: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int32_t dw[2]; memcpy(dw, &cpu.zmm[src_r], 8);
        cpu.zmm[dst_r][0] = (double)dw[0]; cpu.zmm[dst_r][1] = (double)dw[1];
        break;
    }
    case Mnemonic::CVTPD2DQ: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int32_t dw[4] = {0};
        dw[0] = (int32_t)lrint(cpu.zmm[src_r][0]); dw[1] = (int32_t)lrint(cpu.zmm[src_r][1]);
        memcpy(&cpu.zmm[dst_r], dw, 16);
        break;
    }
    case Mnemonic::CVTTPD2DQ: {
        int dst_r = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int src_r = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int32_t dw[4] = {0};
        dw[0] = (int32_t)cpu.zmm[src_r][0]; dw[1] = (int32_t)cpu.zmm[src_r][1];
        memcpy(&cpu.zmm[dst_r], dw, 16);
        break;
    }
    case Mnemonic::AESENC: case Mnemonic::AESENCLAST:
    case Mnemonic::AESDEC: case Mnemonic::AESDECLAST:
    case Mnemonic::AESIMC: case Mnemonic::AESKEYGENASSIST:
    case Mnemonic::PCLMULQDQ:
        return StepResult::Unsupported;
    case Mnemonic::SHA1MSG1: case Mnemonic::SHA1MSG2:
    case Mnemonic::SHA1NEXTE: case Mnemonic::SHA1RNDS4:
    case Mnemonic::SHA256MSG1: case Mnemonic::SHA256MSG2:
    case Mnemonic::SHA256RNDS2:
        return StepResult::Unsupported;
    case Mnemonic::FUCOM: case Mnemonic::FUCOMP: case Mnemonic::FUCOMPP: {
        // Unordered compare ST(0) with ST(i): set C0/C2/C3 in FPU status word
        int i = di.modrm & 7; if (i == 0 && mn != Mnemonic::FUCOMPP) i = 1;
        double a = cpu.fpu_stack[cpu.fpu_top]; double b = cpu.fpu_stack[(cpu.fpu_top + i) & 7];
        cpu.fpu_status &= ~0x4500;
        if (a != a || b != b) cpu.fpu_status |= 0x4500; // unordered
        else if (a < b) cpu.fpu_status |= 0x0100;
        else if (a == b) cpu.fpu_status |= 0x4000;
        if (mn == Mnemonic::FUCOMP || mn == Mnemonic::FUCOMPP) cpu.fpu_top = (cpu.fpu_top + 1) & 7;
        if (mn == Mnemonic::FUCOMPP) cpu.fpu_top = (cpu.fpu_top + 1) & 7;
        break;
    }
    case Mnemonic::FCLEX: case Mnemonic::FNCLEX:
        cpu.fpu_status &= ~0x80FF; break; // clear exception flags
    case Mnemonic::FSQRT: cpu.fpu_stack[cpu.fpu_top] = sqrt(cpu.fpu_stack[cpu.fpu_top]); break;
    case Mnemonic::FRNDINT: cpu.fpu_stack[cpu.fpu_top] = rint(cpu.fpu_stack[cpu.fpu_top]); break;
    case Mnemonic::FSIN: cpu.fpu_stack[cpu.fpu_top] = sin(cpu.fpu_stack[cpu.fpu_top]); break;
    case Mnemonic::FCOS: cpu.fpu_stack[cpu.fpu_top] = cos(cpu.fpu_stack[cpu.fpu_top]); break;
    case Mnemonic::FPTAN: {
        cpu.fpu_stack[cpu.fpu_top] = tan(cpu.fpu_stack[cpu.fpu_top]);
        cpu.fpu_top = (cpu.fpu_top - 1) & 7; cpu.fpu_stack[cpu.fpu_top] = 1.0; break;
    }
    case Mnemonic::FPATAN: {
        double st1 = cpu.fpu_stack[(cpu.fpu_top + 1) & 7];
        cpu.fpu_top = (cpu.fpu_top + 1) & 7;
        cpu.fpu_stack[cpu.fpu_top] = atan2(st1, cpu.fpu_stack[cpu.fpu_top]); break;
    }
    case Mnemonic::FPREM: {
        double st1 = cpu.fpu_stack[(cpu.fpu_top + 1) & 7];
        cpu.fpu_stack[cpu.fpu_top] = fmod(cpu.fpu_stack[cpu.fpu_top], st1); break;
    }
    case Mnemonic::WAIT: break; // NOP (wait for FPU)
    case Mnemonic::FNSAVE: case Mnemonic::FRSTOR:
        return StepResult::Unsupported;
    case Mnemonic::WRMSR:
    case Mnemonic::LGDT: case Mnemonic::LIDT: case Mnemonic::SGDT: case Mnemonic::SIDT:
    case Mnemonic::LLDT: case Mnemonic::SLDT: case Mnemonic::LTR: case Mnemonic::STR:
    case Mnemonic::VERR: case Mnemonic::VERW:
    case Mnemonic::INVLPG: case Mnemonic::INVPCID: case Mnemonic::CLTS:
    case Mnemonic::MONITOR: case Mnemonic::MWAIT:
    case Mnemonic::XGETBV: case Mnemonic::XSETBV:
    case Mnemonic::VMCALL: case Mnemonic::VMLAUNCH: case Mnemonic::VMRESUME: case Mnemonic::VMXOFF:
    case Mnemonic::VMCLEAR: case Mnemonic::VMPTRLD: case Mnemonic::VMPTRST:
    case Mnemonic::VMREAD: case Mnemonic::VMWRITE:
        return StepResult::Unsupported;
    case Mnemonic::VZEROALL: {
        for (int r = 0; r < 16; r++) for (int i = 0; i < 8; i++) cpu.zmm[r][i] = 0;
        break;
    }
    case Mnemonic::VZEROUPPER: {
        for (int r = 0; r < 16; r++) { cpu.zmm[r][2]=0; cpu.zmm[r][3]=0; cpu.zmm[r][4]=0; cpu.zmm[r][5]=0; cpu.zmm[r][6]=0; cpu.zmm[r][7]=0; }
        break;
    }
    case Mnemonic::PUSHF: case Mnemonic::PUSHFD: case Mnemonic::PUSHFQ: {
        cpu.rsp() -= 8;
        if (!mem_check(cpu, cpu.rsp(), 8)) return StepResult::MemFault;
        mem_write(cpu, cpu.rsp(), cpu.rflags, 8);
        break;
    }
    case Mnemonic::POPF: case Mnemonic::POPFD: case Mnemonic::POPFQ: {
        if (!mem_check(cpu, cpu.rsp(), 8)) return StepResult::MemFault;
        cpu.rflags = mem_read(cpu, cpu.rsp(), 8);
        cpu.rsp() += 8;
        break;
    }
    case Mnemonic::XLAT: {
        uint64_t addr = cpu.rbx() + (cpu.rax() & 0xFF);
        if (!mem_check(cpu, addr, 1)) return StepResult::MemFault;
        cpu.rax() = (cpu.rax() & ~0xFFULL) | mem_read(cpu, addr, 1);
        break;
    }
    case Mnemonic::RDFSBASE: {
        write_operand(cpu, di, 0, cpu.fs_base, bits);
        break;
    }
    case Mnemonic::RDGSBASE: {
        write_operand(cpu, di, 0, cpu.gs_base, bits);
        break;
    }
    case Mnemonic::WRFSBASE: {
        cpu.fs_base = read_operand(cpu, di, 0, bits);
        break;
    }
    case Mnemonic::WRGSBASE: {
        cpu.gs_base = read_operand(cpu, di, 0, bits);
        break;
    }
    case Mnemonic::RDPKRU: case Mnemonic::WRPKRU:
        break;
    case Mnemonic::STMXCSR: {
        uint64_t addr = compute_ea(cpu, di);
        if (!mem_check(cpu, addr, 4)) return StepResult::MemFault;
        mem_write(cpu, addr, cpu.mxcsr, 4);
        break;
    }
    case Mnemonic::LDMXCSR: {
        uint64_t addr = compute_ea(cpu, di);
        if (!mem_check(cpu, addr, 4)) return StepResult::MemFault;
        cpu.mxcsr = (uint32_t)mem_read(cpu, addr, 4);
        break;
    }
    case Mnemonic::MOVNTI: {
        uint64_t val = read_operand(cpu, di, 1, bits);
        write_operand(cpu, di, 0, val, bits);
        break;
    }
    case Mnemonic::CMPXCHG16B: {
        uint64_t addr = compute_ea(cpu, di);
        if (!mem_check(cpu, addr, 16)) return StepResult::MemFault;
        uint64_t lo = mem_read(cpu, addr, 8);
        uint64_t hi = mem_read(cpu, addr + 8, 8);
        if (cpu.rax() == lo && cpu.rdx() == hi) {
            mem_write(cpu, addr, cpu.rbx(), 8);
            mem_write(cpu, addr + 8, cpu.rcx(), 8);
            cpu.rflags |= RFLAG_ZF;
        } else {
            cpu.rax() = lo;
            cpu.rdx() = hi;
            cpu.rflags &= ~RFLAG_ZF;
        }
        break;
    }
    case Mnemonic::RDTSCP: {
        cpu.rax() = 0; cpu.rdx() = 0; cpu.rcx() = 0;
        break;
    }
    case Mnemonic::HINT_NOP:
        break;
    case Mnemonic::CMPSB: case Mnemonic::CMPSW: case Mnemonic::CMPSQ: {
        int str_sz = forced_str_sz;
        if (!mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
        if (!mem_check(cpu, cpu.gpr[6], str_sz)) return StepResult::MemFault;
        uint64_t a = mem_read(cpu, cpu.gpr[6], str_sz);
        uint64_t b = mem_read(cpu, cpu.gpr[7], str_sz);
        update_flags_sub(cpu, a, b, (a - b) & mask(str_sz*8), str_sz*8);
        int64_t d = (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        cpu.gpr[6] += d; cpu.gpr[7] += d;
        break;
    }
    case Mnemonic::MOVSB: case Mnemonic::MOVSW: case Mnemonic::MOVSQ: {
        int str_sz = forced_str_sz;
        if (!mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
        if (!mem_check(cpu, cpu.gpr[6], str_sz)) return StepResult::MemFault;
        mem_write(cpu, cpu.gpr[7], mem_read(cpu, cpu.gpr[6], str_sz), str_sz);
        int64_t d = (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        cpu.gpr[6] += d; cpu.gpr[7] += d;
        break;
    }
    case Mnemonic::LODSB: case Mnemonic::LODSW: case Mnemonic::LODSD: case Mnemonic::LODSQ: {
        int str_sz = forced_str_sz;
        if (!mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
        if (!mem_check(cpu, cpu.gpr[6], str_sz)) return StepResult::MemFault;
        uint64_t val = mem_read(cpu, cpu.gpr[6], str_sz);
        if (str_sz == 4) cpu.gpr[0] = val & 0xFFFFFFFF;
        else if (str_sz == 8) cpu.gpr[0] = val;
        else cpu.gpr[0] = (cpu.gpr[0] & ~mask(str_sz*8)) | val;
        cpu.gpr[6] += (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        break;
    }
    case Mnemonic::STOSB: case Mnemonic::STOSD: case Mnemonic::STOSQ: case Mnemonic::STOSW: {
        int str_sz = forced_str_sz;
        if (!mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
        mem_write(cpu, cpu.gpr[7], cpu.gpr[0], str_sz);
        cpu.gpr[7] += (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        break;
    }
    case Mnemonic::SCASB: case Mnemonic::SCASD: case Mnemonic::SCASQ: case Mnemonic::SCASW: {
        int str_sz = forced_str_sz;
        if (!mem_check(cpu, cpu.gpr[7], str_sz)) return StepResult::MemFault;
        uint64_t a = cpu.gpr[0] & mask(str_sz*8);
        uint64_t b = mem_read(cpu, cpu.gpr[7], str_sz);
        update_flags_sub(cpu, a, b, (a - b) & mask(str_sz*8), str_sz*8);
        cpu.gpr[7] += (cpu.rflags & RFLAG_DF) ? -str_sz : str_sz;
        break;
    }
    case Mnemonic::INSB: case Mnemonic::INSD: case Mnemonic::INSW: case Mnemonic::INS:
        return StepResult::Unsupported;
    case Mnemonic::OUTSB: case Mnemonic::OUTSD: case Mnemonic::OUTSW: case Mnemonic::OUTS:
        return StepResult::Unsupported;
    case Mnemonic::IN: case Mnemonic::OUT:
        return StepResult::Unsupported;
    case Mnemonic::LOOPE: {
        cpu.gpr[1]--;
        if (cpu.gpr[1] != 0 && (cpu.rflags & RFLAG_ZF)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::LOOPNE: {
        cpu.gpr[1]--;
        if (cpu.gpr[1] != 0 && !(cpu.rflags & RFLAG_ZF)) {
            cpu.rip = cpu.rip + di.length + di.immediate;
        } else {
            cpu.rip += di.length;
        }
        return StepResult::OK;
    }
    case Mnemonic::JCXZ: {
        if ((cpu.gpr[1] & 0xFFFF) == 0)
            cpu.rip = cpu.rip + di.length + di.immediate;
        else cpu.rip += di.length;
        return StepResult::OK;
    }
    case Mnemonic::JECXZ: {
        if ((cpu.gpr[1] & 0xFFFFFFFF) == 0)
            cpu.rip = cpu.rip + di.length + di.immediate;
        else cpu.rip += di.length;
        return StepResult::OK;
    }
    case Mnemonic::ADDSUBPS: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        float df[4], sf[4]; memcpy(df, cpu.zmm[d], 16); memcpy(sf, cpu.zmm[s], 16);
        df[0] -= sf[0]; df[1] += sf[1]; df[2] -= sf[2]; df[3] += sf[3];
        memcpy(cpu.zmm[d], df, 16); break;
    }
    case Mnemonic::ADDSUBPD: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        cpu.zmm[d][0] -= cpu.zmm[s][0]; cpu.zmm[d][1] += cpu.zmm[s][1]; break;
    }
    case Mnemonic::LDDQU: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        uint64_t addr = compute_ea(cpu, di);
        if (!mem_check(cpu, addr, 16)) return StepResult::MemFault;
        for (int i = 0; i < 2; i++) cpu.zmm[d][i] = 0;
        uint64_t lo = mem_read(cpu, addr, 8); uint64_t hi = mem_read(cpu, addr+8, 8);
        memcpy(&cpu.zmm[d][0], &lo, 8); memcpy(&cpu.zmm[d][1], &hi, 8);
        break;
    }
    case Mnemonic::PSIGNB: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int8_t dv[16], sv[16]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<16;i++) dv[i] = sv[i] < 0 ? -dv[i] : sv[i] == 0 ? 0 : dv[i];
        memcpy(cpu.zmm[d], dv, 16); break;
    }
    case Mnemonic::PSIGNW: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int16_t dv[8], sv[8]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<8;i++) dv[i] = sv[i] < 0 ? -dv[i] : sv[i] == 0 ? 0 : dv[i];
        memcpy(cpu.zmm[d], dv, 16); break;
    }
    case Mnemonic::PSIGND: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int32_t dv[4], sv[4]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<4;i++) dv[i] = sv[i] < 0 ? -dv[i] : sv[i] == 0 ? 0 : dv[i];
        memcpy(cpu.zmm[d], dv, 16); break;
    }
    case Mnemonic::PHADDW: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int16_t dv[8], sv[8], rv[8]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<4;i++) rv[i] = dv[i*2] + dv[i*2+1];
        for (int i=0;i<4;i++) rv[i+4] = sv[i*2] + sv[i*2+1];
        memcpy(cpu.zmm[d], rv, 16); break;
    }
    case Mnemonic::PHADDD: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int32_t dv[4], sv[4], rv[4]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<2;i++) rv[i] = dv[i*2] + dv[i*2+1];
        for (int i=0;i<2;i++) rv[i+2] = sv[i*2] + sv[i*2+1];
        memcpy(cpu.zmm[d], rv, 16); break;
    }
    case Mnemonic::PHADDSW: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int16_t dv[8], sv[8], rv[8]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<4;i++) { int32_t r = (int32_t)dv[i*2] + (int32_t)dv[i*2+1]; rv[i] = r > 32767 ? 32767 : r < -32768 ? -32768 : (int16_t)r; }
        for (int i=0;i<4;i++) { int32_t r = (int32_t)sv[i*2] + (int32_t)sv[i*2+1]; rv[i+4] = r > 32767 ? 32767 : r < -32768 ? -32768 : (int16_t)r; }
        memcpy(cpu.zmm[d], rv, 16); break;
    }
    case Mnemonic::PHSUBW: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int16_t dv[8], sv[8], rv[8]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<4;i++) rv[i] = dv[i*2] - dv[i*2+1];
        for (int i=0;i<4;i++) rv[i+4] = sv[i*2] - sv[i*2+1];
        memcpy(cpu.zmm[d], rv, 16); break;
    }
    case Mnemonic::PHSUBD: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int32_t dv[4], sv[4], rv[4]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<2;i++) rv[i] = dv[i*2] - dv[i*2+1];
        for (int i=0;i<2;i++) rv[i+2] = sv[i*2] - sv[i*2+1];
        memcpy(cpu.zmm[d], rv, 16); break;
    }
    case Mnemonic::PHSUBSW: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        int16_t dv[8], sv[8], rv[8]; memcpy(dv, cpu.zmm[d], 16); memcpy(sv, cpu.zmm[s], 16);
        for (int i=0;i<4;i++) { int32_t r = (int32_t)dv[i*2] - (int32_t)dv[i*2+1]; rv[i] = r > 32767 ? 32767 : r < -32768 ? -32768 : (int16_t)r; }
        for (int i=0;i<4;i++) { int32_t r = (int32_t)sv[i*2] - (int32_t)sv[i*2+1]; rv[i+4] = r > 32767 ? 32767 : r < -32768 ? -32768 : (int16_t)r; }
        memcpy(cpu.zmm[d], rv, 16); break;
    }
    case Mnemonic::PCMPESTRI: case Mnemonic::PCMPESTRM:
    case Mnemonic::PCMPISTRI: case Mnemonic::PCMPISTRM:
    case Mnemonic::PHMINPOSUW:
    case Mnemonic::DPPD: case Mnemonic::DPPS:
    case Mnemonic::MPSADBW:
    case Mnemonic::MASKMOVDQU: case Mnemonic::MASKMOVQ:
        return StepResult::Unsupported;
    case Mnemonic::MOVHLPS: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        memcpy(&cpu.zmm[d][0], &cpu.zmm[s][1], 8); break;
    }
    case Mnemonic::MOVLHPS: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        memcpy(&cpu.zmm[d][1], &cpu.zmm[s][0], 8); break;
    }
    case Mnemonic::MOVSD: {
        int d = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        if (((di.modrm >> 6) & 3) == 3) {
            int s = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
            cpu.zmm[d][0] = cpu.zmm[s][0]; // reg-reg: only low qword
        } else {
            uint64_t addr = compute_ea(cpu, di);
            if (!mem_check(cpu, addr, 8)) return StepResult::MemFault;
            uint64_t v = mem_read(cpu, addr, 8);
            memcpy(&cpu.zmm[d][0], &v, 8); cpu.zmm[d][1] = 0;
        }
        break;
    }
    case Mnemonic::MOVNTQ: case Mnemonic::MOVDQ2Q: case Mnemonic::MOVQ2DQ:
    case Mnemonic::CVTPD2PI: case Mnemonic::CVTPI2PD: case Mnemonic::CVTPI2PS: case Mnemonic::CVTPS2PI:
    case Mnemonic::CVTTPD2PI: case Mnemonic::CVTTPS2PI:
        return StepResult::Unsupported;
    case Mnemonic::VFMADD132PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
            cpu.zmm[dst][i] = a*b + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD132SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[dst][0], b=cpu.zmm[src3][0], c=cpu.zmm[vvvv][0];
        cpu.zmm[dst][0] = a*b + c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD213PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
            cpu.zmm[dst][i] = a*b + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD213SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[vvvv][0], b=cpu.zmm[dst][0], c=cpu.zmm[src3][0];
        cpu.zmm[dst][0] = a*b + c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD231PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
            cpu.zmm[dst][i] = a*b + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADD231SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[vvvv][0], b=cpu.zmm[src3][0], c=cpu.zmm[dst][0];
        cpu.zmm[dst][0] = a*b + c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB132PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
            cpu.zmm[dst][i] = a*b - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB132SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[dst][0], b=cpu.zmm[src3][0], c=cpu.zmm[vvvv][0];
        cpu.zmm[dst][0] = a*b - c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB213PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
            cpu.zmm[dst][i] = a*b - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB213SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[vvvv][0], b=cpu.zmm[dst][0], c=cpu.zmm[src3][0];
        cpu.zmm[dst][0] = a*b - c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB231PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
            cpu.zmm[dst][i] = a*b - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUB231SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[vvvv][0], b=cpu.zmm[src3][0], c=cpu.zmm[dst][0];
        cpu.zmm[dst][0] = a*b - c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD132PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
            cpu.zmm[dst][i] = -(a*b) + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD132SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[dst][0], b=cpu.zmm[src3][0], c=cpu.zmm[vvvv][0];
        cpu.zmm[dst][0] = -(a*b) + c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD213PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
            cpu.zmm[dst][i] = -(a*b) + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD213SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[vvvv][0], b=cpu.zmm[dst][0], c=cpu.zmm[src3][0];
        cpu.zmm[dst][0] = -(a*b) + c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD231PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
            cpu.zmm[dst][i] = -(a*b) + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMADD231SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[vvvv][0], b=cpu.zmm[src3][0], c=cpu.zmm[dst][0];
        cpu.zmm[dst][0] = -(a*b) + c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB132PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
            cpu.zmm[dst][i] = -(a*b) - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB132SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[dst][0], b=cpu.zmm[src3][0], c=cpu.zmm[vvvv][0];
        cpu.zmm[dst][0] = -(a*b) - c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB213PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
            cpu.zmm[dst][i] = -(a*b) - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB213SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[vvvv][0], b=cpu.zmm[dst][0], c=cpu.zmm[src3][0];
        cpu.zmm[dst][0] = -(a*b) - c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB231PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
            cpu.zmm[dst][i] = -(a*b) - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFNMSUB231SD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        double a=cpu.zmm[vvvv][0], b=cpu.zmm[src3][0], c=cpu.zmm[dst][0];
        cpu.zmm[dst][0] = -(a*b) - c;
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADDSUB132PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
            cpu.zmm[dst][i] = (i & 1) ? a*b + c : a*b - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADDSUB213PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
            cpu.zmm[dst][i] = (i & 1) ? a*b + c : a*b - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMADDSUB231PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
            cpu.zmm[dst][i] = (i & 1) ? a*b + c : a*b - c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUBADD132PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[dst][i], b=cpu.zmm[src3][i], c=cpu.zmm[vvvv][i];
            cpu.zmm[dst][i] = (i & 1) ? a*b - c : a*b + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUBADD213PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[dst][i], c=cpu.zmm[src3][i];
            cpu.zmm[dst][i] = (i & 1) ? a*b - c : a*b + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VFMSUBADD231PD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int vvvv = (int)di.vex_vvvv;
        int src3 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            double a=cpu.zmm[vvvv][i], b=cpu.zmm[src3][i], c=cpu.zmm[dst][i];
            cpu.zmm[dst][i] = (i & 1) ? a*b - c : a*b + c;
        }
        vex_zero_upper(cpu, di, dst);
        break;
    }
    case Mnemonic::VGATHERDPD: case Mnemonic::VGATHERDPS:
    case Mnemonic::VGATHERQPD: case Mnemonic::VGATHERQPS:
    case Mnemonic::VPGATHERDD: case Mnemonic::VPGATHERDQ:
    case Mnemonic::VPGATHERQD: case Mnemonic::VPGATHERQQ:
        return StepResult::Unsupported;
    case Mnemonic::VPERMPD: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        uint8_t imm = (uint8_t)di.immediate;
        double s[4]; memcpy(s, cpu.zmm[src], 32);
        for (int i = 0; i < 4; i++) cpu.zmm[dst][i] = s[(imm >> (i*2)) & 3];
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPERMPS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int idx_r = (int)di.vex_vvvv;
        int src = xmm_reg_index(di, di.desc->operands[2]);
        int32_t idx[8]; float sv[8], rv[8]; memcpy(idx, cpu.zmm[idx_r], 32); memcpy(sv, cpu.zmm[src], 32);
        for (int i = 0; i < 8; i++) rv[i] = sv[idx[i] & 7];
        memcpy(cpu.zmm[dst], rv, 32); vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VMFUNC: case Mnemonic::VMXON:
        return StepResult::Unsupported;
    case Mnemonic::KSHIFTLW: {
        int d = (di.modrm >> 3) & 7; int s = di.modrm & 7;
        cpu.opmask[d] = (uint16_t)(cpu.opmask[s] << (di.immediate & 15)); break;
    }
    case Mnemonic::KSHIFTRW: {
        int d = (di.modrm >> 3) & 7; int s = di.modrm & 7;
        cpu.opmask[d] = (uint16_t)(cpu.opmask[s] >> (di.immediate & 15)); break;
    }
    case Mnemonic::KUNPCKBW: {
        int d = (di.modrm >> 3) & 7; int s1 = di.vex_vvvv; int s2 = di.modrm & 7;
        cpu.opmask[d] = ((uint16_t)(cpu.opmask[s1] & 0xFF) << 8) | (uint16_t)(cpu.opmask[s2] & 0xFF); break;
    }
    case Mnemonic::KUNPCKWD: {
        int d = (di.modrm >> 3) & 7; int s1 = di.vex_vvvv; int s2 = di.modrm & 7;
        cpu.opmask[d] = ((uint32_t)(cpu.opmask[s1] & 0xFFFF) << 16) | (uint32_t)(cpu.opmask[s2] & 0xFFFF); break;
    }
    case Mnemonic::KUNPCKDQ: {
        int d = (di.modrm >> 3) & 7; int s1 = di.vex_vvvv; int s2 = di.modrm & 7;
        cpu.opmask[d] = ((uint64_t)(cpu.opmask[s1] & 0xFFFFFFFF) << 32) | (uint64_t)(cpu.opmask[s2] & 0xFFFFFFFF); break;
    }
    case Mnemonic::XSAVE: case Mnemonic::XSAVEC: case Mnemonic::XSAVEOPT: case Mnemonic::XSAVES:
    case Mnemonic::XRSTOR: case Mnemonic::XRSTORS:
        return StepResult::Unsupported;
    case Mnemonic::FXSAVE: case Mnemonic::FXRSTOR:
        return StepResult::Unsupported;
    case Mnemonic::CLRSSBSY: case Mnemonic::INCSSPD: case Mnemonic::INCSSPQ:
    case Mnemonic::RDSSPD: case Mnemonic::RDSSPQ:
    case Mnemonic::RSTORSSP: case Mnemonic::SAVEPREVSSP: case Mnemonic::SETSSBSY:
    case Mnemonic::WRSSD: case Mnemonic::WRSSQ: case Mnemonic::WRUSSD: case Mnemonic::WRUSSQ:
        return StepResult::Unsupported;
    case Mnemonic::CLUI: case Mnemonic::STUI: case Mnemonic::TESTUI:
    case Mnemonic::UIRET: case Mnemonic::SENDUIPI:
        return StepResult::Unsupported;
    case Mnemonic::AESDEC128KL: case Mnemonic::AESDEC256KL:
    case Mnemonic::AESDECWIDE128KL: case Mnemonic::AESDECWIDE256KL:
    case Mnemonic::AESENC128KL: case Mnemonic::AESENC256KL:
    case Mnemonic::AESENCWIDE128KL: case Mnemonic::AESENCWIDE256KL:
    case Mnemonic::ENCODEKEY128: case Mnemonic::ENCODEKEY256:
        return StepResult::Unsupported;
    case Mnemonic::GF2P8AFFINEINVQB: case Mnemonic::GF2P8AFFINEQB: case Mnemonic::GF2P8MULB:
        return StepResult::Unsupported;
    case Mnemonic::MOVDIR64B: case Mnemonic::MOVDIRI:
    case Mnemonic::ENQCMD: case Mnemonic::ENQCMDS:
    case Mnemonic::HRESET: case Mnemonic::PTWRITE:
    case Mnemonic::TPAUSE: case Mnemonic::UMONITOR: case Mnemonic::UMWAIT:
        return StepResult::Unsupported;
    case Mnemonic::CALLF:
    case Mnemonic::IRET: case Mnemonic::IRETD:
    case Mnemonic::SYSRET: case Mnemonic::SYSEXIT: case Mnemonic::SYSENTER:
    case Mnemonic::RSM: case Mnemonic::UD: case Mnemonic::UD2: case Mnemonic::ICEBP:
    case Mnemonic::GETSEC: case Mnemonic::SMSW: case Mnemonic::LMSW:
    case Mnemonic::LAR: case Mnemonic::LSL:
    case Mnemonic::INVEPT: case Mnemonic::INVVPID:
    case Mnemonic::CLI: case Mnemonic::STI:
    case Mnemonic::INVD: case Mnemonic::WBINVD:
    case Mnemonic::RDMSR: case Mnemonic::RDPMC:
    case Mnemonic::INT1: case Mnemonic::INTO:
    case Mnemonic::JMPE: case Mnemonic::JMPF:
        return StepResult::Unsupported;
    case Mnemonic::AAA: case Mnemonic::AAD: case Mnemonic::AAM: case Mnemonic::AAS:
    case Mnemonic::DAA: case Mnemonic::DAS:
    case Mnemonic::PUSHA: case Mnemonic::PUSHAD: case Mnemonic::POPA: case Mnemonic::POPAD:
    case Mnemonic::BOUND: case Mnemonic::SALC:
    case Mnemonic::LDS: case Mnemonic::LES: case Mnemonic::LFS: case Mnemonic::LGS: case Mnemonic::LSS:
    case Mnemonic::SETALC:
        return StepResult::Unsupported;
    case Mnemonic::F2XM1:
    case Mnemonic::FBLD: case Mnemonic::FBSTP:
    case Mnemonic::FCMOVB: case Mnemonic::FCMOVBE: case Mnemonic::FCMOVE:
    case Mnemonic::FCMOVNB: case Mnemonic::FCMOVNBE: case Mnemonic::FCMOVNE:
    case Mnemonic::FCMOVNU: case Mnemonic::FCMOVU:
    case Mnemonic::FDECSTP: case Mnemonic::FDISI: case Mnemonic::FENI:
    case Mnemonic::FFREE: case Mnemonic::FFREEP:
    case Mnemonic::FIADD: case Mnemonic::FICOM: case Mnemonic::FICOMP:
    case Mnemonic::FIDIV: case Mnemonic::FIDIVR: case Mnemonic::FIMUL:
    case Mnemonic::FINCSTP: case Mnemonic::FISTTP:
    case Mnemonic::FISUB: case Mnemonic::FISUBR:
    case Mnemonic::FLDENV:
    case Mnemonic::FNDISI: case Mnemonic::FNENI: case Mnemonic::FNSETPM:
    case Mnemonic::FNSTENV:
    case Mnemonic::FPREM1:
    case Mnemonic::FSAVE: case Mnemonic::FSCALE: case Mnemonic::FSETPM:
    case Mnemonic::FSINCOS: case Mnemonic::FSTENV:
    case Mnemonic::FXAM: case Mnemonic::FXTRACT:
    case Mnemonic::FYL2X: case Mnemonic::FYL2XP1:
        return StepResult::Unsupported;
    case Mnemonic::BEXTR: case Mnemonic::MULX: case Mnemonic::PDEP: case Mnemonic::PEXT:
        return StepResult::Unsupported;
    case Mnemonic::BNDCL: case Mnemonic::BNDCN: case Mnemonic::BNDCU:
    case Mnemonic::BNDLDX: case Mnemonic::BNDMK: case Mnemonic::BNDMOV: case Mnemonic::BNDSTX:
        return StepResult::Unsupported;
    case Mnemonic::ADX: case Mnemonic::AMX: case Mnemonic::INVALID:
        return StepResult::Unsupported;
    case Mnemonic::VPXORD: case Mnemonic::VPXORQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = xmm_reg_index(di, di.desc->operands[1]);
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a ^ b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPANDD: case Mnemonic::VPANDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = xmm_reg_index(di, di.desc->operands[1]);
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a & b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPORD: case Mnemonic::VPORQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = xmm_reg_index(di, di.desc->operands[1]);
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = a | b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPANDND: case Mnemonic::VPANDNQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = xmm_reg_index(di, di.desc->operands[1]);
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
        for (int i = 0; i < count; i++) {
            uint64_t a, b; memcpy(&a, &cpu.zmm[src1][i], 8); memcpy(&b, &cpu.zmm[src2][i], 8);
            uint64_t r = (~a) & b; memcpy(&cpu.zmm[dst][i], &r, 8);
        }
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPTERNLOGD: case Mnemonic::VPTERNLOGQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = xmm_reg_index(di, di.desc->operands[1]);
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        uint8_t imm = (uint8_t)di.immediate;
        bool is_q = (di.desc->mnemonic == Mnemonic::VPTERNLOGQ) || di.vex_W;
        if (is_q) {
            int count = (di.vex_L >= 2) ? 8 : ((di.vex_L >= 1) ? 4 : 2);
            for (int i = 0; i < count; i++) {
                uint64_t a, b, c; memcpy(&a, &cpu.zmm[dst][i], 8); memcpy(&b, &cpu.zmm[src1][i], 8); memcpy(&c, &cpu.zmm[src2][i], 8);
                uint64_t r = 0;
                for (int bit = 0; bit < 64; bit++) {
                    int idx = ((a >> bit) & 1) << 2 | ((b >> bit) & 1) << 1 | ((c >> bit) & 1);
                    r |= (uint64_t)((imm >> idx) & 1) << bit;
                }
                memcpy(&cpu.zmm[dst][i], &r, 8);
            }
        } else {
            int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
            uint32_t d[16], s1[16], s2[16];
            memcpy(d, cpu.zmm[dst], count*4); memcpy(s1, cpu.zmm[src1], count*4); memcpy(s2, cpu.zmm[src2], count*4);
            for (int i = 0; i < count; i++) {
                uint32_t r = 0;
                for (int bit = 0; bit < 32; bit++) {
                    int idx = ((d[i] >> bit) & 1) << 2 | ((s1[i] >> bit) & 1) << 1 | ((s2[i] >> bit) & 1);
                    r |= (uint32_t)((imm >> idx) & 1) << bit;
                }
                d[i] = r;
            }
            memcpy(cpu.zmm[dst], d, count*4);
        }
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPDPBUSD: case Mnemonic::VPDPBUSDS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = xmm_reg_index(di, di.desc->operands[1]);
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
        uint32_t d[16], s1[16], s2[16]; memcpy(d, cpu.zmm[dst], count*4); memcpy(s1, cpu.zmm[src1], count*4); memcpy(s2, cpu.zmm[src2], count*4);
        bool sat = di.desc->mnemonic == Mnemonic::VPDPBUSDS;
        for (int i = 0; i < count; i++) {
            int64_t sum = (int32_t)d[i];
            for (int j = 0; j < 4; j++) {
                uint8_t a = (s1[i] >> (j*8)) & 0xFF;
                int8_t b = (int8_t)((s2[i] >> (j*8)) & 0xFF);
                sum += (int64_t)a * b;
            }
            if (sat) { if (sum > INT32_MAX) sum = INT32_MAX; if (sum < INT32_MIN) sum = INT32_MIN; }
            d[i] = (uint32_t)(int32_t)sum;
        }
        memcpy(cpu.zmm[dst], d, count*4); vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPDPWSSD: case Mnemonic::VPDPWSSDS: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = xmm_reg_index(di, di.desc->operands[1]);
        int src2 = xmm_reg_index(di, di.desc->operands[2]);
        int count = (di.vex_L >= 2) ? 16 : ((di.vex_L >= 1) ? 8 : 4);
        uint32_t d[16]; memcpy(d, cpu.zmm[dst], count*4);
        uint16_t s1[32], s2[32]; memcpy(s1, cpu.zmm[src1], count*4); memcpy(s2, cpu.zmm[src2], count*4);
        bool sat = di.desc->mnemonic == Mnemonic::VPDPWSSDS;
        for (int i = 0; i < count; i++) {
            int64_t sum = (int32_t)d[i];
            sum += (int64_t)(int16_t)s1[i*2] * (int16_t)s2[i*2];
            sum += (int64_t)(int16_t)s1[i*2+1] * (int16_t)s2[i*2+1];
            if (sat) { if (sum > INT32_MAX) sum = INT32_MAX; if (sum < INT32_MIN) sum = INT32_MIN; }
            d[i] = (uint32_t)(int32_t)sum;
        }
        memcpy(cpu.zmm[dst], d, count*4); vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VMOVDQA32: case Mnemonic::VMOVDQA64:
    case Mnemonic::VMOVDQU8: case Mnemonic::VMOVDQU16: case Mnemonic::VMOVDQU32: case Mnemonic::VMOVDQU64: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        int bytes = (di.vex_L >= 2) ? 64 : ((di.vex_L >= 1) ? 32 : 16);
        memcpy(cpu.zmm[dst], cpu.zmm[src], bytes);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VBLENDMPS: case Mnemonic::VBLENDMPD:
    case Mnemonic::VPBLENDMD: case Mnemonic::VPBLENDMQ:
    case Mnemonic::VPBLENDMB: case Mnemonic::VPBLENDMW: {
        // Without mask support, just copy src1 (vvvv) to dst
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src1 = xmm_reg_index(di, di.desc->operands[1]);
        int bytes = (di.vex_L >= 2) ? 64 : ((di.vex_L >= 1) ? 32 : 16);
        memcpy(cpu.zmm[dst], cpu.zmm[src1], bytes);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VCOMPRESSPS: case Mnemonic::VCOMPRESSPD:
    case Mnemonic::VPCOMPRESSD: case Mnemonic::VPCOMPRESSQ:
    case Mnemonic::VEXPANDPS: case Mnemonic::VEXPANDPD:
    case Mnemonic::VPEXPANDD: case Mnemonic::VPEXPANDQ: {
        int dst = xmm_reg_index(di, di.desc->operands[0]);
        int src = xmm_reg_index(di, di.desc->operands[1]);
        int bytes = (di.vex_L >= 2) ? 64 : ((di.vex_L >= 1) ? 32 : 16);
        memcpy(cpu.zmm[dst], cpu.zmm[src], bytes);
        vex_zero_upper(cpu, di, dst); break;
    }
    case Mnemonic::VPROLVD: case Mnemonic::VPROLVQ:
    case Mnemonic::VPRORVD: case Mnemonic::VPRORVQ:
    case Mnemonic::VPROLD: case Mnemonic::VPROLQ:
    case Mnemonic::VPRORD: case Mnemonic::VPRORQ:
    case Mnemonic::VPERMI2D: case Mnemonic::VPERMI2Q: case Mnemonic::VPERMI2PS: case Mnemonic::VPERMI2PD:
    case Mnemonic::VPERMT2D: case Mnemonic::VPERMT2Q: case Mnemonic::VPERMT2PS: case Mnemonic::VPERMT2PD:
    case Mnemonic::VPERMW: case Mnemonic::VPERMB:
    case Mnemonic::VPTESTMD: case Mnemonic::VPTESTMQ: case Mnemonic::VPTESTMB: case Mnemonic::VPTESTMW:
    case Mnemonic::VPTESTNMD: case Mnemonic::VPTESTNMQ: case Mnemonic::VPTESTNMB: case Mnemonic::VPTESTNMW:
    case Mnemonic::VPCONFLICTD: case Mnemonic::VPCONFLICTQ:
    case Mnemonic::VPLZCNTD: case Mnemonic::VPLZCNTQ:
    case Mnemonic::VFIXUPIMMPS: case Mnemonic::VFIXUPIMMPD: case Mnemonic::VFIXUPIMMSS: case Mnemonic::VFIXUPIMMSD:
    case Mnemonic::VGETEXPPS: case Mnemonic::VGETEXPPD: case Mnemonic::VGETEXPSS: case Mnemonic::VGETEXPSD:
    case Mnemonic::VGETMANTPS: case Mnemonic::VGETMANTPD: case Mnemonic::VGETMANTSS: case Mnemonic::VGETMANTSD:
    case Mnemonic::VRNDSCALEPS: case Mnemonic::VRNDSCALEPD: case Mnemonic::VRNDSCALESS: case Mnemonic::VRNDSCALESD:
    case Mnemonic::VSCALEFPS: case Mnemonic::VSCALEFPD: case Mnemonic::VSCALEFSS: case Mnemonic::VSCALEFSD:
    case Mnemonic::VRANGEPS: case Mnemonic::VRANGEPD: case Mnemonic::VRANGESS: case Mnemonic::VRANGESD:
    case Mnemonic::VREDUCEPS: case Mnemonic::VREDUCEPD: case Mnemonic::VREDUCESS: case Mnemonic::VREDUCESD:
    case Mnemonic::VFPCLASSPS: case Mnemonic::VFPCLASSPD: case Mnemonic::VFPCLASSSS: case Mnemonic::VFPCLASSSD:
    case Mnemonic::VDBPSADBW: case Mnemonic::VPALIGNR:
    case Mnemonic::VCVTPS2UDQ: case Mnemonic::VCVTPD2UDQ: case Mnemonic::VCVTTPS2UDQ: case Mnemonic::VCVTTPD2UDQ:
    case Mnemonic::VCVTUDQ2PS: case Mnemonic::VCVTUDQ2PD: case Mnemonic::VCVTUSI2SS: case Mnemonic::VCVTUSI2SD:
    case Mnemonic::VCVTSS2USI: case Mnemonic::VCVTSD2USI: case Mnemonic::VCVTTSS2USI: case Mnemonic::VCVTTSD2USI:
    case Mnemonic::VCVTQQ2PS: case Mnemonic::VCVTQQ2PD: case Mnemonic::VCVTUQQ2PS: case Mnemonic::VCVTUQQ2PD:
    case Mnemonic::VCVTPS2QQ: case Mnemonic::VCVTPD2QQ: case Mnemonic::VCVTPS2UQQ: case Mnemonic::VCVTPD2UQQ:
    case Mnemonic::VCVTTPS2QQ: case Mnemonic::VCVTTPD2QQ: case Mnemonic::VCVTTPS2UQQ: case Mnemonic::VCVTTPD2UQQ:
    case Mnemonic::VINSERTF32X4: case Mnemonic::VINSERTF64X2: case Mnemonic::VINSERTF32X8: case Mnemonic::VINSERTF64X4:
    case Mnemonic::VINSERTI32X4: case Mnemonic::VINSERTI64X2: case Mnemonic::VINSERTI32X8: case Mnemonic::VINSERTI64X4:
    case Mnemonic::VEXTRACTF32X4: case Mnemonic::VEXTRACTF64X2: case Mnemonic::VEXTRACTF32X8: case Mnemonic::VEXTRACTF64X4:
    case Mnemonic::VEXTRACTI32X4: case Mnemonic::VEXTRACTI64X2: case Mnemonic::VEXTRACTI32X8: case Mnemonic::VEXTRACTI64X4:
    case Mnemonic::VBROADCASTF32X2: case Mnemonic::VBROADCASTF32X4: case Mnemonic::VBROADCASTF32X8:
    case Mnemonic::VBROADCASTF64X2: case Mnemonic::VBROADCASTF64X4:
    case Mnemonic::VBROADCASTI32X2: case Mnemonic::VBROADCASTI32X4: case Mnemonic::VBROADCASTI32X8:
    case Mnemonic::VBROADCASTI64X2: case Mnemonic::VBROADCASTI64X4:
    case Mnemonic::VSCATTERDPS: case Mnemonic::VSCATTERDPD: case Mnemonic::VSCATTERQPS: case Mnemonic::VSCATTERQPD:
    case Mnemonic::VPSCATTERDD: case Mnemonic::VPSCATTERDQ: case Mnemonic::VPSCATTERQD: case Mnemonic::VPSCATTERQQ:
    case Mnemonic::VPMADD52LUQ: case Mnemonic::VPMADD52HUQ:
    case Mnemonic::VCVTNEEBF162PS: case Mnemonic::VCVTNEOBF162PS:
    case Mnemonic::VCVTNEEPH2PS: case Mnemonic::VCVTNEOPH2PS: case Mnemonic::VCVTNEPS2BF16:
    case Mnemonic::KADDB: case Mnemonic::KADDD: case Mnemonic::KADDQ: case Mnemonic::KADDW:
    case Mnemonic::KANDB: case Mnemonic::KANDNB: case Mnemonic::KANDND: case Mnemonic::KANDNQ:
    case Mnemonic::KORB: case Mnemonic::KXORB: case Mnemonic::KXNORB: case Mnemonic::KXNORW: case Mnemonic::KXNORD: case Mnemonic::KXNORQ:
    case Mnemonic::KNOTB: case Mnemonic::KORTESTB:
    case Mnemonic::KTESTB: case Mnemonic::KTESTW: case Mnemonic::KTESTD: case Mnemonic::KTESTQ:
    case Mnemonic::KSHIFTLB: case Mnemonic::KSHIFTLD: case Mnemonic::KSHIFTLQ:
    case Mnemonic::KSHIFTRB: case Mnemonic::KSHIFTRD: case Mnemonic::KSHIFTRQ:
    case Mnemonic::LDTILECFG: case Mnemonic::STTILECFG:
    case Mnemonic::TILELOADD: case Mnemonic::TILELOADDT1: case Mnemonic::TILESTORED:
    case Mnemonic::TILEZERO: case Mnemonic::TILERELEASE:
    case Mnemonic::TDPBF16PS: case Mnemonic::TDPBSSD: case Mnemonic::TDPBSUD: case Mnemonic::TDPBUSD: case Mnemonic::TDPBUUD: case Mnemonic::TDPFP16PS:
    case Mnemonic::VMASKMOVPS: case Mnemonic::VMASKMOVPD:
    case Mnemonic::INSERTQ: case Mnemonic::EXTRQ:
    case Mnemonic::PCONFIG:
        return StepResult::Unsupported;
    default:
        return StepResult::Unsupported;
    }
    return StepResult::OK;
}

static EmuHandler g_emu_dispatch[2048] = {};
static bool g_emu_dispatch_ready = false;

static void emu_dispatch_init() {
    if (g_emu_dispatch_ready) return;
    for (size_t i = 0; i < g_instr_table_size; ++i)
        g_emu_dispatch[i] = &emu_exec_switch;
    g_emu_dispatch_ready = true;
}

StepResult emu_step(CpuState& cpu) {
    emu_dispatch_init();
    if (cpu.rip >= cpu.mem_size) return StepResult::MemFault;
    DecodedInstr di;
    memset(&di, 0, sizeof(di));
    size_t consumed = decode(cpu.mem + cpu.rip, cpu.mem_size - cpu.rip, di);
    if (consumed == 0 || !di.desc) return StepResult::DecodeError;

    int bits = 32;
    if (di.desc->num_operands > 0) bits = resolve_opsize(di, di.desc->operands[0]);

    ptrdiff_t idx = di.desc - g_instr_table;
    if (idx >= 0 && static_cast<size_t>(idx) < g_instr_table_size && g_emu_dispatch[idx]) {
        uint64_t saved_rip = cpu.rip;
        StepResult r = g_emu_dispatch[idx](cpu, di, bits);
        if ((r == StepResult::OK || r == StepResult::Unsupported) && cpu.rip == saved_rip)
            cpu.rip += di.length;
        return r;
    }
    cpu.rip += di.length;
    return StepResult::Unsupported;
}

size_t emu_run(CpuState& cpu, size_t max_steps) {
    for (size_t i = 0; i < max_steps; ++i) {
        StepResult r = emu_step(cpu);
        if (r != StepResult::OK) return i + 1;
    }
    return max_steps;
}

} // namespace vedx64
