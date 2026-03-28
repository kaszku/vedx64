// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>
#include <string>
#include "vedx64/core.hpp"
#include "vedx64/codegen.hpp"

namespace vedx64 {

/// Assemble a single x86-64 instruction from Intel-syntax text.
/// Returns encoded bytes on success, std::nullopt on failure.
///
/// Supports 585 mnemonics, memory operands ([rax+rcx*4+8]),
/// size hints (byte/word/dword/qword), segment overrides (fs:[rax]),
/// RIP-relative ([rip+disp]), lock/rep prefixes, relative jumps/calls,
/// data directives (db/dw/dd/dq), and mnemonic aliases (cmova, je, sete).
std::optional<std::vector<uint8_t>> assemble(const std::string& text);

/// Assemble a single instruction with error reporting.
/// On failure, error is set to a description of what went wrong.
std::optional<std::vector<uint8_t>> assemble(const std::string& text, std::string& error);

/// Assemble multiple instructions separated by newlines or semicolons.
/// Supports labels (name: or name: instr) with two-pass resolution
/// for forward/backward jumps. Comments: # or //
/// Returns concatenated bytes on success, std::nullopt if any line fails.
std::optional<std::vector<uint8_t>> assemble_block(const std::string& text);

/// Assemble block with error reporting (line number + description).
std::optional<std::vector<uint8_t>> assemble_block(const std::string& text, std::string& error);

} // namespace vedx64
