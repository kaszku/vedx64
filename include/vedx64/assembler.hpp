// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>
#include <string>
#include "vedx64/core.hpp"
#include "vedx64/codegen.hpp"

namespace vedx64 {

/// Assemble a single x86-64 instruction from text.
/// Returns encoded bytes on success, std::nullopt on parse failure.
/// Examples: "nop", "mov rax, rcx", "add rax, 42", "push rbp"
std::optional<std::vector<uint8_t>> assemble(const std::string& text);

/// Assemble multiple instructions separated by newlines or semicolons.
/// Returns concatenated bytes on success, std::nullopt if any line fails.
std::optional<std::vector<uint8_t>> assemble_block(const std::string& text);

} // namespace vedx64
