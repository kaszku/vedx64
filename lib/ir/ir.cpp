// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#ifdef VEDX64_IR
#include "vedx64/ir.hpp"
#include <cstring>

namespace vedx64 {
namespace ir {

namespace {

Op make_op2(Opcode opc, VarNode dst, VarNode src) {
    Op op;
    op.opcode = opc;
    op.output = dst;
    op.inputs[0] = src;
    op.num_inputs = 1;
    return op;
}

Op make_op3(Opcode opc, VarNode dst, VarNode s0, VarNode s1) {
    Op op;
    op.opcode = opc;
    op.output = dst;
    op.inputs[0] = s0;
    op.inputs[1] = s1;
    op.num_inputs = 2;
    return op;
}

Op make_op4(Opcode opc, VarNode dst, VarNode s0, VarNode s1, VarNode s2) {
    Op op;
    op.opcode = opc;
    op.output = dst;
    op.inputs[0] = s0;
    op.inputs[1] = s1;
    op.inputs[2] = s2;
    op.num_inputs = 3;
    return op;
}

uint8_t op_size_bytes(OpSize sz, bool rex_w, bool has_66) {
    switch (sz) {
    case OpSize::Byte: return 1;
    case OpSize::Word: return 2;
    case OpSize::Dword: return 4;
    case OpSize::Qword: return 8;
    case OpSize::OpSz: case OpSize::OpSzQ: return rex_w ? 8 : (has_66 ? 2 : 4);
    case OpSize::OpSzDS: return rex_w ? 8 : 4;
    case OpSize::ByteSignExt: return 1;
    default: return 4;
    }
}

VarNode reg_from_modrm_reg(const DecodedInstr& di, uint8_t sz) {
    uint8_t id = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
    return VarNode::gpr(id, sz);
}

VarNode reg_from_modrm_rm(const DecodedInstr& di, uint8_t sz) {
    uint8_t id = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
    return VarNode::gpr(id, sz);
}

VarNode compute_ea(Lifted& l, const DecodedInstr& di) {
    VarNode ea = VarNode::temp(10, 8);
    uint8_t mod_ = di.modrm >> 6;
    uint8_t rm = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
    bool has_sib = (di.modrm & 7) == 4 && mod_ != 3;

    if (has_sib) {
        uint8_t base = (di.sib & 7) | ((di.rex & 0x01) ? 8 : 0);
        uint8_t idx  = ((di.sib >> 3) & 7) | ((di.rex & 0x02) ? 8 : 0);
        uint8_t sc   = di.sib >> 6;
        bool no_base  = ((di.sib & 7) == 5) && mod_ == 0;
        bool no_index = (((di.sib >> 3) & 7) == 4) && !(di.rex & 0x02);

        if (no_base || mod_ != 0) {
            l.ops.push_back(make_op2(Opcode::COPY, ea, VarNode::constant(di.displacement, 8)));
        } else {
            l.ops.push_back(make_op2(Opcode::COPY, ea, VarNode::constant(0, 8)));
        }

        if (!no_base) {
            l.ops.push_back(make_op3(Opcode::ADD, ea, ea, VarNode::gpr(base, 8)));
        }

        if (!no_index) {
            VarNode scaled = VarNode::temp(11, 8);
            if (sc == 0) {
                scaled = VarNode::gpr(idx, 8);
            } else {
                l.ops.push_back(make_op3(Opcode::MUL, scaled, VarNode::gpr(idx, 8), VarNode::constant((int64_t)1 << sc, 8)));
            }
            l.ops.push_back(make_op3(Opcode::ADD, ea, ea, scaled));
        }
        if (!no_base && mod_ != 0) {
            // displacement already in ea from COPY above
        }
    } else {
        if ((di.modrm & 7) == 5 && mod_ == 0) {
            int64_t target = (int64_t)di.displacement;
            l.ops.push_back(make_op2(Opcode::COPY, ea, VarNode::constant(target, 8)));
        } else if (mod_ == 0) {
            l.ops.push_back(make_op2(Opcode::COPY, ea, VarNode::gpr(rm, 8)));
        } else {
            l.ops.push_back(make_op3(Opcode::ADD, ea, VarNode::gpr(rm, 8), VarNode::constant(di.displacement, 8)));
        }
    }
    return ea;
}

} // anonymous namespace

using LiftHandler = bool(*)(Lifted&, const DecodedInstr&, uint8_t, bool, bool, size_t, uint64_t);

static bool lift_exec_switch(Lifted& l, const DecodedInstr& di, uint8_t sz, bool rex_w, bool has_66, size_t n, uint64_t address) {
    Mnemonic m = di.desc->mnemonic;
    uint8_t mod_ = di.modrm >> 6;
    bool is_mem = di.desc->has_modrm && mod_ != 3;
    (void)is_mem;

    if (m == Mnemonic::NOP || m == Mnemonic::HINT_NOP) {
        l.ops.push_back({Opcode::NOP});
        return true;
    }

    if (m == Mnemonic::MOV && di.desc->num_operands >= 2 && di.desc->has_modrm) {
        if (mod_ == 3) {
            VarNode dst, src;
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM) {
                dst = reg_from_modrm_rm(di, sz); src = reg_from_modrm_reg(di, sz);
            } else {
                dst = reg_from_modrm_reg(di, sz); src = reg_from_modrm_rm(di, sz);
            }
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM || di.desc->operands[0].addr == AddrMode::MemOnly) {
                // MOV [mem], reg/imm — store
                bool has_imm = false;
                for (uint8_t i = 0; i < di.desc->num_operands; ++i)
                    if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
                VarNode val = has_imm ? VarNode::constant(di.immediate, sz) : reg_from_modrm_reg(di, sz);
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, val));
            } else {
                // MOV reg, [mem] — load
                VarNode dst = reg_from_modrm_reg(di, sz);
                l.ops.push_back(make_op3(Opcode::LOAD, dst, ea, VarNode::ram(sz)));
            }
        }
        return true;
    }

    if (m == Mnemonic::MOV && di.desc->num_operands >= 2) {
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::Immediate ||
                di.desc->operands[i].addr == AddrMode::OpcodeReg) {
                VarNode dst;
                if (di.desc->operands[0].addr == AddrMode::OpcodeReg) {
                    uint8_t id = di.opcode_reg | ((di.rex & 0x01) ? 8 : 0);
                    dst = VarNode::gpr(id, sz);
                } else if (di.desc->operands[0].addr == AddrMode::ModRM_RM && ((di.modrm >> 6) & 3) == 3) {
                    dst = reg_from_modrm_rm(di, sz);
                } else break;
                l.ops.push_back(make_op2(Opcode::COPY, dst, VarNode::constant(di.immediate, sz)));
                return true;
            }
        }
    }

    if ((m == Mnemonic::ADD || m == Mnemonic::SUB || m == Mnemonic::AND ||
         m == Mnemonic::OR || m == Mnemonic::XOR) &&
        di.desc->num_operands >= 2 && di.desc->has_modrm) {
        Opcode opc = Opcode::ADD;
        if (m == Mnemonic::SUB) opc = Opcode::SUB;
        else if (m == Mnemonic::AND) opc = Opcode::AND;
        else if (m == Mnemonic::OR) opc = Opcode::OR;
        else if (m == Mnemonic::XOR) opc = Opcode::XOR;
        Opcode flags_opc = (m == Mnemonic::AND || m == Mnemonic::OR || m == Mnemonic::XOR)
            ? Opcode::AND_FLAGS : (m == Mnemonic::SUB ? Opcode::SUB_FLAGS : Opcode::ADD_FLAGS);
        // Check for immediate second operand
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        if (mod_ == 3) {
            // Register forms
            VarNode dst, src;
            if (has_imm) {
                dst = reg_from_modrm_rm(di, sz);
                src = VarNode::constant(di.immediate, sz);
            } else if (di.desc->operands[0].addr == AddrMode::ModRM_RM) {
                dst = reg_from_modrm_rm(di, sz); src = reg_from_modrm_reg(di, sz);
            } else {
                dst = reg_from_modrm_reg(di, sz); src = reg_from_modrm_rm(di, sz);
            }
            // Compute into temp, flags from original operands (ADD/SUB) or result (AND/OR/XOR), then copy to dst
            VarNode tmp_res = VarNode::temp(30, sz);
            l.ops.push_back(make_op3(opc, tmp_res, dst, src));
            if (flags_opc == Opcode::ADD_FLAGS || flags_opc == Opcode::SUB_FLAGS)
                l.ops.push_back(make_op3(flags_opc, VarNode::flags(), dst, src)); // from original operands
            else
                l.ops.push_back(make_op3(flags_opc, VarNode::flags(), tmp_res, VarNode::constant(0, sz))); // from result
            l.ops.push_back(make_op2(Opcode::COPY, dst, tmp_res));
        } else {
            // Memory forms: LOAD, operate, STORE
            VarNode ea = compute_ea(l, di); // effective address placeholder
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM || di.desc->operands[0].addr == AddrMode::MemOnly) {
                // mem, reg/imm — load mem, op, store back
                VarNode tmp = VarNode::temp(11, sz);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(sz)));
                VarNode src = has_imm ? VarNode::constant(di.immediate, sz) : reg_from_modrm_reg(di, sz);
                VarNode tmp2 = VarNode::temp(30, sz);
                l.ops.push_back(make_op3(opc, tmp2, tmp, src));
                if (flags_opc == Opcode::ADD_FLAGS || flags_opc == Opcode::SUB_FLAGS)
                    l.ops.push_back(make_op3(flags_opc, VarNode::flags(), tmp, src));
                else
                    l.ops.push_back(make_op3(flags_opc, VarNode::flags(), tmp2, VarNode::constant(0, sz)));
                l.ops.push_back(make_op2(Opcode::COPY, tmp, tmp2));
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, tmp));
            } else {
                // reg, mem — load mem into temp, op into reg
                VarNode dst = reg_from_modrm_reg(di, sz);
                VarNode tmp = VarNode::temp(11, sz);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(sz)));
                VarNode tmp2 = VarNode::temp(30, sz);
                l.ops.push_back(make_op3(opc, tmp2, dst, tmp));
                if (flags_opc == Opcode::ADD_FLAGS || flags_opc == Opcode::SUB_FLAGS)
                    l.ops.push_back(make_op3(flags_opc, VarNode::flags(), dst, tmp));
                else
                    l.ops.push_back(make_op3(flags_opc, VarNode::flags(), tmp2, VarNode::constant(0, sz)));
                l.ops.push_back(make_op2(Opcode::COPY, dst, tmp2));
            }
        }
        return true;
    }

    if (m == Mnemonic::PUSH && di.desc->num_operands >= 1) {
        VarNode rsp = VarNode::gpr(4, 8); // RSP
        VarNode eight = VarNode::constant(8, 8);
        l.ops.push_back(make_op3(Opcode::SUB, rsp, rsp, eight));
        bool found_opcreg = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::OpcodeReg) {
                uint8_t id = di.opcode_reg | ((di.rex & 0x01) ? 8 : 0);
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), rsp, VarNode::gpr(id, 8)));
                found_opcreg = true; break;
            }
        }
        if (!found_opcreg && di.desc->has_modrm && mod_ == 3) {
            VarNode reg = reg_from_modrm_rm(di, 8);
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), rsp, reg));
        }
        return true;
    }

    if (m == Mnemonic::POP && di.desc->num_operands >= 1) {
        VarNode rsp = VarNode::gpr(4, 8);
        VarNode eight = VarNode::constant(8, 8);
        bool found_opcreg = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::OpcodeReg) {
                uint8_t id = di.opcode_reg | ((di.rex & 0x01) ? 8 : 0);
                l.ops.push_back(make_op3(Opcode::LOAD, VarNode::gpr(id, 8), rsp, VarNode::ram(8)));
                found_opcreg = true; break;
            }
        }
        if (!found_opcreg && di.desc->has_modrm && mod_ == 3) {
            VarNode reg = reg_from_modrm_rm(di, 8);
            l.ops.push_back(make_op3(Opcode::LOAD, reg, rsp, VarNode::ram(8)));
        }
        l.ops.push_back(make_op3(Opcode::ADD, rsp, rsp, eight));
        return true;
    }

    if (m == Mnemonic::RETN) {
        VarNode rsp = VarNode::gpr(4, 8);
        VarNode rip = VarNode::temp(0, 8);
        VarNode eight = VarNode::constant(8, 8);
        l.ops.push_back(make_op3(Opcode::LOAD, rip, rsp, VarNode::ram(8)));
        l.ops.push_back(make_op3(Opcode::ADD, rsp, rsp, eight));
        l.ops.push_back(make_op2(Opcode::RET, rip, rip));
        return true;
    }

    if (m == Mnemonic::CALL) {
        VarNode rsp = VarNode::gpr(4, 8);
        VarNode eight = VarNode::constant(8, 8);
        VarNode ret_addr = VarNode::constant((int64_t)(address + n), 8);
        l.ops.push_back(make_op3(Opcode::SUB, rsp, rsp, eight));
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), rsp, ret_addr));
        // Branch target
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                VarNode target = VarNode::constant((int64_t)(address + n + di.immediate), 8);
                l.ops.push_back(make_op2(Opcode::CALL, target, target));
                return true;
            }
        }
        // Indirect call
        l.ops.push_back({Opcode::CALL});
        return true;
    }

    if (m == Mnemonic::JMP) {
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                VarNode target = VarNode::constant((int64_t)(address + n + di.immediate), 8);
                l.ops.push_back(make_op2(Opcode::BRANCH, target, target));
                return true;
            }
        }
        if (di.desc->has_modrm) {
            VarNode tgt;
            if (mod_ == 3) { tgt = reg_from_modrm_rm(di, 8); }
            else {
                VarNode ea = compute_ea(l, di);
                tgt = VarNode::temp(30, 8);
                l.ops.push_back(make_op3(Opcode::LOAD, tgt, ea, VarNode::ram(8)));
            }
            l.ops.push_back(make_op2(Opcode::INDIRECT_JMP, tgt, tgt));
            return true;
        }
        l.ops.push_back({Opcode::BRANCH});
        return true;
    }

    if (m == Mnemonic::CMP && di.desc->has_modrm) {
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        VarNode a, b;
        if (mod_ == 3) {
            if (has_imm) {
                a = reg_from_modrm_rm(di, sz); b = VarNode::constant(di.immediate, sz);
            } else if (di.desc->operands[0].addr == AddrMode::ModRM_RM) {
                a = reg_from_modrm_rm(di, sz); b = reg_from_modrm_reg(di, sz);
            } else {
                a = reg_from_modrm_reg(di, sz); b = reg_from_modrm_rm(di, sz);
            }
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode tmp = VarNode::temp(11, sz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(sz)));
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM || di.desc->operands[0].addr == AddrMode::MemOnly) {
                a = tmp; b = has_imm ? VarNode::constant(di.immediate, sz) : reg_from_modrm_reg(di, sz);
            } else {
                a = reg_from_modrm_reg(di, sz); b = tmp;
            }
        }
        l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), a, b));
        return true;
    }

    if (m == Mnemonic::TEST && di.desc->has_modrm) {
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        VarNode a, b;
        if (mod_ == 3) {
            a = reg_from_modrm_rm(di, sz);
            b = has_imm ? VarNode::constant(di.immediate, sz) : reg_from_modrm_reg(di, sz);
        } else {
            VarNode ea = compute_ea(l, di);
            a = VarNode::temp(11, sz);
            l.ops.push_back(make_op3(Opcode::LOAD, a, ea, VarNode::ram(sz)));
            b = has_imm ? VarNode::constant(di.immediate, sz) : reg_from_modrm_reg(di, sz);
        }
        l.ops.push_back(make_op3(Opcode::AND_FLAGS, VarNode::flags(), a, b));
        return true;
    }

    if ((m == Mnemonic::INC || m == Mnemonic::DEC) && di.desc->has_modrm) {
        VarNode one = VarNode::constant(1, sz);
        Opcode opc = (m == Mnemonic::INC) ? Opcode::ADD : Opcode::SUB;
        Opcode flags_opc = (m == Mnemonic::INC) ? Opcode::ADD_FLAGS : Opcode::SUB_FLAGS;
        if (mod_ == 3) {
            VarNode dst = reg_from_modrm_rm(di, sz);
            l.ops.push_back(make_op3(flags_opc, VarNode::flags(), dst, one));
            l.ops.push_back(make_op3(opc, dst, dst, one));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode tmp = VarNode::temp(11, sz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(sz)));
            l.ops.push_back(make_op3(flags_opc, VarNode::flags(), tmp, one));
            l.ops.push_back(make_op3(opc, tmp, tmp, one));
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, tmp));
        }
        return true;
    }

    if ((m == Mnemonic::NEG || m == Mnemonic::NOT) && di.desc->has_modrm) {
        Opcode opc = (m == Mnemonic::NEG) ? Opcode::NEG : Opcode::NOT;
        if (mod_ == 3) {
            VarNode dst = reg_from_modrm_rm(di, sz);
            l.ops.push_back(make_op2(opc, dst, dst));
            if (m == Mnemonic::NEG)
                l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), VarNode::constant(0, sz), dst));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode tmp = VarNode::temp(11, sz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(sz)));
            l.ops.push_back(make_op2(opc, tmp, tmp));
            if (m == Mnemonic::NEG)
                l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), VarNode::constant(0, sz), tmp));
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, tmp));
        }
        return true;
    }

    if ((m == Mnemonic::SHL || m == Mnemonic::SAL || m == Mnemonic::SHR ||
        m == Mnemonic::SAR || m == Mnemonic::ROL || m == Mnemonic::ROR) && di.desc->has_modrm) {
        VarNode count;
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        if (has_imm) count = VarNode::constant(di.immediate, 1);
        else if (di.desc->opcode == 0xD0 || di.desc->opcode == 0xD1) count = VarNode::constant(1, 1);
        else count = VarNode::gpr(1, 1); // CL
        Opcode opc;
        if (m == Mnemonic::SHL || m == Mnemonic::SAL) opc = Opcode::SHL;
        else if (m == Mnemonic::SHR) opc = Opcode::SHR;
        else if (m == Mnemonic::SAR) opc = Opcode::SAR;
        else if (m == Mnemonic::ROL) opc = Opcode::ROL;
        else opc = Opcode::ROR;
        if (mod_ == 3) {
            VarNode dst = reg_from_modrm_rm(di, sz);
            l.ops.push_back(make_op3(opc, dst, dst, count));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode tmp = VarNode::temp(11, sz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(sz)));
            l.ops.push_back(make_op3(opc, tmp, tmp, count));
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, tmp));
        }
        return true;
    }

    if (m == Mnemonic::LEA && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        uint8_t rm = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        if (mod_ == 0 && (di.modrm & 7) == 5) {
            // RIP-relative: dst = RIP + disp + instr_len
            l.ops.push_back(make_op2(Opcode::COPY, dst, VarNode::constant((int64_t)(address + n + di.displacement), sz)));
        } else if ((di.modrm & 7) == 4 && mod_ != 3) {
            // SIB: base + index*scale + disp (simplified: just emit ADD chain)
            uint8_t base_r = (di.sib & 7) | ((di.rex & 0x01) ? 8 : 0);
            uint8_t idx_r = ((di.sib >> 3) & 7) | ((di.rex & 0x02) ? 8 : 0);
            uint8_t scale = di.sib >> 6;
            bool has_base = !(mod_ == 0 && (di.sib & 7) == 5);
            bool has_idx = (idx_r != 4 || (di.rex & 0x02));
            if (has_base) l.ops.push_back(make_op2(Opcode::COPY, dst, VarNode::gpr(base_r, 8)));
            else l.ops.push_back(make_op2(Opcode::COPY, dst, VarNode::constant(di.displacement, 8)));
            if (has_idx) {
                VarNode idx_val = VarNode::gpr(idx_r, 8);
                if (scale > 0) {
                    VarNode sc = VarNode::constant((int64_t)(int64_t)1 << scale, 8);
                    VarNode tmp = VarNode::temp(12, 8);
                    l.ops.push_back(make_op3(Opcode::MUL, tmp, idx_val, sc));
                    l.ops.push_back(make_op3(Opcode::ADD, dst, dst, tmp));
                } else l.ops.push_back(make_op3(Opcode::ADD, dst, dst, idx_val));
            }
            if (has_base && di.displacement != 0)
                l.ops.push_back(make_op3(Opcode::ADD, dst, dst, VarNode::constant(di.displacement, 8)));
        } else {
            // Simple [reg+disp]
            l.ops.push_back(make_op2(Opcode::COPY, dst, VarNode::gpr(rm, 8)));
            if (di.displacement != 0)
                l.ops.push_back(make_op3(Opcode::ADD, dst, dst, VarNode::constant(di.displacement, 8)));
        }
        return true;
    }

    if ((m == Mnemonic::MOVSX || m == Mnemonic::MOVZX || m == Mnemonic::MOVSXD) && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        uint8_t src_sz = op_size_bytes(di.desc->operands[1].size, rex_w, has_66);
        Opcode opc = (m == Mnemonic::MOVZX) ? Opcode::ZEXT : Opcode::SEXT;
        if (mod_ == 3) {
            VarNode src = VarNode::gpr((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), src_sz);
            l.ops.push_back(make_op2(opc, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode tmp = VarNode::temp(11, src_sz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(src_sz)));
            l.ops.push_back(make_op2(opc, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::XCHG) {
        if (di.desc->has_modrm && ((di.modrm >> 6) & 3) == 3) {
            VarNode a = reg_from_modrm_rm(di, sz);
            VarNode b = reg_from_modrm_reg(di, sz);
            VarNode tmp = VarNode::temp(0, sz);
            l.ops.push_back(make_op2(Opcode::COPY, tmp, a));
            l.ops.push_back(make_op2(Opcode::COPY, a, b));
            l.ops.push_back(make_op2(Opcode::COPY, b, tmp));
            return true;
        }
    }

    if (m == Mnemonic::JO || m == Mnemonic::JNO || m == Mnemonic::JB || m == Mnemonic::JNB ||
        m == Mnemonic::JZ || m == Mnemonic::JNZ || m == Mnemonic::JBE || m == Mnemonic::JNBE ||
        m == Mnemonic::JS || m == Mnemonic::JNS || m == Mnemonic::JP || m == Mnemonic::JNP ||
        m == Mnemonic::JL || m == Mnemonic::JNL || m == Mnemonic::JLE || m == Mnemonic::JNLE) {
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                VarNode target = VarNode::constant((int64_t)(address + n + di.immediate), 8);
                VarNode fallthrough = VarNode::constant((int64_t)(address + n), 8);
                l.ops.push_back(make_op3(Opcode::CBRANCH, target, target, fallthrough));
                return true;
            }
        }
    }

    if (m == Mnemonic::LEAVE) {
        VarNode rsp = VarNode::gpr(4, 8);
        VarNode rbp = VarNode::gpr(5, 8);
        VarNode eight = VarNode::constant(8, 8);
        l.ops.push_back(make_op2(Opcode::COPY, rsp, rbp));
        l.ops.push_back(make_op3(Opcode::LOAD, rbp, rsp, VarNode::ram(8)));
        l.ops.push_back(make_op3(Opcode::ADD, rsp, rsp, eight));
        return true;
    }

    if (m == Mnemonic::CWD) {
        uint8_t dsz = rex_w ? 8 : (has_66 ? 2 : 4);
        VarNode rax = VarNode::gpr(0, dsz);
        VarNode rdx = VarNode::gpr(2, dsz);
        VarNode shift = VarNode::constant(dsz * 8 - 1, 1);
        l.ops.push_back(make_op3(Opcode::SAR, rdx, rax, shift));
        return true;
    }

    if (m == Mnemonic::CBW || m == Mnemonic::CWDE) {
        uint8_t dsz = rex_w ? 8 : (has_66 ? 2 : 4);
        uint8_t ssz = dsz / 2;
        VarNode src = VarNode::gpr(0, ssz);
        VarNode dst = VarNode::gpr(0, dsz);
        l.ops.push_back(make_op2(Opcode::SEXT, dst, src));
        return true;
    }

    if (m == Mnemonic::BSWAP) {
        uint8_t id = di.opcode_reg | ((di.rex & 0x01) ? 8 : 0);
        VarNode dst = VarNode::gpr(id, sz);
        // Approximate: rotate by 16 then swap bytes in each half
        // Simplified: emit ROL by 16 for 32-bit swap (not exact but captures the operation)
        VarNode tmp0 = VarNode::temp(12, sz);
        VarNode tmp1 = VarNode::temp(13, sz);
        l.ops.push_back(make_op3(Opcode::ROL, tmp0, dst, VarNode::constant(8, 1)));
        l.ops.push_back(make_op3(Opcode::AND, tmp1, tmp0, VarNode::constant(0x00FF00FF00FF00FFULL, sz)));
        l.ops.push_back(make_op3(Opcode::ROL, tmp0, dst, VarNode::constant(24, 1)));
        l.ops.push_back(make_op3(Opcode::AND, tmp0, tmp0, VarNode::constant(0xFF00FF00FF00FF00ULL, sz)));
        l.ops.push_back(make_op3(Opcode::OR, dst, tmp1, tmp0));
        return true;
    }

    if (m == Mnemonic::CPUID || m == Mnemonic::RDTSC || m == Mnemonic::RDTSCP ||
        m == Mnemonic::SYSCALL || m == Mnemonic::SYSENTER ||
        m == Mnemonic::INT || m == Mnemonic::INT1 || m == Mnemonic::HLT ||
        m == Mnemonic::MFENCE || m == Mnemonic::SFENCE || m == Mnemonic::LFENCE ||
        m == Mnemonic::SERIALIZE || m == Mnemonic::PAUSE || m == Mnemonic::ENDBR32 ||
        m == Mnemonic::ENDBR64 || m == Mnemonic::RDRAND || m == Mnemonic::RDSEED ||
        m == Mnemonic::CLFLUSH || m == Mnemonic::CLFLUSHOPT || m == Mnemonic::WBINVD ||
        m == Mnemonic::INVD || m == Mnemonic::INVLPG || m == Mnemonic::XGETBV ||
        m == Mnemonic::XSETBV || m == Mnemonic::STAC || m == Mnemonic::CLAC ||
        m == Mnemonic::RDMSR || m == Mnemonic::WRMSR || m == Mnemonic::RDPMC ||
        m == Mnemonic::MONITOR || m == Mnemonic::MWAIT || m == Mnemonic::XEND ||
        m == Mnemonic::XTEST || m == Mnemonic::XABORT ||
        m == Mnemonic::FWAIT || m == Mnemonic::FNOP || m == Mnemonic::EMMS) {
        if (m == Mnemonic::RDTSC || m == Mnemonic::RDTSCP)
            l.ops.push_back(make_op2(Opcode::RDTSC, VarNode::gpr(0, 8), VarNode::constant(0, 8)));
        else if (m == Mnemonic::SYSCALL || m == Mnemonic::SYSENTER || m == Mnemonic::INT || m == Mnemonic::INT1)
            l.ops.push_back({Opcode::SYSCALL});
        else
            l.ops.push_back({Opcode::BARRIER});
        return true;
    }
    if (m == Mnemonic::UD || m == Mnemonic::UD2 || m == Mnemonic::ICEBP) {
        l.ops.push_back({Opcode::UD2});
        return true;
    }

    if (m == Mnemonic::CLC) {
        l.ops.push_back(make_op2(Opcode::SET_CF, VarNode::flags(), VarNode::constant(0, 1)));
        return true;
    }
    if (m == Mnemonic::STC) {
        l.ops.push_back(make_op2(Opcode::SET_CF, VarNode::flags(), VarNode::constant(1, 1)));
        return true;
    }
    if (m == Mnemonic::CMC) {
        VarNode cf = VarNode::temp(0, 1);
        l.ops.push_back(make_op2(Opcode::GET_CF, cf, VarNode::flags()));
        l.ops.push_back(make_op2(Opcode::NOT, cf, cf));
        l.ops.push_back(make_op2(Opcode::SET_CF, VarNode::flags(), cf));
        return true;
    }
    if (m == Mnemonic::CLD) {
        l.ops.push_back(make_op2(Opcode::SET_DF, VarNode::flags(), VarNode::constant(0, 1)));
        return true;
    }
    if (m == Mnemonic::STD) {
        l.ops.push_back(make_op2(Opcode::SET_DF, VarNode::flags(), VarNode::constant(1, 1)));
        return true;
    }
    if (m == Mnemonic::CLI || m == Mnemonic::STI) {
        l.ops.push_back({Opcode::NOP});
        return true;
    }

    if (m == Mnemonic::MUL || m == Mnemonic::IMUL) {
        if (di.desc->has_modrm && ((di.modrm >> 6) & 3) == 3 && di.desc->num_operands <= 2) {
            VarNode src = reg_from_modrm_rm(di, sz);
            VarNode rax = VarNode::gpr(0, sz);
            Opcode opc = (m == Mnemonic::MUL) ? Opcode::MUL : Opcode::IMUL;
            l.ops.push_back(make_op3(opc, rax, rax, src));
            return true;
        }
    }

    if (m == Mnemonic::DIV || m == Mnemonic::IDIV) {
        if (di.desc->has_modrm && ((di.modrm >> 6) & 3) == 3) {
            VarNode src = reg_from_modrm_rm(di, sz);
            VarNode rax = VarNode::gpr(0, sz);
            Opcode opc = (m == Mnemonic::DIV) ? Opcode::DIV : Opcode::IDIV;
            l.ops.push_back(make_op3(opc, rax, rax, src));
            return true;
        }
    }

    if (m == Mnemonic::SETO || m == Mnemonic::SETNO || m == Mnemonic::SETB || m == Mnemonic::SETNB ||
        m == Mnemonic::SETZ || m == Mnemonic::SETNZ || m == Mnemonic::SETBE || m == Mnemonic::SETNBE ||
        m == Mnemonic::SETS || m == Mnemonic::SETNS || m == Mnemonic::SETP || m == Mnemonic::SETNP ||
        m == Mnemonic::SETL || m == Mnemonic::SETNL || m == Mnemonic::SETLE || m == Mnemonic::SETNLE) {
        if (di.desc->has_modrm) {
            VarNode dst = (mod_ == 3) ? reg_from_modrm_rm(di, 1) : VarNode::temp(11, 1);
            // Extract the primary flag for the condition
            Opcode get_flag = Opcode::GET_ZF; // default
            bool invert = false;
            if (m == Mnemonic::SETO) get_flag = Opcode::GET_OF;
            else if (m == Mnemonic::SETNO) { get_flag = Opcode::GET_OF; invert = true; }
            else if (m == Mnemonic::SETB) get_flag = Opcode::GET_CF;
            else if (m == Mnemonic::SETNB) { get_flag = Opcode::GET_CF; invert = true; }
            else if (m == Mnemonic::SETZ) get_flag = Opcode::GET_ZF;
            else if (m == Mnemonic::SETNZ) { get_flag = Opcode::GET_ZF; invert = true; }
            else if (m == Mnemonic::SETS) get_flag = Opcode::GET_SF;
            else if (m == Mnemonic::SETNS) { get_flag = Opcode::GET_SF; invert = true; }
            else if (m == Mnemonic::SETP) get_flag = Opcode::GET_PF;
            else if (m == Mnemonic::SETNP) { get_flag = Opcode::GET_PF; invert = true; }
            // For compound conditions (SETBE, SETL, SETLE) — use primary flag as approximation
            else if (m == Mnemonic::SETBE) get_flag = Opcode::GET_CF;
            else if (m == Mnemonic::SETNBE) { get_flag = Opcode::GET_CF; invert = true; }
            else if (m == Mnemonic::SETL) get_flag = Opcode::GET_SF;
            else if (m == Mnemonic::SETNL) { get_flag = Opcode::GET_SF; invert = true; }
            else if (m == Mnemonic::SETLE) get_flag = Opcode::GET_ZF;
            else if (m == Mnemonic::SETNLE) { get_flag = Opcode::GET_ZF; invert = true; }
            l.ops.push_back(make_op2(get_flag, dst, VarNode::flags()));
            if (invert) l.ops.push_back(make_op2(Opcode::NOT, dst, dst));
            if (mod_ != 3) {
                VarNode ea = compute_ea(l, di);
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(1), ea, dst));
            }
            return true;
        }
    }

    if (m == Mnemonic::CMOVO || m == Mnemonic::CMOVNO || m == Mnemonic::CMOVB || m == Mnemonic::CMOVNB ||
        m == Mnemonic::CMOVZ || m == Mnemonic::CMOVNZ || m == Mnemonic::CMOVBE || m == Mnemonic::CMOVNBE ||
        m == Mnemonic::CMOVS || m == Mnemonic::CMOVNS || m == Mnemonic::CMOVP || m == Mnemonic::CMOVNP ||
        m == Mnemonic::CMOVL || m == Mnemonic::CMOVNL || m == Mnemonic::CMOVLE || m == Mnemonic::CMOVNLE) {
        if (di.desc->has_modrm && ((di.modrm >> 6) & 3) == 3) {
            VarNode dst = reg_from_modrm_reg(di, sz);
            VarNode src = reg_from_modrm_rm(di, sz);
            VarNode cond = VarNode::temp(20, 1);
            if (m == Mnemonic::CMOVZ) l.ops.push_back(make_op2(Opcode::GET_ZF, cond, VarNode::flags()));
            else if (m == Mnemonic::CMOVNZ) { l.ops.push_back(make_op2(Opcode::GET_ZF, cond, VarNode::flags()));
                VarNode nc = VarNode::temp(21, 1); l.ops.push_back(make_op3(Opcode::XOR, nc, cond, VarNode::constant(1, 1))); cond = nc; }
            else if (m == Mnemonic::CMOVB) l.ops.push_back(make_op2(Opcode::GET_CF, cond, VarNode::flags()));
            else if (m == Mnemonic::CMOVNB) { l.ops.push_back(make_op2(Opcode::GET_CF, cond, VarNode::flags()));
                VarNode nc = VarNode::temp(21, 1); l.ops.push_back(make_op3(Opcode::XOR, nc, cond, VarNode::constant(1, 1))); cond = nc; }
            else if (m == Mnemonic::CMOVS) l.ops.push_back(make_op2(Opcode::GET_SF, cond, VarNode::flags()));
            else if (m == Mnemonic::CMOVNS) { l.ops.push_back(make_op2(Opcode::GET_SF, cond, VarNode::flags()));
                VarNode nc = VarNode::temp(21, 1); l.ops.push_back(make_op3(Opcode::XOR, nc, cond, VarNode::constant(1, 1))); cond = nc; }
            else if (m == Mnemonic::CMOVO) l.ops.push_back(make_op2(Opcode::GET_OF, cond, VarNode::flags()));
            else if (m == Mnemonic::CMOVNO) { l.ops.push_back(make_op2(Opcode::GET_OF, cond, VarNode::flags()));
                VarNode nc = VarNode::temp(21, 1); l.ops.push_back(make_op3(Opcode::XOR, nc, cond, VarNode::constant(1, 1))); cond = nc; }
            else if (m == Mnemonic::CMOVP) l.ops.push_back(make_op2(Opcode::GET_PF, cond, VarNode::flags()));
            else if (m == Mnemonic::CMOVNP) { l.ops.push_back(make_op2(Opcode::GET_PF, cond, VarNode::flags()));
                VarNode nc = VarNode::temp(21, 1); l.ops.push_back(make_op3(Opcode::XOR, nc, cond, VarNode::constant(1, 1))); cond = nc; }
            else if (m == Mnemonic::CMOVBE) {
                VarNode cf = VarNode::temp(21, 1); VarNode zf = VarNode::temp(22, 1);
                l.ops.push_back(make_op2(Opcode::GET_CF, cf, VarNode::flags()));
                l.ops.push_back(make_op2(Opcode::GET_ZF, zf, VarNode::flags()));
                l.ops.push_back(make_op3(Opcode::OR, cond, cf, zf)); }
            else if (m == Mnemonic::CMOVNBE) {
                VarNode cf = VarNode::temp(21, 1); VarNode zf = VarNode::temp(22, 1);
                l.ops.push_back(make_op2(Opcode::GET_CF, cf, VarNode::flags()));
                l.ops.push_back(make_op2(Opcode::GET_ZF, zf, VarNode::flags()));
                l.ops.push_back(make_op3(Opcode::OR, cond, cf, zf));
                VarNode nc = VarNode::temp(23, 1); l.ops.push_back(make_op3(Opcode::XOR, nc, cond, VarNode::constant(1, 1))); cond = nc; }
            else if (m == Mnemonic::CMOVL) {
                VarNode sf = VarNode::temp(21, 1); VarNode of = VarNode::temp(22, 1);
                l.ops.push_back(make_op2(Opcode::GET_SF, sf, VarNode::flags()));
                l.ops.push_back(make_op2(Opcode::GET_OF, of, VarNode::flags()));
                l.ops.push_back(make_op3(Opcode::XOR, cond, sf, of)); }
            else if (m == Mnemonic::CMOVNL) {
                VarNode sf = VarNode::temp(21, 1); VarNode of = VarNode::temp(22, 1);
                l.ops.push_back(make_op2(Opcode::GET_SF, sf, VarNode::flags()));
                l.ops.push_back(make_op2(Opcode::GET_OF, of, VarNode::flags()));
                l.ops.push_back(make_op3(Opcode::XOR, cond, sf, of));
                VarNode nc = VarNode::temp(23, 1); l.ops.push_back(make_op3(Opcode::XOR, nc, cond, VarNode::constant(1, 1))); cond = nc; }
            else if (m == Mnemonic::CMOVLE) {
                VarNode sf = VarNode::temp(21, 1); VarNode of = VarNode::temp(22, 1); VarNode zf = VarNode::temp(23, 1);
                l.ops.push_back(make_op2(Opcode::GET_SF, sf, VarNode::flags()));
                l.ops.push_back(make_op2(Opcode::GET_OF, of, VarNode::flags()));
                l.ops.push_back(make_op2(Opcode::GET_ZF, zf, VarNode::flags()));
                VarNode ne = VarNode::temp(24, 1); l.ops.push_back(make_op3(Opcode::XOR, ne, sf, of));
                l.ops.push_back(make_op3(Opcode::OR, cond, zf, ne)); }
            else if (m == Mnemonic::CMOVNLE) {
                VarNode sf = VarNode::temp(21, 1); VarNode of = VarNode::temp(22, 1); VarNode zf = VarNode::temp(23, 1);
                l.ops.push_back(make_op2(Opcode::GET_SF, sf, VarNode::flags()));
                l.ops.push_back(make_op2(Opcode::GET_OF, of, VarNode::flags()));
                l.ops.push_back(make_op2(Opcode::GET_ZF, zf, VarNode::flags()));
                VarNode ne = VarNode::temp(24, 1); l.ops.push_back(make_op3(Opcode::XOR, ne, sf, of));
                l.ops.push_back(make_op3(Opcode::OR, cond, zf, ne));
                VarNode nc = VarNode::temp(25, 1); l.ops.push_back(make_op3(Opcode::XOR, nc, cond, VarNode::constant(1, 1))); cond = nc; }
            else { l.ops.push_back(make_op2(Opcode::COPY, cond, VarNode::constant(1, 1))); }
            l.ops.push_back(make_op4(Opcode::SELECT, dst, cond, src, dst));
            return true;
        }
    }

    if (m == Mnemonic::ADC || m == Mnemonic::SBB) {
        if (di.desc->has_modrm && ((di.modrm >> 6) & 3) == 3 && di.desc->num_operands >= 2) {
            VarNode dst, src;
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM) {
                dst = reg_from_modrm_rm(di, sz); src = reg_from_modrm_reg(di, sz);
            } else {
                dst = reg_from_modrm_reg(di, sz); src = reg_from_modrm_rm(di, sz);
            }
            VarNode cf = VarNode::temp(0, sz);
            l.ops.push_back(make_op2(Opcode::GET_CF, cf, VarNode::flags()));
            l.ops.push_back(make_op2(Opcode::ZEXT, cf, cf));
            Opcode opc = (m == Mnemonic::ADC) ? Opcode::ADD : Opcode::SUB;
            l.ops.push_back(make_op3(opc, dst, dst, src));
            l.ops.push_back(make_op3(opc, dst, dst, cf));
            Opcode flags_opc = (m == Mnemonic::ADC) ? Opcode::ADD_FLAGS : Opcode::SUB_FLAGS;
            l.ops.push_back(make_op3(flags_opc, VarNode::flags(), dst, src));
            return true;
        }
    }

    if (m == Mnemonic::LAHF) {
        l.ops.push_back(make_op2(Opcode::COPY, VarNode::gpr(0, 1), VarNode::flags()));
        return true;
    }
    if (m == Mnemonic::SAHF) {
        l.ops.push_back(make_op2(Opcode::COPY, VarNode::flags(), VarNode::gpr(0, 1)));
        return true;
    }

    if ((m == Mnemonic::CRC32 || m == Mnemonic::ADCX || m == Mnemonic::ADOX) && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            uint8_t ssz = op_size_bytes(di.desc->operands[1].size, rex_w, has_66);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(ssz)));
        }
        if (m == Mnemonic::ADCX || m == Mnemonic::ADOX) {
            VarNode cf = VarNode::temp(12, sz);
            l.ops.push_back(make_op2(m == Mnemonic::ADCX ? Opcode::GET_CF : Opcode::GET_OF, cf, VarNode::flags()));
            l.ops.push_back(make_op2(Opcode::ZEXT, cf, cf));
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, src));
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, cf));
        } else {
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, src)); // CRC32 approximated as ADD
        }
        return true;
    }

    if (m == Mnemonic::IMUL && di.desc->has_modrm && di.desc->num_operands >= 2) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        if (has_imm) {
            l.ops.push_back(make_op3(Opcode::IMUL, dst, src, VarNode::constant(di.immediate, sz)));
        } else {
            l.ops.push_back(make_op3(Opcode::IMUL, dst, dst, src));
        }
        l.ops.push_back(make_op3(Opcode::ADD_FLAGS, VarNode::flags(), dst, src));
        return true;
    }

    if ((m == Mnemonic::BT || m == Mnemonic::BTS || m == Mnemonic::BTR || m == Mnemonic::BTC) &&
        di.desc->has_modrm && mod_ == 3) {
        VarNode base = reg_from_modrm_rm(di, sz);
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        VarNode bit = has_imm ? VarNode::constant(di.immediate, sz) : reg_from_modrm_reg(di, sz);
        // Extract bit into CF: CF = (base >> bit) & 1
        VarNode tmp = VarNode::temp(12, sz);
        l.ops.push_back(make_op3(Opcode::SHR, tmp, base, bit));
        l.ops.push_back(make_op3(Opcode::AND, tmp, tmp, VarNode::constant(1, sz)));
        l.ops.push_back(make_op2(Opcode::SET_CF, VarNode::flags(), tmp));
        // BTS/BTR/BTC modify the bit
        if (m == Mnemonic::BTS) {
            VarNode mask = VarNode::temp(13, sz);
            l.ops.push_back(make_op3(Opcode::SHL, mask, VarNode::constant(1, sz), bit));
            l.ops.push_back(make_op3(Opcode::OR, base, base, mask));
        } else if (m == Mnemonic::BTR) {
            VarNode mask = VarNode::temp(13, sz);
            l.ops.push_back(make_op3(Opcode::SHL, mask, VarNode::constant(1, sz), bit));
            l.ops.push_back(make_op2(Opcode::NOT, mask, mask));
            l.ops.push_back(make_op3(Opcode::AND, base, base, mask));
        } else if (m == Mnemonic::BTC) {
            VarNode mask = VarNode::temp(13, sz);
            l.ops.push_back(make_op3(Opcode::SHL, mask, VarNode::constant(1, sz), bit));
            l.ops.push_back(make_op3(Opcode::XOR, base, base, mask));
        }
        return true;
    }

    if (m == Mnemonic::CMPXCHG && di.desc->has_modrm) {
        VarNode src = reg_from_modrm_reg(di, sz);
        VarNode rax = VarNode::gpr(0, sz);
        if (mod_ == 3) {
            VarNode dst = reg_from_modrm_rm(di, sz);
            l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), rax, dst));
            l.ops.push_back(make_op2(Opcode::COPY, dst, src)); // simplified: assumes equal
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode tmp = VarNode::temp(14, sz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(sz)));
            l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), rax, tmp));
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, src)); // simplified: assumes equal
        }
        return true;
    }

    if (m == Mnemonic::XADD && di.desc->has_modrm && mod_ == 3) {
        VarNode dst = reg_from_modrm_rm(di, sz);
        VarNode src = reg_from_modrm_reg(di, sz);
        VarNode tmp = VarNode::temp(12, sz);
        l.ops.push_back(make_op3(Opcode::ADD, tmp, dst, src));
        l.ops.push_back(make_op2(Opcode::COPY, src, dst));
        l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
        l.ops.push_back(make_op3(Opcode::ADD_FLAGS, VarNode::flags(), dst, src));
        return true;
    }
    if (m == Mnemonic::XADD && di.desc->has_modrm && mod_ != 3) {
        VarNode ea = compute_ea(l, di);
        VarNode src = reg_from_modrm_reg(di, sz);
        VarNode old_val = VarNode::temp(12, sz);
        VarNode sum = VarNode::temp(13, sz);
        l.ops.push_back(make_op3(Opcode::LOAD, old_val, ea, VarNode::ram(sz)));
        l.ops.push_back(make_op3(Opcode::ADD, sum, old_val, src));
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, sum));
        l.ops.push_back(make_op2(Opcode::COPY, src, old_val));
        l.ops.push_back(make_op3(Opcode::ADD_FLAGS, VarNode::flags(), old_val, src));
        return true;
    }

    if ((m == Mnemonic::SHLD || m == Mnemonic::SHRD) && di.desc->has_modrm && mod_ == 3) {
        VarNode dst = reg_from_modrm_rm(di, sz);
        VarNode src = reg_from_modrm_reg(di, sz);
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        VarNode count = has_imm ? VarNode::constant(di.immediate, 1) : VarNode::gpr(1, 1);
        Opcode opc = (m == Mnemonic::SHLD) ? Opcode::SHL : Opcode::SHR;
        l.ops.push_back(make_op3(opc, dst, dst, count));
        return true;
    }

    if (m == Mnemonic::PUSH && di.desc->num_operands >= 1) {
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        if (has_imm) {
            VarNode rsp = VarNode::gpr(4, 8);
            VarNode eight = VarNode::constant(8, 8);
            l.ops.push_back(make_op3(Opcode::SUB, rsp, rsp, eight));
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), rsp, VarNode::constant(di.immediate, 8)));
            return true;
        }
    }

    if ((m == Mnemonic::BSF || m == Mnemonic::BSR) && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        // BSF = count trailing zeros, BSR = bit_width-1 - count leading zeros
        l.ops.push_back(make_op2(m == Mnemonic::BSF ? Opcode::CTZ : Opcode::CLZ, dst, src));
        l.ops.push_back(make_op3(Opcode::AND_FLAGS, VarNode::flags(), src, src)); // ZF set if src==0
        return true;
    }

    if ((m == Mnemonic::POPCNT || m == Mnemonic::TZCNT || m == Mnemonic::LZCNT) && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        Opcode cnt_opc = (m == Mnemonic::POPCNT) ? Opcode::POPCNT : (m == Mnemonic::TZCNT) ? Opcode::CTZ : Opcode::CLZ;
        l.ops.push_back(make_op2(cnt_opc, dst, src));
        l.ops.push_back(make_op3(Opcode::AND_FLAGS, VarNode::flags(), dst, dst));
        return true;
    }

    if (m == Mnemonic::ANDN && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        // vvvv operand (implicit in VEX) — approximate as dst
        VarNode vvvv = VarNode::gpr(di.has_vex ? di.vex_vvvv : 0, sz);
        VarNode tmp = VarNode::temp(12, sz);
        l.ops.push_back(make_op2(Opcode::NOT, tmp, vvvv));
        l.ops.push_back(make_op3(Opcode::AND, dst, tmp, src));
        l.ops.push_back(make_op3(Opcode::AND_FLAGS, VarNode::flags(), dst, dst));
        return true;
    }

    if ((m == Mnemonic::BLSI || m == Mnemonic::BLSMSK || m == Mnemonic::BLSR) && di.desc->has_modrm) {
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        VarNode dst = VarNode::gpr(di.has_vex ? di.vex_vvvv : 0, sz);
        VarNode tmp = VarNode::temp(12, sz);
        if (m == Mnemonic::BLSI) {
            l.ops.push_back(make_op2(Opcode::NEG, tmp, src));
            l.ops.push_back(make_op3(Opcode::AND, dst, src, tmp));
        } else if (m == Mnemonic::BLSMSK) {
            l.ops.push_back(make_op3(Opcode::SUB, tmp, src, VarNode::constant(1, sz)));
            l.ops.push_back(make_op3(Opcode::XOR, dst, src, tmp));
        } else {
            l.ops.push_back(make_op3(Opcode::SUB, tmp, src, VarNode::constant(1, sz)));
            l.ops.push_back(make_op3(Opcode::AND, dst, src, tmp));
        }
        l.ops.push_back(make_op3(Opcode::AND_FLAGS, VarNode::flags(), dst, dst));
        return true;
    }

    if (m == Mnemonic::BEXTR && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        VarNode ctl = VarNode::gpr(di.has_vex ? di.vex_vvvv : 0, sz);
        // start = ctl[7:0], len = ctl[15:8]
        VarNode start = VarNode::temp(12, sz);
        l.ops.push_back(make_op3(Opcode::AND, start, ctl, VarNode::constant(0xFF, sz)));
        l.ops.push_back(make_op3(Opcode::SHR, dst, src, start));
        // mask by length — simplified
        l.ops.push_back(make_op3(Opcode::AND_FLAGS, VarNode::flags(), dst, dst));
        return true;
    }

    if ((m == Mnemonic::BZHI || m == Mnemonic::PDEP || m == Mnemonic::PEXT ||
        m == Mnemonic::MULX || m == Mnemonic::SARX || m == Mnemonic::SHLX ||
        m == Mnemonic::SHRX || m == Mnemonic::RORX) && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        VarNode ctl = VarNode::gpr(di.has_vex ? di.vex_vvvv : 0, sz);
        if (m == Mnemonic::SARX) l.ops.push_back(make_op3(Opcode::SAR, dst, src, ctl));
        else if (m == Mnemonic::SHLX) l.ops.push_back(make_op3(Opcode::SHL, dst, src, ctl));
        else if (m == Mnemonic::SHRX) l.ops.push_back(make_op3(Opcode::SHR, dst, src, ctl));
        else if (m == Mnemonic::RORX) l.ops.push_back(make_op3(Opcode::ROR, dst, src, VarNode::constant(di.immediate, 1)));
        else if (m == Mnemonic::MULX) l.ops.push_back(make_op3(Opcode::MUL, dst, src, VarNode::gpr(2, sz)));
        else l.ops.push_back(make_op2(Opcode::COPY, dst, src)); // BZHI/PDEP/PEXT: placeholder
        return true;
    }

    if (m == Mnemonic::MOVBE && di.desc->has_modrm) {
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            if (di.desc->operands[0].addr == AddrMode::ModRM_Reg) {
                VarNode dst = reg_from_modrm_reg(di, sz);
                l.ops.push_back(make_op3(Opcode::LOAD, dst, ea, VarNode::ram(sz)));
            } else {
                VarNode src = reg_from_modrm_reg(di, sz);
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, src));
            }
        }
        return true;
    }

    if (m == Mnemonic::LOOP || m == Mnemonic::LOOPE || m == Mnemonic::LOOPNE ||
        m == Mnemonic::LOOPZ || m == Mnemonic::LOOPNZ) {
        VarNode rcx = VarNode::gpr(1, 8);
        l.ops.push_back(make_op3(Opcode::SUB, rcx, rcx, VarNode::constant(1, 8)));
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                VarNode target = VarNode::constant((int64_t)(address + n + di.immediate), 8);
                VarNode fallthrough = VarNode::constant((int64_t)(address + n), 8);
                l.ops.push_back(make_op3(Opcode::CBRANCH, target, target, fallthrough));
                return true;
            }
        }
        return true;
    }

    if ((m == Mnemonic::RCL || m == Mnemonic::RCR) && di.desc->has_modrm) {
        VarNode dst = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(11, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, dst, ea, VarNode::ram(sz)));
        }
        // Simplified: just rotate (ignoring carry bit integration)
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        VarNode count = has_imm ? VarNode::constant(di.immediate, 1) :
            (di.desc->opcode == 0xD0 || di.desc->opcode == 0xD1) ? VarNode::constant(1, 1) : VarNode::gpr(1, 1);
        Opcode opc = (m == Mnemonic::RCL) ? Opcode::ROL : Opcode::ROR;
        l.ops.push_back(make_op3(opc, dst, dst, count));
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, dst));
        }
        return true;
    }

    if (di.desc->opcode >= 0xD8 && di.desc->opcode <= 0xDF) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::MOVAPS || m == Mnemonic::MOVUPS || m == Mnemonic::MOVAPD || m == Mnemonic::MOVUPD ||
        m == Mnemonic::MOVDQA || m == Mnemonic::MOVDQU) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM || di.desc->operands[0].addr == AddrMode::XmmRM) {
                VarNode src = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(16), ea, src));
            } else {
                VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
                VarNode tmp = VarNode::temp(20, 16);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
                l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
            }
        }
        return true;
    }

    if (m == Mnemonic::MOVSS || m == Mnemonic::MOVSD) {
        uint8_t msz = (m == Mnemonic::MOVSS) ? 4 : 8;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), msz);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), msz);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM || di.desc->operands[0].addr == AddrMode::XmmRM) {
                VarNode src = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), msz);
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(msz), ea, src));
            } else {
                VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), msz);
                VarNode tmp = VarNode::temp(20, msz);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(msz)));
                l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
            }
        }
        return true;
    }

    if (m == Mnemonic::MOVLPS || m == Mnemonic::MOVHPS || m == Mnemonic::MOVLPD || m == Mnemonic::MOVHPD) {
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            VarNode xmm_reg = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM || di.desc->operands[0].addr == AddrMode::XmmRM ||
                di.desc->operands[0].addr == AddrMode::MemOnly) {
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), ea, xmm_reg));
            } else {
                VarNode tmp = VarNode::temp(20, 8);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(8)));
                l.ops.push_back(make_op2(Opcode::COPY, xmm_reg, tmp));
            }
        } else {
            // MOVLHPS / MOVHLPS aliases when mod==3
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        }
        return true;
    }

    if (m == Mnemonic::MOVLHPS || m == Mnemonic::MOVHLPS) {
        VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
        VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
        l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        return true;
    }

    if (m == Mnemonic::MOVNTPS || m == Mnemonic::MOVNTPD || m == Mnemonic::MOVNTDQ) {
        VarNode ea = compute_ea(l, di);
        VarNode src = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(16), ea, src));
        return true;
    }
    if (m == Mnemonic::MOVNTDQA) {
        VarNode ea = compute_ea(l, di);
        VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
        VarNode tmp = VarNode::temp(20, 16);
        l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
        l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
        return true;
    }

    if (m == Mnemonic::MOVQ || m == Mnemonic::MOVD) {
        uint8_t msz = (m == Mnemonic::MOVQ) ? 8 : 4;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), msz);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), msz);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            if (di.desc->operands[0].addr == AddrMode::ModRM_RM || di.desc->operands[0].addr == AddrMode::XmmRM) {
                VarNode src = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), msz);
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(msz), ea, src));
            } else {
                VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), msz);
                VarNode tmp = VarNode::temp(20, msz);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(msz)));
                l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
            }
        }
        return true;
    }

    if (m == Mnemonic::MOVMSKPS || m == Mnemonic::MOVMSKPD) {
        VarNode dst = reg_from_modrm_reg(di, 4);
        VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
        l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        return true;
    }

    if (m == Mnemonic::CMPPS || m == Mnemonic::CMPPD || m == Mnemonic::CMPSS || m == Mnemonic::CMPSD) {
        uint8_t vsz = (m == Mnemonic::CMPSS) ? 4 : (m == Mnemonic::CMPSD) ? 8 : 16;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), vsz);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), vsz);
            l.ops.push_back(make_op3(Opcode::SUB, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), vsz);
            VarNode tmp = VarNode::temp(20, vsz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(vsz)));
            l.ops.push_back(make_op3(Opcode::SUB, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::COMISS || m == Mnemonic::COMISD || m == Mnemonic::UCOMISS || m == Mnemonic::UCOMISD) {
        uint8_t vsz = (m == Mnemonic::COMISS || m == Mnemonic::UCOMISS) ? 4 : 8;
        VarNode a = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), vsz);
        VarNode b;
        if (mod_ == 3) {
            b = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), vsz);
        } else {
            VarNode ea = compute_ea(l, di);
            b = VarNode::temp(20, vsz);
            l.ops.push_back(make_op3(Opcode::LOAD, b, ea, VarNode::ram(vsz)));
        }
        l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), a, b));
        return true;
    }

    if (m == Mnemonic::CVTPS2PD || m == Mnemonic::CVTPD2PS || m == Mnemonic::CVTSS2SD || m == Mnemonic::CVTSD2SS ||
        m == Mnemonic::CVTDQ2PS || m == Mnemonic::CVTPS2DQ || m == Mnemonic::CVTTPS2DQ ||
        m == Mnemonic::CVTDQ2PD || m == Mnemonic::CVTPD2DQ || m == Mnemonic::CVTTPD2DQ ||
        m == Mnemonic::CVTSI2SS || m == Mnemonic::CVTSI2SD || m == Mnemonic::CVTSS2SI || m == Mnemonic::CVTSD2SI ||
        m == Mnemonic::CVTTSS2SI || m == Mnemonic::CVTTSD2SI) {
        // Conversion: approximate as COPY between xmm / gpr
        bool dst_is_gpr = (m == Mnemonic::CVTSS2SI || m == Mnemonic::CVTSD2SI ||
                            m == Mnemonic::CVTTSS2SI || m == Mnemonic::CVTTSD2SI);
        bool src_is_gpr = (m == Mnemonic::CVTSI2SS || m == Mnemonic::CVTSI2SD);
        if (mod_ == 3) {
            if (dst_is_gpr) {
                VarNode dst = reg_from_modrm_reg(di, sz);
                VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
                l.ops.push_back(make_op2(Opcode::COPY, dst, src));
            } else if (src_is_gpr) {
                VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
                VarNode src = reg_from_modrm_rm(di, sz);
                l.ops.push_back(make_op2(Opcode::COPY, dst, src));
            } else {
                VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
                VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
                l.ops.push_back(make_op2(Opcode::COPY, dst, src));
            }
        } else {
            VarNode ea = compute_ea(l, di);
            if (dst_is_gpr) {
                VarNode dst = reg_from_modrm_reg(di, sz);
                VarNode tmp = VarNode::temp(20, 8);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(8)));
                l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
            } else {
                VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
                VarNode tmp = VarNode::temp(20, 16);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
                l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
            }
        }
        return true;
    }

    if (m == Mnemonic::PADDB || m == Mnemonic::PADDW || m == Mnemonic::PADDD || m == Mnemonic::PADDQ ||
        m == Mnemonic::PSUBB || m == Mnemonic::PSUBW || m == Mnemonic::PSUBD || m == Mnemonic::PSUBQ) {
        Opcode opc = (m == Mnemonic::PSUBB || m == Mnemonic::PSUBW || m == Mnemonic::PSUBD || m == Mnemonic::PSUBQ)
            ? Opcode::SUB : Opcode::ADD;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(opc, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(opc, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PMULLW || m == Mnemonic::PMULLD || m == Mnemonic::PMULUDQ || m == Mnemonic::PMADDWD) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(Opcode::MUL, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(Opcode::MUL, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PAND || m == Mnemonic::POR || m == Mnemonic::PXOR || m == Mnemonic::PANDN) {
        bool is_andn = (m == Mnemonic::PANDN);
        Opcode opc = is_andn ? Opcode::AND :
            (m == Mnemonic::POR) ? Opcode::OR : (m == Mnemonic::PXOR) ? Opcode::XOR : Opcode::AND;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            if (is_andn) {
                VarNode tmp = VarNode::temp(20, 16);
                l.ops.push_back(make_op2(Opcode::NOT, tmp, dst));
                l.ops.push_back(make_op3(Opcode::AND, dst, tmp, src));
            } else {
                l.ops.push_back(make_op3(opc, dst, dst, src));
            }
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            if (is_andn) {
                VarNode ntmp = VarNode::temp(21, 16);
                l.ops.push_back(make_op2(Opcode::NOT, ntmp, dst));
                l.ops.push_back(make_op3(Opcode::AND, dst, ntmp, tmp));
            } else {
                l.ops.push_back(make_op3(opc, dst, dst, tmp));
            }
        }
        return true;
    }

    if (m == Mnemonic::PCMPEQB || m == Mnemonic::PCMPEQW || m == Mnemonic::PCMPEQD || m == Mnemonic::PCMPEQQ ||
        m == Mnemonic::PCMPGTB || m == Mnemonic::PCMPGTW || m == Mnemonic::PCMPGTD || m == Mnemonic::PCMPGTQ) {
        Opcode opc = (m == Mnemonic::PCMPEQB || m == Mnemonic::PCMPEQW || m == Mnemonic::PCMPEQD || m == Mnemonic::PCMPEQQ)
            ? Opcode::CMP_EQ : Opcode::CMP_SLT;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(opc, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(opc, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PSLLW || m == Mnemonic::PSLLD || m == Mnemonic::PSLLQ ||
        m == Mnemonic::PSRLW || m == Mnemonic::PSRLD || m == Mnemonic::PSRLQ ||
        m == Mnemonic::PSRAW || m == Mnemonic::PSRAD) {
        Opcode opc = (m == Mnemonic::PSLLW || m == Mnemonic::PSLLD || m == Mnemonic::PSLLQ) ? Opcode::SHL :
                      (m == Mnemonic::PSRLW || m == Mnemonic::PSRLD || m == Mnemonic::PSRLQ) ? Opcode::SHR : Opcode::SAR;
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        if (has_imm) {
            VarNode dst = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(opc, dst, dst, VarNode::constant(di.immediate, 1)));
        } else if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(opc, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(opc, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PUNPCKLBW || m == Mnemonic::PUNPCKLWD || m == Mnemonic::PUNPCKLDQ || m == Mnemonic::PUNPCKLQDQ ||
        m == Mnemonic::PUNPCKHBW || m == Mnemonic::PUNPCKHWD || m == Mnemonic::PUNPCKHDQ || m == Mnemonic::PUNPCKHQDQ ||
        m == Mnemonic::PACKSSWB || m == Mnemonic::PACKSSDW || m == Mnemonic::PACKUSWB ||
        m == Mnemonic::PSHUFB || m == Mnemonic::PSHUFD || m == Mnemonic::PSHUFW || m == Mnemonic::PSHUFHW || m == Mnemonic::PSHUFLW ||
        m == Mnemonic::PALIGNR || m == Mnemonic::PBLENDW || m == Mnemonic::PBLENDVB ||
        m == Mnemonic::BLENDPS || m == Mnemonic::BLENDPD || m == Mnemonic::BLENDVPS || m == Mnemonic::BLENDVPD ||
        m == Mnemonic::SHUFPS || m == Mnemonic::SHUFPD ||
        m == Mnemonic::UNPCKLPS || m == Mnemonic::UNPCKLPD || m == Mnemonic::UNPCKHPS || m == Mnemonic::UNPCKHPD) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(Opcode::OR, dst, dst, src)); // approximate: merge
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(Opcode::OR, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PMINUB || m == Mnemonic::PMAXUB || m == Mnemonic::PMINUW || m == Mnemonic::PMAXUW ||
        m == Mnemonic::PMINSD || m == Mnemonic::PMAXSD || m == Mnemonic::PMINUD || m == Mnemonic::PMAXUD) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(Opcode::SUB, dst, dst, src)); // approximate min/max via sub
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(Opcode::SUB, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PABSB || m == Mnemonic::PABSW || m == Mnemonic::PABSD) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PMOVZXBW || m == Mnemonic::PMOVZXWD || m == Mnemonic::PMOVZXDQ ||
        m == Mnemonic::PMOVSXBW || m == Mnemonic::PMOVSXWD || m == Mnemonic::PMOVSXDQ) {
        Opcode opc = (m == Mnemonic::PMOVZXBW || m == Mnemonic::PMOVZXWD || m == Mnemonic::PMOVZXDQ)
            ? Opcode::ZEXT : Opcode::SEXT;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op2(opc, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 8);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(8)));
            l.ops.push_back(make_op2(opc, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PEXTRB || m == Mnemonic::PEXTRW || m == Mnemonic::PEXTRD || m == Mnemonic::PEXTRQ) {
        uint8_t esz = (m == Mnemonic::PEXTRB) ? 1 : (m == Mnemonic::PEXTRW) ? 2 : (m == Mnemonic::PEXTRD) ? 4 : 8;
        VarNode src = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
        if (mod_ == 3) {
            VarNode dst = reg_from_modrm_rm(di, esz);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(esz), ea, src));
        }
        return true;
    }

    if (m == Mnemonic::PINSRB || m == Mnemonic::PINSRW || m == Mnemonic::PINSRD || m == Mnemonic::PINSRQ) {
        uint8_t esz = (m == Mnemonic::PINSRB) ? 1 : (m == Mnemonic::PINSRW) ? 2 : (m == Mnemonic::PINSRD) ? 4 : 8;
        VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
        if (mod_ == 3) {
            VarNode src = reg_from_modrm_rm(di, esz);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode tmp = VarNode::temp(20, esz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(esz)));
            l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::DPPS || m == Mnemonic::DPPD || m == Mnemonic::MPSADBW ||
        m == Mnemonic::ROUNDPS || m == Mnemonic::ROUNDPD || m == Mnemonic::ROUNDSS || m == Mnemonic::ROUNDSD) {
        uint8_t vsz = (m == Mnemonic::ROUNDSS) ? 4 : (m == Mnemonic::ROUNDSD) ? 8 : 16;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), vsz);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), vsz);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), vsz);
            VarNode tmp = VarNode::temp(20, vsz);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(vsz)));
            l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::MOVS || m == Mnemonic::MOVSB || m == Mnemonic::MOVSW || m == Mnemonic::MOVSD || m == Mnemonic::MOVSQ) {
        uint8_t ssz = ((di.desc->opcode & 1) == 0) ? 1 : (rex_w ? 8 : (has_66 ? 2 : 4));
        if (m == Mnemonic::MOVSB) ssz = 1;
        else if (m == Mnemonic::MOVSW) ssz = 2;
        else if (m == Mnemonic::MOVSD) ssz = 4;
        else if (m == Mnemonic::MOVSQ) ssz = 8;
        VarNode rsi = VarNode::gpr(6, 8), rdi = VarNode::gpr(7, 8);
        VarNode tmp = VarNode::temp(14, ssz);
        VarNode t16 = VarNode::temp(16, 8);
        l.ops.push_back(make_op2(Opcode::GET_DF, t16, VarNode::flags()));
        l.ops.push_back(make_op4(Opcode::SELECT, t16, t16, VarNode::constant(-((int64_t)ssz), 8), VarNode::constant(ssz, 8)));
        l.ops.push_back(make_op3(Opcode::LOAD, tmp, rsi, VarNode::ram(ssz)));
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(ssz), rdi, tmp));
        l.ops.push_back(make_op3(Opcode::ADD, rsi, rsi, t16));
        l.ops.push_back(make_op3(Opcode::ADD, rdi, rdi, t16));
        return true;
    }

    if (m == Mnemonic::STOS || m == Mnemonic::STOSB || m == Mnemonic::STOSW || m == Mnemonic::STOSD || m == Mnemonic::STOSQ) {
        uint8_t ssz = ((di.desc->opcode & 1) == 0) ? 1 : (rex_w ? 8 : (has_66 ? 2 : 4));
        if (m == Mnemonic::STOSB) ssz = 1;
        else if (m == Mnemonic::STOSW) ssz = 2;
        else if (m == Mnemonic::STOSD) ssz = 4;
        else if (m == Mnemonic::STOSQ) ssz = 8;
        VarNode rax = VarNode::gpr(0, ssz), rdi = VarNode::gpr(7, 8);
        VarNode t16 = VarNode::temp(16, 8);
        l.ops.push_back(make_op2(Opcode::GET_DF, t16, VarNode::flags()));
        l.ops.push_back(make_op4(Opcode::SELECT, t16, t16, VarNode::constant(-((int64_t)ssz), 8), VarNode::constant(ssz, 8)));
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(ssz), rdi, rax));
        l.ops.push_back(make_op3(Opcode::ADD, rdi, rdi, t16));
        return true;
    }

    if (m == Mnemonic::LODS || m == Mnemonic::LODSB || m == Mnemonic::LODSW || m == Mnemonic::LODSD || m == Mnemonic::LODSQ) {
        uint8_t ssz = ((di.desc->opcode & 1) == 0) ? 1 : (rex_w ? 8 : (has_66 ? 2 : 4));
        if (m == Mnemonic::LODSB) ssz = 1;
        else if (m == Mnemonic::LODSW) ssz = 2;
        else if (m == Mnemonic::LODSD) ssz = 4;
        else if (m == Mnemonic::LODSQ) ssz = 8;
        VarNode rax = VarNode::gpr(0, ssz), rsi = VarNode::gpr(6, 8);
        VarNode t16 = VarNode::temp(16, 8);
        l.ops.push_back(make_op2(Opcode::GET_DF, t16, VarNode::flags()));
        l.ops.push_back(make_op4(Opcode::SELECT, t16, t16, VarNode::constant(-((int64_t)ssz), 8), VarNode::constant(ssz, 8)));
        l.ops.push_back(make_op3(Opcode::LOAD, rax, rsi, VarNode::ram(ssz)));
        l.ops.push_back(make_op3(Opcode::ADD, rsi, rsi, t16));
        return true;
    }

    if (m == Mnemonic::CMPS || m == Mnemonic::CMPSB || m == Mnemonic::CMPSW || m == Mnemonic::CMPSD || m == Mnemonic::CMPSQ) {
        uint8_t ssz = ((di.desc->opcode & 1) == 0) ? 1 : (rex_w ? 8 : (has_66 ? 2 : 4));
        if (m == Mnemonic::CMPSB) ssz = 1;
        else if (m == Mnemonic::CMPSW) ssz = 2;
        else if (m == Mnemonic::CMPSD) ssz = 4;
        else if (m == Mnemonic::CMPSQ) ssz = 8;
        VarNode rsi = VarNode::gpr(6, 8), rdi = VarNode::gpr(7, 8);
        VarNode t1 = VarNode::temp(14, ssz), t2 = VarNode::temp(15, ssz);
        VarNode t16 = VarNode::temp(16, 8);
        l.ops.push_back(make_op2(Opcode::GET_DF, t16, VarNode::flags()));
        l.ops.push_back(make_op4(Opcode::SELECT, t16, t16, VarNode::constant(-((int64_t)ssz), 8), VarNode::constant(ssz, 8)));
        l.ops.push_back(make_op3(Opcode::LOAD, t1, rsi, VarNode::ram(ssz)));
        l.ops.push_back(make_op3(Opcode::LOAD, t2, rdi, VarNode::ram(ssz)));
        l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), t1, t2));
        l.ops.push_back(make_op3(Opcode::ADD, rsi, rsi, t16));
        l.ops.push_back(make_op3(Opcode::ADD, rdi, rdi, t16));
        return true;
    }

    if (m == Mnemonic::SCAS || m == Mnemonic::SCASB || m == Mnemonic::SCASW || m == Mnemonic::SCASD || m == Mnemonic::SCASQ) {
        uint8_t ssz = ((di.desc->opcode & 1) == 0) ? 1 : (rex_w ? 8 : (has_66 ? 2 : 4));
        if (m == Mnemonic::SCASB) ssz = 1;
        else if (m == Mnemonic::SCASW) ssz = 2;
        else if (m == Mnemonic::SCASD) ssz = 4;
        else if (m == Mnemonic::SCASQ) ssz = 8;
        VarNode rax = VarNode::gpr(0, ssz), rdi = VarNode::gpr(7, 8);
        VarNode tmp = VarNode::temp(14, ssz);
        VarNode t16 = VarNode::temp(16, 8);
        l.ops.push_back(make_op2(Opcode::GET_DF, t16, VarNode::flags()));
        l.ops.push_back(make_op4(Opcode::SELECT, t16, t16, VarNode::constant(-((int64_t)ssz), 8), VarNode::constant(ssz, 8)));
        l.ops.push_back(make_op3(Opcode::LOAD, tmp, rdi, VarNode::ram(ssz)));
        l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), rax, tmp));
        l.ops.push_back(make_op3(Opcode::ADD, rdi, rdi, t16));
        return true;
    }

    if ((m == Mnemonic::MOVAPS || m == Mnemonic::MOVUPS || m == Mnemonic::MOVAPD || m == Mnemonic::MOVUPD ||
         m == Mnemonic::MOVDQA || m == Mnemonic::MOVDQU) && di.desc->has_modrm) {
        uint8_t xsz = 16;
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), xsz);
            VarNode src = VarNode::xmm(((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0)), xsz);
            // Direction depends on opcode
            bool store_dir = (di.desc->opcode == 0x29 || di.desc->opcode == 0x7F);
            if (store_dir) l.ops.push_back(make_op2(Opcode::COPY, src, dst));
            else l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode xmm = VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), xsz);
            bool store_dir = (di.desc->opcode == 0x29 || di.desc->opcode == 0x7F);
            if (store_dir) l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(xsz), ea, xmm));
            else l.ops.push_back(make_op3(Opcode::LOAD, xmm, ea, VarNode::ram(xsz)));
        }
        return true;
    }

    if ((m == Mnemonic::ADDPS || m == Mnemonic::ADDPD || m == Mnemonic::ADDSS || m == Mnemonic::ADDSD ||
         m == Mnemonic::SUBPS || m == Mnemonic::SUBPD || m == Mnemonic::SUBSS || m == Mnemonic::SUBSD ||
         m == Mnemonic::MULPS || m == Mnemonic::MULPD || m == Mnemonic::MULSS || m == Mnemonic::MULSD ||
         m == Mnemonic::DIVPS || m == Mnemonic::DIVPD || m == Mnemonic::DIVSS || m == Mnemonic::DIVSD ||
         m == Mnemonic::SQRTPS || m == Mnemonic::SQRTPD || m == Mnemonic::SQRTSS || m == Mnemonic::SQRTSD ||
         m == Mnemonic::MINPS || m == Mnemonic::MINPD || m == Mnemonic::MINSS || m == Mnemonic::MINSD ||
         m == Mnemonic::MAXPS || m == Mnemonic::MAXPD || m == Mnemonic::MAXSS || m == Mnemonic::MAXSD) &&
        di.desc->has_modrm) {
        uint8_t xsz = 16;
        VarNode dst = VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), xsz);
        VarNode src = (mod_ == 3) ? VarNode::xmm(((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0)), xsz) : VarNode::temp(14, xsz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(xsz)));
        }
        Opcode opc = Opcode::FADD;
        if (m == Mnemonic::SUBPS || m == Mnemonic::SUBPD || m == Mnemonic::SUBSS || m == Mnemonic::SUBSD) opc = Opcode::FSUB;
        else if (m == Mnemonic::MULPS || m == Mnemonic::MULPD || m == Mnemonic::MULSS || m == Mnemonic::MULSD) opc = Opcode::FMUL;
        else if (m == Mnemonic::DIVPS || m == Mnemonic::DIVPD || m == Mnemonic::DIVSS || m == Mnemonic::DIVSD) opc = Opcode::FDIV;
        else if (m == Mnemonic::SQRTPS || m == Mnemonic::SQRTPD || m == Mnemonic::SQRTSS || m == Mnemonic::SQRTSD) opc = Opcode::FSQRT;
        else if (m == Mnemonic::MINPS || m == Mnemonic::MINPD || m == Mnemonic::MINSS || m == Mnemonic::MINSD) opc = Opcode::FMIN;
        else if (m == Mnemonic::MAXPS || m == Mnemonic::MAXPD || m == Mnemonic::MAXSS || m == Mnemonic::MAXSD) opc = Opcode::FMAX;
        if (opc == Opcode::FSQRT) l.ops.push_back(make_op2(opc, dst, src));
        else l.ops.push_back(make_op3(opc, dst, dst, src));
        return true;
    }

    if ((m == Mnemonic::XORPS || m == Mnemonic::XORPD || m == Mnemonic::ANDPS || m == Mnemonic::ANDPD ||
         m == Mnemonic::ORPS || m == Mnemonic::ORPD || m == Mnemonic::ANDNPS || m == Mnemonic::ANDNPD) &&
        di.desc->has_modrm) {
        uint8_t xsz = 16;
        VarNode dst = VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), xsz);
        VarNode src = (mod_ == 3) ? VarNode::xmm(((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0)), xsz) : VarNode::temp(14, xsz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(xsz)));
        }
        if (m == Mnemonic::XORPS || m == Mnemonic::XORPD) l.ops.push_back(make_op3(Opcode::XOR, dst, dst, src));
        else if (m == Mnemonic::ANDPS || m == Mnemonic::ANDPD) l.ops.push_back(make_op3(Opcode::AND, dst, dst, src));
        else if (m == Mnemonic::ORPS || m == Mnemonic::ORPD) l.ops.push_back(make_op3(Opcode::OR, dst, dst, src));
        else { // ANDNPS/ANDNPD: dst = ~dst & src
            l.ops.push_back(make_op2(Opcode::NOT, dst, dst));
            l.ops.push_back(make_op3(Opcode::AND, dst, dst, src));
        }
        return true;
    }

    if ((m == Mnemonic::MOVD || m == Mnemonic::MOVQ) && di.desc->has_modrm) {
        uint8_t gsz = (m == Mnemonic::MOVQ || rex_w) ? 8 : 4;
        if (mod_ == 3) {
            bool to_xmm = (di.desc->opcode == 0x6E);
            if (to_xmm) l.ops.push_back(make_op2(Opcode::COPY, VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), gsz), VarNode::gpr(((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0)), gsz)));
            else l.ops.push_back(make_op2(Opcode::COPY, VarNode::gpr(((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0)), gsz), VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), gsz)));
        } else {
            VarNode ea = compute_ea(l, di);
            bool to_xmm = (di.desc->opcode == 0x6E);
            if (to_xmm) l.ops.push_back(make_op3(Opcode::LOAD, VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), gsz), ea, VarNode::ram(gsz)));
            else l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(gsz), ea, VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), gsz)));
        }
        return true;
    }

    if ((m == Mnemonic::PADDB || m == Mnemonic::PADDW || m == Mnemonic::PADDD || m == Mnemonic::PADDQ ||
         m == Mnemonic::PSUBB || m == Mnemonic::PSUBW || m == Mnemonic::PSUBD || m == Mnemonic::PSUBQ ||
         m == Mnemonic::PAND || m == Mnemonic::PANDN || m == Mnemonic::POR || m == Mnemonic::PXOR) &&
        di.desc->has_modrm) {
        uint8_t xsz = 16;
        VarNode dst = VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), xsz);
        VarNode src = (mod_ == 3) ? VarNode::xmm(((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0)), xsz) : VarNode::temp(14, xsz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(xsz)));
        }
        if (m == Mnemonic::PADDB || m == Mnemonic::PADDW || m == Mnemonic::PADDD || m == Mnemonic::PADDQ) l.ops.push_back(make_op3(Opcode::ADD, dst, dst, src));
        else if (m == Mnemonic::PSUBB || m == Mnemonic::PSUBW || m == Mnemonic::PSUBD || m == Mnemonic::PSUBQ) l.ops.push_back(make_op3(Opcode::SUB, dst, dst, src));
        else if (m == Mnemonic::PAND) l.ops.push_back(make_op3(Opcode::AND, dst, dst, src));
        else if (m == Mnemonic::POR) l.ops.push_back(make_op3(Opcode::OR, dst, dst, src));
        else if (m == Mnemonic::PXOR) l.ops.push_back(make_op3(Opcode::XOR, dst, dst, src));
        else { // PANDN: dst = ~dst & src
            l.ops.push_back(make_op2(Opcode::NOT, dst, dst));
            l.ops.push_back(make_op3(Opcode::AND, dst, dst, src));
        }
        return true;
    }

    if ((m == Mnemonic::CVTSI2SS || m == Mnemonic::CVTSI2SD) && di.desc->has_modrm) {
        VarNode dst = VarNode::xmm((((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0)), 16);
        VarNode src = (mod_ == 3) ? reg_from_modrm_rm(di, sz) : VarNode::temp(14, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        l.ops.push_back(make_op2(Opcode::COPY, dst, src)); // int->float conversion
        return true;
    }

    if ((m == Mnemonic::CVTSS2SI || m == Mnemonic::CVTTSS2SI || m == Mnemonic::CVTSD2SI || m == Mnemonic::CVTTSD2SI) && di.desc->has_modrm) {
        VarNode dst = reg_from_modrm_reg(di, sz);
        VarNode src = (mod_ == 3) ? VarNode::xmm(((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0)), 16) : VarNode::temp(14, sz);
        if (mod_ != 3) {
            VarNode ea = compute_ea(l, di);
            l.ops.push_back(make_op3(Opcode::LOAD, src, ea, VarNode::ram(sz)));
        }
        l.ops.push_back(make_op2(Opcode::COPY, dst, src)); // float->int conversion
        return true;
    }

    if (m == Mnemonic::AESENC || m == Mnemonic::AESENCLAST || m == Mnemonic::AESDEC || m == Mnemonic::AESDECLAST ||
        m == Mnemonic::AESIMC || m == Mnemonic::AESKEYGENASSIST || m == Mnemonic::PCLMULQDQ ||
        m == Mnemonic::SHA1MSG1 || m == Mnemonic::SHA1MSG2 || m == Mnemonic::SHA1NEXTE || m == Mnemonic::SHA1RNDS4 ||
        m == Mnemonic::SHA256MSG1 || m == Mnemonic::SHA256MSG2 || m == Mnemonic::SHA256RNDS2) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::FLD || m == Mnemonic::FST || m == Mnemonic::FSTP || m == Mnemonic::FILD || m == Mnemonic::FIST || m == Mnemonic::FISTP ||
        m == Mnemonic::FADD || m == Mnemonic::FADDP || m == Mnemonic::FSUB || m == Mnemonic::FSUBP || m == Mnemonic::FSUBR || m == Mnemonic::FSUBRP ||
        m == Mnemonic::FMUL || m == Mnemonic::FMULP || m == Mnemonic::FDIV || m == Mnemonic::FDIVP || m == Mnemonic::FDIVR || m == Mnemonic::FDIVRP ||
        m == Mnemonic::FCHS || m == Mnemonic::FABS || m == Mnemonic::FTST || m == Mnemonic::FCOM || m == Mnemonic::FCOMP || m == Mnemonic::FCOMPP ||
        m == Mnemonic::FUCOM || m == Mnemonic::FUCOMP || m == Mnemonic::FUCOMPP || m == Mnemonic::FXCH ||
        m == Mnemonic::FINIT || m == Mnemonic::FNINIT || m == Mnemonic::FCLEX || m == Mnemonic::FNCLEX ||
        m == Mnemonic::FLDZ || m == Mnemonic::FLD1 || m == Mnemonic::FLDPI || m == Mnemonic::FLDLN2 || m == Mnemonic::FLDLG2 ||
        m == Mnemonic::FSQRT || m == Mnemonic::FPREM || m == Mnemonic::FPREM1 || m == Mnemonic::FRNDINT || m == Mnemonic::FSCALE ||
        m == Mnemonic::FSIN || m == Mnemonic::FCOS || m == Mnemonic::FSINCOS || m == Mnemonic::FPTAN || m == Mnemonic::FPATAN ||
        m == Mnemonic::F2XM1 || m == Mnemonic::FYL2X || m == Mnemonic::FYL2XP1) {
        l.ops.push_back(make_op2(Opcode::NOP, VarNode::temp(0, 1), VarNode::constant(0, 1)));
        return true;
    }

    if (m == Mnemonic::XBEGIN) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::CLWB || m == Mnemonic::WBNOINVD || m == Mnemonic::RDPID ||
        m == Mnemonic::INVPCID || m == Mnemonic::LGDT || m == Mnemonic::LIDT || m == Mnemonic::SGDT || m == Mnemonic::SIDT ||
        m == Mnemonic::LLDT || m == Mnemonic::SLDT || m == Mnemonic::LTR || m == Mnemonic::STR ||
        m == Mnemonic::VERR || m == Mnemonic::VERW || m == Mnemonic::SWAPGS || m == Mnemonic::CLTS) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::PREFETCHNTA || m == Mnemonic::PREFETCHT0 || m == Mnemonic::PREFETCHT1 || m == Mnemonic::PREFETCHT2 ||
        m == Mnemonic::PREFETCHW) {
        l.ops.push_back({Opcode::NOP});
        return true;
    }

    if (di.desc->num_operands >= 1 && (
        di.desc->operands[0].addr == AddrMode::XmmReg ||
        di.desc->operands[0].addr == AddrMode::XmmRM ||
        di.desc->operands[0].addr == AddrMode::MmxReg ||
        di.desc->operands[0].addr == AddrMode::MmxRM)) {
        l.ops.push_back({Opcode::NOP});
        return true;
    }


    if (m == Mnemonic::IN || m == Mnemonic::OUT || m == Mnemonic::INS || m == Mnemonic::OUTS) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::CMOVA || m == Mnemonic::CMOVAE || m == Mnemonic::CMOVC || m == Mnemonic::CMOVE ||
        m == Mnemonic::CMOVG || m == Mnemonic::CMOVGE || m == Mnemonic::CMOVNA || m == Mnemonic::CMOVNAE ||
        m == Mnemonic::CMOVNC || m == Mnemonic::CMOVNE || m == Mnemonic::CMOVNG || m == Mnemonic::CMOVNGE ||
        m == Mnemonic::CMOVPE || m == Mnemonic::CMOVPO) {
        if (di.desc->has_modrm && ((di.modrm >> 6) & 3) == 3) {
            VarNode dst = reg_from_modrm_reg(di, sz);
            VarNode src = reg_from_modrm_rm(di, sz);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
            return true;
        }
    }

    if (m == Mnemonic::SETA || m == Mnemonic::SETAE || m == Mnemonic::SETC || m == Mnemonic::SETE ||
        m == Mnemonic::SETG || m == Mnemonic::SETGE || m == Mnemonic::SETNA || m == Mnemonic::SETNAE ||
        m == Mnemonic::SETNC || m == Mnemonic::SETNE || m == Mnemonic::SETNG || m == Mnemonic::SETNGE ||
        m == Mnemonic::SETPE || m == Mnemonic::SETPO) {
        if (di.desc->has_modrm) {
            VarNode dst = (mod_ == 3) ? reg_from_modrm_rm(di, 1) : VarNode::temp(11, 1);
            Opcode get_flag = Opcode::GET_ZF;
            bool invert = false;
            if (m == Mnemonic::SETC || m == Mnemonic::SETNAE) get_flag = Opcode::GET_CF;
            else if (m == Mnemonic::SETNC || m == Mnemonic::SETAE) { get_flag = Opcode::GET_CF; invert = true; }
            else if (m == Mnemonic::SETE) get_flag = Opcode::GET_ZF;
            else if (m == Mnemonic::SETNE) { get_flag = Opcode::GET_ZF; invert = true; }
            else if (m == Mnemonic::SETA) { get_flag = Opcode::GET_CF; invert = true; }
            else if (m == Mnemonic::SETNA) get_flag = Opcode::GET_CF;
            else if (m == Mnemonic::SETG) { get_flag = Opcode::GET_ZF; invert = true; }
            else if (m == Mnemonic::SETNG) get_flag = Opcode::GET_ZF;
            else if (m == Mnemonic::SETGE) { get_flag = Opcode::GET_SF; invert = true; }
            else if (m == Mnemonic::SETNGE) get_flag = Opcode::GET_SF;
            else if (m == Mnemonic::SETPE) get_flag = Opcode::GET_PF;
            else if (m == Mnemonic::SETPO) { get_flag = Opcode::GET_PF; invert = true; }
            l.ops.push_back(make_op2(get_flag, dst, VarNode::flags()));
            if (invert) l.ops.push_back(make_op2(Opcode::NOT, dst, dst));
            if (mod_ != 3) {
                VarNode ea = compute_ea(l, di);
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(1), ea, dst));
            }
            return true;
        }
    }

    if (m == Mnemonic::JA || m == Mnemonic::JAE || m == Mnemonic::JC || m == Mnemonic::JE ||
        m == Mnemonic::JG || m == Mnemonic::JGE || m == Mnemonic::JNA || m == Mnemonic::JNAE ||
        m == Mnemonic::JNC || m == Mnemonic::JNE || m == Mnemonic::JNG || m == Mnemonic::JNGE ||
        m == Mnemonic::JPE || m == Mnemonic::JPO) {
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                VarNode target = VarNode::constant((int64_t)(address + n + di.immediate), 8);
                VarNode fallthrough = VarNode::constant((int64_t)(address + n), 8);
                l.ops.push_back(make_op3(Opcode::CBRANCH, target, target, fallthrough));
                return true;
            }
        }
    }

    if (m == Mnemonic::INSB || m == Mnemonic::INSW || m == Mnemonic::INSD ||
        m == Mnemonic::OUTSB || m == Mnemonic::OUTSW || m == Mnemonic::OUTSD) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::CDQ) {
        VarNode rax = VarNode::gpr(0, 4);
        VarNode rdx = VarNode::gpr(2, 4);
        l.ops.push_back(make_op3(Opcode::SAR, rdx, rax, VarNode::constant(31, 1)));
        return true;
    }
    if (m == Mnemonic::CDQE) {
        VarNode src = VarNode::gpr(0, 4);
        VarNode dst = VarNode::gpr(0, 8);
        l.ops.push_back(make_op2(Opcode::SEXT, dst, src));
        return true;
    }
    if (m == Mnemonic::CQO) {
        VarNode rax = VarNode::gpr(0, 8);
        VarNode rdx = VarNode::gpr(2, 8);
        l.ops.push_back(make_op3(Opcode::SAR, rdx, rax, VarNode::constant(63, 1)));
        return true;
    }

    if (m == Mnemonic::ENTER) {
        VarNode rsp = VarNode::gpr(4, 8);
        VarNode rbp = VarNode::gpr(5, 8);
        VarNode eight = VarNode::constant(8, 8);
        int nesting = (int)(di.displacement & 0x1F); // nesting level 0-31
        l.ops.push_back(make_op3(Opcode::SUB, rsp, rsp, eight));
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), rsp, rbp));
        VarNode frame_temp = VarNode::temp(17, 8);
        l.ops.push_back(make_op2(Opcode::COPY, frame_temp, rsp));
        if (nesting > 0) {
            for (int i = 1; i < nesting; i++) {
                // RBP -= 8; push [RBP] (copy caller's frame ptr)
                l.ops.push_back(make_op3(Opcode::SUB, rbp, rbp, eight));
                VarNode tmp = VarNode::temp(18, 8);
                l.ops.push_back(make_op3(Opcode::LOAD, tmp, rbp, VarNode::ram(8)));
                l.ops.push_back(make_op3(Opcode::SUB, rsp, rsp, eight));
                l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), rsp, tmp));
            }
            l.ops.push_back(make_op3(Opcode::SUB, rsp, rsp, eight));
            l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), rsp, frame_temp));
        }
        l.ops.push_back(make_op2(Opcode::COPY, rbp, frame_temp));
        if (di.immediate > 0)
            l.ops.push_back(make_op3(Opcode::SUB, rsp, rsp, VarNode::constant(di.immediate, 8)));
        return true;
    }

    if (m == Mnemonic::CMPXCHG8B || m == Mnemonic::CMPXCHG16B) {
        // Simplified: emit LOAD from memory, SUB_FLAGS against EDX:EAX, then STORE ECX:EBX
        VarNode ea = compute_ea(l, di);
        uint8_t half = (m == Mnemonic::CMPXCHG8B) ? 4 : 8;
        VarNode lo = VarNode::temp(14, half);
        VarNode hi = VarNode::temp(15, half);
        l.ops.push_back(make_op3(Opcode::LOAD, lo, ea, VarNode::ram(half)));
        VarNode ea2 = VarNode::temp(16, 8);
        l.ops.push_back(make_op3(Opcode::ADD, ea2, ea, VarNode::constant(half, 8)));
        l.ops.push_back(make_op3(Opcode::LOAD, hi, ea2, VarNode::ram(half)));
        // Compare low half with EAX/RAX, set flags
        VarNode rax = VarNode::gpr(0, half);
        l.ops.push_back(make_op3(Opcode::SUB_FLAGS, VarNode::flags(), rax, lo));
        // Simplified: assume equal, store ECX:EBX to memory
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(half), ea, VarNode::gpr(3, half)));
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(half), ea2, VarNode::gpr(1, half)));
        return true;
    }

    if (m == Mnemonic::ADDSUBPS || m == Mnemonic::ADDSUBPD ||
        m == Mnemonic::HADDPS || m == Mnemonic::HADDPD || m == Mnemonic::HSUBPS || m == Mnemonic::HSUBPD) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, tmp));
        }
        return true;
    }
    if (m == Mnemonic::MOVDDUP || m == Mnemonic::MOVSHDUP || m == Mnemonic::MOVSLDUP || m == Mnemonic::LDDQU) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PHADDD || m == Mnemonic::PHADDW || m == Mnemonic::PHADDSW ||
        m == Mnemonic::PHSUBD || m == Mnemonic::PHSUBW || m == Mnemonic::PHSUBSW ||
        m == Mnemonic::PMADDUBSW || m == Mnemonic::PMULHRSW ||
        m == Mnemonic::PSIGNB || m == Mnemonic::PSIGND || m == Mnemonic::PSIGNW) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PACKUSDW || m == Mnemonic::PMAXSB || m == Mnemonic::PMAXSW ||
        m == Mnemonic::PMINSB || m == Mnemonic::PMINSW || m == Mnemonic::PMOVMSKB ||
        m == Mnemonic::PMOVSXBD || m == Mnemonic::PMOVSXBQ || m == Mnemonic::PMOVSXWQ ||
        m == Mnemonic::PMOVZXBD || m == Mnemonic::PMOVZXBQ || m == Mnemonic::PMOVZXWQ ||
        m == Mnemonic::PMULDQ || m == Mnemonic::PMULHUW || m == Mnemonic::PMULHW ||
        m == Mnemonic::PSADBW || m == Mnemonic::PTEST || m == Mnemonic::PHMINPOSUW ||
        m == Mnemonic::EXTRACTPS || m == Mnemonic::INSERTPS ||
        m == Mnemonic::PCMPESTRI || m == Mnemonic::PCMPESTRM || m == Mnemonic::PCMPISTRI || m == Mnemonic::PCMPISTRM) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::PADDSB || m == Mnemonic::PADDSW || m == Mnemonic::PADDUSB || m == Mnemonic::PADDUSW ||
        m == Mnemonic::PSUBSB || m == Mnemonic::PSUBSW || m == Mnemonic::PSUBUSB || m == Mnemonic::PSUBUSW ||
        m == Mnemonic::PAVGB || m == Mnemonic::PAVGW) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            Opcode opc = (m == Mnemonic::PSUBSB || m == Mnemonic::PSUBSW || m == Mnemonic::PSUBUSB || m == Mnemonic::PSUBUSW)
                ? Opcode::SUB : Opcode::ADD;
            l.ops.push_back(make_op3(opc, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            Opcode opc = (m == Mnemonic::PSUBSB || m == Mnemonic::PSUBSW || m == Mnemonic::PSUBUSB || m == Mnemonic::PSUBUSW)
                ? Opcode::SUB : Opcode::ADD;
            l.ops.push_back(make_op3(opc, dst, dst, tmp));
        }
        return true;
    }
    if (m == Mnemonic::PSLLDQ || m == Mnemonic::PSRLDQ) {
        VarNode dst = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
        Opcode opc = (m == Mnemonic::PSLLDQ) ? Opcode::SHL : Opcode::SHR;
        l.ops.push_back(make_op3(opc, dst, dst, VarNode::constant(di.immediate * 8, 1)));
        return true;
    }
    if (m == Mnemonic::MOVNTI) {
        VarNode ea = compute_ea(l, di);
        VarNode src = reg_from_modrm_reg(di, sz);
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(sz), ea, src));
        return true;
    }
    if (m == Mnemonic::MASKMOVDQU || m == Mnemonic::MASKMOVQ ||
        m == Mnemonic::MOVNTQ ||
        m == Mnemonic::MOVDQ2Q || m == Mnemonic::MOVQ2DQ) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }
    if (m == Mnemonic::STMXCSR || m == Mnemonic::LDMXCSR) {
        l.ops.push_back({Opcode::NOP});
        return true;
    }

    if (m == Mnemonic::VFMADD132PD || m == Mnemonic::VFMADD132PS || m == Mnemonic::VFMADD132SD || m == Mnemonic::VFMADD132SS ||
        m == Mnemonic::VFMADD213PD || m == Mnemonic::VFMADD213PS || m == Mnemonic::VFMADD213SD || m == Mnemonic::VFMADD213SS ||
        m == Mnemonic::VFMADD231PD || m == Mnemonic::VFMADD231PS || m == Mnemonic::VFMADD231SD || m == Mnemonic::VFMADD231SS ||
        m == Mnemonic::VFMADDSUB132PD || m == Mnemonic::VFMADDSUB132PS || m == Mnemonic::VFMADDSUB213PD || m == Mnemonic::VFMADDSUB213PS ||
        m == Mnemonic::VFMADDSUB231PD || m == Mnemonic::VFMADDSUB231PS ||
        m == Mnemonic::VFMSUB132PD || m == Mnemonic::VFMSUB132PS || m == Mnemonic::VFMSUB132SD || m == Mnemonic::VFMSUB132SS ||
        m == Mnemonic::VFMSUB213PD || m == Mnemonic::VFMSUB213PS || m == Mnemonic::VFMSUB213SD || m == Mnemonic::VFMSUB213SS ||
        m == Mnemonic::VFMSUB231PD || m == Mnemonic::VFMSUB231PS || m == Mnemonic::VFMSUB231SD || m == Mnemonic::VFMSUB231SS ||
        m == Mnemonic::VFMSUBADD132PD || m == Mnemonic::VFMSUBADD132PS || m == Mnemonic::VFMSUBADD213PD || m == Mnemonic::VFMSUBADD213PS ||
        m == Mnemonic::VFMSUBADD231PD || m == Mnemonic::VFMSUBADD231PS ||
        m == Mnemonic::VFNMADD132PD || m == Mnemonic::VFNMADD132PS || m == Mnemonic::VFNMADD132SD || m == Mnemonic::VFNMADD132SS ||
        m == Mnemonic::VFNMADD213PD || m == Mnemonic::VFNMADD213PS || m == Mnemonic::VFNMADD213SD || m == Mnemonic::VFNMADD213SS ||
        m == Mnemonic::VFNMADD231PD || m == Mnemonic::VFNMADD231PS || m == Mnemonic::VFNMADD231SD || m == Mnemonic::VFNMADD231SS ||
        m == Mnemonic::VFNMSUB132PD || m == Mnemonic::VFNMSUB132PS || m == Mnemonic::VFNMSUB132SD || m == Mnemonic::VFNMSUB132SS ||
        m == Mnemonic::VFNMSUB213PD || m == Mnemonic::VFNMSUB213PS || m == Mnemonic::VFNMSUB213SD || m == Mnemonic::VFNMSUB213SS ||
        m == Mnemonic::VFNMSUB231PD || m == Mnemonic::VFNMSUB231PS || m == Mnemonic::VFNMSUB231SD || m == Mnemonic::VFNMSUB231SS) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op3(Opcode::ADD, dst, dst, tmp));
        }
        return true;
    }

    if (m == Mnemonic::VZEROALL || m == Mnemonic::VZEROUPPER) {
        l.ops.push_back({Opcode::NOP});
        return true;
    }

    if (m == Mnemonic::VBROADCASTSS || m == Mnemonic::VBROADCASTSD || m == Mnemonic::VBROADCASTF128 ||
        m == Mnemonic::VPBROADCASTB || m == Mnemonic::VPBROADCASTW || m == Mnemonic::VPBROADCASTD || m == Mnemonic::VPBROADCASTQ ||
        m == Mnemonic::VPERM2F128 || m == Mnemonic::VPERM2I128 || m == Mnemonic::VPERMD || m == Mnemonic::VPERMQ ||
        m == Mnemonic::VPERMPD || m == Mnemonic::VPERMPS || m == Mnemonic::VPERMILPS || m == Mnemonic::VPERMILPD ||
        m == Mnemonic::VINSERTF128 || m == Mnemonic::VINSERTI128 || m == Mnemonic::VEXTRACTF128 || m == Mnemonic::VEXTRACTI128) {
        if (mod_ == 3) {
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode src = VarNode::xmm((di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0), 16);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
        } else {
            VarNode ea = compute_ea(l, di);
            VarNode dst = VarNode::xmm(((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0), 16);
            VarNode tmp = VarNode::temp(20, 16);
            l.ops.push_back(make_op3(Opcode::LOAD, tmp, ea, VarNode::ram(16)));
            l.ops.push_back(make_op2(Opcode::COPY, dst, tmp));
        }
        return true;
    }
    if (m == Mnemonic::VGATHERDPS || m == Mnemonic::VGATHERDPD || m == Mnemonic::VGATHERQPS || m == Mnemonic::VGATHERQPD ||
        m == Mnemonic::VPGATHERDD || m == Mnemonic::VPGATHERDQ || m == Mnemonic::VPGATHERQD || m == Mnemonic::VPGATHERQQ) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::KMOVB || m == Mnemonic::KMOVW || m == Mnemonic::KMOVD || m == Mnemonic::KMOVQ) {
        uint8_t ksz = (m == Mnemonic::KMOVB) ? 1 : (m == Mnemonic::KMOVW) ? 2 : (m == Mnemonic::KMOVD) ? 4 : 8;
        if (di.desc->has_modrm && ((di.modrm >> 6) & 3) == 3) {
            VarNode dst = VarNode::opmask((di.modrm >> 3) & 7, ksz);
            VarNode src = VarNode::opmask(di.modrm & 7, ksz);
            l.ops.push_back(make_op2(Opcode::COPY, dst, src));
            return true;
        }
        l.ops.push_back({Opcode::NOP}); return true;
    }
    if (m == Mnemonic::KANDW || m == Mnemonic::KANDD || m == Mnemonic::KANDQ || m == Mnemonic::KANDNW ||
        m == Mnemonic::KORW || m == Mnemonic::KORD || m == Mnemonic::KORQ ||
        m == Mnemonic::KXORW || m == Mnemonic::KXORD || m == Mnemonic::KXORQ ||
        m == Mnemonic::KNOTW || m == Mnemonic::KNOTD || m == Mnemonic::KNOTQ) {
        uint8_t ksz = (m==Mnemonic::KANDW||m==Mnemonic::KORW||m==Mnemonic::KXORW||m==Mnemonic::KNOTW||m==Mnemonic::KANDNW)?2:
                      (m==Mnemonic::KANDD||m==Mnemonic::KORD||m==Mnemonic::KXORD||m==Mnemonic::KNOTD)?4:8;
        if (di.desc->has_modrm) {
            VarNode dst = VarNode::opmask((di.modrm >> 3) & 7, ksz);
            VarNode src = VarNode::opmask(di.modrm & 7, ksz);
            Opcode oc = Opcode::AND;
            if (m==Mnemonic::KORW||m==Mnemonic::KORD||m==Mnemonic::KORQ) oc = Opcode::OR;
            else if (m==Mnemonic::KXORW||m==Mnemonic::KXORD||m==Mnemonic::KXORQ) oc = Opcode::XOR;
            else if (m==Mnemonic::KNOTW||m==Mnemonic::KNOTD||m==Mnemonic::KNOTQ) {
                l.ops.push_back(make_op2(Opcode::NOT, dst, src)); return true;
            }
            l.ops.push_back(make_op3(oc, dst, dst, src));
            return true;
        }
        l.ops.push_back({Opcode::NOP}); return true;
    }
    if (m == Mnemonic::KORTESTW || m == Mnemonic::KORTESTD || m == Mnemonic::KORTESTQ) {
        uint8_t ksz = (m==Mnemonic::KORTESTW)?2:(m==Mnemonic::KORTESTD)?4:8;
        if (di.desc->has_modrm) {
            VarNode a = VarNode::opmask((di.modrm >> 3) & 7, ksz);
            VarNode b = VarNode::opmask(di.modrm & 7, ksz);
            VarNode t = VarNode::temp(30, ksz);
            l.ops.push_back(make_op3(Opcode::OR, t, a, b));
            l.ops.push_back(make_op3(Opcode::AND_FLAGS, VarNode::flags(), t, t));
            return true;
        }
        l.ops.push_back({Opcode::NOP}); return true;
    }
    if (m == Mnemonic::KUNPCKBW || m == Mnemonic::KUNPCKWD || m == Mnemonic::KUNPCKDQ ||
        m == Mnemonic::KSHIFTRW || m == Mnemonic::KSHIFTLW) {
        l.ops.push_back({Opcode::NOP}); return true; // simplified: opmask shift/unpack
    }

    if (m == Mnemonic::CALLF || m == Mnemonic::RETF ||
        m == Mnemonic::IRET || m == Mnemonic::IRETD || m == Mnemonic::IRETQ ||
        m == Mnemonic::SYSRET || m == Mnemonic::SYSEXIT || m == Mnemonic::RSM ||
        m == Mnemonic::UD || m == Mnemonic::UD2 || m == Mnemonic::ICEBP ||
        m == Mnemonic::XLAT || m == Mnemonic::XLATB || m == Mnemonic::GETSEC ||
        m == Mnemonic::SMSW || m == Mnemonic::LMSW || m == Mnemonic::LAR || m == Mnemonic::LSL ||
        m == Mnemonic::LDS || m == Mnemonic::LES || m == Mnemonic::LFS || m == Mnemonic::LGS || m == Mnemonic::LSS) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::XSAVE || m == Mnemonic::XSAVEC || m == Mnemonic::XSAVEOPT || m == Mnemonic::XSAVES ||
        m == Mnemonic::XRSTOR || m == Mnemonic::XRSTORS ||
        m == Mnemonic::FXSAVE || m == Mnemonic::FXRSTOR) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::CLRSSBSY || m == Mnemonic::INCSSPD || m == Mnemonic::INCSSPQ ||
        m == Mnemonic::RDSSPD || m == Mnemonic::RDSSPQ || m == Mnemonic::RSTORSSP || m == Mnemonic::SAVEPREVSSP ||
        m == Mnemonic::WRSSD || m == Mnemonic::WRSSQ || m == Mnemonic::WRUSSD || m == Mnemonic::WRUSSQ) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::CLUI || m == Mnemonic::STUI || m == Mnemonic::TESTUI ||
        m == Mnemonic::UIRET || m == Mnemonic::SENDUIPI) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::AESENC128KL || m == Mnemonic::AESDEC128KL || m == Mnemonic::AESENC256KL || m == Mnemonic::AESDEC256KL ||
        m == Mnemonic::AESENCWIDE128KL || m == Mnemonic::AESDECWIDE128KL || m == Mnemonic::AESENCWIDE256KL || m == Mnemonic::AESDECWIDE256KL ||
        m == Mnemonic::ENCODEKEY128 || m == Mnemonic::ENCODEKEY256) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::GF2P8AFFINEINVQB || m == Mnemonic::GF2P8AFFINEQB || m == Mnemonic::GF2P8MULB) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::MOVDIR64B || m == Mnemonic::MOVDIRI ||
        m == Mnemonic::ENQCMD || m == Mnemonic::ENQCMDS || m == Mnemonic::HRESET ||
        m == Mnemonic::PTWRITE || m == Mnemonic::TPAUSE || m == Mnemonic::UMONITOR || m == Mnemonic::UMWAIT ||
        m == Mnemonic::RDPKRU || m == Mnemonic::WRPKRU ||
        m == Mnemonic::RDFSBASE || m == Mnemonic::RDGSBASE || m == Mnemonic::WRFSBASE || m == Mnemonic::WRGSBASE) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::PUSHF || m == Mnemonic::PUSHFQ) {
        VarNode rsp = VarNode::gpr(4, 8);
        VarNode eight = VarNode::constant(8, 8);
        l.ops.push_back(make_op3(Opcode::SUB, rsp, rsp, eight));
        l.ops.push_back(make_op3(Opcode::STORE, VarNode::ram(8), rsp, VarNode::flags()));
        return true;
    }
    if (m == Mnemonic::POPF || m == Mnemonic::POPFQ) {
        VarNode rsp = VarNode::gpr(4, 8);
        VarNode eight = VarNode::constant(8, 8);
        VarNode tmp = VarNode::temp(14, 8);
        l.ops.push_back(make_op3(Opcode::LOAD, tmp, rsp, VarNode::ram(8)));
        l.ops.push_back(make_op2(Opcode::COPY, VarNode::flags(), tmp));
        l.ops.push_back(make_op3(Opcode::ADD, rsp, rsp, eight));
        return true;
    }
    if (m == Mnemonic::WAIT) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::VMCALL || m == Mnemonic::VMCLEAR || m == Mnemonic::VMLAUNCH ||
        m == Mnemonic::VMPTRLD || m == Mnemonic::VMPTRST || m == Mnemonic::VMREAD || m == Mnemonic::VMRESUME ||
        m == Mnemonic::VMWRITE || m == Mnemonic::VMXOFF || m == Mnemonic::VMXON ||
        m == Mnemonic::VMFUNC || m == Mnemonic::INVEPT || m == Mnemonic::INVVPID) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    if (m == Mnemonic::AAA || m == Mnemonic::AAD || m == Mnemonic::AAM || m == Mnemonic::AAS ||
        m == Mnemonic::DAA || m == Mnemonic::DAS ||
        m == Mnemonic::PUSHA || m == Mnemonic::PUSHAD || m == Mnemonic::POPA || m == Mnemonic::POPAD ||
        m == Mnemonic::BOUND || m == Mnemonic::SALC) {
        l.ops.push_back({Opcode::NOP});
        return true;
    }

    if (m == Mnemonic::FBLD || m == Mnemonic::FBSTP ||
        m == Mnemonic::FCMOVB || m == Mnemonic::FCMOVBE || m == Mnemonic::FCMOVE ||
        m == Mnemonic::FCMOVNB || m == Mnemonic::FCMOVNBE || m == Mnemonic::FCMOVNE ||
        m == Mnemonic::FCMOVNU || m == Mnemonic::FCMOVU ||
        m == Mnemonic::FCOMI || m == Mnemonic::FCOMIP ||
        m == Mnemonic::FDECSTP || m == Mnemonic::FINCSTP || m == Mnemonic::FISTTP ||
        m == Mnemonic::FIADD || m == Mnemonic::FICOM || m == Mnemonic::FICOMP ||
        m == Mnemonic::FIDIV || m == Mnemonic::FIDIVR || m == Mnemonic::FIMUL ||
        m == Mnemonic::FISUB || m == Mnemonic::FISUBR ||
        m == Mnemonic::FLDENV || m == Mnemonic::FLDL2E || m == Mnemonic::FLDL2T ||
        m == Mnemonic::FNDISI || m == Mnemonic::FNENI || m == Mnemonic::FNSAVE ||
        m == Mnemonic::FNSTSW || m == Mnemonic::FNSTCW || m == Mnemonic::FLDCW) {
        l.ops.push_back(make_op2(Opcode::NOP, VarNode::temp(0, 1), VarNode::constant(0, 1)));
        return true;
    }

    if (m == Mnemonic::FCOM2 || m == Mnemonic::FCOMP3 || m == Mnemonic::FCOMP5 ||
        m == Mnemonic::FDISI || m == Mnemonic::FENI || m == Mnemonic::FFREE || m == Mnemonic::FFREEP ||
        m == Mnemonic::FNSETPM || m == Mnemonic::FNSTENV || m == Mnemonic::FRSTOR || m == Mnemonic::FSAVE ||
        m == Mnemonic::FSETPM || m == Mnemonic::FSTCW || m == Mnemonic::FSTENV || m == Mnemonic::FSTP1 ||
        m == Mnemonic::FSTP8 || m == Mnemonic::FSTP9 || m == Mnemonic::FSTSW ||
        m == Mnemonic::FUCOMI || m == Mnemonic::FUCOMIP || m == Mnemonic::FXAM ||
        m == Mnemonic::FXCH4 || m == Mnemonic::FXCH7 || m == Mnemonic::FXTRACT) {
        l.ops.push_back(make_op2(Opcode::NOP, VarNode::temp(0, 1), VarNode::constant(0, 1)));
        return true;
    }

    if (m == Mnemonic::BNDCL || m == Mnemonic::BNDCN || m == Mnemonic::BNDCU ||
        m == Mnemonic::BNDLDX || m == Mnemonic::BNDMK || m == Mnemonic::BNDMOV || m == Mnemonic::BNDSTX) {
        l.ops.push_back(make_op2(Opcode::BARRIER, VarNode::temp(0, 1), VarNode::constant(0, 1)));
        return true;
    }

    if (m == Mnemonic::CVTPD2PI || m == Mnemonic::CVTPI2PD || m == Mnemonic::CVTPI2PS ||
        m == Mnemonic::CVTPS2PI || m == Mnemonic::CVTTPD2PI || m == Mnemonic::CVTTPS2PI) {
        l.ops.push_back(make_op2(Opcode::BARRIER, VarNode::temp(0, 1), VarNode::constant(0, 1)));
        return true;
    }

    if (m == Mnemonic::ADX || m == Mnemonic::AMX || m == Mnemonic::INTO ||
        m == Mnemonic::JCXZ || m == Mnemonic::JECXZ || m == Mnemonic::JRCXZ ||
        m == Mnemonic::JMPE || m == Mnemonic::JMPF || m == Mnemonic::SETALC || m == Mnemonic::SETSSBSY ||
        m == Mnemonic::LODSD || m == Mnemonic::STOSD || m == Mnemonic::SCASD ||
        m == Mnemonic::POPFD || m == Mnemonic::PUSHFD || m == Mnemonic::INVALID) {
        l.ops.push_back(make_op2(Opcode::NOP, VarNode::temp(0, 1), VarNode::constant(0, 1)));
        return true;
    }

    if (
        m == Mnemonic::VPXORD
        || m == Mnemonic::VPXORQ
        || m == Mnemonic::VPANDD
        || m == Mnemonic::VPANDQ
        || m == Mnemonic::VPORD
        || m == Mnemonic::VPORQ
        || m == Mnemonic::VPANDND
        || m == Mnemonic::VPANDNQ
        || m == Mnemonic::VPTERNLOGD
        || m == Mnemonic::VPTERNLOGQ
        || m == Mnemonic::VBLENDMPS
        || m == Mnemonic::VBLENDMPD
        || m == Mnemonic::VPBLENDMD
        || m == Mnemonic::VPBLENDMQ
        || m == Mnemonic::VPBLENDMB
        || m == Mnemonic::VPBLENDMW
        || m == Mnemonic::VPERMI2D
        || m == Mnemonic::VPERMI2Q
        || m == Mnemonic::VPERMI2PS
        || m == Mnemonic::VPERMI2PD
        || m == Mnemonic::VPERMT2D
        || m == Mnemonic::VPERMT2Q
        || m == Mnemonic::VPERMT2PS
        || m == Mnemonic::VPERMT2PD
        || m == Mnemonic::VPERMW
        || m == Mnemonic::VPERMB
        || m == Mnemonic::VPTESTMD
        || m == Mnemonic::VPTESTMQ
        || m == Mnemonic::VPTESTMB
        || m == Mnemonic::VPTESTMW
        || m == Mnemonic::VPTESTNMD
        || m == Mnemonic::VPTESTNMQ
        || m == Mnemonic::VPTESTNMB
        || m == Mnemonic::VPTESTNMW
        || m == Mnemonic::VPCONFLICTD
        || m == Mnemonic::VPCONFLICTQ
        || m == Mnemonic::VPLZCNTD
        || m == Mnemonic::VPLZCNTQ
        || m == Mnemonic::VCOMPRESSPS
        || m == Mnemonic::VCOMPRESSPD
        || m == Mnemonic::VPCOMPRESSD
        || m == Mnemonic::VPCOMPRESSQ
        || m == Mnemonic::VEXPANDPS
        || m == Mnemonic::VEXPANDPD
        || m == Mnemonic::VPEXPANDD
        || m == Mnemonic::VPEXPANDQ
        || m == Mnemonic::VMOVDQA32
        || m == Mnemonic::VMOVDQA64
        || m == Mnemonic::VMOVDQU8
        || m == Mnemonic::VMOVDQU16
        || m == Mnemonic::VMOVDQU32
        || m == Mnemonic::VMOVDQU64
        || m == Mnemonic::VPROLVD
        || m == Mnemonic::VPROLVQ
        || m == Mnemonic::VPRORVD
        || m == Mnemonic::VPRORVQ
        || m == Mnemonic::VPROLD
        || m == Mnemonic::VPROLQ
        || m == Mnemonic::VPRORD
        || m == Mnemonic::VPRORQ
        || m == Mnemonic::VFIXUPIMMPS
        || m == Mnemonic::VFIXUPIMMPD
        || m == Mnemonic::VFIXUPIMMSS
        || m == Mnemonic::VFIXUPIMMSD
        || m == Mnemonic::VGETEXPPS
        || m == Mnemonic::VGETEXPPD
        || m == Mnemonic::VGETEXPSS
        || m == Mnemonic::VGETEXPSD
        || m == Mnemonic::VGETMANTPS
        || m == Mnemonic::VGETMANTPD
        || m == Mnemonic::VGETMANTSS
        || m == Mnemonic::VGETMANTSD
        || m == Mnemonic::VRNDSCALEPS
        || m == Mnemonic::VRNDSCALEPD
        || m == Mnemonic::VRNDSCALESS
        || m == Mnemonic::VRNDSCALESD
        || m == Mnemonic::VSCALEFPS
        || m == Mnemonic::VSCALEFPD
        || m == Mnemonic::VSCALEFSS
        || m == Mnemonic::VSCALEFSD
        || m == Mnemonic::VRANGEPS
        || m == Mnemonic::VRANGEPD
        || m == Mnemonic::VRANGESS
        || m == Mnemonic::VRANGESD
        || m == Mnemonic::VREDUCEPS
        || m == Mnemonic::VREDUCEPD
        || m == Mnemonic::VREDUCESS
        || m == Mnemonic::VREDUCESD
        || m == Mnemonic::VFPCLASSPS
        || m == Mnemonic::VFPCLASSPD
        || m == Mnemonic::VFPCLASSSS
        || m == Mnemonic::VFPCLASSSD
        || m == Mnemonic::VDBPSADBW
        || m == Mnemonic::VPALIGNR
        || m == Mnemonic::VPDPBUSD
        || m == Mnemonic::VPDPBUSDS
        || m == Mnemonic::VPDPWSSD
        || m == Mnemonic::VPDPWSSDS
        || m == Mnemonic::VPMADD52LUQ
        || m == Mnemonic::VPMADD52HUQ
        || m == Mnemonic::VCVTPS2UDQ
        || m == Mnemonic::VCVTPD2UDQ
        || m == Mnemonic::VCVTTPS2UDQ
        || m == Mnemonic::VCVTTPD2UDQ
        || m == Mnemonic::VCVTUDQ2PS
        || m == Mnemonic::VCVTUDQ2PD
        || m == Mnemonic::VCVTUSI2SS
        || m == Mnemonic::VCVTUSI2SD
        || m == Mnemonic::VCVTSS2USI
        || m == Mnemonic::VCVTSD2USI
        || m == Mnemonic::VCVTTSS2USI
        || m == Mnemonic::VCVTTSD2USI
        || m == Mnemonic::VCVTQQ2PS
        || m == Mnemonic::VCVTQQ2PD
        || m == Mnemonic::VCVTUQQ2PS
        || m == Mnemonic::VCVTUQQ2PD
        || m == Mnemonic::VCVTPS2QQ
        || m == Mnemonic::VCVTPD2QQ
        || m == Mnemonic::VCVTPS2UQQ
        || m == Mnemonic::VCVTPD2UQQ
        || m == Mnemonic::VCVTTPS2QQ
        || m == Mnemonic::VCVTTPD2QQ
        || m == Mnemonic::VCVTTPS2UQQ
        || m == Mnemonic::VCVTTPD2UQQ
        || m == Mnemonic::VINSERTF32X4
        || m == Mnemonic::VINSERTF64X2
        || m == Mnemonic::VINSERTF32X8
        || m == Mnemonic::VINSERTF64X4
        || m == Mnemonic::VINSERTI32X4
        || m == Mnemonic::VINSERTI64X2
        || m == Mnemonic::VINSERTI32X8
        || m == Mnemonic::VINSERTI64X4
        || m == Mnemonic::VEXTRACTF32X4
        || m == Mnemonic::VEXTRACTF64X2
        || m == Mnemonic::VEXTRACTF32X8
        || m == Mnemonic::VEXTRACTF64X4
        || m == Mnemonic::VEXTRACTI32X4
        || m == Mnemonic::VEXTRACTI64X2
        || m == Mnemonic::VEXTRACTI32X8
        || m == Mnemonic::VEXTRACTI64X4
        || m == Mnemonic::VBROADCASTF32X2
        || m == Mnemonic::VBROADCASTF32X4
        || m == Mnemonic::VBROADCASTF32X8
        || m == Mnemonic::VBROADCASTF64X2
        || m == Mnemonic::VBROADCASTF64X4
        || m == Mnemonic::VBROADCASTI32X2
        || m == Mnemonic::VBROADCASTI32X4
        || m == Mnemonic::VBROADCASTI32X8
        || m == Mnemonic::VBROADCASTI64X2
        || m == Mnemonic::VBROADCASTI64X4
        || m == Mnemonic::VSCATTERDPS
        || m == Mnemonic::VSCATTERDPD
        || m == Mnemonic::VSCATTERQPS
        || m == Mnemonic::VSCATTERQPD
        || m == Mnemonic::VPSCATTERDD
        || m == Mnemonic::VPSCATTERDQ
        || m == Mnemonic::VPSCATTERQD
        || m == Mnemonic::VPSCATTERQQ
        || m == Mnemonic::VCVTNEEBF162PS
        || m == Mnemonic::VCVTNEOBF162PS
        || m == Mnemonic::VCVTNEEPH2PS
        || m == Mnemonic::VCVTNEOPH2PS
        || m == Mnemonic::VCVTNEPS2BF16
        || m == Mnemonic::KADDB
        || m == Mnemonic::KADDD
        || m == Mnemonic::KADDQ
        || m == Mnemonic::KADDW
        || m == Mnemonic::KANDB
        || m == Mnemonic::KANDNB
        || m == Mnemonic::KANDND
        || m == Mnemonic::KANDNQ
        || m == Mnemonic::KORB
        || m == Mnemonic::KXORB
        || m == Mnemonic::KXNORB
        || m == Mnemonic::KXNORW
        || m == Mnemonic::KXNORD
        || m == Mnemonic::KXNORQ
        || m == Mnemonic::KNOTB
        || m == Mnemonic::KORTESTB
        || m == Mnemonic::KTESTB
        || m == Mnemonic::KTESTW
        || m == Mnemonic::KTESTD
        || m == Mnemonic::KTESTQ
        || m == Mnemonic::KSHIFTLB
        || m == Mnemonic::KSHIFTLD
        || m == Mnemonic::KSHIFTLQ
        || m == Mnemonic::KSHIFTRB
        || m == Mnemonic::KSHIFTRD
        || m == Mnemonic::KSHIFTRQ
        || m == Mnemonic::LDTILECFG
        || m == Mnemonic::STTILECFG
        || m == Mnemonic::TILELOADD
        || m == Mnemonic::TILELOADDT1
        || m == Mnemonic::TILESTORED
        || m == Mnemonic::TILEZERO
        || m == Mnemonic::TILERELEASE
        || m == Mnemonic::TDPBF16PS
        || m == Mnemonic::TDPBSSD
        || m == Mnemonic::TDPBSUD
        || m == Mnemonic::TDPBUSD
        || m == Mnemonic::TDPBUUD
        || m == Mnemonic::TDPFP16PS
        || m == Mnemonic::VMASKMOVPS
        || m == Mnemonic::VMASKMOVPD
        || m == Mnemonic::INSERTQ
        || m == Mnemonic::EXTRQ
        || m == Mnemonic::PCONFIG
    ) {
        l.ops.push_back({Opcode::BARRIER});
        return true;
    }

    return false;
}

