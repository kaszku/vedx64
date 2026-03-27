// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/hook.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { if (cond) { printf("PASS: %s\n", msg); g_pass++; } else { printf("FAIL: %s\n", msg); g_fail++; } } while(0)

using namespace vedx64;

void test_initialization_lifecycle() {
    printf("  test_initialization_lifecycle...\n");
    hook::shutdown();
    CHECK(!hook::is_initialized(), "not initialized after shutdown");
    auto status = hook::initialize();
    CHECK(status == hook::Status::Success, "initialize returns Success");
    CHECK(hook::is_initialized(), "is initialized");
    status = hook::initialize();
    CHECK(status == hook::Status::Success, "double init is idempotent");
    hook::shutdown();
    CHECK(!hook::is_initialized(), "not initialized after shutdown");
    hook::shutdown(); // double shutdown is safe
}

#ifdef VEDX64_STRINGS
void test_status_strings() {
    printf("  test_status_strings...\n");
    CHECK(hook::status_to_string(hook::Status::Success) != nullptr, "Success string");
    CHECK(hook::status_to_string(hook::Status::NotInitialized) != nullptr, "NotInitialized string");
    CHECK(hook::status_to_string(hook::Status::InvalidTarget) != nullptr, "InvalidTarget string");
    CHECK(hook::status_to_string(hook::Status::InternalError) != nullptr, "InternalError string");
    CHECK(strlen(hook::status_to_string(hook::Status::Success)) > 0, "Success string not empty");
}
#endif

void test_error_handling() {
    printf("  test_error_handling...\n");
    hook::shutdown();
    int dummy_target = 0, dummy_detour = 0;
    void* original = nullptr;
    hook::HookHandle handle = nullptr;
    auto status = hook::install(reinterpret_cast<void*>(&dummy_target),
                                reinterpret_cast<void*>(&dummy_detour), &original, &handle);
    CHECK(status == hook::Status::NotInitialized, "install before init fails");
    CHECK(handle == nullptr, "handle is null on failure");

    hook::initialize();
    status = hook::install(static_cast<void*>(nullptr),
                           reinterpret_cast<void*>(&dummy_detour), &original, &handle);
    CHECK(status == hook::Status::InvalidTarget, "null target fails");
    status = hook::install(reinterpret_cast<void*>(&dummy_target),
                           static_cast<void*>(nullptr), &original, &handle);
    CHECK(status == hook::Status::InvalidDetour, "null detour fails");
    hook::shutdown();
}

void test_generate_jump() {
    printf("  test_generate_jump...\n");
    uint8_t buf[16] = {};

    // rel32 jump
    auto sz = hook::detail::generate_rel32_jmp(buf, 0x401100);
    CHECK(sz == 5, "rel32 jmp is 5 bytes");
    CHECK(buf[0] == 0xE9, "rel32 starts with E9");

    // abs jump
    memset(buf, 0, sizeof(buf));
    sz = hook::detail::generate_abs_jmp(buf, 0x00007FFF12345678ULL);
    CHECK(sz == 14, "abs jmp is 14 bytes");
    CHECK(buf[0] == 0x48 && buf[1] == 0xB8, "abs starts with MOV RAX");
    CHECK(buf[10] == 0xFF && buf[11] == 0xE0, "abs has JMP RAX");
    uint64_t addr;
    memcpy(&addr, buf + 2, 8);
    CHECK(addr == 0x00007FFF12345678ULL, "abs embeds correct address");
}

void test_is_rip_relative() {
    printf("  test_is_rip_relative...\n");
    // LEA RAX, [RIP+0x12345678] = 48 8D 05 78 56 34 12
    uint8_t rip_rel[] = {0x48, 0x8D, 0x05, 0x78, 0x56, 0x34, 0x12};
    CHECK(hook::detail::is_rip_relative(rip_rel, sizeof(rip_rel)), "LEA [RIP+disp] is RIP-relative");

    // MOV EAX, ECX = 89 C8
    uint8_t not_rip[] = {0x89, 0xC8};
    CHECK(!hook::detail::is_rip_relative(not_rip, sizeof(not_rip)), "MOV EAX,ECX is not RIP-relative");
}

