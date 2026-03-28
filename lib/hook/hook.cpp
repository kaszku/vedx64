// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/hook.hpp"
#include "vedx64/core.hpp"
#include <cstring>
#include <cstdio>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <algorithm>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <sys/mman.h>
#  include <unistd.h>
#endif

namespace vedx64 {
namespace hook {

struct Trampoline {
    uint8_t* code;
    size_t code_size;
    size_t used_size;
    size_t insn_count;
};

struct HookContext {
    void* target;
    void* detour;
    Trampoline trampoline;
    uint8_t original_bytes[detail::kMaxStolenBytes];
    uint8_t hook_bytes[detail::kAbsJmpSize];
    size_t hook_size;
    bool enabled;
    bool valid;
};

struct HookState {
    bool initialized = false;
    Config config;
    std::mutex mutex;
    std::vector<HookContext*> hooks;
    std::unordered_map<void*, HookContext*> target_map;
};

static HookState g_state;

#ifdef VEDX64_STRINGS
const char* status_to_string(Status status) {
    switch (status) {
        case Status::Success: return "Success";
        case Status::NotInitialized: return "Hooking subsystem not initialized";
        case Status::InvalidTarget: return "Invalid target address";
        case Status::InvalidDetour: return "Invalid detour address";
        case Status::AllocationFailed: return "Memory allocation failed";
        case Status::ProtectionFailed: return "Memory protection change failed";
        case Status::DisassemblyFailed: return "Instruction disassembly failed";
        case Status::RelocationFailed: return "Instruction relocation failed";
        case Status::InstructionTooComplex: return "Instruction too complex to relocate";
        case Status::HookAlreadyInstalled: return "Hook already installed at target";
        case Status::HookNotFound: return "Hook not found";
        case Status::HookDisabled: return "Hook is disabled";
        case Status::InternalError: return "Internal error";
        default: return "Unknown error";
    }
}
#endif // VEDX64_STRINGS

void set_config(const Config& config) {
    std::lock_guard<std::mutex> lock(g_state.mutex);
    g_state.config = config;
}

Config get_config() {
    std::lock_guard<std::mutex> lock(g_state.mutex);
    return g_state.config;
}

Status initialize(const Config& cfg) {
    std::lock_guard<std::mutex> lock(g_state.mutex);
    if (g_state.initialized) return Status::Success;
#ifdef _WIN32
    if (!syscall::initialize()) return Status::InternalError;
#endif
    g_state.config = cfg;
    g_state.initialized = true;
    return Status::Success;
}

void shutdown() {
    std::lock_guard<std::mutex> lock(g_state.mutex);
    if (!g_state.initialized) return;
    g_state.mutex.unlock();
    remove_all();
    g_state.mutex.lock();
    g_state.initialized = false;
}

bool is_initialized() {
    std::lock_guard<std::mutex> lock(g_state.mutex);
    return g_state.initialized;
}

namespace detail {

#ifndef _WIN32
static void* alloc_executable_impl(size_t size) {
    void* p = mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    return (p == MAP_FAILED) ? nullptr : p;
}
static void free_executable_impl(void* ptr, size_t size) { munmap(ptr, size); }
static uint32_t make_writable_impl(void* addr, size_t size) {
    uintptr_t page = (uintptr_t)addr & ~(uintptr_t)(sysconf(_SC_PAGESIZE) - 1);
    size_t page_len = ((uintptr_t)addr + size) - page + sysconf(_SC_PAGESIZE);
    mprotect((void*)page, page_len, PROT_READ | PROT_WRITE | PROT_EXEC);
    return 0;
}
static void restore_protection_impl(void* /*addr*/, size_t /*size*/, uint32_t /*old*/) {}
static void flush_icache_impl(void* /*addr*/, size_t /*size*/) {}
static void suspend_threads_impl() {}
static void resume_threads_impl() {}
#endif

bool can_rel32(uintptr_t from, uintptr_t to) {
    int64_t delta = (int64_t)to - (int64_t)(from + kRelJmpSize);
    return delta >= INT32_MIN && delta <= INT32_MAX;
}

size_t generate_rel32_jmp(uint8_t* buffer, uintptr_t target) {
    buffer[0] = 0xE9;
    int32_t rel = (int32_t)((int64_t)target - (int64_t)((uintptr_t)buffer + kRelJmpSize));
    memcpy(buffer + 1, &rel, 4);
    return kRelJmpSize;
}

size_t generate_abs_jmp(uint8_t* buffer, uintptr_t target) {
    buffer[0] = 0x48; buffer[1] = 0xB8; // MOV RAX, imm64
    memcpy(buffer + 2, &target, 8);
    buffer[10] = 0xFF; buffer[11] = 0xE0; // JMP RAX
    buffer[12] = 0x90; buffer[13] = 0x90; // NOP padding
    return kAbsJmpSize;
}

size_t generate_abs_call(uint8_t* buffer, uintptr_t target) {
    buffer[0] = 0x48; buffer[1] = 0xB8; // MOV RAX, imm64
    memcpy(buffer + 2, &target, 8);
    buffer[10] = 0xFF; buffer[11] = 0xD0; // CALL RAX
    buffer[12] = 0x90; buffer[13] = 0x90; // NOP padding
    return kAbsJmpSize;
}

size_t generate_jump(uint8_t* buffer, uintptr_t from, uintptr_t target) {
    if (can_rel32(from, target)) {
        return generate_rel32_jmp(buffer, target);
    }
    return generate_abs_jmp(buffer, target);
}

size_t generate_call(uint8_t* buffer, uintptr_t from, uintptr_t target) {
    if (can_rel32(from, target)) {
        // E8 rel32 (CALL rel32)
        buffer[0] = 0xE8;
        int32_t rel = (int32_t)((int64_t)target - (int64_t)((uintptr_t)buffer + kRelJmpSize));
        memcpy(buffer + 1, &rel, 4);
        return kRelJmpSize;
    }
    return generate_abs_call(buffer, target);
}

bool is_rip_relative(const uint8_t* code, size_t len) {
    DecodedInstr di;
    size_t n = decode(code, len, di);
    if (n == 0 || !di.desc || !di.desc->has_modrm) return false;
    // Find modrm byte position
    size_t modrm_off = di.num_prefixes + (di.rex ? 1 : 0);
    if (di.desc->opcode > 0xFF) modrm_off += 2; else modrm_off += 1;
    if ((di.desc->opcode == 0x0F38 || di.desc->opcode == 0x0F3A) && di.desc->sec_opcd != 0xFF) modrm_off += 1;
    if (modrm_off >= n) return false;
    uint8_t mb = code[modrm_off];
    return (mb >> 6) == 0 && (mb & 7) == 5;
}

bool is_syscall_stub(const uint8_t* target) {
    // mov r10, rcx = 4C 8B D1 (3 bytes) — or 49 89 CA
    bool has_mov_r10 = (target[0] == 0x4C && target[1] == 0x8B && target[2] == 0xD1) ||
                       (target[0] == 0x49 && target[1] == 0x89 && target[2] == 0xCA);
    // mov eax, imm32 = B8 xx xx xx xx
    bool has_mov_eax = (target[3] == 0xB8);
    // After the syscall number there's usually: syscall (0F 05) or test/jne then syscall
    // We check for syscall at offset 8 (after mov r10 + mov eax)
    bool has_syscall = (target[8] == 0x0F && target[9] == 0x05);
    return has_mov_r10 && has_mov_eax && has_syscall;
}

void* resolve_forwarding_stub(void* target) {
    const uint8_t* code = static_cast<const uint8_t*>(target);
    for (int depth = 0; depth < 5; ++depth) {
        if (code[0] != 0xE9) break; // Not a JMP rel32
        int32_t rel;
        memcpy(&rel, code + 1, 4);
        code = code + 5 + rel;
    }
    return const_cast<void*>(static_cast<const void*>(code));
}

size_t calc_stolen_bytes(const uint8_t* code, size_t min_bytes) {
    size_t total = 0;
    while (total < min_bytes) {
        DecodedInstr di;
        size_t n = decode(code + total, 15, di);
        if (n == 0) return 0;
        total += n;
        if (total > kMaxStolenBytes) return 0;
    }
    return total;
}

bool can_relocate(const uint8_t* code, size_t len) {
    DecodedInstr di;
    size_t n = decode(code, len, di);
    if (n == 0) return false;
    // rel8 branches cannot be relocated in-place (would need expansion)
    if (di.desc) {
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset &&
                di.desc->operands[i].size == OpSize::ByteSignExt)
                return false;
        }
    }
    return true; // RIP-relative and rel32 are handled by relocation
}

void* alloc_executable(size_t size) {
#ifdef _WIN32
    return syscall::alloc_executable(size);
#else
    return alloc_executable_impl(size);
#endif
}

void* alloc_executable_near(void* target, size_t size) {
#ifdef _WIN32
    return syscall::alloc_executable_near(target, size);
#else
    return alloc_executable_impl(size);
#endif
}

void free_executable(void* ptr, size_t size) {
#ifdef _WIN32
    syscall::free_executable(ptr, size);
#else
    free_executable_impl(ptr, size);
#endif
}

uint32_t make_writable(void* addr, size_t size) {
#ifdef _WIN32
    return syscall::make_writable(addr, size);
#else
    return make_writable_impl(addr, size);
#endif
}

void make_executable(void* addr, size_t size) {
#ifdef _WIN32
    syscall::make_executable(addr, size);
#else
    (void)addr; (void)size;
#endif
}

void restore_protection(void* addr, size_t size, uint32_t old_protect) {
#ifdef _WIN32
    syscall::restore_protection(addr, size, old_protect);
#else
    restore_protection_impl(addr, size, old_protect);
#endif
}

void flush_icache(void* addr, size_t size) {
#ifdef _WIN32
    syscall::flush_icache(addr, size);
#else
    flush_icache_impl(addr, size);
#endif
}

void suspend_threads() {
#ifdef _WIN32
    syscall::suspend_threads();
#else
    suspend_threads_impl();
#endif
}

void resume_threads() {
#ifdef _WIN32
    syscall::resume_threads();
#else
    resume_threads_impl();
#endif
}

static bool relocate_stolen_bytes(uint8_t* tramp, size_t stolen, uintptr_t src, size_t* out_count) {
    size_t off = 0;
    size_t count = 0;
    while (off < stolen) {
        DecodedInstr di;
        size_t n = decode(tramp + off, stolen - off, di);
        if (n == 0) return false;
        count++;

        // Relocate RIP-relative addressing
        if (di.desc && di.desc->has_modrm) {
            size_t modrm_off = off + di.num_prefixes + (di.rex ? 1 : 0);
            if (di.desc->opcode > 0xFF) modrm_off += 2; else modrm_off += 1;
            if ((di.desc->opcode == 0x0F38 || di.desc->opcode == 0x0F3A) && di.desc->sec_opcd != 0xFF) modrm_off += 1;
            if (modrm_off < off + n) {
                uint8_t mb = tramp[modrm_off];
                if ((mb >> 6) == 0 && (mb & 7) == 5) {
                    size_t disp_off = modrm_off + 1;
                    int32_t old_disp;
                    memcpy(&old_disp, tramp + disp_off, 4);
                    int64_t abs_target = (int64_t)(src + off + n) + old_disp;
                    int32_t new_disp = (int32_t)(abs_target - (int64_t)((uintptr_t)(tramp + off + n)));
                    memcpy(tramp + disp_off, &new_disp, 4);
                }
            }
        }

        // Relocate rel32 jumps/calls
        if (di.desc) {
            for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
                if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                    if (di.desc->operands[i].size == OpSize::ByteSignExt) break; // Can't relocate rel8
                    size_t imm_off = off + n - 4;
                    int32_t old_rel;
                    memcpy(&old_rel, tramp + imm_off, 4);
                    int64_t abs_target = (int64_t)(src + off + n) + old_rel;
                    int32_t new_rel = (int32_t)(abs_target - (int64_t)((uintptr_t)(tramp + off + n)));
                    memcpy(tramp + imm_off, &new_rel, 4);
                    break;
                }
            }
        }
        off += n;
    }
    if (out_count) *out_count = count;
    return true;
}

