// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/instruction.hpp"

namespace vedx64 {

namespace {

uint8_t resolve_op_size_bytes(OpSize sz, bool rex_w, bool has_66) {
    switch (sz) {
    case OpSize::Byte:         return 1;
    case OpSize::Word:         return 2;
    case OpSize::Dword:        return 4;
    case OpSize::Qword:        return 8;
    case OpSize::OpSz:         return rex_w ? 8 : (has_66 ? 2 : 4);
    case OpSize::OpSzQ:        return rex_w ? 8 : (has_66 ? 2 : 4);
    case OpSize::OpSzDS:       return rex_w ? 8 : 4;
    case OpSize::ByteSignExt:  return 1;
    case OpSize::Xmm128:       return 16;
    case OpSize::PackedSingle:  return 16;
    case OpSize::PackedDouble:  return 16;
    case OpSize::ScalarSingle:  return 4;
    case OpSize::ScalarDouble:  return 8;
    case OpSize::Mmx64:        return 8;
    case OpSize::PseudoDesc:   return has_66 ? 6 : 10;
    case OpSize::BoundPair:    return has_66 ? 4 : 8;
    case OpSize::FarPtr:       return has_66 ? 4 : 6;
    default:                   return 0;
    }
}

RegisterClass gpr_class(uint8_t size_bits) {
    switch (size_bits) {
    case 8:  return RegisterClass::GPR8;
    case 16: return RegisterClass::GPR16;
    case 32: return RegisterClass::GPR32;
    case 64: return RegisterClass::GPR64;
    default: return RegisterClass::GPR32;
    }
}

inline uint8_t modrm_reg(uint8_t modrm) { return (modrm >> 3) & 7; }
inline uint8_t modrm_rm(uint8_t modrm) { return modrm & 7; }

} // anonymous namespace

Instruction Instruction::from_decoded(const DecodedInstr& di, const uint8_t* code, uint64_t addr) {
    Instruction instr;
    if (!di.desc) return instr;

    instr.mnemonic = di.desc->mnemonic;
    instr.length = di.length;
    instr.address = addr;
    if (code && di.length > 0) instr.raw_bytes.assign(code, code + di.length);

    ptrdiff_t idx = di.desc - g_instr_table;
    if (idx >= 0 && static_cast<size_t>(idx) < g_instr_table_size)
        instr.encoding_id = static_cast<EncodingId>(idx + 1);

    bool rex_w = (di.rex & 0x08) != 0 || di.vex_W;
    bool has_66 = false;
    for (uint8_t i = 0; i < di.num_prefixes; ++i) {
        switch (di.legacy_prefix[i]) {
        case 0xF0: instr.prefixes |= PFX_LOCK; break;
        case 0xF3: instr.prefixes |= PFX_REP; break;
        case 0xF2: instr.prefixes |= PFX_REPNE; break;
        case 0x66: instr.prefixes |= PFX_DATA16; has_66 = true; break;
        case 0x67: instr.prefixes |= PFX_ADDR32; break;
        case 0x26: instr.prefixes = (instr.prefixes & ~PFX_SEG_MASK) | PFX_SEG_ES; break;
        case 0x2E: instr.prefixes = (instr.prefixes & ~PFX_SEG_MASK) | PFX_SEG_CS; break;
        case 0x36: instr.prefixes = (instr.prefixes & ~PFX_SEG_MASK) | PFX_SEG_SS; break;
        case 0x3E: instr.prefixes = (instr.prefixes & ~PFX_SEG_MASK) | PFX_SEG_DS; break;
        case 0x64: instr.prefixes = (instr.prefixes & ~PFX_SEG_MASK) | PFX_SEG_FS; break;
        case 0x65: instr.prefixes = (instr.prefixes & ~PFX_SEG_MASK) | PFX_SEG_GS; break;
        default: break;
        }
    }

    for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
        const OperandDesc& od = di.desc->operands[i];
        uint8_t sz_bytes = resolve_op_size_bytes(od.size, rex_w, has_66);
        uint8_t sz_bits = sz_bytes * 8;

        switch (od.addr) {
        case AddrMode::ModRM_Reg: {
            uint8_t id = modrm_reg(di.modrm) | ((di.rex & 0x04) ? 8 : 0);
            instr.operands.push_back(Operand::make_reg(gpr_class(sz_bits), id, sz_bits));
            break;
        }
        case AddrMode::ModRM_RM:
        case AddrMode::MemOnly:
        case AddrMode::RegOnly: {
            uint8_t mod_ = (di.modrm >> 6) & 3;
            uint8_t rm = modrm_rm(di.modrm) | ((di.rex & 0x01) ? 8 : 0);
            if (mod_ == 3) {
                instr.operands.push_back(Operand::make_reg(gpr_class(sz_bits), rm, sz_bits));
            } else {
                uint8_t base = 0xFF, index = 0xFF, scale = 1;
                bool rip_rel = false;
                if ((rm & 7) == 4) {
                    // SIB byte present
                    base = (di.sib & 7) | ((di.rex & 0x01) ? 8 : 0);
                    uint8_t sib_idx = ((di.sib >> 3) & 7) | ((di.rex & 0x02) ? 8 : 0);
                    scale = 1u << ((di.sib >> 6) & 3);
                    if (sib_idx != 4) index = sib_idx; // rsp cannot be index
                    if ((base & 7) == 5 && mod_ == 0) base = 0xFF; // disp32 only
                } else if ((rm & 7) == 5 && mod_ == 0) {
                    rip_rel = true;
                } else {
                    base = rm;
                }
                instr.operands.push_back(Operand::make_mem(base, index, scale, di.displacement, sz_bytes, rip_rel));
            }
            break;
        }
        case AddrMode::Immediate:
            instr.operands.push_back(Operand::make_imm(di.immediate, sz_bytes));
            break;
        case AddrMode::RelOffset:
            instr.operands.push_back(Operand::make_rel(di.immediate, sz_bytes));
            break;
        case AddrMode::OpcodeReg: {
            uint8_t id = di.opcode_reg | ((di.rex & 0x01) ? 8 : 0);
            instr.operands.push_back(Operand::make_reg(gpr_class(sz_bits), id, sz_bits));
            break;
        }
        case AddrMode::Fixed: {
            uint8_t id = od.fixed_reg;
            if (id == 0xFF) id = 0;
            instr.operands.push_back(Operand::make_reg(gpr_class(sz_bits), id, sz_bits));
            break;
        }
        case AddrMode::XmmReg: {
            uint8_t id = modrm_reg(di.modrm) | ((di.rex & 0x04) ? 8 : 0);
            instr.operands.push_back(Operand::make_reg(RegisterClass::XMM, id, 128));
            break;
        }
        case AddrMode::VexReg: {
            uint8_t sz = (di.vex_W || (di.rex & 0x08)) ? 64 : 32;
            instr.operands.push_back(Operand::make_reg(gpr_class(sz), di.vex_vvvv & 15, sz));
            break;
        }
        case AddrMode::XmmVvvv: {
            instr.operands.push_back(Operand::make_reg(RegisterClass::XMM, di.vex_vvvv & 15, di.vex_L >= 1 ? 256 : 128));
            break;
        }
        case AddrMode::XmmRM:
        case AddrMode::XmmRegOnly: {
            uint8_t mod_ = (di.modrm >> 6) & 3;
            uint8_t rm = modrm_rm(di.modrm) | ((di.rex & 0x01) ? 8 : 0);
            if (mod_ == 3) {
                instr.operands.push_back(Operand::make_reg(RegisterClass::XMM, rm, 128));
            } else {
                uint8_t base = 0xFF, index = 0xFF, scale = 1;
                bool rip_rel = false;
                if ((rm & 7) == 4) {
                    base = (di.sib & 7) | ((di.rex & 0x01) ? 8 : 0);
                    uint8_t sib_idx = ((di.sib >> 3) & 7) | ((di.rex & 0x02) ? 8 : 0);
                    scale = 1u << ((di.sib >> 6) & 3);
                    if (sib_idx != 4) index = sib_idx;
                    if ((base & 7) == 5 && mod_ == 0) base = 0xFF;
                } else if ((rm & 7) == 5 && mod_ == 0) {
                    rip_rel = true;
                } else {
                    base = rm;
                }
                instr.operands.push_back(Operand::make_mem(base, index, scale, di.displacement, sz_bytes, rip_rel));
            }
            break;
        }
        case AddrMode::MmxReg:
        case AddrMode::MmxRegOnly: {
            uint8_t id = modrm_reg(di.modrm);
            instr.operands.push_back(Operand::make_reg(RegisterClass::MMX, id, 64));
            break;
        }
        case AddrMode::MmxRM: {
            uint8_t mod_ = (di.modrm >> 6) & 3;
            uint8_t rm = modrm_rm(di.modrm);
            if (mod_ == 3) {
                instr.operands.push_back(Operand::make_reg(RegisterClass::MMX, rm, 64));
            } else {
                uint8_t base = rm, index = 0xFF, scale = 1;
                bool rip_rel = false;
                if ((rm & 7) == 4) {
                    base = di.sib & 7;
                    uint8_t sib_idx = (di.sib >> 3) & 7;
                    scale = 1u << ((di.sib >> 6) & 3);
                    if (sib_idx != 4) index = sib_idx;
                    if (base == 5 && mod_ == 0) base = 0xFF;
                } else if (rm == 5 && mod_ == 0) {
                    rip_rel = true; base = 0xFF;
                }
                instr.operands.push_back(Operand::make_mem(base, index, scale, di.displacement, 8, rip_rel));
            }
            break;
        }
        case AddrMode::SegReg: {
            uint8_t id = modrm_reg(di.modrm);
            instr.operands.push_back(Operand::make_reg(RegisterClass::Segment, id, 16));
            break;
        }
        case AddrMode::CtrlReg: {
            uint8_t id = modrm_reg(di.modrm) | ((di.rex & 0x04) ? 8 : 0);
            instr.operands.push_back(Operand::make_reg(RegisterClass::Control, id, 64));
            break;
        }
        case AddrMode::DbgReg: {
            uint8_t id = modrm_reg(di.modrm) | ((di.rex & 0x04) ? 8 : 0);
            instr.operands.push_back(Operand::make_reg(RegisterClass::Debug, id, 64));
            break;
        }
        case AddrMode::Flags:
            instr.operands.push_back(Operand::make_reg(RegisterClass::Flags, 0, sz_bits));
            break;
        case AddrMode::Moffset:
            instr.operands.push_back(Operand::make_mem(0xFF, 0xFF, 1, di.immediate, sz_bytes));
            break;
        case AddrMode::StringSrc:
            instr.operands.push_back(Operand::make_mem(6, 0xFF, 1, 0, sz_bytes)); // rsi
            break;
        case AddrMode::StringDst:
            instr.operands.push_back(Operand::make_mem(7, 0xFF, 1, 0, sz_bytes)); // rdi
            break;
        case AddrMode::Stack:
            instr.operands.push_back(Operand::make_mem(4, 0xFF, 1, 0, sz_bytes)); // rsp
            break;
        default:
            break;
        }
    }

    return instr;
}

size_t Instruction::decode_instruction(const uint8_t* code, size_t len, Instruction& out, uint64_t rip) {
    DecodedInstr di{};
    size_t n = decode(code, len, di);
    if (n == 0) return 0;
    out = from_decoded(di, code, rip);
    return n;
}

} // namespace vedx64
