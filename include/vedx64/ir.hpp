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
    LEA,
    ADD,
    SUB,
    MUL,
    IMUL,
    DIV,
    IDIV,
    NEG,
    MOD,
    SMOD,
    AND,
    OR,
    XOR,
    NOT,
    SHL,
    SHR,
    SAR,
    ROL,
    ROR,
    BSWAP,
    BITREV,
    TEST,
    EXTRACT,
    INSERT,
    CONCAT,
    BITCAST,
    CMP_EQ,
    CMP_NE,
    CMP_SLT,
    CMP_ULT,
    CMP_SLE,
    CMP_ULE,
    SELECT,
    BITSEL,
    ZEXT,
    SEXT,
    TRUNC,
    I2F,
    F2I,
    F2F,
    ADD_FLAGS,
    SUB_FLAGS,
    AND_FLAGS,
    GET_CF,
    GET_ZF,
    GET_SF,
    GET_OF,
    GET_PF,
    GET_DF,
    SET_CF,
    SET_ZF,
    SET_SF,
    SET_OF,
    SET_PF,
    SET_DF,
    BRANCH,
    CBRANCH,
    INDIRECT_JMP,
    CALL,
    VCALL,
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
    VADD,
    VSUB,
    VMUL,
    VAND,
    VOR,
    VXOR,
    VSHL,
    VSHR,
    VCMP,
    VEXTRACT_ELEM,
    VINSERT_ELEM,
    VBROADCAST,
    RDTSC,
    SYSCALL,
    UD2,
    NOP,
    UNDEF,
    BARRIER,
    ARCH_X64,
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
    OpMask,
};

/// A variable/location in the IR.
struct VarNode {
    Space space = Space::Const;
    uint16_t offset = 0;   ///< Register number or temp index.
    uint8_t size = 0;      ///< Size in bytes (1, 2, 4, 8, 16).
    int64_t value = 0;     ///< Constant value (only when space == Const).
    /// Bit offset of the slice within the underlying register. Currently
    /// only set to 8 for the legacy AH/CH/DH/BH high-byte addressing —
    /// every other space and every REX-style 8-bit access uses 0.
    uint8_t bit_lo = 0;

    static VarNode constant(int64_t val, uint8_t sz) { return {Space::Const, 0, sz, val}; }
    static VarNode gpr(uint16_t reg, uint8_t sz) { return {Space::GPR, reg, sz, 0}; }
    /// Legacy high-byte: AH=gpr_byte_hi(0), CH=1, DH=2, BH=3. Always size=1.
    static VarNode gpr_byte_hi(uint16_t reg) { return {Space::GPR, reg, 1, 0, 8}; }
    static VarNode xmm(uint16_t reg, uint8_t sz) { return {Space::XMM, reg, sz, 0}; }
    static VarNode temp(uint16_t id, uint8_t sz) { return {Space::Temp, id, sz, 0}; }
    static VarNode flags() { return {Space::Flags, 0, 1, 0}; }
    static VarNode ram(uint8_t sz) { return {Space::RAM, 0, sz, 0}; }
    static VarNode opmask(uint16_t k, uint8_t sz) { return {Space::OpMask, k, sz, 0}; }
};

/// A single IR micro-operation.
struct Op {
    Opcode opcode = Opcode::NOP;
    VarNode output;         ///< Destination.
    VarNode inputs[3];      ///< Up to 3 source operands.
    uint8_t num_inputs = 0;
};

/// REP prefix mode for string instructions.
enum class RepMode : uint8_t {
    None = 0,   ///< No repeat — single execution.
    Rep = 1,    ///< REP: repeat RCX times, decrement RCX each iteration.
    RepZ = 2,   ///< REPZ/REPE: repeat while RCX > 0 && ZF == 1.
    RepNZ = 3,  ///< REPNZ/REPNE: repeat while RCX > 0 && ZF == 0.
};

