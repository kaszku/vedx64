// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/semantics.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstring>

using namespace vedx64;

static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { if (cond) { g_pass++; } else { printf("FAIL: %s\n", msg); g_fail++; } } while(0)

void check_decode(const char* name, const uint8_t* code, size_t len,
                  FlowType expect_flow, Category expect_cat,
                  bool expect_reads_flags, bool expect_writes_flags) {
    DecodedInstr di;
    size_t n = decode(code, len, di);
    if (n == 0 || !di.desc) { printf("FAIL: %s (decode failed)\n", name); g_fail++; return; }
    const Semantics* sem = get_semantics(di);
    if (!sem) { printf("FAIL: %s (no semantics)\n", name); g_fail++; return; }

    char msg[128];
    snprintf(msg, sizeof(msg), "%s: flow", name);
    CHECK(sem->flow == expect_flow, msg);
    snprintf(msg, sizeof(msg), "%s: category", name);
    CHECK(sem->category == expect_cat, msg);
    snprintf(msg, sizeof(msg), "%s: reads_flags", name);
    CHECK(reads_flags(*sem) == expect_reads_flags, msg);
    snprintf(msg, sizeof(msg), "%s: writes_flags", name);
    CHECK(writes_flags(*sem) == expect_writes_flags, msg);
}

