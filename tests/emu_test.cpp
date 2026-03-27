// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/emu.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace vedx64;

static int tests_run = 0, tests_passed = 0;
#define CHECK(cond, msg) do { tests_run++; if (!(cond)) { printf("FAIL: %s (line %d)\n", msg, __LINE__); } else { tests_passed++; } } while(0)

static void test_add() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0x01; mem[0x101] = 0xD8;
    cpu.gpr[0] = 3; cpu.gpr[3] = 4;
    StepResult r = emu_step(cpu);
    CHECK(r == StepResult::OK, "add step ok");
    CHECK(cpu.gpr[0] == 7, "add 3+4=7");
    CHECK(!(cpu.rflags & RFLAG_ZF), "add ZF clear");
    CHECK(!(cpu.rflags & RFLAG_CF), "add CF clear");
    CHECK(!(cpu.rflags & RFLAG_SF), "add SF clear");
}

static void test_sub() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0x29; mem[0x101] = 0xD8;
    cpu.gpr[0] = 10; cpu.gpr[3] = 7;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 3, "sub 10-7=3");
    CHECK(!(cpu.rflags & RFLAG_CF), "sub CF clear");
}

static void test_xor_self() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0x31; mem[0x101] = 0xC0;
    cpu.gpr[0] = 42;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 0, "xor eax,eax = 0");
    CHECK(cpu.rflags & RFLAG_ZF, "xor ZF set");
}

static void test_cmp_jz() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0x39; mem[0x101] = 0xD8;
    mem[0x102] = 0x74; mem[0x103] = 0x02;
    mem[0x104] = 0x90; mem[0x105] = 0x90;
    mem[0x106] = 0xF4;
    cpu.gpr[0] = 5; cpu.gpr[3] = 5;
    emu_step(cpu);
    emu_step(cpu);
    CHECK(cpu.rip == 0x106, "jz taken when equal");
}

static void test_push_pop() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    cpu.rsp() = 0x800;
    mem[0x100] = 0x50;
    cpu.rax() = 0xDEADBEEF;
    emu_step(cpu);
    CHECK(cpu.rsp() == 0x7F8, "push decrements rsp by 8");
    uint64_t pushed = 0;
    memcpy(&pushed, mem + 0x7F8, 8);
    CHECK(pushed == 0xDEADBEEF, "push writes value to stack");
    mem[0x101] = 0x5B;
    emu_step(cpu);
    CHECK(cpu.rsp() == 0x800, "pop restores rsp");
    CHECK(cpu.rbx() == 0xDEADBEEF, "pop reads value from stack");
}

static void test_mov() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0xB8; mem[0x101] = 0x2A; mem[0x102] = 0; mem[0x103] = 0; mem[0x104] = 0;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 42, "mov eax, 42");
}

static void test_call_ret() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    cpu.rsp() = 0x800;
    mem[0x100] = 0xE8; mem[0x101] = 0x05; mem[0x102] = 0; mem[0x103] = 0; mem[0x104] = 0;
    mem[0x10A] = 0xC3;
    emu_step(cpu);
    CHECK(cpu.rip == 0x10A, "call jumps to target");
    CHECK(cpu.rsp() == 0x7F8, "call pushes return address");
    emu_step(cpu);
    CHECK(cpu.rip == 0x105, "ret returns to caller");
    CHECK(cpu.rsp() == 0x800, "ret restores rsp");
}

static void test_loop_sum() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0xB9; mem[0x101] = 0x0A; mem[0x102] = 0; mem[0x103] = 0; mem[0x104] = 0;
    mem[0x105] = 0x31; mem[0x106] = 0xC0;
    mem[0x107] = 0x01; mem[0x108] = 0xC8;
    mem[0x109] = 0xE2; mem[0x10A] = 0xFC;
    mem[0x10B] = 0xF4;
    emu_run(cpu, 100);
    CHECK(cpu.gpr[0] == 55, "loop sum 1..10 = 55");
    CHECK(cpu.gpr[1] == 0, "loop ecx = 0 after loop");
}

static void test_lea() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0x8D; mem[0x101] = 0x44; mem[0x102] = 0x8B; mem[0x103] = 0x08;
    cpu.gpr[3] = 100; cpu.gpr[1] = 10;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 148, "lea [rbx+rcx*4+8] = 100+40+8 = 148");
}