static LiftHandler g_lift_dispatch[2048] = {};
static bool g_lift_dispatch_ready = false;

static void lift_dispatch_init() {
    if (g_lift_dispatch_ready) return;
    for (size_t i = 0; i < g_instr_table_size; ++i)
        g_lift_dispatch[i] = &lift_exec_switch;
    g_lift_dispatch_ready = true;
}

std::optional<Lifted> lift(const uint8_t* code, size_t len, uint64_t address) {
    lift_dispatch_init();
    DecodedInstr di;
    size_t n = decode(code, len, di);
    if (n == 0 || !di.desc) return std::nullopt;

    Lifted l;
    l.address = address;
    l.length = static_cast<uint8_t>(n);

    bool rex_w = (di.rex & 0x08) != 0 || di.vex_W;
    bool has_66 = false;
    for (uint8_t i = 0; i < di.num_prefixes; ++i)
        if (di.legacy_prefix[i] == 0x66) has_66 = true;

    uint8_t sz = 4;
    if (di.desc->num_operands > 0)
        sz = op_size_bytes(di.desc->operands[0].size, rex_w, has_66);

    Mnemonic m = di.desc->mnemonic;
    bool is_string = (m == Mnemonic::MOVS || m == Mnemonic::STOS || m == Mnemonic::LODS ||
        m == Mnemonic::CMPS || m == Mnemonic::SCAS ||
        m == Mnemonic::MOVSB || m == Mnemonic::MOVSW || m == Mnemonic::MOVSQ ||
        m == Mnemonic::STOSB || m == Mnemonic::STOSW || m == Mnemonic::STOSD || m == Mnemonic::STOSQ ||
        m == Mnemonic::LODSB || m == Mnemonic::LODSW || m == Mnemonic::LODSD || m == Mnemonic::LODSQ ||
        m == Mnemonic::CMPSB || m == Mnemonic::CMPSW || m == Mnemonic::CMPSQ ||
        m == Mnemonic::SCASB || m == Mnemonic::SCASW || m == Mnemonic::SCASD || m == Mnemonic::SCASQ);
    if (is_string) {
        bool has_f3 = false, has_f2 = false;
        for (uint8_t i = 0; i < di.num_prefixes; ++i) {
            if (di.legacy_prefix[i] == 0xF3) has_f3 = true;
            if (di.legacy_prefix[i] == 0xF2) has_f2 = true;
        }
        bool is_scan = (m == Mnemonic::CMPS || m == Mnemonic::SCAS ||
            m == Mnemonic::CMPSB || m == Mnemonic::CMPSW || m == Mnemonic::CMPSQ ||
            m == Mnemonic::SCASB || m == Mnemonic::SCASW || m == Mnemonic::SCASD || m == Mnemonic::SCASQ);
        if (has_f2 && is_scan) l.rep = RepMode::RepNZ;
        else if (has_f3 && is_scan) l.rep = RepMode::RepZ;
        else if (has_f3) l.rep = RepMode::Rep;
    }

    ptrdiff_t idx = di.desc - g_instr_table;
    if (idx >= 0 && static_cast<size_t>(idx) < g_instr_table_size && g_lift_dispatch[idx]) {
        if (g_lift_dispatch[idx](l, di, sz, rex_w, has_66, n, address))
            return l;
    }

    // Fallback: emit a single NOP (unlifted)
    l.ops.push_back({Opcode::NOP});
    return l;
}

