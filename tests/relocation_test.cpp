// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/relocation.hpp"
#include <cstdio>
#include <cstring>
using namespace vedx64;
static int g_pass=0,g_fail=0;
#define CHECK(c,m) do{if(!(c)){printf("FAIL: %s\n",m);g_fail++;}else{printf("PASS: %s\n",m);g_pass++;}}while(0)
int main() {
    printf("=== Relocation Tests ===\n\n");
    { uint8_t c[]={0x90}; CHECK(!is_pc_relative(c,1),"NOP not pc-rel"); CHECK(!is_rip_relative(c,1),"NOP not rip-rel"); CHECK(can_relocate(c,1),"NOP relocatable"); }
    { uint8_t c[]={0xE8,0x10,0x00,0x00,0x00}; CHECK(is_pc_relative(c,5),"CALL pc-rel"); CHECK(can_relocate(c,5),"CALL relocatable"); }
    { uint8_t c[]={0xEB,0x10}; CHECK(is_pc_relative(c,2),"JMP rel8 pc-rel"); CHECK(!can_relocate(c,2),"JMP rel8 not relocatable"); }
    { uint8_t c[]={0xE9,0x10,0x00,0x00,0x00}; CHECK(can_relocate(c,5),"JMP rel32 relocatable"); }
    { uint8_t c[]={0x48,0x8D,0x05,0x10,0x00,0x00,0x00}; CHECK(is_rip_relative(c,7),"LEA rip-rel"); CHECK(is_pc_relative(c,7),"LEA pc-rel"); }
    { uint8_t c[]={0xE8,0x10,0x00,0x00,0x00}; uint8_t out[16]; size_t len;
      bool ok=relocate_instruction(c,5,0x1000,0x5000,out,&len); CHECK(ok,"relocate CALL"); CHECK(len==5,"len=5");
      CHECK(out[0]==0xE8,"opcode preserved");
      int32_t orig_rel=0x10, new_rel; memcpy(&new_rel,out+1,4);
      CHECK(new_rel!=orig_rel,"CALL rel32 was modified"); }
    { uint8_t c[]={0x48,0x8D,0x05,0x10,0x00,0x00,0x00}; uint8_t out[16]; size_t len;
      bool ok=relocate_instruction(c,7,0x1000,0x5000,out,&len); CHECK(ok,"relocate LEA"); CHECK(len==7,"len=7");
      int32_t orig_disp=0x10, new_disp; memcpy(&new_disp,out+3,4);
      CHECK(new_disp!=orig_disp,"LEA disp was modified"); }
    { uint8_t c[]={0x55,0x48,0x89,0xE5,0x48,0x83,0xEC,0x20};
      CHECK(calc_stolen_bytes(c,5)>=5,"stolen>=5"); CHECK(calc_stolen_bytes(c,1)>=1,"stolen>=1"); }
    { uint8_t buf[]={0x55,0x48,0x89,0xE5}; size_t cnt=0;
      bool ok=relocate_block(buf,4,0x1000,&cnt); CHECK(ok,"block ok"); CHECK(cnt==2,"2 insns"); }
    printf("\n%d/%d passed\n",g_pass,g_pass+g_fail);
    return g_fail>0?1:0;
}