static void test_flags_overflow() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0x01; mem[0x101] = 0xD8;
    cpu.gpr[0] = 0x7FFFFFFFFFFFFFFFULL; cpu.gpr[3] = 1;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 0x8000000000000000ULL, "add overflow result");
    CHECK(cpu.rflags & RFLAG_OF, "add OF set on overflow");
    CHECK(cpu.rflags & RFLAG_SF, "add SF set on negative result");
}

static void test_inc_dec() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0xFF; mem[0x101] = 0xC0;
    cpu.gpr[0] = 41;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 42, "inc eax");
    mem[0x104] = 0xFF; mem[0x105] = 0xC8;
    cpu.rip = 0x104;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 41, "dec eax");
}

static void test_neg() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0xF7; mem[0x101] = 0xD8;
    cpu.gpr[0] = 5;
    emu_step(cpu);
    CHECK((uint32_t)cpu.gpr[0] == (uint32_t)-5, "neg 5 = -5");
    CHECK(cpu.rflags & RFLAG_CF, "neg CF set (nonzero)");
}

static void test_and_or() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0x21; mem[0x101] = 0xD8;
    cpu.gpr[0] = 0xFF; cpu.gpr[3] = 0x0F;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 0x0F, "and 0xFF & 0x0F = 0x0F");
    mem[0x104] = 0x09; mem[0x105] = 0xD8;
    cpu.rip = 0x104;
    cpu.gpr[0] = 0xF0; cpu.gpr[3] = 0x0F;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 0xFF, "or 0xF0 | 0x0F = 0xFF");
}

static void test_shl_shr() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    mem[0x100] = 0xD3; mem[0x101] = 0xE0;
    cpu.gpr[0] = 1; cpu.gpr[1] = 4;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 16, "shl 1 << 4 = 16");
    cpu.rip = 0x104;
    mem[0x104] = 0xD3; mem[0x105] = 0xE8;
    cpu.gpr[0] = 16; cpu.gpr[1] = 2;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 4, "shr 16 >> 2 = 4");
}

static void test_rep_stosb() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    // REP STOSB: F3 AA
    mem[0x100] = 0xF3; mem[0x101] = 0xAA;
    cpu.gpr[0] = 0x42; // AL = 0x42
    cpu.gpr[1] = 8;    // RCX = 8
    cpu.gpr[7] = 0x200; // RDI = 0x200
    emu_step(cpu);
    CHECK(cpu.gpr[1] == 0, "rep stosb: rcx=0");
    CHECK(mem[0x200] == 0x42 && mem[0x207] == 0x42, "rep stosb: mem filled");
    CHECK(cpu.gpr[7] == 0x208, "rep stosb: rdi advanced");
}

static void test_rep_movsb() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    // Set up source data
    for (int i = 0; i < 4; i++) mem[0x200 + i] = (uint8_t)(i + 1);
    // REP MOVSB: F3 A4
    mem[0x100] = 0xF3; mem[0x101] = 0xA4;
    cpu.gpr[1] = 4;     // RCX
    cpu.gpr[6] = 0x200; // RSI
    cpu.gpr[7] = 0x300; // RDI
    emu_step(cpu);
    CHECK(cpu.gpr[1] == 0, "rep movsb: rcx=0");
    CHECK(mem[0x300] == 1 && mem[0x303] == 4, "rep movsb: data copied");
}

static void test_repnz_scasb() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    // Buffer: 'h','e','l','l','o'
    mem[0x200] = 'h'; mem[0x201] = 'e'; mem[0x202] = 'l'; mem[0x203] = 'l'; mem[0x204] = 'o';
    // REPNZ SCASB: F2 AE
    mem[0x100] = 0xF2; mem[0x101] = 0xAE;
    cpu.gpr[0] = 'l';   // AL = 'l'
    cpu.gpr[1] = 5;     // RCX
    cpu.gpr[7] = 0x200; // RDI
    emu_step(cpu);
    CHECK(cpu.gpr[7] == 0x203, "repnz scasb: found 'l' at index 2");
    CHECK(cpu.rflags & RFLAG_ZF, "repnz scasb: ZF set on match");
}