static void execute_once(Context& ctx, const Lifted& lifted);

void execute(Context& ctx, const Lifted& lifted) {
    if (lifted.rep == RepMode::None) { execute_once(ctx, lifted); return; }
    // REP loop: decrement RCX each iteration, check termination
    while (ctx.gpr[1] != 0) {
        execute_once(ctx, lifted);
        ctx.gpr[1]--;
        if (lifted.rep == RepMode::RepZ && !ctx.flags[2]) break;  // REPZ: stop if ZF=0
        if (lifted.rep == RepMode::RepNZ && ctx.flags[2]) break;  // REPNZ: stop if ZF=1
    }
}

static void execute_once(Context& ctx, const Lifted& lifted) {
    uint64_t temps[32] = {};

    auto read_var = [&](const VarNode& v) -> uint64_t {
        switch (v.space) {
        case Space::Const: return (uint64_t)v.value;
        case Space::GPR: {
            uint64_t full = ctx.gpr[v.offset & 15];
            if (v.size == 1) return full & 0xFF;
            if (v.size == 2) return full & 0xFFFF;
            if (v.size == 4) return full & 0xFFFFFFFF;
            return full;
        }
        case Space::Temp: return temps[v.offset & 31];
        case Space::Flags: return ctx.flags[0];
        case Space::XMM: { uint64_t val = 0; memcpy(&val, &ctx.xmm[v.offset & 15][0], v.size < 8 ? v.size : 8); return val; }
        case Space::OpMask: return ctx.opmask[v.offset & 7];
        default: return 0;
        }
    };

    auto write_var = [&](const VarNode& v, uint64_t val) {
        switch (v.space) {
        case Space::GPR: {
            if (v.size == 1) ctx.gpr[v.offset & 15] = (ctx.gpr[v.offset & 15] & ~0xFFULL) | (val & 0xFF);
            else if (v.size == 2) ctx.gpr[v.offset & 15] = (ctx.gpr[v.offset & 15] & ~0xFFFFULL) | (val & 0xFFFF);
            else if (v.size == 4) ctx.gpr[v.offset & 15] = val & 0xFFFFFFFF; // 32-bit writes zero-extend
            else ctx.gpr[v.offset & 15] = val;
            break;
        }
        case Space::Temp: temps[v.offset & 31] = val; break;
        case Space::Flags: ctx.flags[0] = (uint8_t)val; break;
        case Space::XMM: memcpy(&ctx.xmm[v.offset & 15][0], &val, v.size < 8 ? v.size : 8); break;
        case Space::OpMask: {
            uint64_t mask = v.size >= 8 ? ~0ULL : ((1ULL << (v.size*8)) - 1);
            ctx.opmask[v.offset & 7] = val & mask; break;
        }
        default: break;
        }
    };

    auto popcnt64 = [](uint64_t x) -> uint64_t { uint64_t c = 0; while (x) { c += x & 1; x >>= 1; } return c; };
    auto ctz64 = [](uint64_t x) -> uint64_t { if (!x) return 64; uint64_t c = 0; while (!(x & 1)) { c++; x >>= 1; } return c; };
    auto clz64 = [](uint64_t x) -> uint64_t { if (!x) return 64; uint64_t c = 0; uint64_t m = 1ULL << 63; while (!(x & m)) { c++; m >>= 1; } return c; };

    for (const auto& op : lifted.ops) {
        uint64_t a = read_var(op.inputs[0]);
        uint64_t b = (op.num_inputs >= 2) ? read_var(op.inputs[1]) : 0;
        switch (op.opcode) {
        case Opcode::NOP: case Opcode::BARRIER: case Opcode::UNDEF: break;
        case Opcode::COPY: write_var(op.output, a); break;
        case Opcode::ADD: write_var(op.output, a + b); break;
        case Opcode::SUB: write_var(op.output, a - b); break;
        case Opcode::MUL: write_var(op.output, a * b); break;
        case Opcode::IMUL: write_var(op.output, (uint64_t)((int64_t)a * (int64_t)b)); break;
        case Opcode::DIV: write_var(op.output, b ? a / b : 0); break;
        case Opcode::IDIV: write_var(op.output, b ? (uint64_t)((int64_t)a / (int64_t)b) : 0); break;
        case Opcode::NEG: write_var(op.output, (uint64_t)(-(int64_t)a)); break;
        case Opcode::AND: write_var(op.output, a & b); break;
        case Opcode::OR:  write_var(op.output, a | b); break;
        case Opcode::XOR: write_var(op.output, a ^ b); break;
        case Opcode::NOT: write_var(op.output, ~a); break;
        case Opcode::SHL: write_var(op.output, a << (b & 63)); break;
        case Opcode::SHR: write_var(op.output, a >> (b & 63)); break;
        case Opcode::SAR: write_var(op.output, (uint64_t)((int64_t)a >> (b & 63))); break;
        case Opcode::ROL: { uint8_t s = b & 63; write_var(op.output, (a << s) | (a >> (64 - s))); break; }
        case Opcode::ROR: { uint8_t s = b & 63; write_var(op.output, (a >> s) | (a << (64 - s))); break; }
        case Opcode::ZEXT: write_var(op.output, a); break;
        case Opcode::SEXT: {
            uint8_t src_sz = op.inputs[0].size;
            if (src_sz == 1) write_var(op.output, (uint64_t)(int64_t)(int8_t)a);
            else if (src_sz == 2) write_var(op.output, (uint64_t)(int64_t)(int16_t)a);
            else if (src_sz == 4) write_var(op.output, (uint64_t)(int64_t)(int32_t)a);
            else write_var(op.output, a);
            break;
        }
        case Opcode::TRUNC: write_var(op.output, a); break;
        case Opcode::POPCNT: write_var(op.output, popcnt64(a)); break;
        case Opcode::CTZ: write_var(op.output, ctz64(a)); break;
        case Opcode::CLZ: write_var(op.output, clz64(a)); break;
        case Opcode::ADD_FLAGS: case Opcode::SUB_FLAGS: case Opcode::AND_FLAGS: {
            uint64_t res = (op.opcode == Opcode::SUB_FLAGS) ? a - b :
                           (op.opcode == Opcode::AND_FLAGS) ? a & b : a + b;
            ctx.flags[0] = (op.opcode == Opcode::SUB_FLAGS) ? (a < b ? 1 : 0) :
                           (op.opcode == Opcode::ADD_FLAGS) ? (res < a ? 1 : 0) : 0; // CF
            ctx.flags[1] = (uint8_t)(popcnt64(res & 0xFF) % 2 == 0 ? 1 : 0); // PF
            ctx.flags[2] = (res == 0) ? 1 : 0; // ZF
            ctx.flags[3] = (res >> 63) ? 1 : 0; // SF
            if (op.opcode == Opcode::SUB_FLAGS) ctx.flags[4] = (((a ^ b) & (a ^ res)) >> 63) ? 1 : 0; // OF
            else if (op.opcode == Opcode::ADD_FLAGS) ctx.flags[4] = (((~(a ^ b)) & (a ^ res)) >> 63) ? 1 : 0;
            else ctx.flags[4] = 0;
            break;
        }
        case Opcode::GET_CF: write_var(op.output, ctx.flags[0]); break;
        case Opcode::GET_ZF: write_var(op.output, ctx.flags[2]); break;
        case Opcode::GET_SF: write_var(op.output, ctx.flags[3]); break;
        case Opcode::GET_OF: write_var(op.output, ctx.flags[4]); break;
        case Opcode::GET_PF: write_var(op.output, ctx.flags[1]); break;
        case Opcode::GET_DF: write_var(op.output, ctx.flags[6]); break;
        case Opcode::SET_CF: ctx.flags[0] = (uint8_t)(a & 1); break;
        case Opcode::SET_ZF: ctx.flags[2] = (uint8_t)(a & 1); break;
        case Opcode::SET_SF: ctx.flags[3] = (uint8_t)(a & 1); break;
        case Opcode::SET_OF: ctx.flags[4] = (uint8_t)(a & 1); break;
        case Opcode::SET_PF: ctx.flags[1] = (uint8_t)(a & 1); break;
        case Opcode::SET_DF: ctx.flags[6] = (uint8_t)(a & 1); break;
        case Opcode::LOAD:
            if (ctx.memory) {
                uint64_t addr = a;
                if (addr + op.output.size <= ctx.memory_size) {
                    uint64_t val = 0;
                    memcpy(&val, ctx.memory + addr, op.output.size < 8 ? op.output.size : 8);
                    write_var(op.output, val);
                }
            }
            break;
        case Opcode::STORE:
            if (ctx.memory) {
                uint64_t addr = a;
                uint64_t val = b;
                uint8_t sz = op.output.size < 8 ? op.output.size : 8;
                if (addr + sz <= ctx.memory_size)
                    memcpy(ctx.memory + addr, &val, sz);
            }
            break;
        case Opcode::BRANCH: ctx.rip = a; return;
        case Opcode::RET: ctx.rip = a; return;
        case Opcode::CALL: ctx.rip = a; return;
        case Opcode::CBRANCH: if (a) { ctx.rip = b; return; } break;
        case Opcode::FADD: case Opcode::FSUB: case Opcode::FMUL: case Opcode::FDIV:
        case Opcode::FSQRT: case Opcode::FMIN: case Opcode::FMAX:
            write_var(op.output, a); break; // placeholder for float ops
        case Opcode::CMP_EQ: write_var(op.output, a == b ? 1 : 0); break;
        case Opcode::CMP_NE: write_var(op.output, a != b ? 1 : 0); break;
        case Opcode::CMP_ULT: write_var(op.output, a < b ? 1 : 0); break;
        case Opcode::CMP_SLT: write_var(op.output, (int64_t)a < (int64_t)b ? 1 : 0); break;
        case Opcode::CMP_ULE: write_var(op.output, a <= b ? 1 : 0); break;
        case Opcode::CMP_SLE: write_var(op.output, (int64_t)a <= (int64_t)b ? 1 : 0); break;
        case Opcode::SELECT: { uint64_t c = read_var(op.inputs[2]); write_var(op.output, a ? b : c); break; }
        case Opcode::BITSEL: { uint64_t c = read_var(op.inputs[2]); write_var(op.output, (b & a) | (c & ~a)); break; }
        case Opcode::LEA: write_var(op.output, a); break;
        case Opcode::MOD: write_var(op.output, b ? a % b : 0); break;
        case Opcode::SMOD: write_var(op.output, b ? (uint64_t)((int64_t)a % (int64_t)b) : 0); break;
        case Opcode::BSWAP: {
            uint8_t sz = op.output.size;
            uint64_t r = 0;
            for (uint8_t i = 0; i < sz; i++) r |= ((a >> (i*8)) & 0xFF) << ((sz-1-i)*8);
            write_var(op.output, r); break;
        }
        case Opcode::BITREV: {
            uint8_t sz = op.output.size; uint64_t r = 0;
            for (uint8_t i = 0; i < sz*8; i++) if ((a >> i) & 1) r |= 1ULL << (sz*8-1-i);
            write_var(op.output, r); break;
        }
        case Opcode::TEST: {
            uint64_t res = a & b;
            ctx.flags[0] = 0; ctx.flags[4] = 0;
            ctx.flags[1] = (uint8_t)(popcnt64(res & 0xFF) % 2 == 0 ? 1 : 0);
            ctx.flags[2] = (res == 0) ? 1 : 0;
            ctx.flags[3] = (res >> 63) ? 1 : 0;
            break;
        }
        case Opcode::EXTRACT: {
            uint8_t lo = (uint8_t)b; uint8_t wid = (uint8_t)read_var(op.inputs[2]);
            uint64_t mask = wid >= 64 ? ~0ULL : ((1ULL << wid) - 1);
            write_var(op.output, (a >> lo) & mask); break;
        }
        case Opcode::INSERT: {
            uint64_t pack = read_var(op.inputs[2]);
            uint8_t lo = (uint8_t)(pack & 0xFF); uint8_t wid = (uint8_t)((pack >> 8) & 0xFF);
            uint64_t mask = wid >= 64 ? ~0ULL : ((1ULL << wid) - 1);
            write_var(op.output, (a & ~(mask << lo)) | ((b & mask) << lo)); break;
        }
        case Opcode::CONCAT: write_var(op.output, (a << (op.inputs[1].size*8)) | b); break;
        case Opcode::BITCAST: write_var(op.output, a); break;
        case Opcode::I2F: case Opcode::F2I: case Opcode::F2F:
            write_var(op.output, a); break;
        case Opcode::INDIRECT_JMP: ctx.rip = a; return;
        case Opcode::VCALL: ctx.rip = a; return;
        case Opcode::UD2: ctx.halted = true; return;
        case Opcode::RDTSC: write_var(op.output, 0); break;
        case Opcode::SYSCALL: break; // host-specific, no-op in pure interp
        case Opcode::VADD: case Opcode::VSUB: case Opcode::VMUL:
        case Opcode::VAND: case Opcode::VOR: case Opcode::VXOR:
        case Opcode::VSHL: case Opcode::VSHR: case Opcode::VCMP:
        case Opcode::VEXTRACT_ELEM: case Opcode::VINSERT_ELEM: case Opcode::VBROADCAST:
        case Opcode::ARCH_X64:
            write_var(op.output, a); break; // placeholder
        default: break;
        }
    }
    ctx.rip = lifted.address + lifted.length;
}

