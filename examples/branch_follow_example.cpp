// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/branch_follow.hpp"
#include <cstdio>
using namespace vedx64;
int main() {
    printf("=== Control Flow Analysis Example ===\n\n");
    printf("--- classify_flow ---\n");
    { uint8_t c[]={0x90}; auto f=classify_flow(c,1,0x1000); printf("  NOP: type=%d fallthrough=%d\n",(int)f.type,f.has_fallthrough); }
    { uint8_t c[]={0xC3}; auto f=classify_flow(c,1,0x1000); printf("  RET: type=%d fallthrough=%d\n",(int)f.type,f.has_fallthrough); }
    { uint8_t c[]={0xE9,0x10,0,0,0}; auto f=classify_flow(c,5,0x1000); printf("  JMP: type=%d target=0x%llx\n",(int)f.type,(unsigned long long)f.target); }
    { uint8_t c[]={0xE8,0x00,0x01,0,0}; auto f=classify_flow(c,5,0x1000); printf("  CALL: type=%d target=0x%llx\n",(int)f.type,(unsigned long long)f.target); }
    printf("\n--- walk_basic_block ---\n");
    uint8_t code[] = {0x55, 0x48,0x89,0xE5, 0x48,0x83,0xEC,0x20, 0x90, 0xC3};
    auto bb = walk_basic_block(0x401000, [&](uint64_t addr, uint8_t* out) -> bool {
        uint64_t off = addr - 0x401000;
        if (off >= sizeof(code)) return false;
        *out = code[off]; return true;
    });
    printf("  Block: 0x%llx - 0x%llx (%zu instructions)\n",
        (unsigned long long)bb.start, (unsigned long long)bb.end, bb.instructions.size());
    for (auto& fi : bb.instructions)
        printf("    0x%llx: type=%d len=%d\n", (unsigned long long)fi.address, (int)fi.type, fi.length);
    printf("\n--- walk_cfg ---\n");
    // nop; jz +2; nop; nop; ret
    uint8_t code2[] = {0x90, 0x74,0x02, 0x90, 0x90, 0xC3};
    auto cfg = walk_cfg(0x1000, [&](uint64_t addr, uint8_t* out) -> bool {
        uint64_t off = addr - 0x1000;
        if (off >= sizeof(code2)) return false;
        *out = code2[off]; return true;
    });
    printf("  CFG: %zu blocks\n", cfg.size());
    for (auto& b : cfg)
        printf("    Block 0x%llx-0x%llx (%zu insns, %zu successors)\n",
            (unsigned long long)b.start, (unsigned long long)b.end, b.instructions.size(), b.successors.size());
    return 0;
}
