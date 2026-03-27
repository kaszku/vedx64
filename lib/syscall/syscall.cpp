// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/syscall.hpp"

#ifdef _WIN32

#include <windows.h>
#include <vector>
#include <cstring>

// NT types not always available from windows.h
#ifndef NTSYSAPI
#define NTSYSAPI __declspec(dllimport)
#endif
#ifndef NTAPI
#define NTAPI __stdcall
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((LONG)0x00000000L)
#endif
#ifndef STATUS_INFO_LENGTH_MISMATCH
#define STATUS_INFO_LENGTH_MISMATCH ((LONG)0xC0000004L)
#endif

#ifndef NtCurrentProcess
#define NtCurrentProcess() (reinterpret_cast<HANDLE>(static_cast<LONG_PTR>(-1)))
#endif

namespace vedx64_nt {

struct UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
};

struct CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
};

struct OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    void* ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
};

struct SYSTEM_BASIC_INFORMATION {
    ULONG Reserved;
    ULONG TimerResolution;
    ULONG PageSize;
    ULONG NumberOfPhysicalPages;
    ULONG LowestPhysicalPageNumber;
    ULONG HighestPhysicalPageNumber;
    ULONG AllocationGranularity;
    ULONG_PTR MinimumUserModeAddress;
    ULONG_PTR MaximumUserModeAddress;
    ULONG_PTR ActiveProcessorsAffinityMask;
    ULONG NumberOfProcessors;
};

struct SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER CreateTime;
    ULONG WaitTime;
    PVOID StartAddress;
    CLIENT_ID ClientId;
    LONG Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;
    ULONG WaitReason;
};

struct SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize;
    ULONG HardFaultCount;
    ULONG NumberOfThreadsHighWatermark;
    ULONGLONG CycleTime;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    LONG BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR UniqueProcessKey;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
    SYSTEM_THREAD_INFORMATION Threads[1];
};

constexpr ULONG SystemBasicInformation = 0;
constexpr ULONG SystemProcessInformation = 5;

} // namespace vedx64_nt

typedef LONG NTSTATUS_T;

extern "C" {
NTSYSAPI NTSTATUS_T NTAPI NtAllocateVirtualMemory(HANDLE, PVOID*, ULONG_PTR, PSIZE_T, ULONG, ULONG);
NTSYSAPI NTSTATUS_T NTAPI NtFreeVirtualMemory(HANDLE, PVOID*, PSIZE_T, ULONG);
NTSYSAPI NTSTATUS_T NTAPI NtProtectVirtualMemory(HANDLE, PVOID*, PSIZE_T, ULONG, PULONG);
NTSYSAPI NTSTATUS_T NTAPI NtFlushInstructionCache(HANDLE, PVOID, SIZE_T);
NTSYSAPI NTSTATUS_T NTAPI NtQuerySystemInformation(ULONG, PVOID, ULONG, PULONG);
NTSYSAPI NTSTATUS_T NTAPI NtOpenThread(PHANDLE, ACCESS_MASK, vedx64_nt::OBJECT_ATTRIBUTES*, vedx64_nt::CLIENT_ID*);
NTSYSAPI NTSTATUS_T NTAPI NtSuspendThread(HANDLE, PULONG);
NTSYSAPI NTSTATUS_T NTAPI NtResumeThread(HANDLE, PULONG);
NTSYSAPI NTSTATUS_T NTAPI NtClose(HANDLE);
} // extern "C"

static ULONG g_AllocationGranularity = 0x10000;
static bool g_syscall_initialized = false;

