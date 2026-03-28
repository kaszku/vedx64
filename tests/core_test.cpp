// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/core.hpp"
#ifdef VEDX64_SEMANTICS
#include "vedx64/semantics.hpp"
#endif
#include <cstdio>
#include <cstring>
#include <cassert>
#include <cstdlib>

using namespace vedx64;

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST_ASSERT(cond, msg) do { \
    g_tests_run++; \
    if (!(cond)) { fprintf(stderr, "FAIL: %s (line %d)\n", msg, __LINE__); } \
    else { g_tests_passed++; } \
} while(0)

void test_table_integrity() {
    TEST_ASSERT(g_instr_table_size > 0, "table has entries");
    TEST_ASSERT(g_instr_table_size > 1044, "table size includes injected entries");

    for (size_t i = 0; i < g_instr_table_size; ++i) {
        const auto& d = g_instr_table[i];
        TEST_ASSERT(d.mnemonic != Mnemonic::INVALID, "entry has valid mnemonic");
        TEST_ASSERT(d.num_operands <= 4, "entry has <= 4 operands");
    }
}

#ifdef VEDX64_STRINGS
void test_mnemonic_names() {
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::INVALID), "???") == 0, "INVALID mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::ADD), "add") == 0, "ADD mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::MOV), "mov") == 0, "MOV mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::NOP), "nop") == 0, "NOP mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::PUSH), "push") == 0, "PUSH mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::POP), "pop") == 0, "POP mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::CALL), "call") == 0, "CALL mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::JMP), "jmp") == 0, "JMP mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::XOR), "xor") == 0, "XOR mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::SUB), "sub") == 0, "SUB mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::CMP), "cmp") == 0, "CMP mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::LEA), "lea") == 0, "LEA mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::AND), "and") == 0, "AND mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::OR), "or") == 0, "OR mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::SHL), "shl") == 0, "SHL mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::SHR), "shr") == 0, "SHR mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::IMUL), "imul") == 0, "IMUL mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::DIV), "div") == 0, "DIV mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::NOT), "not") == 0, "NOT mnemonic name");
    TEST_ASSERT(strcmp(mnemonic_name(Mnemonic::NEG), "neg") == 0, "NEG mnemonic name");
}
#endif // VEDX64_STRINGS

void test_decode_nop() {
    uint8_t code[] = {0x90};
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 1, "NOP length");
    TEST_ASSERT(di.desc != nullptr, "NOP desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::XCHG || di.desc->mnemonic == Mnemonic::NOP, "NOP mnemonic");
    TEST_ASSERT(di.rex == 0, "NOP no rex");
    TEST_ASSERT(di.num_prefixes == 0, "NOP no prefixes");
    if (di.desc->mnemonic == Mnemonic::XCHG) {
        TEST_ASSERT(di.desc->num_operands == 2, "XCHG/NOP has 2 operands");
        TEST_ASSERT(di.desc->operands[0].addr == AddrMode::OpcodeReg, "XCHG op0 is OpcodeReg");
        TEST_ASSERT(di.desc->operands[1].addr == AddrMode::Fixed, "XCHG op1 is Fixed");
        TEST_ASSERT(di.desc->operands[1].fixed_reg == 0, "XCHG op1 fixed_reg=0 (eax)");
        TEST_ASSERT(di.opcode_reg == 0, "NOP opcode_reg=0");
    }
}

void test_decode_ret() {
    uint8_t code[] = {0xC3};
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 1, "RET length");
    TEST_ASSERT(di.desc != nullptr, "RET desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::RETN, "RET mnemonic");
    TEST_ASSERT(di.desc->num_operands == 1, "RET has 1 operand (implicit stack)");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::None, "RET op0 is None (implicit)");
    TEST_ASSERT(di.desc->has_modrm == false, "RET no modrm");
    TEST_ASSERT(di.desc->opcode == 0xC3, "RET opcode is 0xC3");
    TEST_ASSERT(di.rex == 0, "RET no rex");
    TEST_ASSERT(di.immediate == 0, "RET no immediate");
}

void test_decode_push_rbp() {
    uint8_t code[] = {0x55}; // push rbp
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 1, "PUSH RBP length");
    TEST_ASSERT(di.desc != nullptr, "PUSH RBP desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::PUSH, "PUSH mnemonic");
    TEST_ASSERT(di.opcode_reg == 5, "PUSH opcode_reg=5 (rbp)");
    TEST_ASSERT(di.desc->has_modrm == false, "PUSH no modrm");
    TEST_ASSERT(has_opcode_reg(*di.desc), "PUSH uses OpcodeReg");
    TEST_ASSERT(di.desc->num_operands == 2, "PUSH has 2 operands");
    bool found_opreg = false;
    for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
        if (di.desc->operands[i].addr == AddrMode::OpcodeReg) found_opreg = true;
    }
    TEST_ASSERT(found_opreg, "PUSH has OpcodeReg operand");
}

void test_decode_add_reg_reg() {
    uint8_t code[] = {0x01, 0xC8}; // add eax, ecx
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 2, "ADD reg,reg length");
    TEST_ASSERT(di.desc != nullptr, "ADD desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::ADD, "ADD mnemonic");
    TEST_ASSERT(di.desc->has_modrm == true, "ADD has modrm");
    TEST_ASSERT(di.modrm == 0xC8, "ADD modrm=0xC8");
    TEST_ASSERT((di.modrm >> 6) == 3, "ADD mod=11 (register)");
    TEST_ASSERT(((di.modrm >> 3) & 7) == 1, "ADD reg=1 (ecx)");
    TEST_ASSERT((di.modrm & 7) == 0, "ADD rm=0 (eax)");
    TEST_ASSERT(di.desc->num_operands == 2, "ADD has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_RM, "ADD op0 is ModRM_RM");
    TEST_ASSERT(di.desc->operands[0].size == OpSize::OpSzQ, "ADD op0 size is OpSzQ");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::ModRM_Reg, "ADD op1 is ModRM_Reg");
    TEST_ASSERT(di.desc->operands[1].size == OpSize::OpSzQ, "ADD op1 size is OpSzQ");
    TEST_ASSERT(di.displacement == 0, "ADD no displacement");
    TEST_ASSERT(di.immediate == 0, "ADD no immediate");
}