bool relocate_instruction(
    const uint8_t* insn, size_t insn_len,
    uintptr_t old_addr, uintptr_t new_addr,
    uint8_t* out_buf, size_t* out_len) {
    (void)new_addr; // new_addr is implicit from out_buf location
    memcpy(out_buf, insn, insn_len);
    *out_len = insn_len;
    size_t dummy_count = 0;
    return relocate_stolen_bytes(out_buf, insn_len, old_addr, &dummy_count);
}

} // namespace detail

#ifdef VEDX64_STRINGS
void dump_hook(HookHandle handle) {
    if (!handle || !handle->valid) {
        printf("Invalid hook handle\n");
        return;
    }
    printf("Hook Information:\n");
    printf("  Target:      %p\n", handle->target);
    printf("  Detour:      %p\n", handle->detour);
    printf("  Trampoline:  %p\n", handle->trampoline.code);
    printf("  Hook size:   %zu bytes\n", handle->hook_size);
    printf("  Relocated:   %zu instructions\n", handle->trampoline.insn_count);
    printf("  Enabled:     %s\n", handle->enabled ? "yes" : "no");
    printf("  Original bytes: ");
    for (size_t i = 0; i < handle->hook_size; ++i) printf("%02X ", handle->original_bytes[i]);
    printf("\n");
    printf("  Trampoline disassembly:\n");
    size_t off = 0;
    while (off < handle->trampoline.used_size) {
        char buf[128];
        size_t n = disassemble(handle->trampoline.code + off, handle->trampoline.used_size - off,
                               buf, sizeof(buf), (uintptr_t)(handle->trampoline.code + off));
        if (n == 0) break;
        printf("    %p: %s\n", (void*)(handle->trampoline.code + off), buf);
        off += n;
    }
}
#endif // VEDX64_STRINGS