/// Flat IR representation of a single x86-64 instruction.
/// When rep != RepMode::None, the ops describe a single iteration.
/// The consumer is responsible for the loop: decrement RCX,
/// check termination condition (RepZ/RepNZ check ZF after each iteration).
struct Lifted {
    std::vector<Op> ops;
    uint64_t address = 0;
    uint8_t length = 0;
    RepMode rep = RepMode::None;
};

/// Lift a single instruction at `code` into IR.
/// Returns std::nullopt on decode failure.
std::optional<Lifted> lift(const uint8_t* code, size_t len, uint64_t address = 0);

/// True when every instruction in the source was modeled by the lifter.
/// False when at least one Op is `Opcode::UNDEF` — i.e. the lifter doesn't
/// know how to translate the underlying mnemonic. Symbolic-execution callers
/// should refuse to step past `!is_fully_lifted(l)` and either havoc the
/// affected state or abort the path.
inline bool is_fully_lifted(const Lifted& l) {
    for (const auto& op : l.ops) if (op.opcode == Opcode::UNDEF) return false;
    return true;
}

/// Decompose an `ADD_FLAGS` / `SUB_FLAGS` / `AND_FLAGS` op into the per-flag
/// `SET_CF` / `SET_ZF` / `SET_SF` / `SET_OF` / `SET_PF` chain expressed using
/// only primitive ops (ADD/SUB/AND/XOR/SHR/CMP_EQ/CMP_ULT/POPCNT). Returns the
/// op unchanged when it isn't a flag bundle.
///
/// `next_temp_id` is read-then-incremented for fresh temp slots; the caller
/// owns it across multiple expansions to keep IDs unique.
std::vector<Op> expand_flag_op(const Op& op, uint16_t& next_temp_id);

/// Apply expand_flag_op to every op in `lifted`. Returns a new Lifted whose
/// flag updates are all per-flag SET_* ops (no remaining ADD_FLAGS / SUB_FLAGS /
/// AND_FLAGS bundles). Other ops pass through unchanged.
Lifted expand_flag_bundles(const Lifted& lifted);

/// Interpreter context for executing IR.
struct Context {
    uint64_t gpr[16] = {};       ///< RAX=0, RCX=1, ..., R15=15
    uint8_t xmm[16][16] = {};    ///< XMM0-XMM15, 128-bit each
    uint64_t opmask[8] = {};     ///< K0-K7 opmask registers (AVX-512)
    uint8_t flags[7] = {};        ///< CF=0, PF=1, ZF=2, SF=3, OF=4, AF=5, DF=6
    uint64_t rip = 0;
    uint8_t* memory = nullptr;
    size_t memory_size = 0;
    bool halted = false;
};

/// Execute a single lifted instruction on the given context.
void execute(Context& ctx, const Lifted& lifted);

} // namespace ir
} // namespace vedx64
// Forward-declare CodeGen for lowering API.
namespace vedx64 { class CodeGen; }
namespace vedx64 { namespace ir {

/// Lower a single IR op to x86-64 bytes using the given CodeGen.
/// Returns true on success, false if the op has no direct lowering.
bool emit(const Op& op, CodeGen& cg);

/// Lower an entire Lifted instruction sequence to x86-64 bytes.
/// Returns the number of ops successfully lowered. If all ops lowered,
/// result == lifted.ops.size(). The CodeGen accumulates the encoded bytes.
size_t emit_lifted(const Lifted& lifted, CodeGen& cg);

/// Try to lower an entire Lifted sequence. Returns true only if every op lowered.
bool emit_all(const Lifted& lifted, CodeGen& cg);

#ifdef VEDX64_STRINGS
#include <string>
/// Return human-readable name for an IR opcode.
const char* opcode_name(Opcode opc);

/// Format a VarNode as a human-readable string (e.g. "RAX:8", "t0:4", "#42").
std::string varnode_str(const VarNode& v);

/// Format a single IR Op as a human-readable string.
std::string op_str(const Op& op);

/// Dump all ops in a Lifted to stdout.
void dump(const Lifted& lifted);
#endif // VEDX64_STRINGS

} // namespace ir
} // namespace vedx64