void test_decode_mov_imm32() {
    uint8_t code[] = {0xB8, 0x78, 0x56, 0x34, 0x12}; // mov eax, 0x12345678
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 5, "MOV imm32 length");
    TEST_ASSERT(di.desc != nullptr, "MOV imm desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::MOV, "MOV mnemonic");
    TEST_ASSERT(di.immediate == 0x12345678, "MOV imm value");
    TEST_ASSERT(di.opcode_reg == 0, "MOV opcode_reg=0 (eax)");
    TEST_ASSERT(di.desc->num_operands == 2, "MOV imm has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::OpcodeReg, "MOV op0 is OpcodeReg");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::Immediate, "MOV op1 is Immediate");
    TEST_ASSERT(di.desc->operands[1].size == OpSize::OpSzQ, "MOV op1 size is OpSzQ");
    TEST_ASSERT(di.desc->has_modrm == false, "MOV imm no modrm");
}

void test_decode_rex_w() {
    uint8_t code[] = {0x48, 0x89, 0xE5}; // mov rbp, rsp
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 3, "REX.W MOV length");
    TEST_ASSERT(di.rex == 0x48, "REX byte is 0x48");
    TEST_ASSERT((di.rex & 0x08) != 0, "REX.W set");
    TEST_ASSERT((di.rex & 0x04) == 0, "REX.R clear");
    TEST_ASSERT((di.rex & 0x02) == 0, "REX.X clear");
    TEST_ASSERT((di.rex & 0x01) == 0, "REX.B clear");
    TEST_ASSERT(di.desc != nullptr, "REX MOV desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::MOV, "REX MOV mnemonic");
    TEST_ASSERT(di.desc->has_modrm == true, "REX MOV has modrm");
    TEST_ASSERT(di.modrm == 0xE5, "REX MOV modrm=0xE5");
    TEST_ASSERT((di.modrm >> 6) == 3, "REX MOV mod=11");
    TEST_ASSERT(((di.modrm >> 3) & 7) == 4, "REX MOV reg=4 (rsp)");
    TEST_ASSERT((di.modrm & 7) == 5, "REX MOV rm=5 (rbp)");
    TEST_ASSERT(di.desc->num_operands == 2, "REX MOV has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_RM, "REX MOV op0 is ModRM_RM");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::ModRM_Reg, "REX MOV op1 is ModRM_Reg");
}

void test_decode_two_byte() {
    uint8_t code[] = {0x0F, 0x31}; // rdtsc
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 2, "RDTSC length");
    TEST_ASSERT(di.desc != nullptr, "RDTSC desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::RDTSC, "RDTSC mnemonic");
    TEST_ASSERT(di.desc->opcode == 0x0F31, "RDTSC opcode is 0x0F31");
    TEST_ASSERT(di.desc->num_operands == 3, "RDTSC has 3 operands (eax,edx,tsc)");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::Fixed, "RDTSC op0 is Fixed (eax)");
    TEST_ASSERT(di.desc->operands[0].fixed_reg == 0, "RDTSC op0 reg=0 (eax)");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::Fixed, "RDTSC op1 is Fixed (edx)");
    TEST_ASSERT(di.desc->operands[1].fixed_reg == 2, "RDTSC op1 reg=2 (edx)");
    TEST_ASSERT(di.desc->has_modrm == false, "RDTSC no modrm");
    TEST_ASSERT(di.rex == 0, "RDTSC no rex");
}

void test_decode_jmp_rel8() {
    uint8_t code[] = {0xEB, 0x10}; // jmp $+0x10
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 2, "JMP rel8 length");
    TEST_ASSERT(di.desc != nullptr, "JMP rel8 desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::JMP, "JMP mnemonic");
    TEST_ASSERT(di.immediate == 0x10, "JMP rel8 offset=+0x10");
    TEST_ASSERT(di.desc->num_operands == 1, "JMP rel8 has 1 operand");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::RelOffset, "JMP op0 is RelOffset");
    TEST_ASSERT(di.desc->operands[0].size == OpSize::ByteSignExt, "JMP op0 size is ByteSignExt");
    TEST_ASSERT(di.desc->has_modrm == false, "JMP rel8 no modrm");
}

void test_decode_jmp_rel8_neg() {
    uint8_t code[] = {0xEB, 0xF0}; // jmp $-0x10
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 2, "JMP rel8 neg length");
    TEST_ASSERT(di.desc != nullptr, "JMP rel8 neg desc");
    TEST_ASSERT(di.immediate == -16, "JMP rel8 offset=-16");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::RelOffset, "JMP neg op0 is RelOffset");
}