void test_calc_stolen_bytes() {
    printf("  test_calc_stolen_bytes...\n");
    // push rbp; mov rbp, rsp; sub rsp, 0x20
    uint8_t sample[] = {0x55, 0x48, 0x89, 0xE5, 0x48, 0x83, 0xEC, 0x20, 0x89, 0x7D, 0xFC, 0xC3};
    size_t s5 = hook::detail::calc_stolen_bytes(sample, 5);
    CHECK(s5 >= 5, "stolen >= 5 for rel32");
    size_t s14 = hook::detail::calc_stolen_bytes(sample, 14);
    CHECK(s14 >= 14 || s14 == 0, "stolen >= 14 for abs or 0 if too short");
}

void test_configuration() {
    printf("  test_configuration...\n");
    hook::initialize();
    hook::Config cfg;
    cfg.min_hook_size = 14;
    cfg.max_relocated_insns = 64;
    cfg.thread_safe = false;
    cfg.preserve_flags = false;
    cfg.allow_chain = true;
    hook::set_config(cfg);
    auto got = hook::get_config();
    CHECK(got.min_hook_size == 14, "config min_hook_size");
    CHECK(got.max_relocated_insns == 64, "config max_relocated_insns");
    CHECK(got.thread_safe == false, "config thread_safe");
    CHECK(got.preserve_flags == false, "config preserve_flags");
    CHECK(got.allow_chain == true, "config allow_chain");
    hook::shutdown();
}

void test_generate_call() {
    printf("  test_generate_call...\n");
    uint8_t buf[16] = {};

    // abs call
    auto sz = hook::detail::generate_abs_call(buf, 0x00007FFF12345678ULL);
    CHECK(sz == 14, "abs call is 14 bytes");
    CHECK(buf[0] == 0x48 && buf[1] == 0xB8, "abs call starts with MOV RAX");
    CHECK(buf[10] == 0xFF && buf[11] == 0xD0, "abs call has CALL RAX");
    uint64_t addr;
    memcpy(&addr, buf + 2, 8);
    CHECK(addr == 0x00007FFF12345678ULL, "abs call embeds correct address");
}

void test_can_relocate() {
    printf("  test_can_relocate...\n");
    // NOP = 0x90 — safe to relocate
    uint8_t nop[] = {0x90};
    CHECK(hook::detail::can_relocate(nop, sizeof(nop)), "NOP is relocatable");

    // push rbp = 0x55 — safe to relocate
    uint8_t push_rbp[] = {0x55};
    CHECK(hook::detail::can_relocate(push_rbp, sizeof(push_rbp)), "PUSH RBP is relocatable");

    // JMP rel8 (EB xx) — cannot be relocated in-place
    uint8_t jmp_rel8[] = {0xEB, 0x10};
    CHECK(!hook::detail::can_relocate(jmp_rel8, sizeof(jmp_rel8)), "JMP rel8 is NOT relocatable");

    // LEA RAX, [RIP+disp] — relocatable (RIP-relative handled by relocation)
    uint8_t lea_rip[] = {0x48, 0x8D, 0x05, 0x78, 0x56, 0x34, 0x12};
    CHECK(hook::detail::can_relocate(lea_rip, sizeof(lea_rip)), "LEA [RIP+disp] is relocatable");
}

static bool skip_live_hooks() {
    if (getenv("CI") || getenv("GITHUB_ACTIONS")) return true;
#ifdef _WIN32
    USHORT process_machine = 0, native_machine = 0;
    auto fn = (decltype(&IsWow64Process2))GetProcAddress(GetModuleHandleA("kernel32"), "IsWow64Process2");
    if (fn && fn(GetCurrentProcess(), &process_machine, &native_machine))
        return native_machine != 0x8664 && native_machine != 0;
#endif
    return false;
}

