// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski


#include "vedx64/analysis.hpp"
#include "vedx64/instruction.hpp"
#include "vedx64/codegen.hpp"
#include <cstdio>
#include <cstring>

using namespace vedx64;
using namespace vedx64::analysis;

static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { if (cond) ++g_pass; else { ++g_fail; std::printf("FAIL: %s\n", msg); } } while (0)

int main() {
    // ----- Mnemonic queries -----
    CHECK(is_jcc(Mnemonic::JE),                 "is_jcc(JE)");
    CHECK(is_jcc(Mnemonic::JNE),                "is_jcc(JNE)");
    CHECK(!is_jcc(Mnemonic::JMP),               "!is_jcc(JMP)");
    CHECK(is_unconditional_branch(Mnemonic::JMP),"is_unconditional_branch(JMP)");
    CHECK(is_call(Mnemonic::CALL),              "is_call(CALL)");
    CHECK(is_ret(Mnemonic::RETN),               "is_ret(RETN)");
    CHECK(changes_rip(Mnemonic::CALL),          "changes_rip(CALL)");
    CHECK(changes_rip(Mnemonic::HLT),           "changes_rip(HLT)");
    CHECK(!changes_rip(Mnemonic::ADD),          "!changes_rip(ADD)");
    CHECK(is_arith(Mnemonic::ADD),              "is_arith(ADD)");
    CHECK(is_logical(Mnemonic::AND),            "is_logical(AND)");
    CHECK(is_shift(Mnemonic::SHL),              "is_shift(SHL)");
    CHECK(jcc_condition(Mnemonic::JE)  == CondCode::Z,  "jcc_condition(JE)=Z");
    CHECK(jcc_condition(Mnemonic::JG)  == CondCode::NLE,"jcc_condition(JG)=NLE");
    CHECK(jcc_for_condition(CondCode::Z) == Mnemonic::JZ,"jcc_for_condition(Z)=JZ");
    CHECK(is_cmov(Mnemonic::CMOVE),             "is_cmov(CMOVE)");
    CHECK(cmov_condition(Mnemonic::CMOVNE) == CondCode::NZ, "cmov_condition(CMOVNE)=NZ");
    CHECK(canonical_size(Mnemonic::MOVSB) == 1, "canonical_size(MOVSB)=1");
    CHECK(canonical_size(Mnemonic::MOVSQ) == 8, "canonical_size(MOVSQ)=8");
    CHECK(canonical_size(Mnemonic::ADD)   == 0, "canonical_size(ADD)=0 (variable)");

    // ----- EFLAGS read/write -----
    CHECK((sets_eflags(Mnemonic::ADD)  & EF_CF) != 0,"ADD writes CF");
    CHECK((sets_eflags(Mnemonic::ADD)  & EF_ZF) != 0,"ADD writes ZF");
    CHECK((sets_eflags(Mnemonic::INC)  & EF_CF) == 0,"INC does NOT write CF");
    CHECK((sets_eflags(Mnemonic::CLC)) == EF_CF,     "CLC writes only CF");
    CHECK((reads_eflags(Mnemonic::JE)  & EF_ZF) != 0,"JE reads ZF");
    CHECK((reads_eflags(Mnemonic::JG)  & (EF_ZF|EF_SF|EF_OF)) == (EF_ZF|EF_SF|EF_OF),
                                                     "JG reads ZF|SF|OF");
    CHECK((reads_eflags(Mnemonic::ADC) & EF_CF) != 0,"ADC reads CF");
    CHECK((reads_eflags(Mnemonic::CMOVE) & EF_ZF) != 0,"CMOVE reads ZF");

    // ----- Patchers -----
    {
        uint8_t buf[5] = {0,0,0,0,0};
        patch_jmp_rel32(buf, 0x12345678);
        CHECK(buf[0] == 0xE9,                            "patch_jmp_rel32 opcode");
        int32_t rel = 0; std::memcpy(&rel, buf+1, 4);
        CHECK(rel == 0x12345678,                          "patch_jmp_rel32 disp");
    }
    {
        uint8_t buf[6] = {0,0,0,0,0,0};
        patch_jcc_rel32(buf, CondCode::Z, -16);
        CHECK(buf[0] == 0x0F && buf[1] == 0x84,           "patch_jcc_rel32 prefix=0F 84 (JZ)");
        int32_t rel = 0; std::memcpy(&rel, buf+2, 4);
        CHECK(rel == -16,                                  "patch_jcc_rel32 disp");
    }
    {
        uint8_t buf[10] = {0};
        patch_mov_imm64(buf, 1 /* RCX */, 0xCAFEBABEDEADBEEFULL);
        CHECK(buf[0] == 0x48 && buf[1] == 0xB9,            "patch_mov_imm64 REX.W + B8+rcx");
        uint64_t v = 0; std::memcpy(&v, buf+2, 8);
        CHECK(v == 0xCAFEBABEDEADBEEFULL,                  "patch_mov_imm64 imm");
    }
    {
        uint8_t buf[5];
        fill_nop(buf, sizeof(buf));
        for (size_t i = 0; i < sizeof(buf); ++i) CHECK(buf[i] == 0x90, "fill_nop");
        fill_int3(buf, sizeof(buf));
        for (size_t i = 0; i < sizeof(buf); ++i) CHECK(buf[i] == 0xCC, "fill_int3");
    }

    // ----- Pattern matchers -----
    {
        // SUB rsp, 0x20 → 48 83 EC 20
        uint8_t code[] = {0x48, 0x83, 0xEC, 0x20};
        int64_t imm = 0;
        CHECK(is_sub_rsp_imm(code, sizeof(code), &imm),    "is_sub_rsp_imm");
        CHECK(imm == 0x20,                                  "SUB rsp imm value");
    }
    {
        // ADD rsp, 0x20 → 48 83 C4 20
        uint8_t code[] = {0x48, 0x83, 0xC4, 0x20};
        CHECK(is_add_rsp_imm(code, sizeof(code), nullptr),  "is_add_rsp_imm");
    }
    {
        // MOV rbp, rsp → 48 89 E5
        uint8_t code[] = {0x48, 0x89, 0xE5};
        CHECK(is_mov_rbp_rsp(code, sizeof(code)),           "is_mov_rbp_rsp");
    }
    {
        // Win64 prologue: PUSH rbp; MOV rbp,rsp; SUB rsp,0x20
        uint8_t code[] = {0x55, 0x48, 0x89, 0xE5, 0x48, 0x83, 0xEC, 0x20, 0x90 /* nop sentinel */};
        size_t off = skip_prologue(code, sizeof(code));
        CHECK(off == 8,                                     "skip_prologue covers PUSH/MOV/SUB");
    }
    {
        // PUSH 0x42 (6A 42); CALL rel32 (E8 disp32)
        uint8_t code[] = {0x6A, 0x42, 0xE8, 0x00, 0x00, 0x00, 0x00};
        int64_t imm = 0; uint64_t target = 0;
        CHECK(match_push_imm_call(code, sizeof(code), 0x1000, &imm, &target),
                                                            "match_push_imm_call");
        CHECK(imm == 0x42,                                   "push imm = 0x42");
        CHECK(target == 0x1007,                              "call target = 0x1007");
    }

    // ----- DecodedInstr introspection -----
    {
        // SUB rsp, 0x20 → 48 83 EC 20
        uint8_t code[] = {0x48, 0x83, 0xEC, 0x20};
        DecodedInstr di;
        CHECK(decode(code, sizeof(code), di) == 4,           "decode SUB rsp,0x20");
        CHECK(reg_op_id(di, 0) == 4,                          "reg_op_id(0)=RSP");
        CHECK(is_imm_op(di, 1),                                "is_imm_op(1)");
        CHECK(imm_value(di) == 0x20,                          "imm_value=0x20");
    }
    {
        // MOV [rbp - 4], edi → 89 7D FC
        uint8_t code[] = {0x89, 0x7D, 0xFC};
        DecodedInstr di;
        CHECK(decode(code, sizeof(code), di) == 3,           "decode MOV [rbp-4], edi");
        CHECK(is_mem_op(di, 0),                               "is_mem_op(0)");
        CHECK(mem_base(di) == 5,                              "mem_base=RBP");
        CHECK(mem_disp(di) == -4,                             "mem_disp=-4");
        CHECK(reg_op_id(di, 1) == 7,                          "reg_op_id(1)=RDI");
    }

    // ----- Reg::name (codegen extension) -----
    CHECK(std::strcmp(Reg{0,64}.name(), "rax") == 0,         "Reg{0,64}.name=rax");
    CHECK(std::strcmp(Reg{1,8}.name(),  "cl")  == 0,         "Reg{1,8}.name=cl");
    CHECK(std::strcmp(Reg{15,32}.name(),"r15d")== 0,         "Reg{15,32}.name=r15d");

    // ----- DemandPagedMemory -----
    {
        DemandPagedMemory mem;
        mem.write_u64(0x1234, 0xDEADBEEFCAFEBABEULL);
        CHECK(mem.read_u64(0x1234) == 0xDEADBEEFCAFEBABEULL, "DemandPagedMemory roundtrip");
        CHECK(mem.read_u8(0x9999) == 0xCC,                    "untouched page reads as 0xCC");
        CHECK(mem.is_committed(0x1234, 8),                    "is_committed");
        CHECK(!mem.is_committed(0x100000, 1),                 "!is_committed unmapped");
        // Page-spanning write
        mem.write_u32(0x1FFE, 0x11223344);
        CHECK(mem.read_u32(0x1FFE) == 0x11223344,             "cross-page write/read");
        CHECK(mem.page_count() >= 2,                           "at least 2 pages allocated");
    }

    // ----- apply_sparse_gprs -----
    {
        uint64_t gpr[16] = {0};
        std::unordered_map<uint8_t, uint64_t> seed = { {1, 0x42}, {7, 0x99} };
        apply_sparse_gprs(gpr, seed);
        CHECK(gpr[1] == 0x42 && gpr[7] == 0x99 && gpr[0] == 0, "apply_sparse_gprs");
    }

    // ----- backward_chain -----
    {
        // Build: MOV rax, rcx ; MOV rbx, rax ; (then ask "where does rbx come from?")
        std::vector<Instruction> insns;
        Instruction a;
        a.mnemonic = Mnemonic::MOV;
        a.operands.push_back(Operand::make_reg(RegisterClass::GPR64, 0 /*rax*/, 64));
        a.operands.push_back(Operand::make_reg(RegisterClass::GPR64, 1 /*rcx*/, 64));
        insns.push_back(a);
        Instruction b;
        b.mnemonic = Mnemonic::MOV;
        b.operands.push_back(Operand::make_reg(RegisterClass::GPR64, 3 /*rbx*/, 64));
        b.operands.push_back(Operand::make_reg(RegisterClass::GPR64, 0 /*rax*/, 64));
        insns.push_back(b);
        uint8_t source = 0xFF;
        size_t idx = backward_chain(insns, 1, 3 /*rbx*/, &source);
        // No definer that's not a forwarder is found → idx == insns.size()
        CHECK(idx == 2,                                        "backward_chain: no non-forwarder definer");
        CHECK(source == 1,                                     "backward_chain followed rbx<-rax<-rcx");
    }

    std::printf("\n%d/%d analysis tests passed\n", g_pass, g_pass + g_fail);
    return g_fail > 0 ? 1 : 0;
}