namespace vedx64 {
namespace syscall {

bool initialize() {
    if (g_syscall_initialized) return true;

    // Query allocation granularity
    vedx64_nt::SYSTEM_BASIC_INFORMATION sbi = {};
    ULONG len = 0;
    if (NtQuerySystemInformation(vedx64_nt::SystemBasicInformation, &sbi, sizeof(sbi), &len) == STATUS_SUCCESS) {
        g_AllocationGranularity = sbi.AllocationGranularity;
    }

    g_syscall_initialized = true;
    return true;
}

void* alloc_executable(size_t size) {
    PVOID base_addr = nullptr;
    SIZE_T region_size = size;
    LONG status = NtAllocateVirtualMemory(
        NtCurrentProcess(), &base_addr, 0, &region_size,
        MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    return (status == STATUS_SUCCESS) ? base_addr : nullptr;
}

void* alloc_executable_near(void* target, size_t size) {
    uintptr_t alloc_gran = g_AllocationGranularity;
    uintptr_t target_addr = reinterpret_cast<uintptr_t>(target);

    // Try to allocate within ±2GB of target (rel32 range)
    for (uintptr_t offset = alloc_gran; offset < 0x7FFFFFFF; offset += alloc_gran) {
        if (target_addr + offset > target_addr) {
            PVOID base_addr = reinterpret_cast<PVOID>((target_addr + offset) & ~(alloc_gran - 1));
            SIZE_T region_size = size;
            if (NtAllocateVirtualMemory(NtCurrentProcess(), &base_addr, 0, &region_size,
                    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE) == STATUS_SUCCESS)
                return base_addr;
        }
        if (target_addr > offset) {
            PVOID base_addr = reinterpret_cast<PVOID>((target_addr - offset) & ~(alloc_gran - 1));
            SIZE_T region_size = size;
            if (NtAllocateVirtualMemory(NtCurrentProcess(), &base_addr, 0, &region_size,
                    MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE) == STATUS_SUCCESS)
                return base_addr;
        }
    }
    return alloc_executable(size);
}

void free_executable(void* ptr, size_t /*size*/) {
    if (ptr) {
        PVOID base_addr = ptr;
        SIZE_T region_size = 0;
        NtFreeVirtualMemory(NtCurrentProcess(), &base_addr, &region_size, MEM_RELEASE);
    }
}

uint32_t make_writable(void* addr, size_t size) {
    PVOID base_addr = addr;
    SIZE_T region_size = size;
    ULONG old_protect = 0;
    NtProtectVirtualMemory(NtCurrentProcess(), &base_addr, &region_size, PAGE_EXECUTE_READWRITE, &old_protect);
    return static_cast<uint32_t>(old_protect);
}

void make_executable(void* addr, size_t size) {
    PVOID base_addr = addr;
    SIZE_T region_size = size;
    ULONG old_protect = 0;
    NtProtectVirtualMemory(NtCurrentProcess(), &base_addr, &region_size, PAGE_EXECUTE_READ, &old_protect);
}

void restore_protection(void* addr, size_t size, uint32_t old_protect) {
    PVOID base_addr = addr;
    SIZE_T region_size = size;
    ULONG dummy = 0;
    NtProtectVirtualMemory(NtCurrentProcess(), &base_addr, &region_size, static_cast<ULONG>(old_protect), &dummy);
}

void flush_icache(void* addr, size_t size) {
    NtFlushInstructionCache(NtCurrentProcess(), addr, size);
}

void suspend_threads() {
    DWORD current_tid = GetCurrentThreadId();
    DWORD pid = GetCurrentProcessId();

    ULONG buffer_size = 1024 * 1024;
    std::vector<BYTE> buffer(buffer_size);
    ULONG return_length = 0;

    LONG status;
    while ((status = NtQuerySystemInformation(
        vedx64_nt::SystemProcessInformation,
        buffer.data(), static_cast<ULONG>(buffer.size()),
        &return_length)) == STATUS_INFO_LENGTH_MISMATCH) {
        buffer.resize(return_length + 4096);
    }
    if (status != STATUS_SUCCESS) return;

    auto* proc_info = reinterpret_cast<vedx64_nt::SYSTEM_PROCESS_INFORMATION*>(buffer.data());
    while (true) {
        if (HandleToULong(proc_info->UniqueProcessId) == pid) {
            for (ULONG i = 0; i < proc_info->NumberOfThreads; ++i) {
                DWORD tid = HandleToULong(proc_info->Threads[i].ClientId.UniqueThread);
                if (tid != current_tid) {
                    HANDLE thread_handle = nullptr;
                    vedx64_nt::OBJECT_ATTRIBUTES obj_attr = {};
                    obj_attr.Length = sizeof(obj_attr);
                    vedx64_nt::CLIENT_ID client_id = {};
                    client_id.UniqueThread = ULongToHandle(tid);
                    if (NtOpenThread(&thread_handle, THREAD_SUSPEND_RESUME, &obj_attr, &client_id) == STATUS_SUCCESS) {
                        NtSuspendThread(thread_handle, nullptr);
                        NtClose(thread_handle);
                    }
                }
            }
            break;
        }
        if (proc_info->NextEntryOffset == 0) break;
        proc_info = reinterpret_cast<vedx64_nt::SYSTEM_PROCESS_INFORMATION*>(
            reinterpret_cast<BYTE*>(proc_info) + proc_info->NextEntryOffset);
    }
}

void resume_threads() {
    DWORD current_tid = GetCurrentThreadId();
    DWORD pid = GetCurrentProcessId();

    ULONG buffer_size = 1024 * 1024;
    std::vector<BYTE> buffer(buffer_size);
    ULONG return_length = 0;

    LONG status;
    while ((status = NtQuerySystemInformation(
        vedx64_nt::SystemProcessInformation,
        buffer.data(), static_cast<ULONG>(buffer.size()),
        &return_length)) == STATUS_INFO_LENGTH_MISMATCH) {
        buffer.resize(return_length + 4096);
    }
    if (status != STATUS_SUCCESS) return;

    auto* proc_info = reinterpret_cast<vedx64_nt::SYSTEM_PROCESS_INFORMATION*>(buffer.data());
    while (true) {
        if (HandleToULong(proc_info->UniqueProcessId) == pid) {
            for (ULONG i = 0; i < proc_info->NumberOfThreads; ++i) {
                DWORD tid = HandleToULong(proc_info->Threads[i].ClientId.UniqueThread);
                if (tid != current_tid) {
                    HANDLE thread_handle = nullptr;
                    vedx64_nt::OBJECT_ATTRIBUTES obj_attr = {};
                    obj_attr.Length = sizeof(obj_attr);
                    vedx64_nt::CLIENT_ID client_id = {};
                    client_id.UniqueThread = ULongToHandle(tid);
                    if (NtOpenThread(&thread_handle, THREAD_SUSPEND_RESUME, &obj_attr, &client_id) == STATUS_SUCCESS) {
                        NtResumeThread(thread_handle, nullptr);
                        NtClose(thread_handle);
                    }
                }
            }
            break;
        }
        if (proc_info->NextEntryOffset == 0) break;
        proc_info = reinterpret_cast<vedx64_nt::SYSTEM_PROCESS_INFORMATION*>(
            reinterpret_cast<BYTE*>(proc_info) + proc_info->NextEntryOffset);
    }
}

} // namespace syscall
} // namespace vedx64

#endif // _WIN32