void test_decode_modrm_memory() {
    uint8_t code[] = {0x8B, 0x45, 0xFC}; // mov eax, [rbp-4]
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 3, "MOV mem length");
    TEST_ASSERT(di.desc != nullptr, "MOV mem desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::MOV, "MOV mem mnemonic");
    TEST_ASSERT(di.modrm == 0x45, "MOV mem modrm=0x45");
    TEST_ASSERT((di.modrm >> 6) == 1, "MOV mem mod=01 (disp8)");
    TEST_ASSERT(((di.modrm >> 3) & 7) == 0, "MOV mem reg=0 (eax)");
    TEST_ASSERT((di.modrm & 7) == 5, "MOV mem rm=5 (rbp)");
    TEST_ASSERT(di.displacement == -4, "MOV mem disp=-4");
    TEST_ASSERT(di.desc->num_operands == 2, "MOV mem has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_Reg, "MOV mem op0 is ModRM_Reg");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::ModRM_RM, "MOV mem op1 is ModRM_RM");
    TEST_ASSERT(di.immediate == 0, "MOV mem no immediate");
    TEST_ASSERT(di.sib == 0, "MOV mem no sib");
}

void test_decode_sib() {
    uint8_t code[] = {0x8B, 0x04, 0x88}; // mov eax, [rax+rcx*4]
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 3, "MOV SIB length");
    TEST_ASSERT(di.desc != nullptr, "MOV SIB desc");
    TEST_ASSERT(di.sib == 0x88, "SIB byte=0x88");
    TEST_ASSERT((di.sib >> 6) == 2, "SIB scale=2 (*4)");
    TEST_ASSERT(((di.sib >> 3) & 7) == 1, "SIB index=1 (rcx)");
    TEST_ASSERT((di.sib & 7) == 0, "SIB base=0 (rax)");
    TEST_ASSERT((di.modrm >> 6) == 0, "SIB mod=00");
    TEST_ASSERT((di.modrm & 7) == 4, "SIB rm=4 (SIB follows)");
    TEST_ASSERT(di.displacement == 0, "SIB no displacement");
    TEST_ASSERT(di.desc->num_operands == 2, "MOV SIB has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_Reg, "MOV SIB op0 is ModRM_Reg");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::ModRM_RM, "MOV SIB op1 is ModRM_RM");
}

void test_decode_sib_disp32() {
    uint8_t code[] = {0x8B, 0x84, 0x8B, 0x00, 0x01, 0x00, 0x00};
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 7, "SIB disp32 length");
    TEST_ASSERT(di.desc != nullptr, "SIB disp32 desc");
    TEST_ASSERT((di.modrm >> 6) == 2, "SIB disp32 mod=10");
    TEST_ASSERT(di.sib == 0x8B, "SIB byte=0x8B");
    TEST_ASSERT((di.sib >> 6) == 2, "SIB scale=2 (*4)");
    TEST_ASSERT(((di.sib >> 3) & 7) == 1, "SIB index=1 (rcx)");
    TEST_ASSERT((di.sib & 7) == 3, "SIB base=3 (rbx)");
    TEST_ASSERT(di.displacement == 0x100, "SIB disp=0x100");
}

void test_decode_rip_relative() {
    uint8_t code[] = {0x8B, 0x05, 0x78, 0x56, 0x34, 0x12};
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 6, "RIP-rel length");
    TEST_ASSERT(di.desc != nullptr, "RIP-rel desc");
    TEST_ASSERT((di.modrm >> 6) == 0, "RIP-rel mod=00");
    TEST_ASSERT((di.modrm & 7) == 5, "RIP-rel rm=5");
    TEST_ASSERT(di.displacement == 0x12345678, "RIP-rel disp=0x12345678");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_Reg, "RIP-rel op0 is ModRM_Reg");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::ModRM_RM, "RIP-rel op1 is ModRM_RM");
}

void test_decode_prefix_66() {
    uint8_t code[] = {0x66, 0x90}; // xchg ax, ax
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 2, "66 NOP length");
    TEST_ASSERT(di.desc != nullptr, "66 NOP desc");
    TEST_ASSERT(di.num_prefixes > 0, "66 NOP has prefix");
}

void test_decode_sub_imm8() {
    uint8_t code[] = {0x83, 0xEC, 0x20}; // sub esp, 0x20
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 3, "SUB imm8 length");
    TEST_ASSERT(di.desc != nullptr, "SUB imm8 desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::SUB, "SUB mnemonic");
    TEST_ASSERT(di.immediate == 0x20, "SUB imm=0x20");
    TEST_ASSERT(di.modrm == 0xEC, "SUB modrm=0xEC");
    TEST_ASSERT((di.modrm >> 6) == 3, "SUB mod=11 (register)");
    TEST_ASSERT(((di.modrm >> 3) & 7) == 5, "SUB reg=5 (/5 ext)");
    TEST_ASSERT((di.modrm & 7) == 4, "SUB rm=4 (esp)");
    TEST_ASSERT(di.desc->opcd_ext == 5, "SUB opcd_ext=5");
    TEST_ASSERT(di.desc->num_operands == 2, "SUB has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_RM, "SUB op0 is ModRM_RM");
    TEST_ASSERT(di.desc->operands[0].size == OpSize::OpSzQ, "SUB op0 size is OpSzQ");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::Immediate, "SUB op1 is Immediate");
    TEST_ASSERT(di.desc->operands[1].size == OpSize::ByteSignExt, "SUB op1 size is ByteSignExt");
}

void test_decode_add_al_imm() {
    uint8_t code[] = {0x04, 0x42}; // add al, 0x42
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 2, "ADD al,imm length");
    TEST_ASSERT(di.desc != nullptr, "ADD al,imm desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::ADD, "ADD al,imm mnemonic");
    TEST_ASSERT(di.immediate == 0x42, "ADD al,imm imm=0x42");
    TEST_ASSERT(di.desc->num_operands == 2, "ADD al,imm has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::Fixed, "ADD al,imm op0 is Fixed");
    TEST_ASSERT(di.desc->operands[0].fixed_reg == 0, "ADD al,imm op0 fixed_reg=0 (al)");
    TEST_ASSERT(di.desc->operands[0].size == OpSize::Byte, "ADD al,imm op0 size is Byte");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::Immediate, "ADD al,imm op1 is Immediate");
    TEST_ASSERT(di.desc->operands[1].size == OpSize::Byte, "ADD al,imm op1 size is Byte");
    TEST_ASSERT(di.desc->has_modrm == false, "ADD al,imm no modrm");
}

