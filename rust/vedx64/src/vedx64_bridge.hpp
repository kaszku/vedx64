// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "vedx64/core.hpp"
#include "vedx64/assembler.hpp"
#include "vedx64/relocation.hpp"
#include "vedx64/branch_follow.hpp"
#include "vedx64/semantics.hpp"
#include "vedx64/analysis.hpp"
#ifdef VEDX64_EMU
#include "vedx64/emu.hpp"
#endif
#ifdef VEDX64_IR
#include "vedx64/ir.hpp"
#endif
#include "rust/cxx.h"

namespace vedx64 {
namespace bridge {

/// Opaque wrapper around DecodedInstr for Rust ownership.
struct Decoded { DecodedInstr di{}; };

#ifdef VEDX64_EMU
struct Emu {
    CpuState cpu{};
    std::vector<uint8_t> memory;
};
#endif

#ifdef VEDX64_IR
struct IrLiftedOp {
    uint8_t opcode;
    uint8_t out_space, out_size, num_inputs;
    uint16_t out_offset;
    int64_t out_value;
    uint8_t in_space[3], in_size[3];
    uint16_t in_offset[3];
    int64_t in_value[3];
};
struct IrLifted {
    std::vector<IrLiftedOp> ops;
    uint64_t address{0};
    uint8_t length{0};
};
#endif

// Forward declarations — defined by cxx in bridge.rs.h.
struct FlowResult;
struct SemResult;

// Core
std::unique_ptr<Decoded> decode(rust::Slice<const uint8_t> code);
rust::Vec<uint8_t> encode(const Decoded& d);
uint8_t  decoded_length(const Decoded& d);
uint16_t decoded_mnemonic(const Decoded& d);
uint8_t  decoded_num_operands(const Decoded& d);
uint8_t  decoded_rex(const Decoded& d);
uint8_t  decoded_modrm(const Decoded& d);
uint8_t  decoded_sib(const Decoded& d);
int64_t  decoded_displacement(const Decoded& d);
int64_t  decoded_immediate(const Decoded& d);
bool     decoded_has_vex(const Decoded& d);
uint8_t  decoded_vex_vvvv(const Decoded& d);
uint8_t  decoded_vex_l(const Decoded& d);
bool     decoded_vex_w(const Decoded& d);

// Strings + table
rust::String disassemble(rust::Slice<const uint8_t> code, uint64_t rip);
rust::String mnemonic_name(uint16_t m);
size_t       table_size();

// Assembler
rust::Vec<uint8_t> assemble(rust::Str text);
rust::Vec<uint8_t> assemble_block(rust::Str text);

// Relocation
bool can_relocate(rust::Slice<const uint8_t> code);
bool is_rip_relative(rust::Slice<const uint8_t> code);

// Branch following + semantics — return cxx-shared structs (defined in bridge.cpp).
FlowResult classify_flow(rust::Slice<const uint8_t> code, uint64_t addr);
SemResult  get_semantics(const Decoded& d);

// Analysis helpers (mnemonic queries / EFLAGS / patchers).
// All take a Mnemonic as a uint16_t (matching the enum's underlying type).
bool is_jcc(uint16_t m);
bool is_cmov(uint16_t m);
bool is_call(uint16_t m);
bool is_ret(uint16_t m);
bool is_unconditional_branch(uint16_t m);
bool is_relative_branch(uint16_t m);
bool changes_rip(uint16_t m);
bool is_arith(uint16_t m);
bool is_logical(uint16_t m);
bool is_shift(uint16_t m);
uint8_t jcc_condition(uint16_t m);  // CondCode as 4-bit; 0xFF if not Jcc
uint8_t cmov_condition(uint16_t m); // ditto for CMOVcc
uint16_t jcc_for_condition(uint8_t cc); // CondCode -> Mnemonic
uint8_t sets_eflags(uint16_t m);
uint8_t reads_eflags(uint16_t m);
uint8_t canonical_size(uint16_t m);

// In-place byte patchers (return the new buffer as a Vec since cxx's
// rust::Slice<uint8_t> can't be written through, and we don't want to
// expose mutable raw pointers via the bridge).
rust::Vec<uint8_t> build_jmp_rel32(int32_t disp);
rust::Vec<uint8_t> build_jcc_rel32(uint8_t cc, int32_t disp);
rust::Vec<uint8_t> build_call_rel32(int32_t disp);
rust::Vec<uint8_t> build_mov_imm64(uint8_t reg_id, uint64_t imm);

#ifdef VEDX64_EMU
std::unique_ptr<Emu> emu_new(size_t mem_size);
int32_t  emu_step(Emu& e);
size_t   emu_run(Emu& e, size_t max_steps);
uint64_t emu_rip(const Emu& e);
void     emu_set_rip(Emu& e, uint64_t v);
uint64_t emu_gpr(const Emu& e, size_t i);
void     emu_set_gpr(Emu& e, size_t i, uint64_t v);
uint64_t emu_rflags(const Emu& e);
void     emu_set_rflags(Emu& e, uint64_t v);
void     emu_write_mem(Emu& e, size_t offset, rust::Slice<const uint8_t> data);
rust::Vec<uint8_t> emu_read_mem(const Emu& e, size_t offset, size_t len);
#endif // VEDX64_EMU

#ifdef VEDX64_IR
std::unique_ptr<IrLifted> ir_lift(rust::Slice<const uint8_t> code, uint64_t addr);
uint64_t ir_lifted_address(const IrLifted& l);
uint8_t  ir_lifted_length(const IrLifted& l);
size_t   ir_lifted_op_count(const IrLifted& l);
uint8_t  ir_lifted_op_opcode(const IrLifted& l, size_t i);
uint8_t  ir_lifted_op_num_inputs(const IrLifted& l, size_t i);
#endif // VEDX64_IR

} // namespace bridge
} // namespace vedx64
