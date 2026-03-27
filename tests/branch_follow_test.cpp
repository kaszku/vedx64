// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/branch_follow.hpp"
#include <cstdio>
#include <cstring>
using namespace vedx64;
static int g_pass=0,g_fail=0;
#define CHECK(c,m) do{if(!(c)){printf("FAIL: %s\n",m);g_fail++;}else{printf("PASS: %s\n",m);g_pass++;}}while(0)
int main() {
    printf("=== Branch Follow Tests ===\n\n");
    { uint8_t c[]={0x90}; auto f=classify_flow(c,1,0x1000);
      CHECK(f.type==BranchFlowType::Sequential,"NOP sequential"); CHECK(f.has_fallthrough,"NOP falls through"); CHECK(f.length==1,"NOP len=1"); }
    { uint8_t c[]={0xC3}; auto f=classify_flow(c,1,0x1000);
      CHECK(f.type==BranchFlowType::Return,"RET return"); CHECK(!f.has_fallthrough,"RET no fallthrough"); }
    { uint8_t c[]={0xE9,0x10,0x00,0x00,0x00}; auto f=classify_flow(c,5,0x1000);
      CHECK(f.type==BranchFlowType::Branch,"JMP branch"); CHECK(!f.has_fallthrough,"JMP no fallthrough");
      CHECK(f.target==0x1015,"JMP target=0x1015"); CHECK(!f.is_indirect,"JMP direct"); }
    { uint8_t c[]={0xE8,0x00,0x01,0x00,0x00}; auto f=classify_flow(c,5,0x1000);
      CHECK(f.type==BranchFlowType::Call,"CALL call"); CHECK(f.has_fallthrough,"CALL falls through");
      CHECK(f.target==0x1105,"CALL target"); }
    { uint8_t c[]={0x0F,0x84,0x10,0x00,0x00,0x00}; auto f=classify_flow(c,6,0x1000);
      CHECK(f.type==BranchFlowType::ConditionalBranch,"JZ cond branch"); CHECK(f.has_fallthrough,"JZ falls through");
      CHECK(f.target==0x1016,"JZ target"); }
    { uint8_t c[]={0xCC}; auto f=classify_flow(c,1,0x1000);
      CHECK(f.type==BranchFlowType::Exception,"INT exception"); }
    { uint8_t c[]={0x0F,0x0B}; auto f=classify_flow(c,2,0x1000);
      CHECK(f.type==BranchFlowType::Exception,"UD2 exception"); CHECK(!f.has_fallthrough,"UD2 no fallthrough"); }
    { uint8_t c[]={0xFF,0xE0}; auto f=classify_flow(c,2,0x1000);
      CHECK(f.type==BranchFlowType::Branch,"JMP indirect"); CHECK(f.is_indirect,"is indirect"); }
    {
        // Small code: push rbp; mov rbp,rsp; nop; ret
        uint8_t code[] = {0x55, 0x48,0x89,0xE5, 0x90, 0xC3};
        auto bb = walk_basic_block(0x1000, [&](uint64_t addr, uint8_t* out) -> bool {
            uint64_t off = addr - 0x1000;
            if (off >= sizeof(code)) return false;
            *out = code[off]; return true;
        });
        CHECK(bb.start==0x1000,"bb start");
        CHECK(bb.end==0x1006,"bb end=0x1006");
        CHECK(bb.instructions.size()==4,"bb has 4 insns");
        CHECK(bb.instructions.back().type==BranchFlowType::Return,"last is ret");
        CHECK(bb.successors.empty(),"ret has no successors");
    }
    {
        // nop; jz +1; nop; ret (two blocks)
        uint8_t code[] = {0x90, 0x74,0x01, 0x90, 0xC3};
        auto cfg = walk_cfg(0x2000, [&](uint64_t addr, uint8_t* out) -> bool {
            uint64_t off = addr - 0x2000;
            if (off >= sizeof(code)) return false;
            *out = code[off]; return true;
        });
        CHECK(cfg.size()>=2,"cfg has >=2 blocks");
        printf("  CFG: %zu blocks\n", cfg.size());
    }
    printf("\n%d/%d passed\n",g_pass,g_pass+g_fail);
    return g_fail>0?1:0;
}