#ifdef _MSC_VER
#define NOINLINE __declspec(noinline)
#pragma optimize("", off)
#else
#define NOINLINE __attribute__((noinline))
#endif

static volatile int g_hook_called = 0;

NOINLINE int target_add(int a, int b) {
    volatile int x = a;
    volatile int y = b;
    volatile int sum = x + y;
    return sum;
}

#ifdef _MSC_VER
#pragma optimize("", on)
#endif

using AddFn = int(*)(int, int);
static AddFn g_original_add = nullptr;
static hook::HookHandle g_add_handle = nullptr;

NOINLINE int hooked_add(int a, int b) {
    g_hook_called++;
    return g_original_add(a, b);
}

static void setup_test_config() {
    hook::Config cfg;
    cfg.thread_safe = false;
    hook::set_config(cfg);
}

void test_live_hook() {
    printf("  test_live_hook...\n");
    hook::initialize();
    setup_test_config();
    g_hook_called = 0;

    CHECK(target_add(3, 4) == 7, "before hook: 3+4=7");

    auto status = hook::install(&target_add, &hooked_add, &g_original_add, &g_add_handle);
    CHECK(status == hook::Status::Success, "install Success");
    CHECK(g_add_handle != nullptr, "handle is set");
    CHECK(!hook::is_enabled(g_add_handle), "starts disabled");

    status = hook::enable(g_add_handle);
    CHECK(status == hook::Status::Success, "enable Success");
    CHECK(hook::is_enabled(g_add_handle), "is enabled");

    int r = target_add(3, 4);
    CHECK(r == 7, "hooked call returns correct result");
    CHECK(g_hook_called == 1, "hook was called");

    int r2 = g_original_add(10, 20);
    CHECK(r2 == 30, "trampoline calls original: 10+20=30");
    CHECK(g_hook_called == 1, "trampoline doesn't trigger hook");

    // Query info
    CHECK(hook::get_target(g_add_handle) == reinterpret_cast<void*>(&target_add), "get_target");
    CHECK(hook::get_detour(g_add_handle) == reinterpret_cast<void*>(&hooked_add), "get_detour");
    CHECK(hook::get_trampoline(g_add_handle) != nullptr, "get_trampoline");
    CHECK(hook::get_hook_size(g_add_handle) >= 5, "get_hook_size >= 5");
    CHECK(hook::get_relocated_count(g_add_handle) > 0, "get_relocated_count > 0");

    hook::remove(g_add_handle);
    g_hook_called = 0;
    CHECK(target_add(5, 6) == 11, "after remove: 5+6=11");
    CHECK(g_hook_called == 0, "hook not called after remove");
    hook::shutdown();
}

void test_hook_enable_disable() {
    printf("  test_hook_enable_disable...\n");
    hook::initialize();
    setup_test_config();
    g_hook_called = 0;

    auto status = hook::install(&target_add, &hooked_add, &g_original_add, &g_add_handle);
    CHECK(status == hook::Status::Success, "install ok");
    CHECK(!hook::is_enabled(g_add_handle), "starts disabled");

    hook::enable(g_add_handle);
    CHECK(hook::is_enabled(g_add_handle), "enabled");

    hook::disable(g_add_handle);
    CHECK(!hook::is_enabled(g_add_handle), "disabled");
    g_hook_called = 0;
    CHECK(target_add(1, 2) == 3, "disabled: original works");
    CHECK(g_hook_called == 0, "disabled: hook not called");

    hook::enable(g_add_handle);
    CHECK(target_add(1, 2) == 3, "re-enabled: hook works");
    CHECK(g_hook_called == 1, "re-enabled: hook called");

    hook::remove(g_add_handle);
    hook::shutdown();
}

