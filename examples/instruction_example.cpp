// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/instruction.hpp"
#include "vedx64/encoding_id.hpp"
#include <cstdio>
using namespace vedx64;
int main() {
    printf("=== Instruction API Example ===\n\n");
    uint8_t code[] = {0x55, 0x48,0x89,0xE5, 0x48,0x83,0xEC,0x20, 0x8B,0x45,0xFC, 0x83,0xC0,0x01, 0x0F,0xAF,0xC0, 0xC3};
    size_t off=0;
    while(off<sizeof(code)) {
        Instruction i; size_t n=Instruction::decode_instruction(code+off,sizeof(code)-off,i,0x401000+off);
        if(!n) break;
#ifdef VEDX64_STRINGS
        printf("  %llx %-6s ", (unsigned long long)i.address, i.mnemonic_str());
#else
        printf("  %llx m=%-3d ", (unsigned long long)i.address, (int)i.mnemonic);
#endif
        for(size_t j=0;j<i.operands.size();++j) {
            if(j) printf(", ");
            auto&op=i.operands[j];
            if(op.is_register()) printf("reg(%d,%d)",op.reg.reg_id,op.reg.size_bits);
            else if(op.is_immediate()) printf("imm(0x%llx)",(unsigned long long)op.imm.value);
            else if(op.is_memory()) { printf("mem("); if(op.mem_rip_relative()) printf("rip"); else if(op.mem.base_reg!=0xFF) printf("r%d",op.mem.base_reg); if(op.mem.index_reg!=0xFF) printf("+r%d*%d",op.mem.index_reg,op.mem_scale()); if(op.mem.disp) printf("%+lld",(long long)op.mem.disp); printf(")"); }
            else if(op.is_relative()) printf("rel(%+lld)",(long long)op.rel.offset);
        }
#ifdef VEDX64_STRINGS
        printf(" [%s]",encoding_id_name(i.encoding_id));
#endif
        printf("\n"); off+=n;
    }
    return 0;
}