static void test_leave() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    // LEAVE: C9
    mem[0x100] = 0xC9;
    cpu.gpr[5] = 0x400; // RBP
    // Write saved RBP on stack at 0x400
    uint64_t saved_rbp = 0x500;
    memcpy(mem + 0x400, &saved_rbp, 8);
    emu_step(cpu);
    CHECK(cpu.gpr[4] == 0x408, "leave: rsp = old rbp + 8");
    CHECK(cpu.gpr[5] == 0x500, "leave: rbp restored");
}

static void test_cmpxchg() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    // CMPXCHG EBX, ECX: 0F B1 CB
    mem[0x100] = 0x0F; mem[0x101] = 0xB1; mem[0x102] = 0xCB;
    // Success case: EAX == EBX, so EBX gets ECX
    cpu.gpr[0] = 42; cpu.gpr[3] = 42; cpu.gpr[1] = 99;
    emu_step(cpu);
    CHECK(cpu.gpr[3] == 99, "cmpxchg success: dst=src");
    CHECK(cpu.rflags & RFLAG_ZF, "cmpxchg success: ZF set");
    // Failure case: EAX != EBX, so EAX gets EBX
    cpu.rip = 0x100;
    cpu.gpr[0] = 10; cpu.gpr[3] = 20; cpu.gpr[1] = 99;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 20, "cmpxchg fail: eax=dst");
    CHECK(!(cpu.rflags & RFLAG_ZF), "cmpxchg fail: ZF clear");
}

static void test_shld_shrd() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    // SHLD EAX, EBX, 4: 0F A4 D8 04
    mem[0x100] = 0x0F; mem[0x101] = 0xA4; mem[0x102] = 0xD8; mem[0x103] = 0x04;
    cpu.gpr[0] = 0x12345678; cpu.gpr[3] = 0xABCDEF00;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 0x123456780ULL, "shld rax,rbx,4");
    // SHRD EAX, EBX, 4: 0F AC D8 04
    cpu.rip = 0x104;
    mem[0x104] = 0x0F; mem[0x105] = 0xAC; mem[0x106] = 0xD8; mem[0x107] = 0x04;
    cpu.gpr[0] = 0x12345678; cpu.gpr[3] = 0xABCDEF00;
    emu_step(cpu);
    CHECK(cpu.gpr[0] == 0x01234567, "shrd eax,ebx,4");
}

static void test_fpu_basic() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;
    // FLD1: D9 E8
    mem[0x100] = 0xD9; mem[0x101] = 0xE8;
    emu_step(cpu);
    CHECK(cpu.fpu_stack[cpu.fpu_top] == 1.0, "fld1 pushes 1.0");
    // FLDPI: D9 EB
    mem[0x102] = 0xD9; mem[0x103] = 0xEB;
    emu_step(cpu);
    CHECK(cpu.fpu_stack[cpu.fpu_top] > 3.14 && cpu.fpu_stack[cpu.fpu_top] < 3.15, "fldpi pushes pi");
    // FADDP: DE C1
    mem[0x104] = 0xDE; mem[0x105] = 0xC1;
    emu_step(cpu);
    double result = cpu.fpu_stack[cpu.fpu_top];
    CHECK(result > 4.14 && result < 4.15, "faddp: 1.0 + pi ~ 4.14");
    // FSTP to memory (64-bit): DD 1D disp32
    // Actually simpler: use DD 5D 00 with RBP pointing to a buffer
    // Let's just verify the stack state is correct
}