void test_double_hook_rejection() {
    printf("  test_double_hook_rejection...\n");
    hook::initialize();
    setup_test_config();

    AddFn orig1 = nullptr, orig2 = nullptr;
    hook::HookHandle h1 = nullptr, h2 = nullptr;
    auto s1 = hook::install(&target_add, &hooked_add, &orig1, &h1);
    CHECK(s1 == hook::Status::Success, "first install ok");
    auto s2 = hook::install(&target_add, &hooked_add, &orig2, &h2);
    CHECK(s2 == hook::Status::HookAlreadyInstalled, "double hook rejected");
    CHECK(h2 == nullptr, "second handle is null");

    hook::remove(h1);
    hook::shutdown();
}

#ifdef _WIN32

using VirtualAlloc_t = LPVOID (WINAPI *)(LPVOID, SIZE_T, DWORD, DWORD);
static VirtualAlloc_t original_VirtualAlloc = nullptr;
static volatile int g_valloc_count = 0;

static LPVOID WINAPI hooked_VirtualAlloc(LPVOID addr, SIZE_T size, DWORD type, DWORD protect) {
    g_valloc_count++;
    return original_VirtualAlloc(addr, size, type, protect);
}

void test_winapi_hook() {
    printf("  test_winapi_hook (VirtualAlloc)...\n");
    hook::initialize();
    setup_test_config();
    g_valloc_count = 0;

    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    auto target = reinterpret_cast<VirtualAlloc_t>(GetProcAddress(kernel32, "VirtualAlloc"));
    CHECK(target != nullptr, "found VirtualAlloc");

    hook::HookHandle handle = nullptr;
    auto status = hook::install(target, &hooked_VirtualAlloc, &original_VirtualAlloc, &handle);
    CHECK(status == hook::Status::Success, "install VirtualAlloc hook");

    hook::enable(handle);
    LPVOID mem = VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CHECK(mem != nullptr, "VirtualAlloc succeeded through hook");
    CHECK(g_valloc_count == 1, "hook intercepted VirtualAlloc");
    VirtualFree(mem, 0, MEM_RELEASE);

    hook::remove(handle);
    g_valloc_count = 0;
    mem = VirtualAlloc(nullptr, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    CHECK(mem != nullptr, "VirtualAlloc works after unhook");
    CHECK(g_valloc_count == 0, "hook not called after remove");
    VirtualFree(mem, 0, MEM_RELEASE);
    hook::shutdown();
}

void test_syscall_hook() {
    printf("  test_syscall_hook (NtClose)...\n");
    hook::initialize();
    setup_test_config();

    typedef LONG NTSTATUS;
    typedef NTSTATUS (NTAPI *NtClose_fn)(HANDLE);

    static NtClose_fn original_NtClose = nullptr;
    static volatile int close_count = 0;

    struct D {
        static NTSTATUS NTAPI hooked_NtClose(HANDLE h) {
            close_count++;
            return original_NtClose(h);
        }
    };

    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    auto target = reinterpret_cast<NtClose_fn>(GetProcAddress(ntdll, "NtClose"));
    CHECK(target != nullptr, "found NtClose");

    hook::HookHandle handle = nullptr;
    auto status = hook::install(target, &D::hooked_NtClose, &original_NtClose, &handle);
    CHECK(status == hook::Status::Success, "install NtClose hook");
    hook::enable(handle);

    HANDLE evt = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    close_count = 0;
    CloseHandle(evt);
    CHECK(close_count > 0, "NtClose hook intercepted CloseHandle");

    hook::remove(handle);
    close_count = 0;
    evt = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    CloseHandle(evt);
    CHECK(close_count == 0, "NtClose hook removed");
    hook::shutdown();
}

using GetEnvVarA_t = DWORD (WINAPI *)(LPCSTR, LPSTR, DWORD);
static GetEnvVarA_t original_GetEnvVarA = nullptr;
static volatile int g_getenv_count = 0;

static DWORD WINAPI hooked_GetEnvVarA(LPCSTR name, LPSTR buf, DWORD size) {
    g_getenv_count++;
    return original_GetEnvVarA(name, buf, size);
}

void test_winapi_hook_getenv() {
    printf("  test_winapi_hook (GetEnvironmentVariableA)...\n");
    hook::initialize();
    setup_test_config();
    g_getenv_count = 0;

    HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
    auto target = reinterpret_cast<GetEnvVarA_t>(GetProcAddress(kernel32, "GetEnvironmentVariableA"));
    CHECK(target != nullptr, "found GetEnvironmentVariableA");

    hook::HookHandle handle = nullptr;
    auto status = hook::install(target, &hooked_GetEnvVarA, &original_GetEnvVarA, &handle);
    CHECK(status == hook::Status::Success, "install GetEnvVarA hook");
    hook::enable(handle);

    char envbuf[256];
    GetEnvironmentVariableA("PATH", envbuf, sizeof(envbuf));
    CHECK(g_getenv_count == 1, "hook intercepted GetEnvironmentVariableA");

    hook::remove(handle);
    hook::shutdown();
}

void test_syscall_hook_query_memory() {
    printf("  test_syscall_hook (NtQueryVirtualMemory)...\n");
    hook::initialize();
    setup_test_config();

    typedef LONG NTSTATUS;
    typedef NTSTATUS (NTAPI *NtQueryVirtualMemory_fn)(
        HANDLE, PVOID, ULONG, PVOID, SIZE_T, PSIZE_T);

    static NtQueryVirtualMemory_fn original_NtQVM = nullptr;
    static volatile int query_count = 0;

    struct QD {
        static NTSTATUS NTAPI hooked_NtQVM(
            HANDLE ph, PVOID ba, ULONG mic, PVOID mi, SIZE_T mil, PSIZE_T rl) {
            query_count++;
            return original_NtQVM(ph, ba, mic, mi, mil, rl);
        }
    };

    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    auto target = reinterpret_cast<NtQueryVirtualMemory_fn>(GetProcAddress(ntdll, "NtQueryVirtualMemory"));
    CHECK(target != nullptr, "found NtQueryVirtualMemory");

    hook::HookHandle handle = nullptr;
    auto status = hook::install(target, &QD::hooked_NtQVM, &original_NtQVM, &handle);
    CHECK(status == hook::Status::Success, "install NtQueryVirtualMemory hook");
    hook::enable(handle);

    MEMORY_BASIC_INFORMATION mbi = {};
    query_count = 0;
    VirtualQuery(GetModuleHandleA(nullptr), &mbi, sizeof(mbi));
    CHECK(query_count > 0, "NtQueryVirtualMemory hook intercepted VirtualQuery");

    hook::remove(handle);
    hook::shutdown();
}

#endif // _WIN32

int main() {
    printf("=== Hook API Tests ===\n\n");

    // Cross-platform tests
    test_initialization_lifecycle();
#ifdef VEDX64_STRINGS
    test_status_strings();
#endif
    test_error_handling();
    test_configuration();
    test_generate_jump();
    test_generate_call();
    test_is_rip_relative();
    test_can_relocate();
    test_calc_stolen_bytes();

    if (skip_live_hooks()) {
        printf("\nRunning under emulation — skipping live hook tests.\n");
        printf("%d/%d hook tests passed (emulation mode)\n", g_pass, g_pass + g_fail);
        return g_fail > 0 ? 1 : 0;
    }

    // Live hook tests
    test_live_hook();
    test_hook_enable_disable();
    test_double_hook_rejection();

#ifdef _WIN32
    test_winapi_hook();
    test_winapi_hook_getenv();
    test_syscall_hook();
    test_syscall_hook_query_memory();
#endif

    printf("\n%d/%d hook tests passed\n", g_pass, g_pass + g_fail);
    return g_fail > 0 ? 1 : 0;
}