#ifdef VEDX64_STRINGS
#include <string>
#include <cstdio>

const char* opcode_name(Opcode opc) {
    switch (opc) {
    case Opcode::COPY: return "COPY";
    case Opcode::LOAD: return "LOAD";
    case Opcode::STORE: return "STORE";
    case Opcode::LEA: return "LEA";
    case Opcode::ADD: return "ADD";
    case Opcode::SUB: return "SUB";
    case Opcode::MUL: return "MUL";
    case Opcode::IMUL: return "IMUL";
    case Opcode::DIV: return "DIV";
    case Opcode::IDIV: return "IDIV";
    case Opcode::NEG: return "NEG";
    case Opcode::MOD: return "MOD";
    case Opcode::SMOD: return "SMOD";
    case Opcode::AND: return "AND";
    case Opcode::OR: return "OR";
    case Opcode::XOR: return "XOR";
    case Opcode::NOT: return "NOT";
    case Opcode::SHL: return "SHL";
    case Opcode::SHR: return "SHR";
    case Opcode::SAR: return "SAR";
    case Opcode::ROL: return "ROL";
    case Opcode::ROR: return "ROR";
    case Opcode::BSWAP: return "BSWAP";
    case Opcode::BITREV: return "BITREV";
    case Opcode::TEST: return "TEST";
    case Opcode::EXTRACT: return "EXTRACT";
    case Opcode::INSERT: return "INSERT";
    case Opcode::CONCAT: return "CONCAT";
    case Opcode::BITCAST: return "BITCAST";
    case Opcode::CMP_EQ: return "CMP_EQ";
    case Opcode::CMP_NE: return "CMP_NE";
    case Opcode::CMP_SLT: return "CMP_SLT";
    case Opcode::CMP_ULT: return "CMP_ULT";
    case Opcode::CMP_SLE: return "CMP_SLE";
    case Opcode::CMP_ULE: return "CMP_ULE";
    case Opcode::SELECT: return "SELECT";
    case Opcode::BITSEL: return "BITSEL";
    case Opcode::ZEXT: return "ZEXT";
    case Opcode::SEXT: return "SEXT";
    case Opcode::TRUNC: return "TRUNC";
    case Opcode::I2F: return "I2F";
    case Opcode::F2I: return "F2I";
    case Opcode::F2F: return "F2F";
    case Opcode::ADD_FLAGS: return "ADD_FLAGS";
    case Opcode::SUB_FLAGS: return "SUB_FLAGS";
    case Opcode::AND_FLAGS: return "AND_FLAGS";
    case Opcode::GET_CF: return "GET_CF";
    case Opcode::GET_ZF: return "GET_ZF";
    case Opcode::GET_SF: return "GET_SF";
    case Opcode::GET_OF: return "GET_OF";
    case Opcode::GET_PF: return "GET_PF";
    case Opcode::GET_DF: return "GET_DF";
    case Opcode::SET_CF: return "SET_CF";
    case Opcode::SET_ZF: return "SET_ZF";
    case Opcode::SET_SF: return "SET_SF";
    case Opcode::SET_OF: return "SET_OF";
    case Opcode::SET_PF: return "SET_PF";
    case Opcode::SET_DF: return "SET_DF";
    case Opcode::BRANCH: return "BRANCH";
    case Opcode::CBRANCH: return "CBRANCH";
    case Opcode::INDIRECT_JMP: return "INDIRECT_JMP";
    case Opcode::CALL: return "CALL";
    case Opcode::VCALL: return "VCALL";
    case Opcode::RET: return "RET";
    case Opcode::POPCNT: return "POPCNT";
    case Opcode::CTZ: return "CTZ";
    case Opcode::CLZ: return "CLZ";
    case Opcode::FADD: return "FADD";
    case Opcode::FSUB: return "FSUB";
    case Opcode::FMUL: return "FMUL";
    case Opcode::FDIV: return "FDIV";
    case Opcode::FSQRT: return "FSQRT";
    case Opcode::FMIN: return "FMIN";
    case Opcode::FMAX: return "FMAX";
    case Opcode::VADD: return "VADD";
    case Opcode::VSUB: return "VSUB";
    case Opcode::VMUL: return "VMUL";
    case Opcode::VAND: return "VAND";
    case Opcode::VOR: return "VOR";
    case Opcode::VXOR: return "VXOR";
    case Opcode::VSHL: return "VSHL";
    case Opcode::VSHR: return "VSHR";
    case Opcode::VCMP: return "VCMP";
    case Opcode::VEXTRACT_ELEM: return "VEXTRACT_ELEM";
    case Opcode::VINSERT_ELEM: return "VINSERT_ELEM";
    case Opcode::VBROADCAST: return "VBROADCAST";
    case Opcode::RDTSC: return "RDTSC";
    case Opcode::SYSCALL: return "SYSCALL";
    case Opcode::UD2: return "UD2";
    case Opcode::NOP: return "NOP";
    case Opcode::UNDEF: return "UNDEF";
    case Opcode::BARRIER: return "BARRIER";
    case Opcode::ARCH_X64: return "ARCH_X64";
    default: return "???";
    }
}