static void test_fma() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;

    float v1[] = {1.0f,0,0,0}; memcpy(cpu.zmm[0], v1, 16);
    float v2[] = {2.0f,0,0,0}; memcpy(cpu.zmm[1], v2, 16);
    float v3[] = {3.0f,0,0,0}; memcpy(cpu.zmm[2], v3, 16);
    mem[0x100]=0xC4; mem[0x101]=0xE2; mem[0x102]=0x71; mem[0x103]=0xB8; mem[0x104]=0xC2;
    emu_step(cpu);
    float fma_r; memcpy(&fma_r, &cpu.zmm[0][0], 4);
    CHECK(fma_r == 7.0f, "vfmadd231ps: 2*3+1=7");

    cpu.zmm[0][0] = 5.0; cpu.zmm[1][0] = 3.0; cpu.zmm[2][0] = 10.0;
    cpu.rip = 0x110;
    mem[0x110]=0xC4; mem[0x111]=0xE2; mem[0x112]=0xF1; mem[0x113]=0xA9; mem[0x114]=0xC2;
    emu_step(cpu);
    CHECK(cpu.zmm[0][0] == 25.0, "vfmadd213sd: 3*5+10=25");

    cpu.zmm[0][0] = 100.0; cpu.zmm[1][0] = 3.0; cpu.zmm[2][0] = 10.0;
    cpu.rip = 0x120;
    mem[0x120]=0xC4; mem[0x121]=0xE2; mem[0x122]=0xF1; mem[0x123]=0xBD; mem[0x124]=0xC2;
    emu_step(cpu);
    CHECK(cpu.zmm[0][0] == 70.0, "vfnmadd231sd: -(3*10)+100=70");

    float fs1=6.0f; memcpy(&cpu.zmm[0][0], &fs1, 4);
    float fs2=2.0f; memcpy(&cpu.zmm[1][0], &fs2, 4);
    float fs3=5.0f; memcpy(&cpu.zmm[2][0], &fs3, 4);
    cpu.rip = 0x130;
    mem[0x130]=0xC4; mem[0x131]=0xE2; mem[0x132]=0x71; mem[0x133]=0x9B; mem[0x134]=0xC2;
    emu_step(cpu);
    float fms_r; memcpy(&fms_r, &cpu.zmm[0][0], 4);
    CHECK(fms_r == 28.0f, "vfmsub132ss: 6*5-2=28");
}

static void test_broadcast() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;

    float bval = 42.0f; memcpy(&cpu.zmm[1][0], &bval, 4);
    mem[0x100]=0xC4; mem[0x101]=0xE2; mem[0x102]=0x7D; mem[0x103]=0x18; mem[0x104]=0xC1;
    emu_step(cpu);
    float br[8]; memcpy(br, cpu.zmm[0], 32);
    CHECK(br[0] == 42.0f && br[3] == 42.0f && br[7] == 42.0f, "vbroadcastss ymm: all lanes 42");

    uint32_t dval = 0xDEADBEEF; memcpy(&cpu.zmm[2][0], &dval, 4);
    cpu.rip = 0x110;
    mem[0x110]=0xC4; mem[0x111]=0xE2; mem[0x112]=0x79; mem[0x113]=0x58; mem[0x114]=0xC2;
    emu_step(cpu);
    uint32_t dr[4]; memcpy(dr, cpu.zmm[0], 16);
    CHECK(dr[0] == 0xDEADBEEF && dr[1] == 0xDEADBEEF && dr[3] == 0xDEADBEEF, "vpbroadcastd xmm: all dwords");
}

static void test_vinsert_vextract() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;

    cpu.zmm[1][0]=1.0; cpu.zmm[1][1]=2.0; cpu.zmm[1][2]=3.0; cpu.zmm[1][3]=4.0;
    cpu.zmm[2][0]=10.0; cpu.zmm[2][1]=20.0;
    mem[0x100]=0xC4; mem[0x101]=0xE3; mem[0x102]=0x75; mem[0x103]=0x18; mem[0x104]=0xC2; mem[0x105]=0x01;
    emu_step(cpu);
    CHECK(cpu.zmm[0][0] == 1.0, "vinsertf128: low[0] from src1");
    CHECK(cpu.zmm[0][1] == 2.0, "vinsertf128: low[1] from src1");
    CHECK(cpu.zmm[0][2] == 10.0, "vinsertf128: high[0] from src2");
    CHECK(cpu.zmm[0][3] == 20.0, "vinsertf128: high[1] from src2");
}

