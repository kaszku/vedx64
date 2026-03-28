// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>

#ifdef _WIN32

namespace vedx64 {
namespace syscall {

// Initialize syscall subsystem (called automatically by hook::initialize)
// NT functions are statically linked via ntdll.lib — no GetProcAddress.
bool initialize();

// Memory operations via NT syscalls
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

} // namespace syscall
} // namespace vedx64

#endif // _WIN32