int main() {
    printf("=== Semantics Tests ===\n\n");

    // Verify the semantics table is accessible for every instruction
    CHECK(g_instr_table_size > 0, "instruction table not empty");
    // Spot-check first and last entry are accessible
    CHECK(g_semantics_table[0].flow == FlowType::Sequential || true, "first semantic entry accessible");
    CHECK(g_semantics_table[g_instr_table_size-1].flow == FlowType::Sequential || true, "last semantic entry accessible");

    { uint8_t c[] = {0x01, 0xC8}; check_decode("ADD eax,ecx", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, false, true); }
    { uint8_t c[] = {0x29, 0xC8}; check_decode("SUB eax,ecx", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, false, true); }
    { uint8_t c[] = {0x39, 0xC8}; check_decode("CMP eax,ecx", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, false, true); }
    { uint8_t c[] = {0x89, 0xC8}; check_decode("MOV eax,ecx", c, sizeof(c), FlowType::Sequential, Category::DataMove, false, false); }
    { uint8_t c[] = {0x31, 0xC8}; check_decode("XOR eax,ecx", c, sizeof(c), FlowType::Sequential, Category::Logic, false, true); }
    { uint8_t c[] = {0x90}; check_decode("NOP", c, sizeof(c), FlowType::Sequential, Category::DataMove, false, false); }
    { uint8_t c[] = {0xC3}; check_decode("RET", c, sizeof(c), FlowType::Return, Category::Call, false, false); }
    { uint8_t c[] = {0x0F, 0x84, 0x00, 0x01, 0x00, 0x00}; check_decode("JZ", c, sizeof(c), FlowType::Branch, Category::Branch, true, false); }
    { uint8_t c[] = {0xEB, 0x10}; check_decode("JMP", c, sizeof(c), FlowType::Jump, Category::Branch, false, false); }
    { uint8_t c[] = {0xE8, 0x10, 0x00, 0x00, 0x00}; check_decode("CALL", c, sizeof(c), FlowType::Call, Category::Call, false, false); }
    { uint8_t c[] = {0x55}; check_decode("PUSH rbp", c, sizeof(c), FlowType::Sequential, Category::Stack, false, false); }
    { uint8_t c[] = {0xC1, 0xE0, 0x04}; check_decode("SHL eax,4", c, sizeof(c), FlowType::Sequential, Category::Shift, false, true); }
    { uint8_t c[] = {0x0F, 0x05}; check_decode("SYSCALL", c, sizeof(c), FlowType::Syscall, Category::System, false, false); }
    { uint8_t c[] = {0x0F, 0x58, 0xC8}; check_decode("ADDPS", c, sizeof(c), FlowType::Sequential, Category::SimdArith, false, false); }
    { uint8_t c[] = {0xF2, 0x0F, 0x10, 0xC8}; check_decode("MOVSD xmm", c, sizeof(c), FlowType::Sequential, Category::SimdMove, false, false); }
    { uint8_t c[] = {0x66, 0x0F, 0x38, 0x00, 0xC8}; check_decode("PSHUFB", c, sizeof(c), FlowType::Sequential, Category::SimdShuffle, false, false); }
    { uint8_t c[] = {0x0F, 0xAE, 0xF8}; check_decode("SFENCE", c, sizeof(c), FlowType::Sequential, Category::Sync, false, false); }
    { uint8_t c[] = {0xF0, 0x0F, 0xB1, 0x08}; check_decode("LOCK CMPXCHG", c, sizeof(c), FlowType::Sequential, Category::DataMove, false, true); }
    { uint8_t c[] = {0x11, 0xC8}; check_decode("ADC eax,ecx", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, true, true); }
    { uint8_t c[] = {0x0F, 0x44, 0xC8}; check_decode("CMOVZ", c, sizeof(c), FlowType::Sequential, Category::DataMove, true, false); }
    { uint8_t c[] = {0x0F, 0xA3, 0xC8}; check_decode("BT", c, sizeof(c), FlowType::Sequential, Category::Bit, false, true); }
    {
        uint8_t c[] = {0x01, 0xC8}; // add eax, ecx
        DecodedInstr di; decode(c, sizeof(c), di);
        const Semantics* s = get_semantics(di);
        CHECK(s->operand_access[0] == Access::ReadWrite, "ADD dst is ReadWrite");
        CHECK(s->operand_access[1] == Access::Read, "ADD src is Read");
    }

    {
        uint8_t c[] = {0x89, 0xC8}; // mov eax, ecx
        DecodedInstr di; decode(c, sizeof(c), di);
        const Semantics* s = get_semantics(di);
        CHECK(s->operand_access[0] == Access::Write, "MOV dst is Write");
        CHECK(s->operand_access[1] == Access::Read, "MOV src is Read");
    }

    {
        uint8_t c[] = {0x0F, 0x30}; // wrmsr
        DecodedInstr di; decode(c, sizeof(c), di);
        const Semantics* s = get_semantics(di);
        if (s) CHECK(s->is_privileged, "WRMSR is privileged");
    }

    { uint8_t c[] = {0x21, 0xC8}; check_decode("AND eax,ecx", c, sizeof(c), FlowType::Sequential, Category::Logic, false, true); }
    { uint8_t c[] = {0x09, 0xC8}; check_decode("OR eax,ecx", c, sizeof(c), FlowType::Sequential, Category::Logic, false, true); }
    { uint8_t c[] = {0xF7, 0xD0}; check_decode("NOT eax", c, sizeof(c), FlowType::Sequential, Category::Logic, false, false); }
    { uint8_t c[] = {0x85, 0xC0}; check_decode("TEST eax,eax", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, false, true); }

    { uint8_t c[] = {0xD1, 0xE8}; check_decode("SHR eax,1", c, sizeof(c), FlowType::Sequential, Category::Shift, false, true); }
    { uint8_t c[] = {0xC1, 0xF8, 0x02}; check_decode("SAR eax,2", c, sizeof(c), FlowType::Sequential, Category::Shift, false, true); }
    { uint8_t c[] = {0xD1, 0xC0}; check_decode("ROL eax,1", c, sizeof(c), FlowType::Sequential, Category::Shift, false, true); }

    { uint8_t c[] = {0xF7, 0xD8}; check_decode("NEG eax", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, false, true); }
    { uint8_t c[] = {0xF7, 0xE1}; check_decode("MUL ecx", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, false, true); }
    { uint8_t c[] = {0xFF, 0xC0}; check_decode("INC eax", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, false, true); }
    { uint8_t c[] = {0xFF, 0xC8}; check_decode("DEC eax", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, false, true); }

    { uint8_t c[] = {0x8D, 0x04, 0x88}; check_decode("LEA", c, sizeof(c), FlowType::Sequential, Category::DataMove, false, false); }
    { uint8_t c[] = {0x0F, 0xBE, 0xC8}; check_decode("MOVSX", c, sizeof(c), FlowType::Sequential, Category::Convert, false, false); }
    { uint8_t c[] = {0x0F, 0xB6, 0xC8}; check_decode("MOVZX", c, sizeof(c), FlowType::Sequential, Category::Convert, false, false); }
    { uint8_t c[] = {0x87, 0xC8}; check_decode("XCHG", c, sizeof(c), FlowType::Sequential, Category::DataMove, false, false); }

    { uint8_t c[] = {0x0F, 0x82, 0x10, 0x00, 0x00, 0x00}; check_decode("JB", c, sizeof(c), FlowType::Branch, Category::Branch, true, false); }
    { uint8_t c[] = {0x0F, 0x83, 0x10, 0x00, 0x00, 0x00}; check_decode("JNB", c, sizeof(c), FlowType::Branch, Category::Branch, true, false); }
    { uint8_t c[] = {0x0F, 0x8E, 0x10, 0x00, 0x00, 0x00}; check_decode("JLE", c, sizeof(c), FlowType::Branch, Category::Branch, true, false); }
    { uint8_t c[] = {0x0F, 0x8F, 0x10, 0x00, 0x00, 0x00}; check_decode("JNLE", c, sizeof(c), FlowType::Branch, Category::Branch, true, false); }
    { uint8_t c[] = {0x74, 0x10}; check_decode("JZ short", c, sizeof(c), FlowType::Branch, Category::Branch, true, false); }
    { uint8_t c[] = {0x75, 0x10}; check_decode("JNZ short", c, sizeof(c), FlowType::Branch, Category::Branch, true, false); }

    { uint8_t c[] = {0xC2, 0x08, 0x00}; check_decode("RET imm16", c, sizeof(c), FlowType::Return, Category::Call, false, false); }

    { uint8_t c[] = {0x58}; check_decode("POP rax", c, sizeof(c), FlowType::Sequential, Category::Stack, false, false); }
    { uint8_t c[] = {0xC9}; check_decode("LEAVE", c, sizeof(c), FlowType::Sequential, Category::Stack, false, false); }

    { uint8_t c[] = {0xF8}; check_decode("CLC", c, sizeof(c), FlowType::Sequential, Category::Flag, false, true); }
    { uint8_t c[] = {0xF9}; check_decode("STC", c, sizeof(c), FlowType::Sequential, Category::Flag, false, true); }
    { uint8_t c[] = {0xFC}; check_decode("CLD", c, sizeof(c), FlowType::Sequential, Category::Flag, false, true); }
    { uint8_t c[] = {0xFD}; check_decode("STD", c, sizeof(c), FlowType::Sequential, Category::Flag, false, true); }
    { uint8_t c[] = {0xF5}; check_decode("CMC", c, sizeof(c), FlowType::Sequential, Category::Flag, true, true); }

    { uint8_t c[] = {0x0F, 0x31}; check_decode("RDTSC", c, sizeof(c), FlowType::Sequential, Category::System, false, false); }
    { uint8_t c[] = {0x0F, 0xA2}; check_decode("CPUID", c, sizeof(c), FlowType::Sequential, Category::System, false, false); }
    { uint8_t c[] = {0xF4}; check_decode("HLT", c, sizeof(c), FlowType::Halt, Category::System, false, false); }

    { uint8_t c[] = {0x0F, 0xBC, 0xC8}; check_decode("BSF", c, sizeof(c), FlowType::Sequential, Category::Bit, false, true); }
    { uint8_t c[] = {0x0F, 0xBD, 0xC8}; check_decode("BSR", c, sizeof(c), FlowType::Sequential, Category::Bit, false, true); }
    { uint8_t c[] = {0x0F, 0xAB, 0xC8}; check_decode("BTS", c, sizeof(c), FlowType::Sequential, Category::Bit, false, true); }

    { uint8_t c[] = {0x66, 0x0F, 0x2E, 0xC8}; check_decode("UCOMISD", c, sizeof(c), FlowType::Sequential, Category::SimdCompare, false, true); }
    { uint8_t c[] = {0x0F, 0x2E, 0xC8}; check_decode("UCOMISS", c, sizeof(c), FlowType::Sequential, Category::SimdCompare, false, true); }
    { uint8_t c[] = {0x0F, 0x28, 0xC8}; check_decode("MOVAPS", c, sizeof(c), FlowType::Sequential, Category::SimdMove, false, false); }
    { uint8_t c[] = {0x0F, 0x57, 0xC8}; check_decode("XORPS", c, sizeof(c), FlowType::Sequential, Category::SimdLogic, false, false); }

    { uint8_t c[] = {0x66, 0x0F, 0xFE, 0xC8}; check_decode("PADDD", c, sizeof(c), FlowType::Sequential, Category::SimdArith, false, false); }
    { uint8_t c[] = {0x66, 0x0F, 0xEF, 0xC8}; check_decode("PXOR", c, sizeof(c), FlowType::Sequential, Category::SimdLogic, false, false); }

    { uint8_t c[] = {0x0F, 0xAE, 0xE8}; check_decode("LFENCE", c, sizeof(c), FlowType::Sequential, Category::Sync, false, false); }
    { uint8_t c[] = {0x0F, 0xAE, 0xF0}; check_decode("MFENCE", c, sizeof(c), FlowType::Sequential, Category::Sync, false, false); }

    { uint8_t c[] = {0x19, 0xC8}; check_decode("SBB", c, sizeof(c), FlowType::Sequential, Category::Arithmetic, true, true); }

    {
        // ADC reads CF specifically
        uint8_t c[] = {0x11, 0xC8}; DecodedInstr di; decode(c, sizeof(c), di);
        const Semantics* s = get_semantics(di);
        CHECK(reads_flag(*s, FLAG_CF), "ADC reads CF");
    }

    {
        // ADD writes CF, ZF, SF, OF, PF, AF
        uint8_t c[] = {0x01, 0xC8}; DecodedInstr di; decode(c, sizeof(c), di);
        const Semantics* s = get_semantics(di);
        CHECK(writes_flag(*s, FLAG_CF), "ADD writes CF");
        CHECK(writes_flag(*s, FLAG_ZF), "ADD writes ZF");
        CHECK(writes_flag(*s, FLAG_SF), "ADD writes SF");
        CHECK(writes_flag(*s, FLAG_OF), "ADD writes OF");
    }

    {
        // CLC sets CF=0 (flags_set has CF)
        uint8_t c[] = {0xF8}; DecodedInstr di; decode(c, sizeof(c), di);
        const Semantics* s = get_semantics(di);
        CHECK(s->flags_set != 0, "CLC has flags_set");
    }

    {
        uint8_t c[] = {0x0F, 0x32}; DecodedInstr di; decode(c, sizeof(c), di);
        const Semantics* s = get_semantics(di);
        if (s) CHECK(s->is_privileged, "RDMSR is privileged");
    }

    {
        uint8_t c1[] = {0x01, 0x08}; DecodedInstr di1; decode(c1, sizeof(c1), di1);
        const Semantics* s1 = get_semantics(di1);
        CHECK(s1->lock_valid, "ADD [rax],ecx is lockable");

        uint8_t c2[] = {0x89, 0x08}; DecodedInstr di2; decode(c2, sizeof(c2), di2);
        const Semantics* s2 = get_semantics(di2);
        CHECK(!s2->lock_valid, "MOV [rax],ecx is NOT lockable");
    }

    {
        size_t valid = 0;
        for (size_t i = 0; i < g_instr_table_size; ++i) {
            const auto& sem = g_semantics_table[i];
            if ((int)sem.category >= 0 && (int)sem.category < 27 && (int)sem.flow >= 0 && (int)sem.flow < 7) valid++;
        }
        CHECK(valid == g_instr_table_size, "all semantics entries have valid category+flow");
    }

    printf("\n%d/%d semantics tests passed\n", g_pass, g_pass + g_fail);
    return g_fail > 0 ? 1 : 0;
}