static void test_vperm2f128() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;

    cpu.zmm[1][0]=1.0; cpu.zmm[1][1]=2.0; cpu.zmm[1][2]=3.0; cpu.zmm[1][3]=4.0;
    cpu.zmm[2][0]=5.0; cpu.zmm[2][1]=6.0; cpu.zmm[2][2]=7.0; cpu.zmm[2][3]=8.0;
    mem[0x100]=0xC4; mem[0x101]=0xE3; mem[0x102]=0x75; mem[0x103]=0x06; mem[0x104]=0xC2; mem[0x105]=0x31;
    emu_step(cpu);
    CHECK(cpu.zmm[0][0] == 3.0, "vperm2f128: low[0]=YMM1 high[0]");
    CHECK(cpu.zmm[0][1] == 4.0, "vperm2f128: low[1]=YMM1 high[1]");
    CHECK(cpu.zmm[0][2] == 7.0, "vperm2f128: high[0]=YMM2 high[0]");
    CHECK(cpu.zmm[0][3] == 8.0, "vperm2f128: high[1]=YMM2 high[1]");
}

static void test_vzero() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));

    for (int r = 0; r < 4; r++) for (int i = 0; i < 4; i++) cpu.zmm[r][i] = 99.0;
    cpu.rip = 0x100;
    mem[0x100]=0xC5; mem[0x101]=0xF8; mem[0x102]=0x77;
    emu_step(cpu);
    CHECK(cpu.zmm[0][0] == 99.0, "vzeroupper: low unchanged");
    CHECK(cpu.zmm[0][1] == 99.0, "vzeroupper: low[1] unchanged");
    CHECK(cpu.zmm[0][2] == 0.0, "vzeroupper: upper[0] zeroed");
    CHECK(cpu.zmm[0][3] == 0.0, "vzeroupper: upper[1] zeroed");
    CHECK(cpu.zmm[3][2] == 0.0, "vzeroupper: reg3 upper zeroed");

    for (int r = 0; r < 4; r++) for (int i = 0; i < 4; i++) cpu.zmm[r][i] = 99.0;
    cpu.rip = 0x110;
    mem[0x110]=0xC5; mem[0x111]=0xFC; mem[0x112]=0x77;
    emu_step(cpu);
    CHECK(cpu.zmm[0][0] == 0.0 && cpu.zmm[0][1] == 0.0, "vzeroall: reg0 low zeroed");
    CHECK(cpu.zmm[3][0] == 0.0 && cpu.zmm[3][3] == 0.0, "vzeroall: reg3 all zeroed");
}

static void test_avx2_vpaddd() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;

    uint32_t a[] = {1,2,3,4,5,6,7,8};
    uint32_t b[] = {10,20,30,40,50,60,70,80};
    memcpy(cpu.zmm[1], a, 32); memcpy(cpu.zmm[2], b, 32);
    mem[0x100]=0xC5; mem[0x101]=0xF5; mem[0x102]=0xFE; mem[0x103]=0xC2;
    emu_step(cpu);
    uint32_t r[8]; memcpy(r, cpu.zmm[0], 32);
    CHECK(r[0] == 11, "vpaddd ymm: elem0 = 1+10");
    CHECK(r[3] == 44, "vpaddd ymm: elem3 = 4+40");
    CHECK(r[7] == 88, "vpaddd ymm: elem7 = 8+80");
}

static void test_evex_addps_512() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;

    float z1[16], z2[16];
    for (int i = 0; i < 16; i++) { z1[i] = (float)(i+1); z2[i] = (float)((i+1)*10); }
    memcpy(cpu.zmm[1], z1, 64); memcpy(cpu.zmm[2], z2, 64);
    mem[0x100]=0x62; mem[0x101]=0xF1; mem[0x102]=0x74; mem[0x103]=0x48;
    mem[0x104]=0x58; mem[0x105]=0xC2;
    emu_step(cpu);
    float zr[16]; memcpy(zr, cpu.zmm[0], 64);
    CHECK(zr[0] == 11.0f, "evex vaddps 512: elem0 = 1+10");
    CHECK(zr[7] == 88.0f, "evex vaddps 512: elem7 = 8+80");
    CHECK(zr[15] == 176.0f, "evex vaddps 512: elem15 = 16+160");
}

static void test_vpermq() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;

    cpu.zmm[1][0]=10.0; cpu.zmm[1][1]=20.0; cpu.zmm[1][2]=30.0; cpu.zmm[1][3]=40.0;
    mem[0x100]=0xC4; mem[0x101]=0xE3; mem[0x102]=0xFD; mem[0x103]=0x00; mem[0x104]=0xC1; mem[0x105]=0x1B;
    emu_step(cpu);
    CHECK(cpu.zmm[0][0] == 40.0, "vpermq: elem0 = src[3]");
    CHECK(cpu.zmm[0][1] == 30.0, "vpermq: elem1 = src[2]");
    CHECK(cpu.zmm[0][2] == 20.0, "vpermq: elem2 = src[1]");
    CHECK(cpu.zmm[0][3] == 10.0, "vpermq: elem3 = src[0]");
}