static Status create_trampoline(void* target, size_t hook_size, Trampoline* out_tramp) {
    *out_tramp = {};
    Trampoline& tramp = *out_tramp;

    size_t max_size = detail::kTrampolineSize;
#ifdef _WIN32
    tramp.code = static_cast<uint8_t*>(syscall::alloc_executable_near(target, max_size));
#else
    tramp.code = static_cast<uint8_t*>(detail::alloc_executable_impl(max_size));
#endif
    if (!tramp.code) return Status::AllocationFailed;
    tramp.code_size = max_size;

    // Copy stolen bytes
    memcpy(tramp.code, target, hook_size);

    // Relocate instructions
    size_t insn_count = 0;
    if (!detail::relocate_stolen_bytes(tramp.code, hook_size, (uintptr_t)target, &insn_count)) {
#ifdef _WIN32
        syscall::free_executable(tramp.code, tramp.code_size);
#else
        detail::free_executable_impl(tramp.code, tramp.code_size);
#endif
        tramp.code = nullptr;
        return Status::RelocationFailed;
    }
    tramp.insn_count = insn_count;

    // Write jump back to original code after stolen bytes
    uintptr_t return_addr = (uintptr_t)target + hook_size;
    size_t jmp_size = detail::generate_jump(tramp.code + hook_size, (uintptr_t)(tramp.code + hook_size), return_addr);
    tramp.used_size = hook_size + jmp_size;

#ifdef _WIN32
    syscall::flush_icache(tramp.code, tramp.used_size);
#endif

    return Status::Success;
}