void test_decode_call_rel32() {
    uint8_t code[] = {0xE8, 0x10, 0x00, 0x00, 0x00}; // call $+0x10
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 5, "CALL rel32 length");
    TEST_ASSERT(di.desc != nullptr, "CALL rel32 desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::CALL, "CALL mnemonic");
    TEST_ASSERT(di.immediate == 0x10, "CALL rel32 offset=0x10");
    TEST_ASSERT(di.desc->num_operands == 2, "CALL has 2 operands (stack + rel)");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::None, "CALL op0 is None (implicit stack)");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::RelOffset, "CALL op1 is RelOffset");
    TEST_ASSERT(di.desc->operands[1].size == OpSize::OpSzDS, "CALL op1 size is OpSzDS");
}

void test_decode_lea_sib() {
    uint8_t code[] = {0x8D, 0x04, 0x8B}; // lea eax, [rbx+rcx*4]
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 3, "LEA SIB length");
    TEST_ASSERT(di.desc != nullptr, "LEA desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::LEA, "LEA mnemonic");
    TEST_ASSERT(di.desc->num_operands == 2, "LEA has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_Reg, "LEA op0 is ModRM_Reg");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::MemOnly, "LEA op1 is MemOnly");
    TEST_ASSERT(di.sib == 0x8B, "LEA SIB=0x8B");
    TEST_ASSERT((di.sib >> 6) == 2, "LEA SIB scale=2 (*4)");
    TEST_ASSERT(((di.sib >> 3) & 7) == 1, "LEA SIB index=1 (rcx)");
    TEST_ASSERT((di.sib & 7) == 3, "LEA SIB base=3 (rbx)");
}

void test_decode_imul_3op() {
    uint8_t code[] = {0x6B, 0xC0, 0x05}; // imul eax, eax, 5
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 3, "IMUL 3op length");
    TEST_ASSERT(di.desc != nullptr, "IMUL 3op desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::IMUL, "IMUL mnemonic");
    TEST_ASSERT(di.immediate == 5, "IMUL imm=5");
    TEST_ASSERT(di.desc->num_operands == 3, "IMUL has 3 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_Reg, "IMUL op0 is ModRM_Reg");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::ModRM_RM, "IMUL op1 is ModRM_RM");
    TEST_ASSERT(di.desc->operands[2].addr == AddrMode::Immediate, "IMUL op2 is Immediate");
    TEST_ASSERT(di.desc->operands[2].size == OpSize::ByteSignExt, "IMUL op2 size is ByteSignExt");
}

void test_decode_sse_movsd() {
    uint8_t code[] = {0xF2, 0x0F, 0x10, 0xC8}; // movsd xmm1, xmm0
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 4, "MOVSD length");
    TEST_ASSERT(di.desc != nullptr, "MOVSD desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::MOVSD, "MOVSD mnemonic");
    TEST_ASSERT(di.desc->prefix == 0xF2, "MOVSD mandatory prefix=0xF2");
    TEST_ASSERT(di.desc->num_operands == 2, "MOVSD has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::XmmReg, "MOVSD op0 is XmmReg");
    TEST_ASSERT(di.desc->operands[0].size == OpSize::ScalarDouble, "MOVSD op0 size is ScalarDouble");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::XmmRM, "MOVSD op1 is XmmRM");
    TEST_ASSERT(di.modrm == 0xC8, "MOVSD modrm=0xC8");
    TEST_ASSERT((di.modrm >> 6) == 3, "MOVSD mod=11 (register)");
}

void test_decode_test_imm() {
    uint8_t code[] = {0xF7, 0xC0, 0xFF, 0x00, 0x00, 0x00}; // test eax, 0xFF
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 6, "TEST imm length");
    TEST_ASSERT(di.desc != nullptr, "TEST imm desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::TEST, "TEST mnemonic");
    TEST_ASSERT(di.immediate == 0xFF, "TEST imm=0xFF");
    TEST_ASSERT(di.desc->opcd_ext == 0, "TEST opcd_ext=0");
    TEST_ASSERT(di.desc->num_operands == 2, "TEST has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::ModRM_RM, "TEST op0 is ModRM_RM");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::Immediate, "TEST op1 is Immediate");
}

void test_decode_xchg_opcreg() {
    uint8_t code[] = {0x91}; // xchg ecx, eax
    DecodedInstr di;
    size_t n = decode(code, sizeof(code), di);
    TEST_ASSERT(n == 1, "XCHG opcreg length");
    TEST_ASSERT(di.desc != nullptr, "XCHG opcreg desc");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::XCHG, "XCHG mnemonic");
    TEST_ASSERT(di.opcode_reg == 1, "XCHG opcode_reg=1 (ecx)");
    TEST_ASSERT(di.desc->num_operands == 2, "XCHG has 2 operands");
    TEST_ASSERT(di.desc->operands[0].addr == AddrMode::OpcodeReg, "XCHG op0 is OpcodeReg");
    TEST_ASSERT(di.desc->operands[1].addr == AddrMode::Fixed, "XCHG op1 is Fixed");
    TEST_ASSERT(di.desc->operands[1].fixed_reg == 0, "XCHG op1 fixed_reg=0 (eax)");
}

void test_encode_roundtrip() {
    // Test that decode->encode produces the same bytes
    static const uint8_t rt_nop[] = {0x90};
    static const uint8_t rt_ret[] = {0xC3};
    static const uint8_t rt_push[] = {0x55};
    static const uint8_t rt_add[] = {0x01, 0xC8};
    static const uint8_t rt_mov[] = {0x48, 0x89, 0xE5};
    static const uint8_t rt_rdtsc[] = {0x0F, 0x31};
    static const uint8_t rt_sub[] = {0x83, 0xEC, 0x20};

    {
        DecodedInstr di;
        size_t n = decode(rt_nop, sizeof(rt_nop), di);
        TEST_ASSERT(n > 0, "roundtrip NOP: decode ok");
        if (n > 0) {
            uint8_t buf[16];
            size_t enc = encode(di, buf, sizeof(buf));
            TEST_ASSERT(enc == n, "roundtrip NOP: encode length");
            TEST_ASSERT(memcmp(rt_nop, buf, n) == 0, "roundtrip NOP: bytes match");
        }
    }
    {
        DecodedInstr di;
        size_t n = decode(rt_ret, sizeof(rt_ret), di);
        TEST_ASSERT(n > 0, "roundtrip RET: decode ok");
        if (n > 0) {
            uint8_t buf[16];
            size_t enc = encode(di, buf, sizeof(buf));
            TEST_ASSERT(enc == n, "roundtrip RET: encode length");
            TEST_ASSERT(memcmp(rt_ret, buf, n) == 0, "roundtrip RET: bytes match");
        }
    }
    {
        DecodedInstr di;
        size_t n = decode(rt_push, sizeof(rt_push), di);
        TEST_ASSERT(n > 0, "roundtrip PUSH RBP: decode ok");
        if (n > 0) {
            uint8_t buf[16];
            size_t enc = encode(di, buf, sizeof(buf));
            TEST_ASSERT(enc == n, "roundtrip PUSH RBP: encode length");
            TEST_ASSERT(memcmp(rt_push, buf, n) == 0, "roundtrip PUSH RBP: bytes match");
        }
    }
    {
        DecodedInstr di;
        size_t n = decode(rt_add, sizeof(rt_add), di);
        TEST_ASSERT(n > 0, "roundtrip ADD EAX,ECX: decode ok");
        if (n > 0) {
            uint8_t buf[16];
            size_t enc = encode(di, buf, sizeof(buf));
            TEST_ASSERT(enc == n, "roundtrip ADD EAX,ECX: encode length");
            TEST_ASSERT(memcmp(rt_add, buf, n) == 0, "roundtrip ADD EAX,ECX: bytes match");
        }
    }
    {
        DecodedInstr di;
        size_t n = decode(rt_mov, sizeof(rt_mov), di);
        TEST_ASSERT(n > 0, "roundtrip MOV RBP,RSP: decode ok");
        if (n > 0) {
            uint8_t buf[16];
            size_t enc = encode(di, buf, sizeof(buf));
            TEST_ASSERT(enc == n, "roundtrip MOV RBP,RSP: encode length");
            TEST_ASSERT(memcmp(rt_mov, buf, n) == 0, "roundtrip MOV RBP,RSP: bytes match");
        }
    }
    {
        DecodedInstr di;
        size_t n = decode(rt_rdtsc, sizeof(rt_rdtsc), di);
        TEST_ASSERT(n > 0, "roundtrip RDTSC: decode ok");
        if (n > 0) {
            uint8_t buf[16];
            size_t enc = encode(di, buf, sizeof(buf));
            TEST_ASSERT(enc == n, "roundtrip RDTSC: encode length");
            TEST_ASSERT(memcmp(rt_rdtsc, buf, n) == 0, "roundtrip RDTSC: bytes match");
        }
    }
    {
        DecodedInstr di;
        size_t n = decode(rt_sub, sizeof(rt_sub), di);
        TEST_ASSERT(n > 0, "roundtrip SUB ESP,0x20: decode ok");
        if (n > 0) {
            uint8_t buf[16];
            size_t enc = encode(di, buf, sizeof(buf));
            TEST_ASSERT(enc == n, "roundtrip SUB ESP,0x20: encode length");
            TEST_ASSERT(memcmp(rt_sub, buf, n) == 0, "roundtrip SUB ESP,0x20: bytes match");
        }
    }
}

#ifdef VEDX64_STRINGS
void test_disassemble() {
    char buf[256];

    uint8_t nop[] = {0x90};
    size_t n = disassemble(nop, sizeof(nop), buf, sizeof(buf), 0);
    TEST_ASSERT(n == 1, "disasm NOP consumed 1 byte");
    TEST_ASSERT(strlen(buf) > 0, "disasm NOP produced output");

    uint8_t ret[] = {0xC3};
    n = disassemble(ret, sizeof(ret), buf, sizeof(buf), 0);
    TEST_ASSERT(n == 1, "disasm RET consumed 1 byte");
    TEST_ASSERT(strstr(buf, "ret") != nullptr, "disasm RET contains ret");

    uint8_t add[] = {0x01, 0xC8};
    n = disassemble(add, sizeof(add), buf, sizeof(buf), 0);
    TEST_ASSERT(n == 2, "disasm ADD consumed 2 bytes");
    TEST_ASSERT(strstr(buf, "add") != nullptr, "disasm ADD contains add");

    uint8_t jmp[] = {0xEB, 0x10};
    n = disassemble(jmp, sizeof(jmp), buf, sizeof(buf), 0x1000);
    TEST_ASSERT(n == 2, "disasm JMP consumed 2 bytes");
    TEST_ASSERT(strstr(buf, "jmp") != nullptr, "disasm JMP contains jmp");
    TEST_ASSERT(strstr(buf, "1012") != nullptr, "disasm JMP target is rip+2+0x10=0x1012");
}
#endif // VEDX64_STRINGS

void test_decode_edge_cases() {
    DecodedInstr di;
    TEST_ASSERT(decode(nullptr, 0, di) == 0, "null input returns 0");
    uint8_t empty[1] = {0};
    TEST_ASSERT(decode(empty, 0, di) == 0, "empty input returns 0");

    // Truncated two-byte opcode
    uint8_t trunc[] = {0x0F};
    TEST_ASSERT(decode(trunc, 1, di) == 0, "truncated 0F returns 0");
}

void test_encode_edge_cases() {
    DecodedInstr di;
    memset(&di, 0, sizeof(di));
    uint8_t buf[16];
    TEST_ASSERT(encode(di, buf, sizeof(buf)) == 0, "null desc returns 0");
    TEST_ASSERT(encode(di, nullptr, 16) == 0, "null output returns 0");
}

void test_decode_sse() {
    DecodedInstr di;
    uint8_t movaps[] = {0x0F, 0x28, 0xC8};
    size_t n = decode(movaps, sizeof(movaps), di);
    TEST_ASSERT(n == 3, "MOVAPS length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::MOVAPS, "MOVAPS mnemonic");
    TEST_ASSERT((di.modrm >> 6) == 3, "MOVAPS mod=3 (reg)");
    uint8_t addps[] = {0x0F, 0x58, 0xC8};
    n = decode(addps, sizeof(addps), di);
    TEST_ASSERT(n == 3, "ADDPS length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::ADDPS, "ADDPS mnemonic");
    uint8_t movsd[] = {0xF2, 0x0F, 0x10, 0xC8};
    n = decode(movsd, sizeof(movsd), di);
    TEST_ASSERT(n == 4, "MOVSD length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::MOVSD, "MOVSD mnemonic");
    uint8_t addpd[] = {0x66, 0x0F, 0x58, 0xC8};
    n = decode(addpd, sizeof(addpd), di);
    TEST_ASSERT(n == 4, "ADDPD length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::ADDPD, "ADDPD mnemonic");
    uint8_t pxor[] = {0x66, 0x0F, 0xEF, 0xC0};
    n = decode(pxor, sizeof(pxor), di);
    TEST_ASSERT(n == 4, "PXOR length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::PXOR, "PXOR mnemonic");
}

void test_decode_vex() {
    DecodedInstr di;
    uint8_t vbcast[] = {0xC4, 0xE2, 0x79, 0x18, 0xC8};
    size_t n = decode(vbcast, sizeof(vbcast), di);
    TEST_ASSERT(n == 5, "VBROADCASTSS length");
    TEST_ASSERT(di.has_vex, "VBROADCASTSS has_vex");
    TEST_ASSERT(di.vex_L == 0, "VBROADCASTSS vex_L=0");
    uint8_t andn[] = {0xC4, 0xE2, 0x78, 0xF2, 0xC8};
    n = decode(andn, sizeof(andn), di);
    TEST_ASSERT(n == 5, "ANDN length");
    TEST_ASSERT(di.has_vex, "ANDN has_vex");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::ANDN, "ANDN mnemonic");
    uint8_t vbsd[] = {0xC4, 0xE2, 0x7D, 0x19, 0xC8};
    n = decode(vbsd, sizeof(vbsd), di);
    TEST_ASSERT(n == 5, "VBROADCASTSD length");
    TEST_ASSERT(di.vex_L == 1, "VBROADCASTSD vex_L=1");
}

void test_decode_prefixes() {
    DecodedInstr di;
    uint8_t lock_xchg[] = {0xF0, 0x87, 0x08};
    size_t n = decode(lock_xchg, sizeof(lock_xchg), di);
    TEST_ASSERT(n == 3, "LOCK XCHG length");
    TEST_ASSERT(di.num_prefixes == 1, "LOCK prefix count");
    TEST_ASSERT(di.legacy_prefix[0] == 0xF0, "LOCK prefix byte");
    uint8_t rep_movs[] = {0xF3, 0xA4};
    n = decode(rep_movs, sizeof(rep_movs), di);
    TEST_ASSERT(n == 2, "REP MOVSB length");
    TEST_ASSERT(di.num_prefixes == 1, "REP prefix count");
    uint8_t addr32[] = {0x67, 0x89, 0x00};
    n = decode(addr32, sizeof(addr32), di);
    TEST_ASSERT(n == 3, "ADDR32 prefix length");
}

void test_decode_special() {
    DecodedInstr di;
    uint8_t syscall[] = {0x0F, 0x05};
    size_t n = decode(syscall, sizeof(syscall), di);
    TEST_ASSERT(n == 2, "SYSCALL length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::SYSCALL, "SYSCALL mnemonic");
    uint8_t cpuid[] = {0x0F, 0xA2};
    n = decode(cpuid, sizeof(cpuid), di);
    TEST_ASSERT(n == 2, "CPUID length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::CPUID, "CPUID mnemonic");
    uint8_t rdtsc[] = {0x0F, 0x31};
    n = decode(rdtsc, sizeof(rdtsc), di);
    TEST_ASSERT(n == 2, "RDTSC length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::RDTSC, "RDTSC mnemonic");
    uint8_t endbr64[] = {0xF3, 0x0F, 0x1E, 0xFA};
    n = decode(endbr64, sizeof(endbr64), di);
    TEST_ASSERT(n > 0, "ENDBR64 decodes");
    uint8_t mfence[] = {0x0F, 0xAE, 0xF0};
    n = decode(mfence, sizeof(mfence), di);
    TEST_ASSERT(n == 3, "MFENCE length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::MFENCE, "MFENCE mnemonic");
    uint8_t tzcnt[] = {0xF3, 0x0F, 0xBC, 0xC8};
    n = decode(tzcnt, sizeof(tzcnt), di);
    TEST_ASSERT(n == 4, "TZCNT length");
}

void test_decode_x87() {
    DecodedInstr di;
    uint8_t fld[] = {0xD9, 0xC0};
    size_t n = decode(fld, sizeof(fld), di);
    TEST_ASSERT(n == 2, "FLD length");
    uint8_t fadd_m[] = {0xD8, 0x00};
    n = decode(fadd_m, sizeof(fadd_m), di);
    TEST_ASSERT(n == 2, "FADD mem decodes");
    uint8_t fmul_m[] = {0xD8, 0x08};
    n = decode(fmul_m, sizeof(fmul_m), di);
    TEST_ASSERT(n >= 1, "FMUL mem decodes");
}

void test_decode_rex_operand_size() {
    DecodedInstr di;
    uint8_t mov64[] = {0x48, 0x89, 0xC8};
    size_t n = decode(mov64, sizeof(mov64), di);
    TEST_ASSERT(n == 3, "MOV r64 length");
    TEST_ASSERT((di.rex & 0x08) != 0, "REX.W set");
    uint8_t mov32[] = {0x89, 0xC8};
    n = decode(mov32, sizeof(mov32), di);
    TEST_ASSERT(n == 2, "MOV r32 length");
    TEST_ASSERT(di.rex == 0, "no REX for r32");
    uint8_t mov_ext[] = {0x4D, 0x89, 0xC8};
    n = decode(mov_ext, sizeof(mov_ext), di);
    TEST_ASSERT(n == 3, "MOV r8,r9 length");
    TEST_ASSERT(di.rex == 0x4D, "REX=0x4D");
}

void test_decode_displacement_imm() {
    DecodedInstr di;
    size_t n;
    uint8_t rip_rel[] = {0x48, 0x8B, 0x05, 0x78, 0x56, 0x34, 0x12};
    n = decode(rip_rel, sizeof(rip_rel), di);
    TEST_ASSERT(n == 7, "MOV rax,[rip+disp32] length");
    TEST_ASSERT(di.displacement == 0x12345678, "RIP-relative disp32");
    uint8_t mov_imm64[] = {0x48, 0xB8, 0xEF, 0xCD, 0xAB, 0x90, 0x78, 0x56, 0x34, 0x12};
    n = decode(mov_imm64, sizeof(mov_imm64), di);
    TEST_ASSERT(n == 10, "MOV rax,imm64 length");
    TEST_ASSERT(di.immediate == (int64_t)0x1234567890ABCDEFull, "64-bit immediate value");
    uint8_t neg_sib[] = {0xF7, 0x9C, 0x8B, 0x00, 0x01, 0x00, 0x00};
    n = decode(neg_sib, sizeof(neg_sib), di);
    TEST_ASSERT(n == 7, "NEG [rbx+rcx*4+disp] length");
    TEST_ASSERT(di.displacement == 0x100, "SIB displacement");
    TEST_ASSERT((di.sib & 0xC0) == 0x80, "SIB scale=4 (bits 7:6 = 10)");
    uint8_t push_imm[] = {0x6A, 0x42};
    n = decode(push_imm, sizeof(push_imm), di);
    TEST_ASSERT(n == 2, "PUSH imm8 length");
    TEST_ASSERT(di.immediate == 0x42, "PUSH imm8 value");
    uint8_t nop3[] = {0x0F, 0x1F, 0x00};
    n = decode(nop3, sizeof(nop3), di);
    TEST_ASSERT(n == 3, "3-byte NOP length");
    uint8_t int3[] = {0xCC};
    n = decode(int3, sizeof(int3), di);
    TEST_ASSERT(n == 1, "INT3 length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::INT, "INT3 mnemonic");
    uint8_t hlt[] = {0xF4};
    n = decode(hlt, sizeof(hlt), di);
    TEST_ASSERT(n == 1, "HLT length");
    TEST_ASSERT(di.desc->mnemonic == Mnemonic::HLT, "HLT mnemonic");
}

void test_operand_count() {
    DecodedInstr di;
    { uint8_t c[] = {0x90}; decode(c, 1, di); }
    TEST_ASSERT(di.desc->num_operands <= 2, "NOP operands <= 2");
    { uint8_t c[] = {0xC3}; decode(c, 1, di); }
    TEST_ASSERT(di.desc->num_operands >= 0, "RET operands >= 0");
    { uint8_t c[] = {0x01, 0xC8}; decode(c, 2, di); }
    TEST_ASSERT(di.desc->num_operands >= 2, "ADD reg,reg has >= 2 operands");
    { uint8_t c[] = {0x6B, 0xC8, 0x42}; decode(c, 3, di); }
    TEST_ASSERT(di.desc->num_operands >= 2, "IMUL 3-op has >= 2 operands");
}

#ifdef VEDX64_SEMANTICS
void test_semantics_operand_access() {
    DecodedInstr di;
    uint8_t add[] = {0x01, 0xC8};
    decode(add, sizeof(add), di);
    auto* s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "ADD semantics exists");
    TEST_ASSERT(s->category == Category::Arithmetic, "ADD is Arithmetic");
    TEST_ASSERT(s->flags_written != 0, "ADD writes flags");
    TEST_ASSERT(s->operand_access[0] == Access::ReadWrite, "ADD dst is ReadWrite");
    TEST_ASSERT(s->operand_access[1] == Access::Read, "ADD src is Read");
    TEST_ASSERT(s->flow == FlowType::Sequential, "ADD is Sequential");

    uint8_t cmp[] = {0x39, 0xC8};
    decode(cmp, sizeof(cmp), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "CMP semantics exists");
    TEST_ASSERT(s->category == Category::Arithmetic || s->category == Category::Compare, "CMP is Arithmetic or Compare");
    TEST_ASSERT(s->flags_written != 0, "CMP writes flags");

    uint8_t jmp[] = {0xEB, 0x10};
    decode(jmp, sizeof(jmp), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "JMP semantics exists");
    TEST_ASSERT(s->flow == FlowType::Jump, "JMP is Jump");

    uint8_t call[] = {0xE8, 0x00, 0x01, 0x00, 0x00};
    decode(call, sizeof(call), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "CALL semantics exists");
    TEST_ASSERT(s->flow == FlowType::Call, "CALL is Call");

    uint8_t ret[] = {0xC3};
    decode(ret, sizeof(ret), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "RET semantics exists");
    TEST_ASSERT(s->flow == FlowType::Return, "RET is Return");

    uint8_t nop[] = {0x90};
    decode(nop, sizeof(nop), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "NOP semantics exists");
    TEST_ASSERT(s->category == Category::Nop || s->category == Category::DataMove, "NOP is Nop or DataMove");
    TEST_ASSERT(s->flags_written == 0, "NOP writes no flags");

    uint8_t mov[] = {0x48, 0x89, 0xC1};
    decode(mov, sizeof(mov), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "MOV semantics exists");
    TEST_ASSERT(s->category == Category::DataMove, "MOV is DataMove");
    TEST_ASSERT(s->flags_written == 0, "MOV writes no flags");

    uint8_t push[] = {0x55};
    decode(push, sizeof(push), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "PUSH semantics exists");
    TEST_ASSERT(s->category == Category::Stack, "PUSH is Stack");

    uint8_t syscall[] = {0x0F, 0x05};
    decode(syscall, sizeof(syscall), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "SYSCALL semantics exists");
    TEST_ASSERT(s->flow == FlowType::Syscall, "SYSCALL is Syscall");

    uint8_t test[] = {0x85, 0xC0};
    decode(test, sizeof(test), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "TEST semantics exists");
    TEST_ASSERT(s->category == Category::Logic || s->category == Category::Arithmetic, "TEST is Logic or Arithmetic");
    TEST_ASSERT(s->flags_written != 0, "TEST writes flags");

    uint8_t shl[] = {0xD1, 0xE0};
    decode(shl, sizeof(shl), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "SHL semantics exists");
    TEST_ASSERT(s->category == Category::Shift, "SHL is Shift");

    uint8_t jz[] = {0x74, 0x10};
    decode(jz, sizeof(jz), di);
    s = get_semantics(di);
    TEST_ASSERT(s != nullptr, "JZ semantics exists");
    TEST_ASSERT(s->flow == FlowType::Branch, "JZ is Branch");
    TEST_ASSERT(s->flags_read != 0, "JZ reads flags");
}
#endif // VEDX64_SEMANTICS

void test_roundtrip_advanced() {
    DecodedInstr di;
    uint8_t enc[16];
    size_t n, e;

    { // mov rax, 10
        uint8_t code[] = {0x48, 0xC7, 0xC0, 0x0A, 0x00, 0x00, 0x00};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "mov rax, 10: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "mov rax, 10: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "mov rax, 10: roundtrip match");
    }
    { // add rax, 5
        uint8_t code[] = {0x48, 0x83, 0xC0, 0x05};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "add rax, 5: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "add rax, 5: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "add rax, 5: roundtrip match");
    }
    { // xor rax, rax
        uint8_t code[] = {0x48, 0x31, 0xC0};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "xor rax, rax: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "xor rax, rax: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "xor rax, rax: roundtrip match");
    }
    { // imul eax, ecx
        uint8_t code[] = {0x0F, 0xAF, 0xC1};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "imul eax, ecx: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "imul eax, ecx: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "imul eax, ecx: roundtrip match");
    }
    { // div ecx
        uint8_t code[] = {0xF7, 0xF1};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "div ecx: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "div ecx: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "div ecx: roundtrip match");
    }
    { // lea rax, [rbx+rcx*4+0x10]
        uint8_t code[] = {0x48, 0x8D, 0x44, 0x8B, 0x10};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "lea rax, [rbx+rcx*4+0x10]: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "lea rax, [rbx+rcx*4+0x10]: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "lea rax, [rbx+rcx*4+0x10]: roundtrip match");
    }
    { // movzx eax, cl
        uint8_t code[] = {0x0F, 0xB6, 0xC1};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "movzx eax, cl: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "movzx eax, cl: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "movzx eax, cl: roundtrip match");
    }
    { // pxor xmm0, xmm0
        uint8_t code[] = {0x66, 0x0F, 0xEF, 0xC0};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "pxor xmm0, xmm0: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "pxor xmm0, xmm0: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "pxor xmm0, xmm0: roundtrip match");
    }
    { // addss xmm0, xmm1
        uint8_t code[] = {0xF3, 0x0F, 0x58, 0xC1};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "addss xmm0, xmm1: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "addss xmm0, xmm1: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "addss xmm0, xmm1: roundtrip match");
    }
    { // jz +256
        uint8_t code[] = {0x0F, 0x84, 0x00, 0x01, 0x00, 0x00};
        n = decode(code, sizeof(code), di);
        TEST_ASSERT(n == sizeof(code), "jz +256: decode length");
        e = encode(di, enc, sizeof(enc));
        TEST_ASSERT(e == n, "jz +256: encode length");
        TEST_ASSERT(memcmp(code, enc, n) == 0, "jz +256: roundtrip match");
    }
}

int main() {
    test_table_integrity();
#ifdef VEDX64_STRINGS
    test_mnemonic_names();
#endif // VEDX64_STRINGS
    test_decode_nop();
    test_decode_ret();
    test_decode_push_rbp();
    test_decode_add_reg_reg();
    test_decode_mov_imm32();
    test_decode_rex_w();
    test_decode_two_byte();
    test_decode_jmp_rel8();
    test_decode_jmp_rel8_neg();
    test_decode_modrm_memory();
    test_decode_sib();
    test_decode_sib_disp32();
    test_decode_rip_relative();
    test_decode_prefix_66();
    test_decode_sub_imm8();
    test_decode_add_al_imm();
    test_decode_call_rel32();
    test_decode_lea_sib();
    test_decode_imul_3op();
    test_decode_sse_movsd();
    test_decode_test_imm();
    test_decode_xchg_opcreg();
    test_encode_roundtrip();
#ifdef VEDX64_STRINGS
    test_disassemble();
#endif // VEDX64_STRINGS
    test_decode_edge_cases();
    test_encode_edge_cases();
    test_decode_sse();
    test_decode_vex();
    test_decode_prefixes();
    test_decode_special();
    test_decode_x87();
    test_decode_rex_operand_size();
#ifdef VEDX64_SEMANTICS
    test_semantics_operand_access();
#endif // VEDX64_SEMANTICS
    test_roundtrip_advanced();
    test_decode_displacement_imm();
    test_operand_count();

    printf("\n%d/%d tests passed\n", g_tests_passed, g_tests_run);
    return (g_tests_passed == g_tests_run) ? 0 : 1;
}
