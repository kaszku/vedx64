// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/relocation.hpp"
#include <cstdio>
#include <cstring>
using namespace vedx64;
int main() {
    printf("=== Relocation Example ===\n\n");
    uint8_t call[]={0xE8,0x00,0x01,0x00,0x00};
    printf("  CALL at 0x1000 → target 0x%llx\n",(unsigned long long)(0x1005+0x100));
    printf("  is_pc_relative: %s\n",is_pc_relative(call,5)?"yes":"no");
    printf("  can_relocate:   %s\n",can_relocate(call,5)?"yes":"no");
    uint8_t out[16]; size_t len;
    relocate_instruction(call,5,0x1000,(uintptr_t)out,out,&len);
    int32_t r; memcpy(&r,out+1,4);
    printf("  Relocated → new rel32 = %d, target preserved at 0x1105\n", r);
    uint8_t pro[]={0x55,0x48,0x89,0xE5,0x48,0x83,0xEC,0x20};
    printf("  Stolen bytes for 5-byte hook: %zu\n",calc_stolen_bytes(pro,5));
    return 0;
}
