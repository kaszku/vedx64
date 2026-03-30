// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>
#include "vedx64/core.hpp"

namespace vedx64 {
namespace ir {

/// IR micro-operation opcodes.
enum class Opcode : uint8_t {
    COPY,
    LOAD,
    STORE,
    ADD,
    SUB,
    MUL,
    IMUL,
    DIV,
    IDIV,
    NEG,
    AND,
    OR,
    XOR,
    NOT,
    SHL,
    SHR,
    SAR,
    ROL,
    ROR,
    CMP_EQ,
    CMP_NE,
    CMP_SLT,
    CMP_ULT,
    CMP_SLE,
    CMP_ULE,
    ZEXT,
    SEXT,
    TRUNC,
    ADD_FLAGS,
    SUB_FLAGS,
    AND_FLAGS,
    GET_CF,
    GET_ZF,
    GET_SF,
    GET_OF,
    GET_PF,
    SET_CF,
    SET_ZF,
    SET_SF,
    SET_OF,
    SET_PF,
    BRANCH,
    CBRANCH,
    CALL,
    RET,
    POPCNT,
    CTZ,
    CLZ,
    FADD,
    FSUB,
    FMUL,
    FDIV,
    FSQRT,
    FMIN,
    FMAX,
    NOP,
    UNDEF,
    BARRIER,
};

/// Address space for IR variables.
enum class Space : uint8_t {
    Const,
    Temp,
    GPR,
    Flags,
    XMM,
    MMX,
    Seg,
    RAM,
};

/// A variable/location in the IR.
struct VarNode {
    Space space = Space::Const;
    uint16_t offset = 0;   ///< Register number or temp index.
    uint8_t size = 0;      ///< Size in bytes (1, 2, 4, 8, 16).
    int64_t value = 0;     ///< Constant value (only when space == Const).

    static VarNode constant(int64_t val, uint8_t sz) { return {Space::Const, 0, sz, val}; }
    static VarNode gpr(uint16_t reg, uint8_t sz) { return {Space::GPR, reg, sz, 0}; }
    static VarNode xmm(uint16_t reg, uint8_t sz) { return {Space::XMM, reg, sz, 0}; }
    static VarNode temp(uint16_t id, uint8_t sz) { return {Space::Temp, id, sz, 0}; }
    static VarNode flags() { return {Space::Flags, 0, 1, 0}; }
    static VarNode ram(uint8_t sz) { return {Space::RAM, 0, sz, 0}; }
};

/// A single IR micro-operation.
struct Op {
    Opcode opcode = Opcode::NOP;
    VarNode output;         ///< Destination.
    VarNode inputs[3];      ///< Up to 3 source operands.
    uint8_t num_inputs = 0;
};

/// Flat IR representation of a single x86-64 instruction.
struct Lifted {
    std::vector<Op> ops;
    uint64_t address = 0;
    uint8_t length = 0;
};

/// Lift a single instruction at `code` into IR.
/// Returns std::nullopt on decode failure.
std::optional<Lifted> lift(const uint8_t* code, size_t len, uint64_t address = 0);

/// Interpreter context for executing IR.
struct Context {
    uint64_t gpr[16] = {};       ///< RAX=0, RCX=1, ..., R15=15
    uint8_t xmm[16][16] = {};    ///< XMM0-XMM15, 128-bit each
    uint8_t flags[6] = {};        ///< CF=0, PF=1, ZF=2, SF=3, OF=4, AF=5
    uint64_t rip = 0;
    uint8_t* memory = nullptr;
    size_t memory_size = 0;
    bool halted = false;
};

/// Execute a single lifted instruction on the given context.
void execute(Context& ctx, const Lifted& lifted);

} // namespace ir
} // namespace vedx64