Status install_impl(void* target, void* detour, void** original, HookHandle* handle) {
    if (handle) *handle = nullptr;
    if (!g_state.initialized) return Status::NotInitialized;
    if (!target) return Status::InvalidTarget;
    if (!detour) return Status::InvalidDetour;

    // Resolve forwarding stubs
    target = detail::resolve_forwarding_stub(target);

    std::lock_guard<std::mutex> lock(g_state.mutex);

    // Check if already hooked
    if (g_state.target_map.find(target) != g_state.target_map.end()) {
        if (!g_state.config.allow_chain) return Status::HookAlreadyInstalled;
    }

    // Determine jump size needed
    uintptr_t src = (uintptr_t)target;
    uintptr_t dst = (uintptr_t)detour;
    size_t jmp_size = detail::can_rel32(src, dst) ? detail::kRelJmpSize : detail::kAbsJmpSize;
    size_t min_size = jmp_size;
    if (g_state.config.min_hook_size > min_size) min_size = g_state.config.min_hook_size;

    // Calculate stolen bytes
    size_t stolen = detail::calc_stolen_bytes((const uint8_t*)target, min_size);
    if (stolen == 0) return Status::DisassemblyFailed;

    // Create context
    HookContext* ctx = new HookContext{};
    ctx->target = target;
    ctx->detour = detour;
    ctx->hook_size = stolen;
    ctx->enabled = false;
    ctx->valid = false;

    // Save original bytes
    memcpy(ctx->original_bytes, target, stolen);

    // Create trampoline
    Status tramp_status = create_trampoline(target, stolen, &ctx->trampoline);
    if (tramp_status != Status::Success) {
        delete ctx;
        return tramp_status;
    }

    // Generate hook jump (stored, not written to target yet — starts disabled)
    memset(ctx->hook_bytes, 0x90, sizeof(ctx->hook_bytes)); // NOP-fill
    if (detail::can_rel32(src, dst)) {
        detail::generate_rel32_jmp(ctx->hook_bytes, dst);
    } else {
        detail::generate_abs_jmp(ctx->hook_bytes, dst);
    }

    ctx->valid = true;

    g_state.hooks.push_back(ctx);
    g_state.target_map[target] = ctx;

    if (original) *original = ctx->trampoline.code;
    if (handle) *handle = ctx;

    return Status::Success;
}

