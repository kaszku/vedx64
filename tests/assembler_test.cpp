// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#ifdef VEDX64_ASSEMBLER
#include "vedx64/assembler.hpp"
#include "vedx64/core.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

static int g_pass = 0, g_fail = 0;

#define CHECK(cond, msg) do { if (!(cond)) { printf("FAIL: %s\n", msg); g_fail++; } else g_pass++; } while(0)

static bool asm_ok(const char* text) {
    auto bytes = vedx64::assemble(text);
    if (!bytes || bytes->empty()) return false;
    vedx64::DecodedInstr di;
    auto len = vedx64::decode(bytes->data(), bytes->size(), di);
    return len > 0 && di.length > 0;
}

static bool asm_byte(const char* text, uint8_t expected, size_t expected_len = 0) {
    auto bytes = vedx64::assemble(text);
    if (!bytes || bytes->empty()) return false;
    if (expected_len > 0 && bytes->size() != expected_len) return false;
    return (*bytes)[0] == expected;
}

int main() {
    printf("=== Assembler Tests ===\n");

    printf("  Zero-operand...\n");
    CHECK(asm_byte("nop", 0x90, 1), "nop");
    CHECK(asm_byte("ret", 0xC3, 1), "ret");
    CHECK(asm_byte("int3", 0xCC, 1), "int3");
    CHECK(asm_ok("hlt"), "hlt");
    CHECK(asm_ok("clc"), "clc");
    CHECK(asm_ok("stc"), "stc");
    CHECK(asm_ok("cmc"), "cmc");
    CHECK(asm_ok("cld"), "cld");
    CHECK(asm_ok("std"), "std_");
    CHECK(asm_ok("syscall"), "syscall");
    CHECK(asm_ok("cdq"), "cdq");
    CHECK(asm_ok("cbw"), "cbw");
    CHECK(asm_ok("cwde"), "cwde");
    CHECK(asm_ok("lahf"), "lahf");
    CHECK(asm_ok("sahf"), "sahf");
    CHECK(asm_ok("cpuid"), "cpuid");
    CHECK(asm_ok("rdtsc"), "rdtsc");

    printf("  Single register...\n");
    CHECK(asm_byte("push rbp", 0x55, 1), "push rbp");
    CHECK(asm_byte("push rax", 0x50, 1), "push rax");
    CHECK(asm_ok("push r12"), "push r12");
    CHECK(asm_ok("pop rbp"), "pop rbp");
    CHECK(asm_ok("pop r15"), "pop r15");
    CHECK(asm_ok("inc rax"), "inc rax");
    CHECK(asm_ok("inc ecx"), "inc ecx");
    CHECK(asm_ok("dec rdx"), "dec rdx");
    CHECK(asm_ok("neg rax"), "neg rax");
    CHECK(asm_ok("not rax"), "not rax");
    CHECK(asm_ok("bswap eax"), "bswap eax");
    CHECK(asm_ok("bswap rcx"), "bswap rcx");

    printf("  Register-register...\n");
    CHECK(asm_ok("mov rax, rcx"), "mov rr 64");
    CHECK(asm_ok("mov eax, ecx"), "mov rr 32");
    CHECK(asm_ok("mov ax, cx"), "mov rr 16");
    CHECK(asm_ok("mov al, cl"), "mov rr 8");
    CHECK(asm_ok("mov r8, r15"), "mov rr ext");
    CHECK(asm_ok("add rax, rcx"), "add rr");
    CHECK(asm_ok("sub rax, rcx"), "sub rr");
    CHECK(asm_ok("xor eax, eax"), "xor rr");
    CHECK(asm_ok("and rax, rdx"), "and rr");
    CHECK(asm_ok("or rax, rdx"), "or rr");
    CHECK(asm_ok("cmp rax, rcx"), "cmp rr");
    CHECK(asm_ok("test rax, rcx"), "test rr");
    CHECK(asm_ok("xchg rax, rcx"), "xchg rr");
    CHECK(asm_ok("adc rax, rcx"), "adc rr");
    CHECK(asm_ok("sbb rax, rcx"), "sbb rr");
    CHECK(asm_ok("bt eax, ecx"), "bt rr");
    CHECK(asm_ok("bts eax, ecx"), "bts rr");
    CHECK(asm_ok("btr eax, ecx"), "btr rr");
    CHECK(asm_ok("bsf eax, ecx"), "bsf rr");
    CHECK(asm_ok("bsr eax, ecx"), "bsr rr");
    CHECK(asm_ok("movzx eax, cl"), "movzx rr");
    CHECK(asm_ok("movsx rax, ecx"), "movsx rr");
    CHECK(asm_ok("cmovb eax, ecx"), "cmovb rr");
    CHECK(asm_ok("cmovl eax, ecx"), "cmovl rr");

    printf("  Register-immediate...\n");
    CHECK(asm_ok("mov rax, 0x1234567890"), "mov ri 64");
    CHECK(asm_ok("mov eax, 42"), "mov ri 32");
    CHECK(asm_ok("mov ax, 0x1234"), "mov ri 16");
    CHECK(asm_ok("mov al, 0xFF"), "mov ri 8");
    CHECK(asm_ok("add rax, 42"), "add ri");
    CHECK(asm_ok("sub rsp, 0x20"), "sub ri");
    CHECK(asm_ok("xor eax, 0xFF"), "xor ri");
    CHECK(asm_ok("and eax, 0x0F"), "and ri");
    CHECK(asm_ok("or eax, 0x80"), "or ri");
    CHECK(asm_ok("cmp rax, 0"), "cmp ri");
    CHECK(asm_ok("test eax, 0xFF"), "test ri");
    CHECK(asm_ok("shl eax, 4"), "shl ri");
    CHECK(asm_ok("shr rax, 1"), "shr ri");
    CHECK(asm_ok("sar rax, 8"), "sar ri");
    CHECK(asm_ok("rol eax, 3"), "rol ri");
    CHECK(asm_ok("ror eax, 5"), "ror ri");

    printf("  Memory operands...\n");
    CHECK(asm_ok("mov rax, [rbx]"), "mov rm base");
    CHECK(asm_ok("mov rax, [rbx+8]"), "mov rm base+disp");
    CHECK(asm_ok("mov rax, [rbx-8]"), "mov rm base-disp");
    CHECK(asm_ok("mov rax, [rbx+rcx*4]"), "mov rm sib");
    CHECK(asm_ok("mov rax, [rbx+rcx*4+16]"), "mov rm sib+disp");
    CHECK(asm_ok("mov rax, [rbx+rcx*8]"), "mov rm scale8");
    CHECK(asm_ok("mov rax, [rbx+rcx*2+0x100]"), "mov rm scale2+bigdisp");
    CHECK(asm_ok("mov eax, [rsp]"), "mov rm rsp");
    CHECK(asm_ok("mov eax, [rsp+4]"), "mov rm rsp+disp");
    CHECK(asm_ok("mov rax, [rbp+0]"), "mov rm rbp+0");
    CHECK(asm_ok("add rax, [rbx]"), "add rm");
    CHECK(asm_ok("sub rax, [rcx+8]"), "sub rm");
    CHECK(asm_ok("cmp rax, [rdx]"), "cmp rm");
    CHECK(asm_ok("xor eax, [rsi]"), "xor rm");
    CHECK(asm_ok("and rax, [rdi+rcx*2]"), "and rm sib");
    CHECK(asm_ok("or eax, [rbx+16]"), "or rm disp");
    CHECK(asm_ok("lea rax, [rbx+rcx*4+8]"), "lea rm sib+disp");
    CHECK(asm_ok("lea rax, [rsp+0x20]"), "lea rm rsp+disp");
    CHECK(asm_ok("mov [rax], rcx"), "mov mr base");
    CHECK(asm_ok("mov [rax+8], rcx"), "mov mr base+disp");
    CHECK(asm_ok("mov [rax+rcx*4], rdx"), "mov mr sib");
    CHECK(asm_ok("mov [rsp], rax"), "mov mr rsp");
    CHECK(asm_ok("mov [rsp+8], rbp"), "mov mr rsp+disp");
    CHECK(asm_ok("add [rbx], rax"), "add mr");
    CHECK(asm_ok("sub [rcx+16], rdx"), "sub mr");
    CHECK(asm_ok("xor [rdi], rsi"), "xor mr");
    CHECK(asm_ok("add dword [rax], 42"), "add mi dword");
    CHECK(asm_ok("sub qword [rbx], 1"), "sub mi qword");
    CHECK(asm_ok("cmp dword [rsp+8], 0"), "cmp mi rsp+disp");
    CHECK(asm_ok("and dword [rax], 0xFF"), "and mi");
    CHECK(asm_ok("or dword [rcx+4], 0x80"), "or mi");
    CHECK(asm_ok("mov dword [rax], 100"), "mov mi dword");
    CHECK(asm_ok("mov qword [rbx+8], 0"), "mov mi qword");
    CHECK(asm_ok("mov byte [rax], 0x41"), "mov mi byte");
    CHECK(asm_ok("mov word [rax], 0x1234"), "mov mi word");
    CHECK(asm_ok("add byte [rax], 1"), "add mi byte");
    CHECK(asm_ok("add word [rbx], 100"), "add mi word");
    CHECK(asm_ok("cmp byte [rcx], 0"), "cmp mi byte");

    printf("  XMM register-register...\n");
    CHECK(asm_ok("movaps xmm0, xmm1"), "movaps xx");
    CHECK(asm_ok("movups xmm2, xmm3"), "movups xx");
    CHECK(asm_ok("addps xmm0, xmm1"), "addps xx");
    CHECK(asm_ok("subps xmm0, xmm1"), "subps xx");
    CHECK(asm_ok("mulps xmm0, xmm1"), "mulps xx");
    CHECK(asm_ok("divps xmm0, xmm1"), "divps xx");
    CHECK(asm_ok("addpd xmm0, xmm1"), "addpd xx");
    CHECK(asm_ok("subpd xmm0, xmm1"), "subpd xx");
    CHECK(asm_ok("mulpd xmm0, xmm1"), "mulpd xx");
    CHECK(asm_ok("divpd xmm0, xmm1"), "divpd xx");
    CHECK(asm_ok("addss xmm0, xmm1"), "addss xx");
    CHECK(asm_ok("subss xmm0, xmm1"), "subss xx");
    CHECK(asm_ok("mulss xmm0, xmm1"), "mulss xx");
    CHECK(asm_ok("divss xmm0, xmm1"), "divss xx");
    CHECK(asm_ok("addsd xmm0, xmm1"), "addsd xx");
    CHECK(asm_ok("subsd xmm0, xmm1"), "subsd xx");
    CHECK(asm_ok("xorps xmm0, xmm1"), "xorps xx");
    CHECK(asm_ok("xorpd xmm0, xmm1"), "xorpd xx");
    CHECK(asm_ok("andps xmm0, xmm1"), "andps xx");
    CHECK(asm_ok("andpd xmm0, xmm1"), "andpd xx");
    CHECK(asm_ok("orps xmm0, xmm1"), "orps xx");
    CHECK(asm_ok("orpd xmm0, xmm1"), "orpd xx");
    CHECK(asm_ok("movapd xmm4, xmm5"), "movapd xx");
    CHECK(asm_ok("movdqa xmm6, xmm7"), "movdqa xx");
    CHECK(asm_ok("movdqu xmm8, xmm9"), "movdqu xx");
    CHECK(asm_ok("paddb xmm0, xmm1"), "paddb xx");
    CHECK(asm_ok("paddw xmm0, xmm1"), "paddw xx");
    CHECK(asm_ok("paddd xmm0, xmm1"), "paddd xx");
    CHECK(asm_ok("paddq xmm0, xmm1"), "paddq xx");
    CHECK(asm_ok("psubb xmm0, xmm1"), "psubb xx");
    CHECK(asm_ok("psubw xmm0, xmm1"), "psubw xx");
    CHECK(asm_ok("psubd xmm0, xmm1"), "psubd xx");
    CHECK(asm_ok("pxor xmm0, xmm1"), "pxor xx");
    CHECK(asm_ok("pand xmm0, xmm1"), "pand xx");
    CHECK(asm_ok("por xmm0, xmm1"), "por xx");
    CHECK(asm_ok("pcmpeqb xmm0, xmm1"), "pcmpeqb xx");
    CHECK(asm_ok("pcmpeqd xmm0, xmm1"), "pcmpeqd xx");
    CHECK(asm_ok("pmullw xmm0, xmm1"), "pmullw xx");
    CHECK(asm_ok("sqrtps xmm0, xmm1"), "sqrtps xx");
    CHECK(asm_ok("sqrtpd xmm0, xmm1"), "sqrtpd xx");
    CHECK(asm_ok("sqrtss xmm0, xmm1"), "sqrtss xx");
    CHECK(asm_ok("sqrtsd xmm0, xmm1"), "sqrtsd xx");
    CHECK(asm_ok("minps xmm0, xmm1"), "minps xx");
    CHECK(asm_ok("maxps xmm0, xmm1"), "maxps xx");
    CHECK(asm_ok("minpd xmm0, xmm1"), "minpd xx");
    CHECK(asm_ok("maxpd xmm0, xmm1"), "maxpd xx");

    printf("  XMM with memory...\n");
    CHECK(asm_ok("movaps xmm0, [rax]"), "movaps xm");
    CHECK(asm_ok("movups xmm0, [rbx+16]"), "movups xm");
    CHECK(asm_ok("addps xmm0, [rcx]"), "addps xm");
    CHECK(asm_ok("subpd xmm1, [rdx+8]"), "subpd xm");
    CHECK(asm_ok("mulss xmm0, [rax]"), "mulss xm");
    CHECK(asm_ok("divsd xmm0, [rbx]"), "divsd xm");
    CHECK(asm_ok("movdqa xmm0, [rsp]"), "movdqa xm");
    CHECK(asm_ok("movdqu xmm0, [rsp+0x10]"), "movdqu xm");
    CHECK(asm_ok("movaps [rax], xmm0"), "movaps mx");
    CHECK(asm_ok("movups [rbx+16], xmm1"), "movups mx");
    CHECK(asm_ok("movdqa [rsp], xmm0"), "movdqa mx");
    CHECK(asm_ok("movdqu [rcx+8], xmm2"), "movdqu mx");

    printf("  XMM 3-operand...\n");
    CHECK(asm_ok("shufps xmm0, xmm1, 0"), "shufps xxi");
    CHECK(asm_ok("cmpps xmm0, xmm1, 0"), "cmpps xxi");
    CHECK(asm_ok("pslld xmm0, 4"), "pslld xi");
    CHECK(asm_ok("psrld xmm0, 4"), "psrld xi");
    CHECK(asm_ok("psllq xmm0, 1"), "psllq xi");

    printf("  3-operand GPR...\n");
    CHECK(asm_ok("imul eax, ecx, 42"), "imul rri");
    CHECK(asm_ok("imul rax, rcx, 100"), "imul rri 64");
    CHECK(asm_ok("shld eax, ecx, 4"), "shld rri");
    CHECK(asm_ok("shrd eax, ecx, 4"), "shrd rri");

    printf("  Register sizes...\n");
    CHECK(asm_ok("add al, 1"), "add ri 8");
    CHECK(asm_ok("add ax, 1"), "add ri 16");
    CHECK(asm_ok("add eax, 1"), "add ri 32");
    CHECK(asm_ok("add rax, 1"), "add ri 64");
    CHECK(asm_ok("mov r8, r9"), "mov rr ext64");
    CHECK(asm_ok("mov r8d, r9d"), "mov rr ext32");
    CHECK(asm_ok("mov r8w, r9w"), "mov rr ext16");
    CHECK(asm_ok("mov r8b, r9b"), "mov rr ext8");
    CHECK(asm_ok("xor r12d, r12d"), "xor r12d");
    CHECK(asm_ok("add r13, 0x100"), "add r13 imm");

    printf("  Cross-type GPR/XMM...\n");
    CHECK(asm_ok("movd xmm0, eax"), "movd xr");
    CHECK(asm_ok("cvtsi2ss xmm0, eax"), "cvtsi2ss xr");
    CHECK(asm_ok("cvtsi2sd xmm0, eax"), "cvtsi2sd xr");
    CHECK(asm_ok("cvttss2si eax, xmm0"), "cvttss2si rx");
    CHECK(asm_ok("cvttsd2si eax, xmm0"), "cvttsd2si rx");

    printf("  Miscellaneous...\n");
    CHECK(asm_ok("xchg eax, ecx"), "xchg");
    CHECK(asm_ok("cmpxchg [rax], rcx"), "cmpxchg mr");
    CHECK(asm_ok("xadd [rax], rcx"), "xadd mr");
    CHECK(asm_ok("movnti [rax], ecx"), "movnti mr");
    CHECK(asm_ok("lfence"), "lfence");
    CHECK(asm_ok("sfence"), "sfence");
    CHECK(asm_ok("mfence"), "mfence");
    CHECK(asm_ok("pause"), "pause");
    CHECK(asm_ok("rdmsr"), "rdmsr");
    CHECK(asm_ok("wrmsr"), "wrmsr");

    printf("  Aliases...\n");
    CHECK(asm_ok("cmova eax, ecx"), "cmova");
    CHECK(asm_ok("cmovae eax, ecx"), "cmovae");
    CHECK(asm_ok("cmove eax, ecx"), "cmove");
    CHECK(asm_ok("cmovne eax, ecx"), "cmovne");
    CHECK(asm_ok("cmovg eax, ecx"), "cmovg");
    CHECK(asm_ok("cmovge eax, ecx"), "cmovge");
    CHECK(asm_ok("cmovc eax, ecx"), "cmovc");
    CHECK(asm_ok("cmovnc eax, ecx"), "cmovnc");
    CHECK(asm_ok("cqo"), "cqo alias");
    CHECK(asm_ok("cdqe"), "cdqe alias");

    printf("  Relative jumps/calls...\n");
    CHECK(asm_ok("jmp 0x10"), "jmp rel");
    CHECK(asm_ok("call 0x100"), "call rel");
    CHECK(asm_ok("je 0x20"), "je rel");
    CHECK(asm_ok("jne 0x30"), "jne rel");
    CHECK(asm_ok("jz 0x10"), "jz rel");
    CHECK(asm_ok("jnz 0x10"), "jnz rel");
    CHECK(asm_ok("jb 0x10"), "jb rel");
    CHECK(asm_ok("jnb 0x10"), "jnb rel");
    CHECK(asm_ok("jl 0x10"), "jl rel");
    CHECK(asm_ok("jle 0x10"), "jle rel");
    CHECK(asm_ok("ja 0x10"), "ja rel");
    CHECK(asm_ok("jg 0x10"), "jg rel");
    CHECK(asm_ok("jo 0x10"), "jo rel");
    CHECK(asm_ok("jno 0x10"), "jno rel");
    CHECK(asm_ok("js 0x10"), "js rel");
    CHECK(asm_ok("jns 0x10"), "jns rel");
    CHECK(asm_ok("call [rax]"), "call mem");
    CHECK(asm_ok("call [rax+8]"), "call mem+disp");
    CHECK(asm_ok("jmp [rax]"), "jmp mem");
    CHECK(asm_ok("jmp [rbx+rcx*8]"), "jmp mem sib");

    printf("  Lock/rep prefixes...\n");
    CHECK(asm_ok("lock add [rax], rcx"), "lock add mr");
    CHECK(asm_ok("lock xadd [rax], rcx"), "lock xadd");
    CHECK(asm_ok("lock cmpxchg [rax], rcx"), "lock cmpxchg");
    CHECK(asm_ok("lock inc dword [rax]"), "lock inc");
    CHECK(asm_ok("lock dec qword [rbx]"), "lock dec");
    CHECK(asm_ok("rep movsb"), "rep movsb");
    CHECK(asm_ok("rep movsq"), "rep movsq");
    CHECK(asm_ok("rep stosb"), "rep stosb");
    CHECK(asm_ok("rep stosq"), "rep stosq");
    CHECK(asm_ok("repe cmpsb"), "repe cmpsb");
    CHECK(asm_ok("repne scasb"), "repne scasb");
    {
        auto lock_bytes = vedx64::assemble("lock add [rax], rcx");
        CHECK(lock_bytes.has_value() && lock_bytes->size() > 0 && (*lock_bytes)[0] == 0xF0, "lock prefix is 0xF0");
    }

    printf("  Mem-only instructions...\n");
    CHECK(asm_ok("prefetcht0 [rax]"), "prefetcht0");
    CHECK(asm_ok("prefetcht1 [rbx]"), "prefetcht1");
    CHECK(asm_ok("prefetcht2 [rcx]"), "prefetcht2");
    CHECK(asm_ok("prefetchnta [rdx]"), "prefetchnta");
    CHECK(asm_ok("clflush [rax]"), "clflush");
    CHECK(asm_ok("inc dword [rax]"), "inc mem");
    CHECK(asm_ok("dec qword [rbx]"), "dec mem");
    CHECK(asm_ok("neg dword [rcx]"), "neg mem");
    CHECK(asm_ok("not qword [rdx]"), "not mem");
    CHECK(asm_ok("div [rax]"), "div mem");
    CHECK(asm_ok("idiv [rbx]"), "idiv mem");
    CHECK(asm_ok("mul [rcx]"), "mul mem");
    CHECK(asm_ok("fld [rax]"), "fld mem");
    CHECK(asm_ok("fstp [rbx]"), "fstp mem");
    CHECK(asm_ok("lgdt [rax]"), "lgdt mem");

    printf("  Segment overrides...\n");
    CHECK(asm_ok("mov rax, fs:[rbx]"), "fs:[rbx]");
    CHECK(asm_ok("mov rax, gs:[rcx]"), "gs:[rcx]");
    CHECK(asm_ok("mov rax, fs:[rbx+8]"), "fs:[rbx+8]");
    CHECK(asm_ok("mov rax, gs:[0x28]"), "gs:[0x28]");
    CHECK(asm_ok("mov rax, fs:[rbx+rcx*4]"), "fs:sib");
    CHECK(asm_ok("mov gs:[rax], rcx"), "gs:store");
    CHECK(asm_ok("add dword fs:[rax], 1"), "fs:add mi");

    printf("  RIP-relative...\n");
    CHECK(asm_ok("mov rax, [rip+0x100]"), "rip+disp");
    CHECK(asm_ok("mov eax, [rip+0]"), "rip+0");
    CHECK(asm_ok("lea rax, [rip+0x1000]"), "lea rip");
    CHECK(asm_ok("cmp dword [rip+4], 0"), "cmp rip mi");

    printf("  Data directives...\n");
    {
        auto d1 = vedx64::assemble("db 0x90");
        CHECK(d1.has_value() && d1->size() == 1 && (*d1)[0] == 0x90, "db single");
    }
    {
        auto d2 = vedx64::assemble("db 0xCC, 0xCC, 0x90");
        CHECK(d2.has_value() && d2->size() == 3, "db multi");
    }
    {
        auto d3 = vedx64::assemble("db \"ABC\"");
        CHECK(d3.has_value() && d3->size() == 3 && (*d3)[0] == 'A' && (*d3)[1] == 'B' && (*d3)[2] == 'C', "db string");
    }
    {
        auto d4 = vedx64::assemble("dw 0x1234");
        CHECK(d4.has_value() && d4->size() == 2, "dw");
        CHECK((*d4)[0] == 0x34 && (*d4)[1] == 0x12, "dw little-endian");
    }
    {
        auto d5 = vedx64::assemble("dd 0xDEADBEEF");
        CHECK(d5.has_value() && d5->size() == 4, "dd");
    }
    {
        auto d6 = vedx64::assemble("dq 0x123456789ABCDEF0");
        CHECK(d6.has_value() && d6->size() == 8, "dq");
    }
    {
        auto d7 = vedx64::assemble("dd 1, 2, 3, 4");
        CHECK(d7.has_value() && d7->size() == 16, "dd multi");
    }
    {
        auto d8 = vedx64::assemble("db \"Hello\", 0");
        CHECK(d8.has_value() && d8->size() == 6 && (*d8)[5] == 0, "db string+null");
    }
    {
        auto d9 = vedx64::assemble_block("jmp skip\ndb 0xCC, 0xCC\nskip:\nnop\nret");
        CHECK(d9.has_value(), "db in block with labels");
    }

    printf("  MMX registers...\n");
    CHECK(asm_ok("movq mm0, mm1"), "movq mm mm");
    CHECK(asm_ok("paddb mm0, mm1"), "paddb mm");
    CHECK(asm_ok("paddw mm0, mm1"), "paddw mm");
    CHECK(asm_ok("paddd mm0, mm1"), "paddd mm");
    CHECK(asm_ok("pxor mm0, mm1"), "pxor mm");
    CHECK(asm_ok("pand mm0, mm1"), "pand mm");
    CHECK(asm_ok("por mm0, mm1"), "por mm");
    CHECK(asm_ok("movq mm0, [rax]"), "movq mm mem");
    CHECK(asm_ok("cvtpi2ps xmm0, mm0"), "cvtpi2ps xmm mm");
    CHECK(asm_ok("cvtps2pi mm0, xmm0"), "cvtps2pi mm xmm");

    printf("  Times directive...\n");
    {
        auto t1 = vedx64::assemble("times 5 nop");
        CHECK(t1.has_value() && t1->size() == 5, "times 5 nop");
        for (size_t i = 0; i < 5 && t1.has_value(); ++i) CHECK((*t1)[i] == 0x90, "times nop byte");
    }
    {
        auto t2 = vedx64::assemble("times 3 db 0xCC");
        CHECK(t2.has_value() && t2->size() == 3, "times 3 db");
    }
    {
        auto t3 = vedx64::assemble("times 0 nop");
        CHECK(t3.has_value() && t3->size() == 0, "times 0");
    }

    printf("  Equ constants...\n");
    {
        auto e1 = vedx64::assemble_block("size equ 0x20\nsub rsp, size\nadd rsp, size");
        CHECK(e1.has_value(), "equ basic");
    }
    {
        auto e2 = vedx64::assemble_block("val equ 42\nmov eax, val");
        CHECK(e2.has_value(), "equ in mov");
    }

    printf("  Error cases...\n");
    CHECK(!vedx64::assemble("foobar").has_value(), "invalid mnem");
    CHECK(!vedx64::assemble("").has_value(), "empty string");
    CHECK(!vedx64::assemble("mov").has_value(), "mov no operands");
    CHECK(!vedx64::assemble("mov rax").has_value(), "mov one operand");

    printf("  Block assembly...\n");
    auto block = vedx64::assemble_block("push rbp\nmov rbp, rsp\nsub rsp, 0x20\nnop\nadd rsp, 0x20\npop rbp\nret");
    CHECK(block.has_value(), "block assembles");
    CHECK(block->size() >= 7, "block has enough bytes");

    auto block2 = vedx64::assemble_block("push rbp; mov rbp, rsp # setup\nnop\nret");
    CHECK(block2.has_value(), "block with semicolons+comments");

    auto bad_block = vedx64::assemble_block("push rbp\nfoobar\nret");
    CHECK(!bad_block.has_value(), "block with invalid fails");

    printf("  Labels...\n");
    auto lbl1 = vedx64::assemble_block("jmp end\nnop\nnop\nend:\nret");
    CHECK(lbl1.has_value(), "forward jump label");
    if (lbl1.has_value()) {
        // jmp rel32 (5 bytes) + nop + nop + ret = 8 bytes
        CHECK(lbl1->size() == 8, "forward jump size");
        CHECK((*lbl1)[0] == 0xE9, "forward jump opcode");
        // rel32 should be 2 (skip 2 nops)
        int32_t rel1 = *(int32_t*)(lbl1->data() + 1);
        CHECK(rel1 == 2, "forward jump offset is 2");
    }

    auto lbl2 = vedx64::assemble_block("loop:\nnop\njmp loop");
    CHECK(lbl2.has_value(), "backward jump label");
    if (lbl2.has_value()) {
        // nop (1) + jmp rel32 (5) = 6 bytes
        CHECK(lbl2->size() == 6, "backward jump size");
        int32_t rel2 = *(int32_t*)(lbl2->data() + 2);
        // rel = 0 - (1 + 5) = -6
        CHECK(rel2 == -6, "backward jump offset is -6");
    }

    auto lbl3 = vedx64::assemble_block("cmp eax, 0\nje done\nadd eax, 1\ndone:\nret");
    CHECK(lbl3.has_value(), "conditional jump label");

    auto lbl4 = vedx64::assemble_block("start: nop\njmp start");
    CHECK(lbl4.has_value(), "label on same line");

    auto lbl5 = vedx64::assemble_block("jmp skip\nfail: int3\nskip:\nret");
    CHECK(lbl5.has_value(), "multiple labels");

    auto lbl6 = vedx64::assemble_block("call func\nret\nfunc:\nnop\nret");
    CHECK(lbl6.has_value(), "call label");

    printf("  Mass roundtrip...\n");
    const char* roundtrip_tests[] = {
        // Zero-operand
        "nop", "ret", "int3", "hlt", "clc", "stc", "cld",
        // Single register
        "push rbp", "push rax", "push r12", "pop rbp", "pop r15",
        "inc rax", "inc ecx", "dec rdx", "neg rax", "not rax",
        // Register-register
        "mov rax, rcx", "mov eax, ecx", "mov r8, r15",
        "add rax, rcx", "sub rax, rcx", "xor eax, eax",
        "and rax, rdx", "or rax, rdx", "cmp rax, rcx",
        "test rax, rcx", "xchg rax, rcx",
        // Register-immediate
        "mov eax, 42", "add rax, 42", "sub rsp, 0x20",
        "xor eax, 0xFF", "and eax, 0x0F", "cmp rax, 0",
        // Memory
        "mov rax, [rbx]", "mov [rax], rcx", "mov rax, [rbx+8]",
        "mov rax, [rbx+rcx*4]", "mov rax, [rbx+rcx*4+16]",
        "mov rax, [rbp-8]", "add rax, [rbx]", "cmp rax, [rdx]",
        "lea rax, [rbx+rcx*4+8]",
        // XMM
        "movaps xmm0, xmm1", "addps xmm0, xmm1", "xorps xmm0, xmm0",
        "movaps xmm0, [rax]", "movaps [rax], xmm0",
        // Cross-type
        "movd xmm0, eax",
        // Aliases
        "cmova eax, ecx", "cmove eax, ecx", "cmovne eax, ecx",
        // Jumps/calls
        "jmp 0x10", "call 0x100", "je 0x20", "jne 0x30",
        "call [rax]", "jmp [rax]",
        // Prefixes
        "lock add [rax], rcx", "rep movsb",
        // Mem-only
        "prefetcht0 [rax]", "clflush [rax]", "inc dword [rax]",
        // Segment/RIP
        "mov rax, fs:[rbx]", "mov rax, gs:[0x28]", "mov rax, [rip+0x100]",
    };
    int rt_pass = 0, rt_total = 0;
    for (auto text : roundtrip_tests) {
        rt_total++;
        if (asm_ok(text)) rt_pass++;
        else printf("    roundtrip FAIL: %s\n", text);
    }
    printf("    Roundtrip: %d/%d\n", rt_pass, rt_total);
    CHECK(rt_pass == rt_total, "all roundtrips pass");

    printf("\n  Results: %d passed, %d failed\n", g_pass, g_fail);
    if (g_fail > 0) { printf("ASSEMBLER TESTS FAILED\n"); return 1; }
    printf("All assembler tests passed!\n");
    return 0;
}
#else
#include <cstdio>
int main() { printf("VEDX64_ASSEMBLER not enabled, skipping.\n"); return 0; }
#endif
