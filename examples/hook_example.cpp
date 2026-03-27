// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/hook.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstdlib>
#include <vector>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
static bool skip_live_hooks() {
    USHORT process_machine = 0, native_machine = 0;
    auto fn = (decltype(&IsWow64Process2))GetProcAddress(GetModuleHandleA("kernel32"), "IsWow64Process2");
    if (fn && fn(GetCurrentProcess(), &process_machine, &native_machine))
        return native_machine != 0x8664 && native_machine != 0;
    return false;
}
#else
static bool skip_live_hooks() { return false; }
#endif

using namespace vedx64;

#ifdef _MSC_VER
#pragma optimize("", off)
#endif
#ifdef _MSC_VER
__declspec(noinline)
#else
__attribute__((noinline))
#endif
static int target_function(int a, int b) {
    volatile int x = a;
    volatile int y = b;
    return x + y;
}
#ifdef _MSC_VER
#pragma optimize("", on)
#endif

using AddFn = int(*)(int, int);
static AddFn original_func = nullptr;
static hook::HookHandle func_handle = nullptr;

static int detour_function(int a, int b) {
    printf("  [hook] target_function(%d, %d) intercepted\n", a, b);
    int result = original_func(a, b);
    printf("  [hook] original returned %d, adding 1000\n", result);
    return result + 1000;
}

static void example_basic_hook() {
    printf("=== Basic function hook ===\n");
    printf("  Before hook: target_function(3, 4) = %d\n", target_function(3, 4));

    hook::initialize();
    hook::Config cfg;
    cfg.thread_safe = false;
    hook::set_config(cfg);

    auto status = hook::install(&target_function, &detour_function, &original_func, &func_handle);
    if (status == hook::Status::Success) {
        hook::enable(func_handle);
        printf("  After hook:\n");
        int result = target_function(3, 4);
        printf("  Result: %d (expected 1007)\n", result);

        hook::disable(func_handle);
        printf("  Disabled: target_function(3, 4) = %d (expected 7)\n", target_function(3, 4));

        hook::enable(func_handle);
        printf("  Re-enabled: target_function(3, 4) = %d (expected 1007)\n", target_function(3, 4));

        hook::remove(func_handle);
        printf("  Removed: target_function(3, 4) = %d (expected 7)\n", target_function(3, 4));
    } else {
        printf("  Hook install failed: %d\n", static_cast<int>(status));
    }
}

#ifdef _MSC_VER
#pragma optimize("", off)
#endif
#ifdef _MSC_VER
__declspec(noinline)
#else
__attribute__((noinline))
#endif
static int func_a(int x) { volatile int v = x; return v * 2; }
#ifdef _MSC_VER
__declspec(noinline)
#else
__attribute__((noinline))
#endif
static int func_b(int x) { volatile int v = x; return v * 3; }
#ifdef _MSC_VER
#pragma optimize("", on)
#endif

using IntFn = int(*)(int);
static IntFn orig_a = nullptr, orig_b = nullptr;
static hook::HookHandle handle_a = nullptr, handle_b = nullptr;

static int detour_a(int x) { return orig_a(x) + 100; }
static int detour_b(int x) { return orig_b(x) + 200; }

static void example_batch_hooks() {
    printf("\n=== Batch install_all + enable/disable ===\n");

    // Use install_all to install + enable multiple hooks atomically
    hook::HookEntry entries[] = {
        { (void*)&func_a, (void*)&detour_a, (void**)&orig_a, &handle_a },
        { (void*)&func_b, (void*)&detour_b, (void**)&orig_b, &handle_b },
    };
    auto status = hook::install_all(entries, 2);
    if (status != hook::Status::Success) {
        printf("  install_all failed: %d\n", (int)status);
        return;
    }

    printf("  After install_all: func_a(5)=%d, func_b(5)=%d\n", func_a(5), func_b(5));
    hook::disable_all();
    printf("  After disable_all: func_a(5)=%d, func_b(5)=%d\n", func_a(5), func_b(5));
    hook::enable_all();
    printf("  After enable_all:  func_a(5)=%d, func_b(5)=%d\n", func_a(5), func_b(5));
    hook::remove_all();
    printf("  After remove_all:  func_a(5)=%d, func_b(5)=%d\n", func_a(5), func_b(5));
}

class HookManager {
public:
    struct HookInfo {
        hook::HookHandle handle;
        const char* name;
    };

    HookManager() { hook::initialize(); hook::Config cfg; cfg.thread_safe = false; hook::set_config(cfg); }
    ~HookManager() { remove_all(); }

    bool add(const char* name, void* target, void* detour, void** original) {
        hook::HookHandle h = nullptr;
        if (hook::install(target, detour, original, &h) != hook::Status::Success) return false;
        if (hook::enable(h) != hook::Status::Success) { hook::remove(h); return false; }
        hooks_.push_back({h, name});
        return true;
    }

