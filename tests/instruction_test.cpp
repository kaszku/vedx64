// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/instruction.hpp"
#include "vedx64/encoding_id.hpp"
#include <cstdio>
using namespace vedx64;
static int g_pass=0,g_fail=0;
#define CHECK(c,m) do{if(!(c)){printf("FAIL: %s\n",m);g_fail++;}else{printf("PASS: %s\n",m);g_pass++;}}while(0)

int main() {
    printf("=== Instruction/Operand/EncodingId Tests ===\n\n");
    { uint8_t c[]={0x48,0x89,0xE5}; Instruction i; size_t n=Instruction::decode_instruction(c,sizeof(c),i,0x401000);
      CHECK(n==3,"mov rbp,rsp: 3 bytes"); CHECK(i.mnemonic==Mnemonic::MOV,"mnemonic MOV");
      CHECK(i.address==0x401000,"address"); CHECK(i.operands.size()==2,"2 operands");
      CHECK(i.encoding_id!=EncodingId::UNKNOWN,"encoding_id set"); }
    { uint8_t c[]={0x48,0x01,0xC8}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.mnemonic==Mnemonic::ADD,"ADD mnemonic"); CHECK(i.operands.size()==2,"2 ops");
      CHECK(i.operands[0].is_register(),"dst is reg"); CHECK(i.operands[0].reg.reg_class==RegisterClass::GPR64,"dst GPR64");
      CHECK(i.operands[0].reg.reg_id==0,"dst RAX"); CHECK(i.operands[1].reg.reg_id==1,"src RCX"); }
    { uint8_t c[]={0x48,0x83,0xEC,0x20}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.mnemonic==Mnemonic::SUB,"SUB"); bool has_imm=false;
      for(auto&op:i.operands) if(op.is_immediate()){has_imm=true; CHECK(op.imm.value==0x20,"imm=0x20");}
      CHECK(has_imm,"has imm"); }
    { uint8_t c[]={0x8B,0x45,0xFC}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.operands[1].is_memory(),"src is mem"); CHECK(i.operands[1].mem.base_reg==5,"base=RBP");
      CHECK(i.operands[1].mem.disp==-4,"disp=-4"); }
    { uint8_t c[]={0x48,0x8D,0x05,0x10,0x00,0x00,0x00}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.operands[1].is_memory(),"src mem"); CHECK(i.operands[1].mem_rip_relative(),"rip-rel"); }
    { uint8_t c[]={0xE8,0x00,0x01,0x00,0x00}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.mnemonic==Mnemonic::CALL,"CALL"); CHECK(i.operands[0].is_relative(),"rel");
      CHECK(i.operands[0].rel.offset==0x100,"offset=0x100"); }
    { uint8_t c[]={0x0F,0x58,0xC8}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.operands[0].reg.reg_class==RegisterClass::XMM,"XMM class");
      CHECK(i.operands[0].reg.reg_id==1,"XMM1"); CHECK(i.operands[1].reg.reg_id==0,"XMM0"); }
    { uint8_t c[]={0x8B,0x04,0x88}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.operands[1].mem.base_reg==0,"base RAX"); CHECK(i.operands[1].mem.index_reg==1,"idx RCX");
      CHECK(i.operands[1].mem_scale()==4,"scale=4"); }
    { auto r=Operand::make_reg(RegisterClass::GPR64,0,64); CHECK(r.is_register(),"make_reg");
      auto im=Operand::make_imm(42,4); CHECK(im.is_immediate()&&im.imm.value==42,"make_imm");
      auto m=Operand::make_mem(5,0xFF,1,-8,8); CHECK(m.is_memory()&&m.mem.disp==-8,"make_mem");
      auto rl=Operand::make_rel(0x100,4); CHECK(rl.is_relative()&&rl.rel.offset==0x100,"make_rel"); }
    { uint8_t c[]={0xF0,0x01,0x08}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.has_lock(),"LOCK prefix set"); CHECK(!i.has_rep(),"no REP"); }
    { uint8_t c[]={0xF3,0xA4}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.has_rep(),"REP prefix set"); CHECK(!i.has_lock(),"no LOCK"); }
    { uint8_t c[]={0x64,0x8B,0x03}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.has_segment(),"has segment"); CHECK(i.segment()==5,"segment=FS(5)"); }
    { uint8_t c[]={0x65,0x8B,0x03}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.segment()==6,"segment=GS(6)"); }
    { uint8_t c[]={0x67,0x66,0x90}; Instruction i; Instruction::decode_instruction(c,sizeof(c),i);
      CHECK(i.has_addr32(),"ADDR32 set"); CHECK(i.has_data16(),"DATA16 set"); }
    { uint8_t c[]={0x90}; Instruction i; Instruction::decode_instruction(c,1,i);
      CHECK(i.prefixes==PFX_NONE,"no prefixes"); }
#ifdef VEDX64_STRINGS
    { uint8_t c[]={0x90}; Instruction i; Instruction::decode_instruction(c,1,i);
      const char*nm=encoding_id_name(i.encoding_id); CHECK(nm!=nullptr,"NOP eid name");
      printf("  NOP encoding: %s\n",nm); }
#endif
    printf("\n%d/%d passed\n",g_pass,g_pass+g_fail);
    return g_fail>0?1:0;
}