Status remove(HookHandle handle) {
    if (!g_state.initialized) return Status::NotInitialized;
    if (!handle || !handle->valid) return Status::HookNotFound;

    std::lock_guard<std::mutex> lock(g_state.mutex);

    auto it = std::find(g_state.hooks.begin(), g_state.hooks.end(), handle);
    if (it == g_state.hooks.end()) return Status::HookNotFound;

    if (handle->enabled) {
#ifdef _WIN32
        if (g_state.config.thread_safe) syscall::suspend_threads();
        uint32_t old_protect = syscall::make_writable(handle->target, handle->hook_size);
        memcpy(handle->target, handle->original_bytes, handle->hook_size);
        syscall::restore_protection(handle->target, handle->hook_size, old_protect);
        syscall::flush_icache(handle->target, handle->hook_size);
        if (g_state.config.thread_safe) syscall::resume_threads();
#else
        detail::make_writable_impl(handle->target, handle->hook_size);
        memcpy(handle->target, handle->original_bytes, handle->hook_size);
        detail::flush_icache_impl(handle->target, handle->hook_size);
#endif
    }

    if (handle->trampoline.code) {
#ifdef _WIN32
        syscall::free_executable(handle->trampoline.code, handle->trampoline.code_size);
#else
        detail::free_executable_impl(handle->trampoline.code, handle->trampoline.code_size);
#endif
    }

    g_state.target_map.erase(handle->target);
    g_state.hooks.erase(it);
    handle->valid = false;
    delete handle;

    return Status::Success;
}

void remove_all() {
    if (!g_state.initialized) return;
    std::lock_guard<std::mutex> lock(g_state.mutex);
    std::vector<HookContext*> hooks_copy = g_state.hooks;
    g_state.mutex.unlock();
    for (auto* ctx : hooks_copy) remove(ctx);
    g_state.mutex.lock();
}

Status enable(HookHandle handle) {
    if (!g_state.initialized) return Status::NotInitialized;
    if (!handle || !handle->valid) return Status::HookNotFound;
    std::lock_guard<std::mutex> lock(g_state.mutex);
    if (handle->enabled) return Status::Success;

#ifdef _WIN32
    if (g_state.config.thread_safe) syscall::suspend_threads();
    uint32_t old_protect = syscall::make_writable(handle->target, handle->hook_size);
    memcpy(handle->target, handle->hook_bytes, handle->hook_size);
    // NOP-fill remaining stolen bytes after the jump
    size_t jmp_written = detail::can_rel32((uintptr_t)handle->target, (uintptr_t)handle->detour)
        ? detail::kRelJmpSize : detail::kAbsJmpSize;
    for (size_t i = jmp_written; i < handle->hook_size; ++i) ((uint8_t*)handle->target)[i] = 0x90;
    syscall::restore_protection(handle->target, handle->hook_size, old_protect);
    syscall::flush_icache(handle->target, handle->hook_size);
    if (g_state.config.thread_safe) syscall::resume_threads();
#else
    detail::make_writable_impl(handle->target, handle->hook_size);
    memcpy(handle->target, handle->hook_bytes, handle->hook_size);
    size_t jmp_written = detail::can_rel32((uintptr_t)handle->target, (uintptr_t)handle->detour)
        ? detail::kRelJmpSize : detail::kAbsJmpSize;
    for (size_t i = jmp_written; i < handle->hook_size; ++i) ((uint8_t*)handle->target)[i] = 0x90;
    detail::flush_icache_impl(handle->target, handle->hook_size);
#endif

    handle->enabled = true;
    return Status::Success;
}

