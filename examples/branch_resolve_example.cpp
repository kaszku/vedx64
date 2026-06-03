// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/branch_resolve.hpp"
#include <cstdio>
#include <cstdint>

using namespace vedx64;
using namespace vedx64::branch;

static const char* form_name(Form f) {
    switch (f) {
    case Form::RelNear: return "RelNear";
    case Form::CountConditional: return "CountConditional";
    case Form::IndirectReg: return "IndirectReg";
    case Form::IndirectMem: return "IndirectMem";
    case Form::FarDirect: return "FarDirect";
    case Form::FarIndirect: return "FarIndirect";
    case Form::ReturnNear: return "ReturnNear";
    case Form::ReturnFar: return "ReturnFar";
    case Form::ReturnInterrupt: return "ReturnInterrupt";
    case Form::Interrupt: return "Interrupt";
    case Form::Syscall: return "Syscall";
    case Form::PushImmRet: return "PushImmRet";
    case Form::PushRegRet: return "PushRegRet";
    case Form::PushSplitImm64Ret: return "PushSplitImm64Ret";
    case Form::MovRegBranch: return "MovRegBranch";
    case Form::LeaRegBranch: return "LeaRegBranch";
    case Form::MovLeaPushRet: return "MovLeaPushRet";
    case Form::LoadMemBranch: return "LoadMemBranch";
    case Form::ComputedRegBranch: return "ComputedRegBranch";
    default: return "None";
    }
}
static const char* res_name(Resolution r) {
    switch (r) {
    case Resolution::Concrete: return "Concrete";
    case Resolution::MemoryPointer: return "MemoryPointer";
    case Resolution::RegisterDynamic: return "RegisterDynamic";
    case Resolution::FarPointer: return "FarPointer";
    case Resolution::StackDynamic: return "StackDynamic";
    default: return "Unknown";
    }
}

struct Sample { const char* desc; uint8_t bytes[16]; size_t len; };

int main() {
    Sample samples[] = {
        {"jmp rel32",            {0xE9,0x00,0x01,0x00,0x00}, 5},
        {"call [rip+0]",         {0xFF,0x15,0x00,0x00,0x00,0x00}, 6},
        {"jmp rax",              {0xFF,0xE0}, 2},
        {"push 0xDEADBEEF; ret", {0x68,0xEF,0xBE,0xAD,0xDE,0xC3}, 6},
        {"mov rax,imm64; jmp rax",{0x48,0xB8,0x88,0x77,0x66,0x55,0x44,0x33,0x22,0x11,0xFF,0xE0}, 12},
        {"lea rax,[rip]; jmp rax",{0x48,0x8D,0x05,0x00,0x00,0x00,0x00,0xFF,0xE0}, 9},
        {"ret",                  {0xC3}, 1},
        {"syscall",              {0x0F,0x05}, 2},
    };
    const uint64_t base = 0x401000;
    std::printf("%-26s %-18s %-16s size dest\n", "idiom", "form", "resolution");
    std::printf("--------------------------------------------------------------------------\n");
    for (auto& s : samples) {
        auto p = recognize(s.bytes, s.len, base);
        std::printf("%-26s %-18s %-16s %4u ", s.desc, form_name(p.form), res_name(p.resolution), p.total_length);
        if (p.resolution == Resolution::Concrete) std::printf("0x%llx\n", (unsigned long long)p.target);
        else if (p.resolution == Resolution::MemoryPointer) std::printf("*[0x%llx]\n", (unsigned long long)p.slot.abs_addr);
        else if (p.resolution == Resolution::RegisterDynamic) std::printf("reg%u\n", p.reg);
        else std::printf("-\n");
    }
    return 0;
}
