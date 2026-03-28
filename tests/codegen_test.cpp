// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/codegen.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstring>

using namespace vedx64;

static int g_pass = 0, g_fail = 0;

void check(const char* name, const uint8_t* expected, size_t elen, const uint8_t* got, size_t glen) {
    if (elen == glen && memcmp(expected, got, elen) == 0) {
#ifdef VEDX64_STRINGS
        char dis[128];
        disassemble(got, glen, dis, sizeof(dis), 0);
        printf("PASS: %-35s -> %s\n", name, dis);
#else
        printf("PASS: %s\n", name);
#endif // VEDX64_STRINGS
        g_pass++;
    } else {
        printf("FAIL: %s  expected:", name);
        for (size_t i=0;i<elen;i++) printf(" %02X", expected[i]);
        printf("  got:");
        for (size_t i=0;i<glen;i++) printf(" %02X", got[i]);
        printf("\n");
        g_fail++;
    }
}

int main() {
    printf("=== CodeGen API Tests ===\n\n");

    #define TEST_ASM(name, code, ...) do { \
        CodeGen g; code; \
        static const uint8_t exp[] = {__VA_ARGS__}; \
        check(name, exp, sizeof(exp), g.data(), g.size()); \
    } while(0)

    TEST_ASM("nop", g.nop(), 0x90);
    TEST_ASM("ret", g.ret(), 0xC3);
    TEST_ASM("int3", g.int3(), 0xCC);

    TEST_ASM("push rbp", g.push(rbp), 0x55);
    TEST_ASM("push rax", g.push(rax), 0x50);
    TEST_ASM("pop rbp", g.pop(rbp), 0x5D);
    TEST_ASM("push r8", g.push(r8), 0x41, 0x50);

    TEST_ASM("add eax,ecx", g.add(eax,ecx), 0x01, 0xC8);
    TEST_ASM("add rax,rcx", g.add(rax,rcx), 0x48, 0x01, 0xC8);
    TEST_ASM("sub eax,ecx", g.sub(eax,ecx), 0x29, 0xC8);
    TEST_ASM("xor eax,ecx", g.xor_(eax,ecx), 0x31, 0xC8);
    TEST_ASM("mov rbp,rsp", g.mov(rbp,rsp), 0x48, 0x89, 0xE5);
    TEST_ASM("mov r8,rax", g.mov(r8,rax), 0x49, 0x89, 0xC0);

    TEST_ASM("sub esp,0x20", g.sub(esp, 0x20), 0x83, 0xEC, 0x20);
    TEST_ASM("sub rsp,0x20", g.sub(rsp, 0x20), 0x48, 0x83, 0xEC, 0x20);

    TEST_ASM("mov eax,[rcx]", g.mov(eax, ptr(rcx)), 0x8B, 0x01);
    TEST_ASM("mov eax,[rbp-4]", g.mov(eax, ptr(rbp,-4)), 0x8B, 0x45, 0xFC);
    TEST_ASM("mov [rax],ecx", g.mov(ptr(rax), ecx), 0x89, 0x08);

    TEST_ASM("mov eax,[rax+rcx*4]", g.mov(eax, ptr(rax,rcx,4)), 0x8B, 0x04, 0x88);

    TEST_ASM("addps xmm1,xmm0", g.addps(xmm1,xmm0), 0x0F, 0x58, 0xC8);
    TEST_ASM("addpd xmm1,xmm0", g.addpd(xmm1,xmm0), 0x66, 0x0F, 0x58, 0xC8);
    TEST_ASM("movsd xmm1,xmm0", g.movsd(xmm1,xmm0), 0xF2, 0x0F, 0x10, 0xC8);

    {
        CodeGen g;
        auto loop = g.label();
        g.bind(loop);
        g.nop();
        g.jmp(loop);
        // Expected: 90 E9 FA FF FF FF (nop; jmp back to offset 0, disp = 0-6 = -6)
        static const uint8_t exp[] = {0x90, 0xE9, 0xFA, 0xFF, 0xFF, 0xFF};
        check("jmp backward label", exp, sizeof(exp), g.data(), g.size());
    }

    {
        CodeGen g;
        auto skip = g.label();
        g.jmp(skip);
        g.nop();
        g.bind(skip);
        g.ret();
        // Expected: E9 01 00 00 00 90 C3 (jmp +1; nop; ret)
        static const uint8_t exp[] = {0xE9, 0x01, 0x00, 0x00, 0x00, 0x90, 0xC3};
        check("jmp forward label", exp, sizeof(exp), g.data(), g.size());
    }

    {
        CodeGen g;
        g.push(rbp);
        g.mov(rbp, rsp);
        g.sub(rsp, 0x20);
        g.add(rsp, 0x20);
        g.pop(rbp);
        g.ret();
        static const uint8_t exp[] = {0x55, 0x48,0x89,0xE5, 0x48,0x83,0xEC,0x20, 0x48,0x83,0xC4,0x20, 0x5D, 0xC3};
        check("function prologue/epilogue", exp, sizeof(exp), g.data(), g.size());
    }

    printf("\n--- Prefix override tests ---\n");
    TEST_ASM("mov rax,fs:[rbx]", g.mov(rax, fs(ptr(rbx))), 0x64, 0x48, 0x8B, 0x03);
    TEST_ASM("mov rax,gs:[rbx+8]", g.mov(rax, gs(ptr(rbx, 8))), 0x65, 0x48, 0x8B, 0x43, 0x08);
    TEST_ASM("mov eax,fs:[rcx]", g.mov(eax, fs_ptr(rcx)), 0x64, 0x8B, 0x01);
    TEST_ASM("mov eax,gs:[rax+0x10]", g.mov(eax, gs_ptr(rax, 0x10)), 0x65, 0x8B, 0x40, 0x10);
    TEST_ASM("lock add [rax],ecx", g.lock().add(ptr(rax), ecx), 0xF0, 0x01, 0x08);
    TEST_ASM("data16 nop", g.data16().nop(), 0x66, 0x90);
    TEST_ASM("addr32 mov eax,[ecx]", g.addr32().mov(eax, ptr(ecx)), 0x67, 0x8B, 0x01);
    TEST_ASM("rep movsb", g.rep_movsb(), 0xF3, 0xA4);
    TEST_ASM("repne scasb", g.repne_scasb(), 0xF2, 0xAE);
    TEST_ASM("lock xadd [rcx],eax", g.lock().xadd(ptr(rcx), eax), 0xF0, 0x0F, 0xC1, 0x01);
    TEST_ASM("prefix(Lock) nop", g.prefix(Prefix::Lock).nop(), 0xF0, 0x90);
    TEST_ASM("prefix(FS) rep movsb", g.prefix(Prefix::FS).rep_movsb(), 0x64, 0xF3, 0xA4);
    {
        CodeGen g;
        auto skip = g.label();
        g.prefix(Prefix::BranchNotTaken).jz(skip);
        g.nop();
        g.bind(skip);
        // 2E 0F 84 01 00 00 00 90  (hint-not-taken jz +1; nop)
        static const uint8_t exp[] = {0x2E, 0x0F, 0x84, 0x01, 0x00, 0x00, 0x00, 0x90};
        check("prefix(BranchNotTaken) jz", exp, sizeof(exp), g.data(), g.size());
    }
    TEST_ASM("prefix(Addr32) mov", g.prefix(Prefix::Addr32).mov(eax, ptr(ecx)), 0x67, 0x8B, 0x01);
    TEST_ASM("mov ss:[rbx],eax", g.mov(ss(ptr(rbx)), eax), 0x36, 0x89, 0x03);

    printf("\n%d/%d codegen tests passed\n", g_pass, g_pass+g_fail);
    return g_fail > 0 ? 1 : 0;
}