Status disable(HookHandle handle) {
    if (!g_state.initialized) return Status::NotInitialized;
    if (!handle || !handle->valid) return Status::HookNotFound;
    std::lock_guard<std::mutex> lock(g_state.mutex);
    if (!handle->enabled) return Status::Success;

#ifdef _WIN32
    if (g_state.config.thread_safe) syscall::suspend_threads();
    uint32_t old_protect = syscall::make_writable(handle->target, handle->hook_size);
    memcpy(handle->target, handle->original_bytes, handle->hook_size);
    syscall::restore_protection(handle->target, handle->hook_size, old_protect);
    syscall::flush_icache(handle->target, handle->hook_size);
    if (g_state.config.thread_safe) syscall::resume_threads();
#else
    detail::make_writable_impl(handle->target, handle->hook_size);
    memcpy(handle->target, handle->original_bytes, handle->hook_size);
    detail::flush_icache_impl(handle->target, handle->hook_size);
#endif

    handle->enabled = false;
    return Status::Success;
}

Status enable_all() {
    if (!g_state.initialized) return Status::NotInitialized;
    std::lock_guard<std::mutex> lock(g_state.mutex);
#ifdef _WIN32
    if (g_state.config.thread_safe) syscall::suspend_threads();
#endif
    for (auto* ctx : g_state.hooks) {
        if (!ctx->valid || ctx->enabled) continue;
#ifdef _WIN32
        uint32_t old_protect = syscall::make_writable(ctx->target, ctx->hook_size);
#else
        detail::make_writable_impl(ctx->target, ctx->hook_size);
#endif
        memcpy(ctx->target, ctx->hook_bytes, ctx->hook_size);
        size_t jmp_written = detail::can_rel32((uintptr_t)ctx->target, (uintptr_t)ctx->detour)
            ? detail::kRelJmpSize : detail::kAbsJmpSize;
        for (size_t i = jmp_written; i < ctx->hook_size; ++i) ((uint8_t*)ctx->target)[i] = 0x90;
#ifdef _WIN32
        syscall::restore_protection(ctx->target, ctx->hook_size, old_protect);
        syscall::flush_icache(ctx->target, ctx->hook_size);
#else
        detail::flush_icache_impl(ctx->target, ctx->hook_size);
#endif
        ctx->enabled = true;
    }
#ifdef _WIN32
    if (g_state.config.thread_safe) syscall::resume_threads();
#endif
    return Status::Success;
}

Status disable_all() {
    if (!g_state.initialized) return Status::NotInitialized;
    std::lock_guard<std::mutex> lock(g_state.mutex);
#ifdef _WIN32
    if (g_state.config.thread_safe) syscall::suspend_threads();
#endif
    for (auto* ctx : g_state.hooks) {
        if (!ctx->valid || !ctx->enabled) continue;
#ifdef _WIN32
        uint32_t old_protect = syscall::make_writable(ctx->target, ctx->hook_size);
#else
        detail::make_writable_impl(ctx->target, ctx->hook_size);
#endif
        memcpy(ctx->target, ctx->original_bytes, ctx->hook_size);
#ifdef _WIN32
        syscall::restore_protection(ctx->target, ctx->hook_size, old_protect);
        syscall::flush_icache(ctx->target, ctx->hook_size);
#else
        detail::flush_icache_impl(ctx->target, ctx->hook_size);
#endif
        ctx->enabled = false;
    }
#ifdef _WIN32
    if (g_state.config.thread_safe) syscall::resume_threads();
#endif
    return Status::Success;
}

Status install_all(const HookEntry* entries, size_t count) {
    if (!g_state.initialized) return Status::NotInitialized;
    // Phase 1: install all hooks
    std::vector<HookHandle> installed;
    installed.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        HookHandle h = nullptr;
        Status s = install_impl(entries[i].target, entries[i].detour, entries[i].original, &h);
        if (s != Status::Success) {
            // Rollback: remove all partially installed hooks
            for (auto* rh : installed) remove(rh);
            return s;
        }
        if (entries[i].handle) *entries[i].handle = h;
        installed.push_back(h);
    }
    // Phase 2: enable all atomically
    return enable_all();
}

bool is_enabled(HookHandle handle) {
    return handle && handle->valid && handle->enabled;
}

void* get_target(HookHandle handle) {
    return (handle && handle->valid) ? handle->target : nullptr;
}

void* get_detour(HookHandle handle) {
    return (handle && handle->valid) ? handle->detour : nullptr;
}

void* get_trampoline(HookHandle handle) {
    return (handle && handle->valid) ? handle->trampoline.code : nullptr;
}

size_t get_hook_size(HookHandle handle) {
    return (handle && handle->valid) ? handle->hook_size : 0;
}

size_t get_relocated_count(HookHandle handle) {
    return (handle && handle->valid) ? handle->trampoline.insn_count : 0;
}

} // namespace hook
} // namespace vedx64
