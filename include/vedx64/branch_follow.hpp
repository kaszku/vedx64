// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <functional>
#include "vedx64/core.hpp"

namespace vedx64 {

/// Control-flow classification for a single instruction.
enum class BranchFlowType : uint8_t {
    Sequential,
    Branch,
    Call,
    ConditionalBranch,
    Return,
    Exception,
    Unknown,
};

/// Detailed flow information for a decoded instruction.
struct FlowInfo {
    BranchFlowType type = BranchFlowType::Sequential;
    uint64_t address = 0;        ///< Address of this instruction.
    uint64_t target = 0;          ///< Branch/call target (0 if indirect or N/A).
    bool is_indirect = false;     ///< True for indirect jmp/call (target unknown statically).
    bool has_fallthrough = true;   ///< Can execution continue to the next instruction?
    uint8_t length = 0;           ///< Instruction length in bytes.
};

/// A basic block: a maximal sequence of instructions with no internal branches.
struct BasicBlock {
    uint64_t start = 0;           ///< Address of first instruction.
    uint64_t end = 0;             ///< Address past the last instruction.
    std::vector<FlowInfo> instructions;
    std::vector<uint64_t> successors;  ///< Addresses of successor basic blocks.
};

/// Classify the control flow of a single instruction at `address`.
FlowInfo classify_flow(const uint8_t* code, size_t len, uint64_t address);

/// Walk a basic block starting at `start` until a terminator is reached.
/// `read_byte` provides a byte at a given address (return false if unmapped).
BasicBlock walk_basic_block(
    uint64_t start,
    std::function<bool(uint64_t addr, uint8_t* out_byte)> read_byte,
    size_t max_insns = 1024);

/// Walk a control flow graph starting at `entry`.
/// Returns all reachable basic blocks.
std::vector<BasicBlock> walk_cfg(
    uint64_t entry,
    std::function<bool(uint64_t addr, uint8_t* out_byte)> read_byte,
    size_t max_blocks = 256,
    size_t max_insns_per_block = 1024);

} // namespace vedx64