std::string varnode_str(const VarNode& v) {
    char buf[64];
    switch (v.space) {
    case Space::Const: snprintf(buf, sizeof(buf), "#%lld", (long long)v.value); return buf;
    case Space::Temp: snprintf(buf, sizeof(buf), "t%d:%d", v.offset, v.size); return buf;
    case Space::GPR: {
        static const char* names[] = {"RAX","RCX","RDX","RBX","RSP","RBP","RSI","RDI","R8","R9","R10","R11","R12","R13","R14","R15"};
        const char* name = (v.offset < 16) ? names[v.offset] : "R??";
        if (v.size == 8) return name;
        snprintf(buf, sizeof(buf), "%s:%d", name, v.size); return buf;
    }
    case Space::Flags: return "FLAGS";
    case Space::XMM: snprintf(buf, sizeof(buf), "XMM%d:%d", v.offset, v.size); return buf;
    case Space::MMX: snprintf(buf, sizeof(buf), "MMX%d", v.offset); return buf;
    case Space::Seg: snprintf(buf, sizeof(buf), "SEG%d", v.offset); return buf;
    case Space::RAM: snprintf(buf, sizeof(buf), "[%d]", v.size); return buf;
    case Space::OpMask: snprintf(buf, sizeof(buf), "K%d:%d", v.offset, v.size); return buf;
    default: return "???";
    }
}

