// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/branch_resolve.hpp"
#include "vedx64/analysis.hpp"
#include "vedx64/branch_follow.hpp"
#include <cstring>

namespace vedx64 {
namespace branch {

using M = Mnemonic;

namespace {

// Decode one instruction at `addr`, pulling bytes via read_code. We try the
// maximum legal length (15) and shrink so a short instruction near the end of a
// mapped range still decodes. Returns the instruction length, or 0.
size_t fill_decode(uint64_t addr, const ReadCode& rc, DecodedInstr& di) {
    uint8_t buf[16];
    for (size_t n = 15; n >= 1; --n) {
        if (rc(addr, buf, n)) return decode(buf, n, di);
    }
    return 0;
}

// Register-number reconstruction (low 3 bits + REX/REX2 high bits).
uint8_t reg_from_opcode(const DecodedInstr& di) {
    uint8_t r = di.opcode_reg;
    if (di.rex & 0x1) r += 8;
    if (di.has_rex2 && di.rex2_b4) r += 16;
    return r;
}
uint8_t reg_from_rm(const DecodedInstr& di) {
    uint8_t r = di.modrm & 7;
    if (di.rex & 0x1) r += 8;
    if (di.has_rex2 && di.rex2_b4) r += 16;
    return r;
}
uint8_t reg_from_reg(const DecodedInstr& di) {
    uint8_t r = (di.modrm >> 3) & 7;
    if (di.rex & 0x4) r += 8;
    if (di.has_rex2 && di.rex2_r4) r += 16;
    return r;
}

// Build an effective-address description from a decoded memory operand.
MemRef build_memref(const DecodedInstr& di, uint64_t insn_addr) {
    MemRef m;
    for (int i = 0; i < 4 && di.legacy_prefix[i]; ++i) {
        uint8_t pfx = di.legacy_prefix[i];
        if (pfx == 0x2E || pfx == 0x36 || pfx == 0x3E ||
            pfx == 0x26 || pfx == 0x64 || pfx == 0x65)
            m.segment = pfx;
    }
    uint8_t mod = di.modrm >> 6;
    uint8_t rm  = di.modrm & 7;
    m.disp = di.displacement;
    if (rm == 4) {
        // SIB byte present.
        uint8_t idx  = (di.sib >> 3) & 7;
        uint8_t base = di.sib & 7;
        m.scale = (uint8_t)(1u << (di.sib >> 6));
        if (!(idx == 4 && !(di.rex & 0x2)))
            m.index = idx | ((di.rex & 0x2) ? 8 : 0);
        if (mod == 0 && base == 5) {
            // No base register: pure disp32 (absolute) form.
            m.base = 0xFF;
            m.static_addr = (m.index == 0xFF);
            m.abs_addr = (uint64_t)m.disp;
        } else {
            m.base = base | ((di.rex & 0x1) ? 8 : 0);
        }
    } else if (mod == 0 && rm == 5) {
        // RIP-relative.
        m.rip_relative = true;
        m.base = 0xFF;
        m.static_addr = true;
        m.abs_addr = insn_addr + di.length + (uint64_t)m.disp;
    } else {
        m.base = rm | ((di.rex & 0x1) ? 8 : 0);
    }
    return m;
}

// --- setup-op detectors (return true and fill outputs) ---

bool mov_reg_imm(const DecodedInstr& di, uint8_t* reg, uint64_t* val) {
    if (!di.desc || di.desc->mnemonic != M::MOV) return false;
    const auto& ops = di.desc->operands;
    bool has_imm = false;
    for (int i = 0; i < di.desc->num_operands; ++i)
        if (ops[i].addr == AddrMode::Immediate) has_imm = true;
    if (!has_imm) return false;
    bool rexw = (di.rex & 0x8) != 0;
    if (ops[0].addr == AddrMode::OpcodeReg) {
        *reg = reg_from_opcode(di);
    } else if (ops[0].addr == AddrMode::ModRM_RM && (di.modrm >> 6) == 3) {
        *reg = reg_from_rm(di);
    } else {
        return false;
    }
    // movabs (8-byte imm) keeps the full value; 32-bit dst zero-extends; a
    // REX.W mov r64,imm32 sign-extends (the decoder already did that).
    *val = rexw ? (uint64_t)di.immediate : (uint32_t)(uint64_t)di.immediate;
    return true;
}

bool lea_rip(const DecodedInstr& di, uint64_t insn_addr, uint8_t* reg, uint64_t* val) {
    if (!di.desc || di.desc->mnemonic != M::LEA) return false;
    uint8_t mod = di.modrm >> 6, rm = di.modrm & 7;
    if (!(mod == 0 && rm == 5)) return false;   // only RIP-relative folds to a constant
    *reg = reg_from_reg(di);
    *val = insn_addr + di.length + (uint64_t)di.displacement;
    return true;
}

bool mov_reg_mem(const DecodedInstr& di, uint64_t insn_addr,
                 uint8_t* reg, MemRef* mr, uint8_t* psize) {
    if (!di.desc || di.desc->mnemonic != M::MOV) return false;
    const auto& ops = di.desc->operands;
    if (ops[0].addr == AddrMode::ModRM_Reg && ops[1].addr == AddrMode::ModRM_RM &&
        (di.modrm >> 6) != 3) {
        *reg = reg_from_reg(di);
        *mr = build_memref(di, insn_addr);
        *psize = (di.rex & 0x8) ? 8 : 4;
        return true;
    }
    return false;
}

// Does any operand use this addressing mode? (PUSH models the stack as a
// separate operand, so its imm/reg operand is not necessarily index 0.)
bool has_addr(const DecodedInstr& di, AddrMode a) {
    if (!di.desc) return false;
    for (int i = 0; i < di.desc->num_operands; ++i)
        if (di.desc->operands[i].addr == a) return true;
    return false;
}

bool push_imm(const DecodedInstr& di, uint64_t* sext, uint32_t* low) {
    if (!di.desc || di.desc->mnemonic != M::PUSH) return false;
    if (!has_addr(di, AddrMode::Immediate)) return false;
    *sext = (uint64_t)di.immediate;
    *low  = (uint32_t)(uint64_t)di.immediate;
    return true;
}

bool push_reg(const DecodedInstr& di, uint8_t* reg) {
    if (!di.desc || di.desc->mnemonic != M::PUSH) return false;
    if (!has_addr(di, AddrMode::OpcodeReg)) return false;
    *reg = reg_from_opcode(di);
    return true;
}

// mov dword [rsp+4], imm32  — the high half of a split 64-bit push/ret target.
bool mov_rsp4_imm(const DecodedInstr& di, uint32_t* high) {
    if (!di.desc || di.desc->mnemonic != M::MOV) return false;
    const auto& ops = di.desc->operands;
    if (!(ops[0].addr == AddrMode::ModRM_RM && (di.modrm >> 6) != 3)) return false;
    bool has_imm = false;
    for (int i = 0; i < di.desc->num_operands; ++i)
        if (ops[i].addr == AddrMode::Immediate) has_imm = true;
    if (!has_imm) return false;
    MemRef m = build_memref(di, 0);
    if (m.base == 4 && m.index == 0xFF && m.disp == 4) {  // [rsp+4]
        *high = (uint32_t)(uint64_t)di.immediate;
        return true;
    }
    return false;
}

bool arith_imm(const DecodedInstr& di, uint8_t* reg, M* mn, int64_t* imm) {
    if (!di.desc) return false;
    M m = di.desc->mnemonic;
    if (!(m == M::ADD || m == M::SUB || m == M::OR || m == M::XOR ||
          m == M::AND || m == M::SHL || m == M::SHR || m == M::SAR))
        return false;
    const auto& ops = di.desc->operands;
    if (!(ops[0].addr == AddrMode::ModRM_RM && (di.modrm >> 6) == 3)) return false;
    bool has_imm = false;
    for (int i = 0; i < di.desc->num_operands; ++i)
        if (ops[i].addr == AddrMode::Immediate) has_imm = true;
    if (!has_imm) return false;
    *reg = reg_from_rm(di);
    *mn = m;
    *imm = di.immediate;
    return true;
}

void apply_arith(M m, uint64_t* v, int64_t imm) {
    switch (m) {
    case M::ADD: *v += (uint64_t)imm; break;
    case M::SUB: *v -= (uint64_t)imm; break;
    case M::OR:  *v |= (uint64_t)imm; break;
    case M::XOR: *v ^= (uint64_t)imm; break;
    case M::AND: *v &= (uint64_t)imm; break;
    case M::SHL: *v <<= (imm & 63);   break;
    case M::SHR: *v >>= (imm & 63);   break;
    case M::SAR: *v = (uint64_t)((int64_t)*v >> (imm & 63)); break;
    default: break;
    }
}

// Terminating jmp/call reg. Returns 0 (none), 1 (jmp), 2 (call); fills reg.
int branch_reg(const DecodedInstr& di, uint8_t* reg) {
    if (!di.desc) return 0;
    M m = di.desc->mnemonic;
    if (!(m == M::JMP || m == M::CALL)) return 0;
    if (!analysis::is_indirect_branch(di)) return 0;
    auto info = analysis::indirect_branch_info(di);
    if (info.is_mem) return 0;
    *reg = info.reg_id;
    return (m == M::CALL) ? 2 : 1;
}

bool is_retn(const DecodedInstr& di) {
    return di.desc && di.desc->mnemonic == M::RETN;
}

} // anonymous namespace

bool resolve_target(BranchPattern& p, const ReadMem& read_mem, const Options& opt) {
    if (!read_mem) return false;
    if (!opt.follow_memory) return false;
    if (p.resolution != Resolution::MemoryPointer) return false;
    if (!p.slot.static_addr) return false;
    uint8_t ps = p.ptr_size ? p.ptr_size : 8;
    if (ps > 8) ps = 8;
    uint8_t b[8] = {0};
    if (!read_mem(p.slot.abs_addr, b, ps)) return false;
    uint64_t v = 0;
    std::memcpy(&v, b, ps);
    p.final_target = v;
    p.resolved = true;
    p.chain_depth = 0;   // one level only; chain chasing needs read_code (see header).
    return true;
}

BranchPattern recognize(uint64_t address, const ReadCode& read_code,
                        const Options& opt, const ReadMem& read_mem) {
    BranchPattern p;
    p.address = address;

    DecodedInstr di0;
    size_t l0 = fill_decode(address, read_code, di0);
    if (l0 == 0 || !di0.desc) return p;   // valid stays false

    // ===================== multi-instruction idioms =====================

    // push imm ; ret      and      push imm32 ; mov [rsp+4],imm32 ; ret
    {
        uint64_t sext; uint32_t low;
        if (push_imm(di0, &sext, &low)) {
            DecodedInstr di1; size_t l1 = fill_decode(address + l0, read_code, di1);
            if (l1) {
                if (is_retn(di1)) {
                    p.valid = true; p.effect = Effect::Jump; p.form = Form::PushImmRet;
                    p.resolution = Resolution::Concrete; p.target = sext;
                    p.has_fallthrough = false;
                    p.insn_count = 2; p.insn_len[0] = (uint8_t)l0; p.insn_len[1] = (uint8_t)l1;
                    p.total_length = (uint32_t)(l0 + l1);
                    return p;
                }
                uint32_t high;
                if (mov_rsp4_imm(di1, &high)) {
                    DecodedInstr di2; size_t l2 = fill_decode(address + l0 + l1, read_code, di2);
                    if (l2 && is_retn(di2)) {
                        p.valid = true; p.effect = Effect::Jump; p.form = Form::PushSplitImm64Ret;
                        p.resolution = Resolution::Concrete;
                        p.target = (uint64_t)low | ((uint64_t)high << 32);
                        p.has_fallthrough = false;
                        p.insn_count = 3;
                        p.insn_len[0] = (uint8_t)l0; p.insn_len[1] = (uint8_t)l1; p.insn_len[2] = (uint8_t)l2;
                        p.total_length = (uint32_t)(l0 + l1 + l2);
                        return p;
                    }
                }
            }
        }
    }

    // push reg ; ret
    {
        uint8_t r;
        if (push_reg(di0, &r)) {
            DecodedInstr di1; size_t l1 = fill_decode(address + l0, read_code, di1);
            if (l1 && is_retn(di1)) {
                p.valid = true; p.effect = Effect::Jump; p.form = Form::PushRegRet;
                p.resolution = Resolution::RegisterDynamic; p.reg = r;
                p.has_fallthrough = false;
                p.insn_count = 2; p.insn_len[0] = (uint8_t)l0; p.insn_len[1] = (uint8_t)l1;
                p.total_length = (uint32_t)(l0 + l1);
                return p;
            }
        }
    }

    // mov/lea/load reg ; [imm-arith on reg]* ; (jmp/call reg | push reg ; ret)
    {
        enum Kind { K_NONE, K_CONST, K_LEA, K_MEM } kind = K_NONE;
        uint8_t treg = 0xFF; uint64_t val = 0; MemRef mr; uint8_t psize = 8; bool arith = false;
        uint8_t r; uint64_t v; MemRef m; uint8_t ps;
        if (mov_reg_imm(di0, &r, &v))                       { kind = K_CONST; treg = r; val = v; }
        else if (lea_rip(di0, address, &r, &v))             { kind = K_LEA;   treg = r; val = v; }
        else if (mov_reg_mem(di0, address, &r, &m, &ps))    { kind = K_MEM;   treg = r; mr = m; psize = ps; }

        if (kind != K_NONE) {
            BranchPattern ip; ip.address = address;
            ip.insn_len[0] = (uint8_t)l0;
            uint8_t cnt = 1; uint64_t off = l0;
            for (uint8_t i = 1; i < opt.max_idiom_insns && i < 6; ++i) {
                DecodedInstr di; size_t li = fill_decode(address + off, read_code, di);
                if (li == 0) break;

                uint8_t br; int bt = branch_reg(di, &br);
                if (bt && br == treg) {
                    if (cnt < 6) ip.insn_len[cnt] = (uint8_t)li;
                    cnt++; off += li;
                    ip.valid = true;
                    ip.effect = (bt == 2) ? Effect::Call : Effect::Jump;
                    ip.pushes_return = (bt == 2);
                    ip.has_fallthrough = (bt == 2);
                    if (kind == K_MEM) {
                        ip.resolution = Resolution::MemoryPointer; ip.form = Form::LoadMemBranch;
                        ip.slot = mr; ip.ptr_size = psize;
                    } else {
                        ip.resolution = Resolution::Concrete; ip.target = val;
                        ip.form = (kind == K_LEA) ? Form::LeaRegBranch
                                : (arith ? Form::ComputedRegBranch : Form::MovRegBranch);
                    }
                    ip.insn_count = cnt; ip.total_length = (uint32_t)off;
                    break;
                }

                uint8_t pr;
                if (push_reg(di, &pr) && pr == treg) {
                    DecodedInstr dr; size_t lr = fill_decode(address + off + li, read_code, dr);
                    if (lr && is_retn(dr)) {
                        if (cnt < 6) ip.insn_len[cnt] = (uint8_t)li; cnt++;
                        if (cnt < 6) ip.insn_len[cnt] = (uint8_t)lr; cnt++;
                        off += li + lr;
                        ip.valid = true; ip.effect = Effect::Jump; ip.has_fallthrough = false;
                        ip.form = Form::MovLeaPushRet;
                        if (kind == K_MEM) {
                            ip.resolution = Resolution::MemoryPointer; ip.slot = mr; ip.ptr_size = psize;
                        } else {
                            ip.resolution = Resolution::Concrete; ip.target = val;
                        }
                        ip.insn_count = cnt; ip.total_length = (uint32_t)off;
                    }
                    break;
                }

                uint8_t ar; M am; int64_t aimm;
                if (opt.fold_arith && kind == K_CONST && arith_imm(di, &ar, &am, &aimm) && ar == treg) {
                    apply_arith(am, &val, aimm); arith = true;
                    if (cnt < 6) ip.insn_len[cnt] = (uint8_t)li;
                    cnt++; off += li;
                    continue;
                }

                // Any other instruction: not a contiguous idiom.
                break;
            }
            if (ip.valid) {
                if (ip.resolution == Resolution::MemoryPointer) resolve_target(ip, read_mem, opt);
                return ip;
            }
        }
    }

    // ===================== single-instruction forms =====================

    M m0 = di0.desc->mnemonic;
    p.insn_count = 1; p.insn_len[0] = (uint8_t)l0; p.total_length = (uint32_t)l0;

    // Count-conditional (LOOP/LOOPE/LOOPNE/JrCXZ): rel8, but not covered by
    // find_relative_target (it isn't a Jcc/JMP), so compute the target directly.
    if (analysis::is_count_conditional_branch(m0)) {
        p.valid = true; p.form = Form::CountConditional; p.effect = Effect::ConditionalJump;
        p.is_conditional = true; p.has_fallthrough = true; p.resolution = Resolution::Concrete;
        p.target = address + l0 + (uint64_t)di0.immediate;
        return p;
    }

    // Direct relative jump / conditional / call.
    uint64_t tgt;
    if ((analysis::is_relative_branch(m0) || analysis::is_call(m0)) &&
        analysis::find_relative_target(di0, address, &tgt)) {
        p.valid = true; p.form = Form::RelNear; p.resolution = Resolution::Concrete; p.target = tgt;
        if (analysis::is_call(m0)) {
            p.effect = Effect::Call; p.pushes_return = true; p.has_fallthrough = true;
        } else if (analysis::is_jcc(m0)) {
            p.effect = Effect::ConditionalJump; p.is_conditional = true; p.has_fallthrough = true;
        } else {
            p.effect = Effect::Jump; p.has_fallthrough = false;
        }
        return p;
    }

    // Far direct (EA/9A) and far indirect (FF /5, FF /3).
    if (m0 == M::JMPF || m0 == M::CALLF) {
        p.valid = true; p.is_far = true;
        p.effect = (m0 == M::CALLF) ? Effect::Call : Effect::Jump;
        p.pushes_return = (m0 == M::CALLF);
        p.has_fallthrough = (m0 == M::CALLF);
        if (di0.desc->has_modrm) {
            p.form = Form::FarIndirect; p.resolution = Resolution::MemoryPointer;
            p.slot = build_memref(di0, address); p.ptr_size = 8;
            resolve_target(p, read_mem, opt);
        } else {
            p.form = Form::FarDirect; p.resolution = Resolution::FarPointer;
            p.target = (uint64_t)(uint32_t)di0.immediate;
            p.selector = (uint16_t)(di0.displacement & 0xFFFF);
        }
        return p;
    }

    // Near indirect jmp/call (FF /4, FF /2).
    if ((analysis::is_unconditional_branch(m0) || analysis::is_call(m0)) &&
        analysis::is_indirect_branch(di0)) {
        auto info = analysis::indirect_branch_info(di0);
        p.valid = true;
        p.effect = analysis::is_call(m0) ? Effect::Call : Effect::Jump;
        p.pushes_return = analysis::is_call(m0);
        p.has_fallthrough = analysis::is_call(m0);
        if (info.is_mem) {
            p.form = Form::IndirectMem; p.resolution = Resolution::MemoryPointer;
            p.slot = build_memref(di0, address); p.ptr_size = 8;
            resolve_target(p, read_mem, opt);
        } else {
            p.form = Form::IndirectReg; p.resolution = Resolution::RegisterDynamic;
            p.reg = info.reg_id;
        }
        return p;
    }

    // Returns (near / far / interrupt-return).
    if (analysis::is_ret(m0)) {
        p.valid = true; p.effect = Effect::Return; p.has_fallthrough = false;
        p.resolution = Resolution::StackDynamic;
        if (m0 == M::RETN)       p.form = Form::ReturnNear;
        else if (m0 == M::RETF) { p.form = Form::ReturnFar; p.is_far = true; }
        else                     p.form = Form::ReturnInterrupt;
        return p;
    }

    // Interrupts / undefined (INT/INT1/INTO/UD/UD2). IRET* already handled above.
    if (analysis::is_int_or_ud(m0)) {
        p.valid = true; p.form = Form::Interrupt; p.effect = Effect::Interrupt;
        p.resolution = Resolution::Unknown;
        p.has_fallthrough = !(m0 == M::UD || m0 == M::UD2);
        return p;
    }

    // Syscall family.
    if (m0 == M::SYSCALL || m0 == M::SYSRET || m0 == M::SYSENTER || m0 == M::SYSEXIT) {
        p.valid = true; p.form = Form::Syscall; p.effect = Effect::SysTransfer;
        p.resolution = Resolution::Unknown; p.has_fallthrough = true;
        return p;
    }

    p.valid = false;
    return p;
}

BranchPattern recognize(const uint8_t* code, size_t len, uint64_t address,
                        const Options& opt, const ReadMem& read_mem) {
    ReadCode rc = [code, len, address](uint64_t a, uint8_t* o, size_t n) -> bool {
        if (a < address) return false;
        uint64_t off = a - address;
        if (off > len || off + n > len) return false;
        std::memcpy(o, code + off, n);
        return true;
    };
    return recognize(address, rc, opt, read_mem);
}

} // namespace branch
} // namespace vedx64
