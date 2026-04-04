// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include "vedx64/core.hpp"
#include "vedx64/relocation.hpp"
#include "vedx64/branch_follow.hpp"
#ifdef VEDX64_EMU
#include "vedx64/emu.hpp"
#endif
#ifdef VEDX64_IR
#include "vedx64/ir.hpp"
#endif
#include "rust/cxx.h"

// Bridge between Rust (via cxx) and vedx64 C++ API.
// No extern "C" — all functions are C++ and called through cxx.

namespace vedx64 {
namespace bridge {

/// Opaque wrapper around DecodedInstr for Rust ownership.
struct Decoded {
    DecodedInstr di{};
};

/// Decode one instruction. Returns nullptr on failure.
std::unique_ptr<Decoded> decode(rust::Slice<const uint8_t> code) {
    auto d = std::make_unique<Decoded>();
    size_t n = vedx64::decode(code.data(), code.size(), d->di);
    if (n == 0) return nullptr;
    return d;
}

/// Re-encode a decoded instruction into bytes.
rust::Vec<uint8_t> encode(const Decoded& d) {
    uint8_t buf[15];
    size_t n = vedx64::encode(d.di, buf, sizeof(buf));
    rust::Vec<uint8_t> out;
    out.reserve(n);
    for (size_t i = 0; i < n; ++i) out.push_back(buf[i]);
    return out;
}

/// Get instruction length.
uint8_t decoded_length(const Decoded& d) { return d.di.length; }

/// Get mnemonic as u16.
uint16_t decoded_mnemonic(const Decoded& d) {
    return d.di.desc ? static_cast<uint16_t>(d.di.desc->mnemonic) : 0;
}

/// Get num_operands.
uint8_t decoded_num_operands(const Decoded& d) {
    return d.di.desc ? d.di.desc->num_operands : 0;
}

/// Get REX prefix byte.
uint8_t decoded_rex(const Decoded& d) { return d.di.rex; }

/// Get ModRM byte.
uint8_t decoded_modrm(const Decoded& d) { return d.di.modrm; }

/// Get SIB byte.
uint8_t decoded_sib(const Decoded& d) { return d.di.sib; }

/// Get displacement.
int64_t decoded_displacement(const Decoded& d) { return d.di.displacement; }

/// Get immediate.
int64_t decoded_immediate(const Decoded& d) { return d.di.immediate; }

/// VEX fields.
bool decoded_has_vex(const Decoded& d) { return d.di.has_vex; }
uint8_t decoded_vex_vvvv(const Decoded& d) { return d.di.vex_vvvv; }
uint8_t decoded_vex_l(const Decoded& d) { return d.di.vex_L; }
bool decoded_vex_w(const Decoded& d) { return d.di.vex_W; }

#ifdef VEDX64_STRINGS
rust::String disassemble(rust::Slice<const uint8_t> code, uint64_t rip) {
    char buf[512];
    size_t n = vedx64::disassemble(code.data(), code.size(), buf, sizeof(buf), rip);
    if (n == 0) return rust::String();
    return rust::String(buf);
}

rust::String mnemonic_name(uint16_t m) {
    const char* s = vedx64::mnemonic_name(static_cast<Mnemonic>(m));
    return rust::String(s ? s : "???");
}
#endif // VEDX64_STRINGS

#ifdef VEDX64_ASSEMBLER
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
#endif // VEDX64_ASSEMBLER

size_t table_size() { return vedx64::g_instr_table_size; }

bool can_relocate(rust::Slice<const uint8_t> code) {
    return vedx64::can_relocate(code.data(), code.size());
}

bool is_rip_relative(rust::Slice<const uint8_t> code) {
    return vedx64::is_rip_relative(code.data(), code.size());
}

struct FlowResult {
    uint8_t flow_type;  // FlowType as u8
    uint64_t address;
    uint64_t target;
    bool is_indirect;
    bool has_fallthrough;
    uint8_t length;
};

FlowResult classify_flow(rust::Slice<const uint8_t> code, uint64_t addr) {
    auto fi = vedx64::classify_flow(code.data(), code.size(), addr);
    return FlowResult{static_cast<uint8_t>(fi.type), fi.address, fi.target, fi.is_indirect, fi.has_fallthrough, fi.length};
}

#ifdef VEDX64_SEMANTICS
struct SemResult {
    uint8_t flags_read;
    uint8_t flags_written;
    uint8_t flags_undefined;
    uint8_t flow;    // FlowType
    uint8_t category; // Category
    uint8_t ring;
    bool lock_valid;
    bool is_privileged;
};

SemResult get_semantics(const Decoded& d) {
    auto s = vedx64::get_semantics(d.di);
    if (!s) return SemResult{};
    return SemResult{s->flags_read, s->flags_written, s->flags_undefined,
        static_cast<uint8_t>(s->flow), static_cast<uint8_t>(s->category),
        s->ring, s->lock_valid, s->is_privileged};
}
#endif // VEDX64_SEMANTICS

#ifdef VEDX64_EMU
struct Emu {
    CpuState cpu{};
    std::vector<uint8_t> memory;
};

std::unique_ptr<Emu> emu_new(size_t mem_size) {
    auto e = std::make_unique<Emu>();
    e->memory.resize(mem_size, 0);
    vedx64::emu_init(e->cpu, e->memory.data(), e->memory.size());
    return e;
}

int32_t emu_step(Emu& e) { return static_cast<int32_t>(vedx64::emu_step(e.cpu)); }
size_t emu_run(Emu& e, size_t max_steps) { return vedx64::emu_run(e.cpu, max_steps); }
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
struct IrVarNode {
    uint8_t space;   // ir::Space
    uint16_t offset;
    uint8_t size;
    int64_t value;
};

struct IrOp {
    uint8_t opcode;  // ir::Opcode
    IrVarNode output;
    IrVarNode input0;
    IrVarNode input1;
    IrVarNode input2;
    uint8_t num_inputs;
};

struct IrLifted {
    rust::Vec<IrOp> ops;
    uint64_t address;
    uint8_t length;
};

std::unique_ptr<IrLifted> ir_lift(rust::Slice<const uint8_t> code, uint64_t addr) {
    auto result = vedx64::ir::lift(code.data(), code.size(), addr);
    if (!result) return nullptr;
    auto out = std::make_unique<IrLifted>();
    out->address = result->address;
    out->length = result->length;
    for (auto& op : result->ops) {
        IrVarNode onode{static_cast<uint8_t>(op.output.space), op.output.offset, op.output.size, op.output.value};
        IrVarNode i0{}, i1{}, i2{};
        if (op.num_inputs > 0) i0 = {static_cast<uint8_t>(op.inputs[0].space), op.inputs[0].offset, op.inputs[0].size, op.inputs[0].value};
        if (op.num_inputs > 1) i1 = {static_cast<uint8_t>(op.inputs[1].space), op.inputs[1].offset, op.inputs[1].size, op.inputs[1].value};
        if (op.num_inputs > 2) i2 = {static_cast<uint8_t>(op.inputs[2].space), op.inputs[2].offset, op.inputs[2].size, op.inputs[2].value};
        out->ops.push_back(IrOp{static_cast<uint8_t>(op.opcode), onode, i0, i1, i2, op.num_inputs});
    }
    return out;
}
#endif // VEDX64_IR

} // namespace bridge
} // namespace vedx64