std::string op_str(const Op& op) {
    std::string s = opcode_name(op.opcode);
    // For ops with output (most ops)
    bool has_output = (op.output.space != Space::Const || op.output.size != 0);
    if (has_output) s += " " + varnode_str(op.output);
    for (uint8_t i = 0; i < op.num_inputs; i++) {
        s += (has_output || i > 0) ? ", " : " ";
        s += varnode_str(op.inputs[i]);
    }
    return s;
}

const char* rep_mode_str(RepMode r) {
    switch (r) {
    case RepMode::None: return nullptr;
    case RepMode::Rep: return "REP (loop RCX times)";
    case RepMode::RepZ: return "REPZ (loop while RCX>0 && ZF=1)";
    case RepMode::RepNZ: return "REPNZ (loop while RCX>0 && ZF=0)";
    default: return "???";
    }
}

void dump(const Lifted& lifted) {
    if (lifted.rep != RepMode::None)
        printf("    ; %s\n", rep_mode_str(lifted.rep));
    for (size_t i = 0; i < lifted.ops.size(); i++)
        printf("    %s\n", op_str(lifted.ops[i]).c_str());
}
#endif // VEDX64_STRINGS

} // namespace ir
} // namespace vedx64

#include "vedx64/codegen.hpp"
namespace vedx64 { namespace ir {
static inline Reg ir_gpr(const VarNode& v) { return Reg{(uint8_t)(v.offset & 15), (uint16_t)(v.size*8)}; }
bool emit(const Op& op, CodeGen& cg) {
    auto is_gpr_reg = [](const VarNode& v) { return v.space == Space::GPR && (v.size==1||v.size==2||v.size==4||v.size==8); };
    switch (op.opcode) {
    case Opcode::NOP:   cg.nop(); return true;
    case Opcode::RET:   cg.ret(); return true;
    case Opcode::UD2:   cg.db(0x0F).db(0x0B); return true;
    case Opcode::SYSCALL: cg.db(0x0F).db(0x05); return true;
    case Opcode::RDTSC:   cg.db(0x0F).db(0x31); return true;
    case Opcode::BARRIER: return true; // abstract fence, no emit
    case Opcode::COPY: {
        if (is_gpr_reg(op.output) && is_gpr_reg(op.inputs[0]) && op.output.size == op.inputs[0].size) {
            cg.mov(ir_gpr(op.output), ir_gpr(op.inputs[0])); return true;
        }
        if (is_gpr_reg(op.output) && op.inputs[0].space == Space::Const) {
            cg.mov(ir_gpr(op.output), (int64_t)op.inputs[0].value); return true;
        }
        return false;
    }
    case Opcode::ADD:
    case Opcode::SUB:
    case Opcode::AND:
    case Opcode::OR:
    case Opcode::XOR: {
        if (!is_gpr_reg(op.output) || !is_gpr_reg(op.inputs[0]) || op.num_inputs < 2) return false;
        if (op.output.offset != op.inputs[0].offset || op.output.size != op.inputs[0].size) return false;
        Reg d = ir_gpr(op.output);
        if (is_gpr_reg(op.inputs[1]) && op.inputs[1].size == op.output.size) {
            Reg s = ir_gpr(op.inputs[1]);
            switch (op.opcode) {
            case Opcode::ADD: cg.add(d, s); return true;
            case Opcode::SUB: cg.sub(d, s); return true;
            case Opcode::AND: cg.and_(d, s); return true;
            case Opcode::OR:  cg.or_(d, s); return true;
            case Opcode::XOR: cg.xor_(d, s); return true;
            default: return false;
            }
        }
        return false;
    }
    default: return false;
    }
}
} }
#endif // VEDX64_IR
