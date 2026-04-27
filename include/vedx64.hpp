// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once

// vedx64 — x86-64 instruction toolkit
#include "vedx64/core.hpp"
#include "vedx64/operand.hpp"
#include "vedx64/encoding_id.hpp"
#include "vedx64/instruction.hpp"
#include "vedx64/relocation.hpp"
#include "vedx64/branch_follow.hpp"
#include "vedx64/semantics.hpp"
#include "vedx64/analysis.hpp"
#ifdef VEDX64_CODEGEN
#include "vedx64/codegen.hpp"
#endif
#ifdef VEDX64_EMU
#include "vedx64/emu.hpp"
#endif
#ifdef VEDX64_HOOK
#include "vedx64/hook.hpp"
#endif
#ifdef VEDX64_ASSEMBLER
#include "vedx64/assembler.hpp"
#endif
#ifdef VEDX64_IR
#include "vedx64/ir.hpp"
#endif
