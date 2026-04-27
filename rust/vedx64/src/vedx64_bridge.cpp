// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64_bridge.hpp"
#include "vedx64/src/bridge.rs.h"

namespace vedx64 {
namespace bridge {

std::unique_ptr<Decoded> decode(rust::Slice<const uint8_t> code) {
    auto d = std::make_unique<Decoded>();
    if (vedx64::decode(code.data(), code.size(), d->di) == 0) return nullptr;
    return d;
}
rust::Vec<uint8_t> encode(const Decoded& d) {
    uint8_t buf[15];
    size_t n = vedx64::encode(d.di, buf, sizeof(buf));
    rust::Vec<uint8_t> out; out.reserve(n);
    for (size_t i = 0; i < n; ++i) out.push_back(buf[i]);
    return out;
}
uint8_t  decoded_length(const Decoded& d) { return d.di.length; }
uint16_t decoded_mnemonic(const Decoded& d) { return d.di.desc ? static_cast<uint16_t>(d.di.desc->mnemonic) : 0; }
uint8_t  decoded_num_operands(const Decoded& d) { return d.di.desc ? d.di.desc->num_operands : 0; }
uint8_t  decoded_rex(const Decoded& d) { return d.di.rex; }
uint8_t  decoded_modrm(const Decoded& d) { return d.di.modrm; }
uint8_t  decoded_sib(const Decoded& d) { return d.di.sib; }
int64_t  decoded_displacement(const Decoded& d) { return d.di.displacement; }
int64_t  decoded_immediate(const Decoded& d) { return d.di.immediate; }
bool     decoded_has_vex(const Decoded& d) { return d.di.has_vex; }
uint8_t  decoded_vex_vvvv(const Decoded& d) { return d.di.vex_vvvv; }
uint8_t  decoded_vex_l(const Decoded& d) { return d.di.vex_L; }
bool     decoded_vex_w(const Decoded& d) { return d.di.vex_W; }

rust::String disassemble(rust::Slice<const uint8_t> code, uint64_t rip) {
#ifdef VEDX64_STRINGS
    char buf[512];
    size_t n = vedx64::disassemble(code.data(), code.size(), buf, sizeof(buf), rip);
    return n ? rust::String(buf) : rust::String();
#else
    (void)code; (void)rip; return rust::String();
#endif
}
rust::String mnemonic_name(uint16_t m) {
#ifdef VEDX64_STRINGS
    const char* s = vedx64::mnemonic_name(static_cast<Mnemonic>(m));
    return rust::String(s ? s : "???");
#else
    (void)m; return rust::String("???");
#endif
}
size_t table_size() { return vedx64::g_instr_table_size; }

rust::Vec<uint8_t> assemble(rust::Str text) {
    rust::Vec<uint8_t> result;
    auto bytes = vedx64::assemble(std::string(text.data(), text.size()));
    if (bytes) for (auto b : *bytes) result.push_back(b);
    return result;
}
rust::Vec<uint8_t> assemble_block(rust::Str text) {
    rust::Vec<uint8_t> result;
    auto bytes = vedx64::assemble_block(std::string(text.data(), text.size()));
    if (bytes) for (auto b : *bytes) result.push_back(b);
    return result;
}

bool can_relocate(rust::Slice<const uint8_t> code) { return vedx64::can_relocate(code.data(), code.size()); }
bool is_rip_relative(rust::Slice<const uint8_t> code) { return vedx64::is_rip_relative(code.data(), code.size()); }

FlowResult classify_flow(rust::Slice<const uint8_t> code, uint64_t addr) {
    auto fi = vedx64::classify_flow(code.data(), code.size(), addr);
    return FlowResult{static_cast<uint8_t>(fi.type), fi.address, fi.target, fi.is_indirect, fi.has_fallthrough, fi.length};
}
SemResult get_semantics(const Decoded& d) {
    auto s = vedx64::get_semantics(d.di);
    if (!s) return SemResult{};
    return SemResult{s->flags_read, s->flags_written, s->flags_undefined,
                      static_cast<uint8_t>(s->flow), static_cast<uint8_t>(s->category),
                      s->ring, s->lock_valid, s->is_privileged};
}

bool is_jcc(uint16_t m)                  { return vedx64::analysis::is_jcc(static_cast<Mnemonic>(m)); }
bool is_cmov(uint16_t m)                 { return vedx64::analysis::is_cmov(static_cast<Mnemonic>(m)); }
bool is_call(uint16_t m)                 { return vedx64::analysis::is_call(static_cast<Mnemonic>(m)); }
bool is_ret(uint16_t m)                  { return vedx64::analysis::is_ret(static_cast<Mnemonic>(m)); }
bool is_unconditional_branch(uint16_t m) { return vedx64::analysis::is_unconditional_branch(static_cast<Mnemonic>(m)); }
bool is_relative_branch(uint16_t m)      { return vedx64::analysis::is_relative_branch(static_cast<Mnemonic>(m)); }
bool changes_rip(uint16_t m)             { return vedx64::analysis::changes_rip(static_cast<Mnemonic>(m)); }
bool is_arith(uint16_t m)                { return vedx64::analysis::is_arith(static_cast<Mnemonic>(m)); }
bool is_logical(uint16_t m)              { return vedx64::analysis::is_logical(static_cast<Mnemonic>(m)); }
bool is_shift(uint16_t m)                { return vedx64::analysis::is_shift(static_cast<Mnemonic>(m)); }
uint8_t jcc_condition(uint16_t m) {
    auto mn = static_cast<Mnemonic>(m);
    return vedx64::analysis::is_jcc(mn) ? static_cast<uint8_t>(vedx64::analysis::jcc_condition(mn)) : 0xFF;
}
uint8_t cmov_condition(uint16_t m) {
    auto mn = static_cast<Mnemonic>(m);
    return vedx64::analysis::is_cmov(mn) ? static_cast<uint8_t>(vedx64::analysis::cmov_condition(mn)) : 0xFF;
}
uint16_t jcc_for_condition(uint8_t cc) {
    return static_cast<uint16_t>(vedx64::analysis::jcc_for_condition(static_cast<vedx64::analysis::CondCode>(cc & 0xF)));
}
uint8_t sets_eflags(uint16_t m)    { return vedx64::analysis::sets_eflags(static_cast<Mnemonic>(m)); }
uint8_t reads_eflags(uint16_t m)   { return vedx64::analysis::reads_eflags(static_cast<Mnemonic>(m)); }
uint8_t canonical_size(uint16_t m) { return vedx64::analysis::canonical_size(static_cast<Mnemonic>(m)); }

rust::Vec<uint8_t> build_jmp_rel32(int32_t disp) {
    uint8_t buf[5]{}; vedx64::analysis::patch_jmp_rel32(buf, disp);
    rust::Vec<uint8_t> out; for (auto b : buf) out.push_back(b); return out;
}
rust::Vec<uint8_t> build_jcc_rel32(uint8_t cc, int32_t disp) {
    uint8_t buf[6]{};
    vedx64::analysis::patch_jcc_rel32(buf, static_cast<vedx64::analysis::CondCode>(cc & 0xF), disp);
    rust::Vec<uint8_t> out; for (auto b : buf) out.push_back(b); return out;
}
rust::Vec<uint8_t> build_call_rel32(int32_t disp) {
    uint8_t buf[5]{}; vedx64::analysis::patch_call_rel32(buf, disp);
    rust::Vec<uint8_t> out; for (auto b : buf) out.push_back(b); return out;
}
rust::Vec<uint8_t> build_mov_imm64(uint8_t reg_id, uint64_t imm) {
    uint8_t buf[10]{}; vedx64::analysis::patch_mov_imm64(buf, reg_id, imm);
    rust::Vec<uint8_t> out; for (auto b : buf) out.push_back(b); return out;
}

#ifdef VEDX64_EMU
std::unique_ptr<Emu> emu_new(size_t mem_size) {
    auto e = std::make_unique<Emu>();
    e->memory.resize(mem_size, 0);
    vedx64::emu_init(e->cpu, e->memory.data(), e->memory.size());
    return e;
}
int32_t emu_step(Emu& e) { return static_cast<int32_t>(vedx64::emu_step(e.cpu)); }
size_t  emu_run(Emu& e, size_t max_steps) { return vedx64::emu_run(e.cpu, max_steps); }
uint64_t emu_rip(const Emu& e) { return e.cpu.rip; }
void emu_set_rip(Emu& e, uint64_t v) { e.cpu.rip = v; }
uint64_t emu_gpr(const Emu& e, size_t i) { return i < 16 ? e.cpu.gpr[i] : 0; }
void emu_set_gpr(Emu& e, size_t i, uint64_t v) { if (i < 16) e.cpu.gpr[i] = v; }
uint64_t emu_rflags(const Emu& e) { return e.cpu.rflags; }
void emu_set_rflags(Emu& e, uint64_t v) { e.cpu.rflags = v; }
void emu_write_mem(Emu& e, size_t offset, rust::Slice<const uint8_t> data) {
    if (offset + data.size() <= e.memory.size())
        std::memcpy(&e.memory[offset], data.data(), data.size());
}
rust::Vec<uint8_t> emu_read_mem(const Emu& e, size_t offset, size_t len) {
    rust::Vec<uint8_t> out;
    size_t end = offset + len <= e.memory.size() ? offset + len : e.memory.size();
    for (size_t i = offset; i < end; ++i) out.push_back(e.memory[i]);
    return out;
}
#endif // VEDX64_EMU

#ifdef VEDX64_IR
std::unique_ptr<IrLifted> ir_lift(rust::Slice<const uint8_t> code, uint64_t addr) {
    auto result = vedx64::ir::lift(code.data(), code.size(), addr);
    if (!result) return nullptr;
    auto out = std::make_unique<IrLifted>();
    out->address = result->address;
    out->length  = result->length;
    for (auto& op : result->ops) {
        IrLiftedOp lop{};
        lop.opcode = static_cast<uint8_t>(op.opcode);
        lop.out_space  = static_cast<uint8_t>(op.output.space);
        lop.out_offset = op.output.offset;
        lop.out_size   = op.output.size;
        lop.out_value  = op.output.value;
        lop.num_inputs = op.num_inputs;
        for (uint8_t i = 0; i < op.num_inputs && i < 3; ++i) {
            lop.in_space[i]  = static_cast<uint8_t>(op.inputs[i].space);
            lop.in_offset[i] = op.inputs[i].offset;
            lop.in_size[i]   = op.inputs[i].size;
            lop.in_value[i]  = op.inputs[i].value;
        }
        out->ops.push_back(lop);
    }
    return out;
}
uint64_t ir_lifted_address(const IrLifted& l) { return l.address; }
uint8_t  ir_lifted_length(const IrLifted& l) { return l.length; }
size_t   ir_lifted_op_count(const IrLifted& l) { return l.ops.size(); }
uint8_t  ir_lifted_op_opcode(const IrLifted& l, size_t i) { return i < l.ops.size() ? l.ops[i].opcode : 0; }
uint8_t  ir_lifted_op_num_inputs(const IrLifted& l, size_t i) { return i < l.ops.size() ? l.ops[i].num_inputs : 0; }
#endif // VEDX64_IR

} // namespace bridge
} // namespace vedx64
