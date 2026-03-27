// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#pragma once
#include "vedx64/core.hpp"
#include <cstdint>
#include <cstddef>

#ifdef _WIN32
#include "vedx64/syscall.hpp"
#endif

namespace vedx64 {
namespace hook {

struct HookContext;
struct Trampoline;

enum class Status {
    Success = 0,
    NotInitialized,
    InvalidTarget,
    InvalidDetour,
    AllocationFailed,
    ProtectionFailed,
    DisassemblyFailed,
    RelocationFailed,
    InstructionTooComplex,
    HookAlreadyInstalled,
    HookNotFound,
    HookDisabled,
    InternalError
};

#ifdef VEDX64_STRINGS
const char* status_to_string(Status status);
#endif

using HookHandle = HookContext*;

struct Config {
    uint32_t min_hook_size = 5;     // Minimum bytes to overwrite (5 for rel32)
    uint32_t max_relocated_insns = 32;
    bool thread_safe = true;         // Suspend threads during patching
    bool preserve_flags = true;
    bool allow_chain = false;        // Allow hooking already-hooked targets
};

void set_config(const Config& config);
Config get_config();

Status initialize(const Config& cfg = {});
void shutdown();
bool is_initialized();

Status install_impl(void* target, void* detour, void** original, HookHandle* handle);

template<typename Fn>
Status install(Fn* target, Fn* detour, Fn** original, HookHandle* handle = nullptr) {
    return install_impl(reinterpret_cast<void*>(target),
                        reinterpret_cast<void*>(detour),
                        reinterpret_cast<void**>(original), handle);
}

inline Status install(void* target, void* detour, void** original, HookHandle* handle = nullptr) {
    return install_impl(target, detour, original, handle);
}

struct HookEntry {
    void* target;
    void* detour;
    void** original;
    HookHandle* handle;
};

// Install all hooks in the array, then enable them all.
// On failure, removes any hooks that were partially installed.
Status install_all(const HookEntry* entries, size_t count);

Status remove(HookHandle handle);
void remove_all();

Status enable(HookHandle handle);
Status disable(HookHandle handle);
Status enable_all();
Status disable_all();

bool is_enabled(HookHandle handle);
void* get_target(HookHandle handle);
void* get_detour(HookHandle handle);
void* get_trampoline(HookHandle handle);
size_t get_hook_size(HookHandle handle);
size_t get_relocated_count(HookHandle handle);

#ifdef VEDX64_STRINGS
// Debug: dump hook information to stdout
void dump_hook(HookHandle handle);
#endif

namespace detail {

// x86-64 jump constants
static constexpr size_t kRelJmpSize = 5;   // E9 rel32
static constexpr size_t kAbsJmpSize = 14;  // MOV RAX, imm64; JMP RAX
static constexpr size_t kMaxStolenBytes = 64;
static constexpr size_t kTrampolineSize = kMaxStolenBytes + kAbsJmpSize;

// Generate a rel32 jump (5 bytes)
size_t generate_rel32_jmp(uint8_t* buffer, uintptr_t target);

// Generate an absolute jump (14 bytes): MOV RAX, imm64; JMP RAX
size_t generate_abs_jmp(uint8_t* buffer, uintptr_t target);

// Generate an absolute call (14 bytes): MOV RAX, imm64; CALL RAX
size_t generate_abs_call(uint8_t* buffer, uintptr_t target);

// Generate a jump (auto-select rel32 or abs)
size_t generate_jump(uint8_t* buffer, uintptr_t from, uintptr_t target);

// Generate a call (auto-select rel32 or abs)
size_t generate_call(uint8_t* buffer, uintptr_t from, uintptr_t target);

// Check if rel32 can reach target from source
bool can_rel32(uintptr_t from, uintptr_t to);

// Check if an instruction uses RIP-relative addressing
bool is_rip_relative(const uint8_t* code, size_t len);

// Check if an instruction can be safely relocated
bool can_relocate(const uint8_t* code, size_t len);

// Relocate a single instruction to a new address
// Returns true on success, writes relocated bytes to out_buf, sets out_len
bool relocate_instruction(
    const uint8_t* insn, size_t insn_len,
    uintptr_t old_addr, uintptr_t new_addr,
    uint8_t* out_buf, size_t* out_len);

// Detect x86-64 syscall stub pattern: mov r10, rcx; mov eax, imm32; syscall
bool is_syscall_stub(const uint8_t* target);

// Resolve forwarding stubs (follows JMP rel32 chains)
void* resolve_forwarding_stub(void* target);

// Calculate stolen bytes needed
size_t calc_stolen_bytes(const uint8_t* code, size_t min_bytes);

// Memory operations (delegates to syscall:: on Windows, fallback on Unix)
void* alloc_executable(size_t size);
void* alloc_executable_near(void* target, size_t size);
void free_executable(void* ptr, size_t size);
uint32_t make_writable(void* addr, size_t size);
void make_executable(void* addr, size_t size);
void restore_protection(void* addr, size_t size, uint32_t old_protect);
void flush_icache(void* addr, size_t size);

// Thread suspension for safe hook installation
void suspend_threads();
void resume_threads();

} // namespace detail

} // namespace hook
} // namespace vedx64