    void remove_all() {
        for (auto& info : hooks_) hook::remove(info.handle);
        hooks_.clear();
    }

    void print_status() const {
        printf("HookManager: %zu hooks\n", hooks_.size());
        for (const auto& info : hooks_)
            printf("  %s [%s]\n", info.name, hook::is_enabled(info.handle) ? "on" : "off");
    }

private:
    std::vector<HookInfo> hooks_;
};

#ifdef _WIN32
extern "C" __declspec(dllimport) LONG NTAPI NtAllocateVirtualMemory(
    HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits,
    PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);

using NtAllocateVirtualMemory_t = LONG(NTAPI*)(HANDLE, PVOID*, ULONG_PTR, PSIZE_T, ULONG, ULONG);
static NtAllocateVirtualMemory_t orig_NtAllocateVirtualMemory = nullptr;
static hook::HookHandle nt_alloc_handle = nullptr;
static int nt_alloc_call_count = 0;

static LONG NTAPI hooked_NtAllocateVirtualMemory(
    HANDLE process, PVOID* base, ULONG_PTR zero_bits,
    PSIZE_T size, ULONG alloc_type, ULONG protect) {
    nt_alloc_call_count++;
    printf("  [syscall hook] NtAllocateVirtualMemory(size=%zu, protect=0x%X)\n",
           (size_t)*size, (unsigned)protect);
    return orig_NtAllocateVirtualMemory(process, base, zero_bits, size, alloc_type, protect);
}

static void example_syscall_hook() {
    printf("\n=== NT syscall stub hook (NtAllocateVirtualMemory) ===\n");

    // NtAllocateVirtualMemory is statically linked from ntdll.lib
    auto* target = &NtAllocateVirtualMemory;
    printf("  Target: %p\n", (void*)target);
    printf("  Is syscall stub: %s\n",
           hook::detail::is_syscall_stub((const uint8_t*)target) ? "yes" : "no");

#ifdef VEDX64_STRINGS
    printf("  Stub disassembly:\n");
    { size_t off = 0;
      for (int i = 0; i < 6 && off < 32; i++) {
        char buf[128];
        size_t n = disassemble((const uint8_t*)target + off, 32 - off,
                               buf, sizeof(buf), (uint64_t)target + off);
        if (n == 0) break;
        printf("    %p: %s\n", (const uint8_t*)target + off, buf);
        off += n;
    } }
#endif

    auto status = hook::install(target, &hooked_NtAllocateVirtualMemory,
                                &orig_NtAllocateVirtualMemory, &nt_alloc_handle);
    if (status != hook::Status::Success) {
#ifdef VEDX64_STRINGS
        printf("  install failed: %s\n", hook::status_to_string(status));
#else
        printf("  install failed: %d\n", (int)status);
#endif
        return;
    }
    hook::enable(nt_alloc_handle);
    printf("\n  Hook active — calling VirtualAlloc to trigger:\n");

    nt_alloc_call_count = 0;
    void* mem = VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (mem) {
        printf("  VirtualAlloc -> %p\n", mem);
        VirtualFree(mem, 0, MEM_RELEASE);
    }
    printf("  Intercepted %d call(s)\n", nt_alloc_call_count);

#ifdef VEDX64_STRINGS
    hook::dump_hook(nt_alloc_handle);
#endif
    hook::remove(nt_alloc_handle);
    printf("  Hook removed\n");
}
#endif // _WIN32

int main() {
    printf("=== Hook API Example ===\n\n");

    if (skip_live_hooks()) {
        printf("Running under emulation — showing analysis only.\n\n");
    }

    // 1. Stolen bytes analysis
    uint8_t sample[] = {0x55, 0x48, 0x89, 0xE5, 0x48, 0x83, 0xEC, 0x20, 0x89, 0x7D, 0xFC, 0xC3};
    size_t stolen5 = hook::detail::calc_stolen_bytes(sample, 5);
    printf("  Bytes to steal for rel32 jmp (5): %zu\n", stolen5);

    // 2. Jump encoding demo
    uint8_t jmp_buf[16] = {};
    hook::detail::generate_abs_jmp(jmp_buf, 0x00007FFF12345678ULL);
    printf("  Abs jmp to 0x7FFF12345678: ");
    for (int i = 0; i < 14; i++) printf("%02X ", jmp_buf[i]);
    printf("\n");

    // 3. Constants
    printf("  Rel32 jmp size: %zu\n", hook::detail::kRelJmpSize);
    printf("  Abs jmp size:   %zu\n", hook::detail::kAbsJmpSize);
    printf("  Max stolen:     %zu\n", hook::detail::kMaxStolenBytes);
    printf("  Trampoline:     %zu\n", hook::detail::kTrampolineSize);

    if (!skip_live_hooks()) {
        example_basic_hook();
        example_batch_hooks();
#ifdef _WIN32
        example_syscall_hook();
#endif
    }

    hook::shutdown();
    return 0;
}