static void test_vex_basic() {
    uint8_t mem[4096] = {};
    CpuState cpu;
    emu_init(cpu, mem, sizeof(mem));
    cpu.rip = 0x100;

    // Set up XMM1 = [1.0, 2.0, 3.0, 4.0] (packed single)
    float vals1[] = {1.0f, 2.0f, 3.0f, 4.0f};
    memcpy(cpu.zmm[1], vals1, 16);
    // Set up XMM2 = [10.0, 20.0, 30.0, 40.0]
    float vals2[] = {10.0f, 20.0f, 30.0f, 40.0f};
    memcpy(cpu.zmm[2], vals2, 16);

    // VADDPS XMM0, XMM1, XMM2: C5 F0 58 C2
    // C5 = 2-byte VEX, F0 = R=1,vvvv=0001(~=14),L=0,pp=00, 58=ADDPS, C2=modrm(xmm0,xmm2)
    // vvvv = ~(0xF0 >> 3) & 0xF = ~14 & 0xF = 1  -> XMM1
    // Actually: C5 byte2: R=1(bit7), vvvv=1110(bits6-3, inverted=0001=1), L=0(bit2), pp=00(bits1-0)
    // byte2 = 0b11110000 = 0xF0
    mem[0x100] = 0xC5; mem[0x101] = 0xF0; mem[0x102] = 0x58; mem[0x103] = 0xC2;
    emu_step(cpu);
    float result[4]; memcpy(result, cpu.zmm[0], 16);
    CHECK(result[0] == 11.0f, "vaddps: elem0 = 1+10 = 11");
    CHECK(result[1] == 22.0f, "vaddps: elem1 = 2+20 = 22");
    CHECK(result[2] == 33.0f, "vaddps: elem2 = 3+30 = 33");
    CHECK(result[3] == 44.0f, "vaddps: elem3 = 4+40 = 44");

    // Verify upper 128 bits are zeroed (VEX.L=0)
    cpu.zmm[0][2] = 999.0; cpu.zmm[0][3] = 999.0;
    cpu.rip = 0x100;
    emu_step(cpu);
    CHECK(cpu.zmm[0][2] == 0.0, "vaddps L=0: upper zeroed[2]");
    CHECK(cpu.zmm[0][3] == 0.0, "vaddps L=0: upper zeroed[3]");

    // VXORPS XMM3, XMM3, XMM3 (zero idiom): C5 E0 57 DB
    // vvvv=0011(inverted=1100=0xC), pp=00, L=0
    // byte2: R=1, vvvv=1100, L=0, pp=00 = 0b11100000 = 0xE0
    cpu.zmm[3][0] = 42.0; cpu.zmm[3][1] = 42.0;
    cpu.rip = 0x104;
    mem[0x104] = 0xC5; mem[0x105] = 0xE0; mem[0x106] = 0x57; mem[0x107] = 0xDB;
    emu_step(cpu);
    CHECK(cpu.zmm[3][0] == 0.0 && cpu.zmm[3][1] == 0.0, "vxorps xmm3,xmm3,xmm3 zeroes");
}

int main() {
    test_add();
    test_sub();
    test_xor_self();
    test_cmp_jz();
    test_push_pop();
    test_mov();
    test_call_ret();
    test_loop_sum();
    test_lea();
    test_flags_overflow();
    test_inc_dec();
    test_neg();
    test_and_or();
    test_shl_shr();
    test_rep_stosb();
    test_rep_movsb();
    test_repnz_scasb();
    test_leave();
    test_cmpxchg();
    test_shld_shrd();
    test_fpu_basic();
    test_fma();
    test_broadcast();
    test_vinsert_vextract();
    test_vperm2f128();
    test_vzero();
    test_avx2_vpaddd();
    test_evex_addps_512();
    test_vpermq();
    test_vex_basic();

    printf("vedx64_emu: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
