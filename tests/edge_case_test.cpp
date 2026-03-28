// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/core.hpp"
#include "vedx64/instruction.hpp"
#include <cstdio>
#include <cstring>
using namespace vedx64;
static int g_pass=0,g_fail=0;
#define CHECK(c,m) do{if(!(c)){printf("FAIL: %s\n",m);g_fail++;}else{printf("PASS: %s\n",m);g_pass++;}}while(0)

int main() {
    printf("=== Edge Case / Bogus Instruction Tests ===\n\n");

    printf("--- Overlong instructions ---\n");
    {
        uint8_t c[16]; memset(c, 0x66, 15); c[15] = 0x90;
        DecodedInstr di; size_t n = decode(c, 16, di);
        // Decoder consumes all prefixes (no 15-byte hardware limit enforced in software)
        CHECK(n > 0, "16 bytes with redundant prefixes: decodes");
        CHECK(di.num_prefixes >= 4, "many prefixes recorded (capped at prefix slots)");
    }
    {
        uint8_t c[15]; memset(c, 0x66, 14); c[14] = 0x90;
        DecodedInstr di; size_t n = decode(c, 15, di);
        CHECK(n > 0, "15-byte with prefixes: decode succeeds");
    }
    {
        uint8_t c[] = {0x0F}; // two-byte escape, no opcode
        DecodedInstr di; size_t n = decode(c, 1, di);
        CHECK(n == 0, "truncated 0F escape: fails");
    }

    printf("--- Multiple/redundant prefixes ---\n");
    {
        uint8_t c[] = {0x66, 0x66, 0x90};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "double 0x66 + NOP decodes");
        CHECK(di.num_prefixes >= 2, "two prefixes recorded");
    }
    {
        uint8_t c[] = {0xF0, 0x64, 0x66, 0x67, 0x48, 0x01, 0x08}; // lock fs: data16 addr32 REX.W add [rax],rcx
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n == 7, "all 4 prefix groups + REX: 7 bytes");
        CHECK(di.rex == 0x48, "REX.W present");
    }
    {
        uint8_t c[] = {0x26, 0x2E, 0x64, 0x8B, 0x00}; // es: cs: fs: mov eax,[rax]
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "triple segment override decodes");
        CHECK(di.num_prefixes == 3, "3 prefix bytes");
    }
    {
        uint8_t c[] = {0xF3, 0xF3, 0xA4}; // rep rep movsb
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "double REP + MOVSB decodes");
    }

    printf("--- Undocumented/obscure instructions ---\n");
    {
        uint8_t c[] = {0xD6};
        DecodedInstr di; size_t n = decode(c, 1, di);
        CHECK(n == 1, "SALC (0xD6) decodes to 1 byte");
    }
    {
        uint8_t c[] = {0xF1};
        DecodedInstr di; size_t n = decode(c, 1, di);
        CHECK(n == 1, "INT1/ICEBP (0xF1) decodes");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::INT1, "mnemonic is INT1");
    }
    {
        uint8_t c[] = {0x0F, 0x0B};
        DecodedInstr di; size_t n = decode(c, 2, di);
        CHECK(n == 2, "UD2 decodes to 2 bytes");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::UD2, "mnemonic is UD2");
    }
    {
        uint8_t c[] = {0xF3, 0x90};
        DecodedInstr di; size_t n = decode(c, 2, di);
        CHECK(n == 2, "PAUSE (F3 90) decodes");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::PAUSE, "mnemonic is PAUSE");
    }
    {
        uint8_t c[] = {0x90};
        DecodedInstr di; size_t n = decode(c, 1, di);
        CHECK(n == 1, "NOP/XCHG (0x90) decodes");
    }
    {
        uint8_t c[] = {0x48, 0x63, 0xC8}; // movsxd rcx, eax
        DecodedInstr di; size_t n = decode(c, 3, di);
        CHECK(n == 3, "MOVSXD decodes to 3 bytes");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::MOVSXD, "mnemonic is MOVSXD");
    }
    {
        uint8_t c[] = {0x0F, 0x05};
        DecodedInstr di; size_t n = decode(c, 2, di);
        CHECK(n == 2, "SYSCALL decodes");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::SYSCALL, "mnemonic is SYSCALL");
    }
    {
        uint8_t c[] = {0x0F, 0xA2};
        DecodedInstr di; size_t n = decode(c, 2, di);
        CHECK(n == 2, "CPUID decodes");
    }

    printf("--- Truncated instructions ---\n");
    {
        uint8_t c[] = {0xE8, 0x10, 0x00};
        DecodedInstr di; size_t n = decode(c, 3, di);
        CHECK(n == 0, "truncated CALL (3/5 bytes): fails");
    }
    {
        uint8_t c[] = {0x8B, 0x85, 0x00, 0x01};
        DecodedInstr di; size_t n = decode(c, 4, di);
        CHECK(n == 0, "truncated MOV [RBP+disp32]: fails");
    }
    {
        uint8_t c[] = {0x0F};
        DecodedInstr di; size_t n = decode(c, 1, di);
        CHECK(n == 0, "lone 0F escape byte: fails");
    }
    {
        DecodedInstr di; size_t n = decode(nullptr, 0, di);
        CHECK(n == 0, "null/zero-length input: fails");
    }
    {
        uint8_t c[] = {0x48};
        DecodedInstr di; size_t n = decode(c, 1, di);
        CHECK(n == 0, "lone REX prefix: fails");
    }

    printf("--- Garbage/random bytes ---\n");
    {
        uint8_t c[16]; memset(c, 0, 16);
        DecodedInstr di; size_t n = decode(c, 16, di);
        CHECK(n > 0, "all-zeros: decodes (ADD [RAX],AL)");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::ADD, "all-zeros mnemonic is ADD");
    }
    {
        uint8_t c[16]; memset(c, 0xFF, 16);
        DecodedInstr di; size_t n = decode(c, 16, di);
        // FF /7 is undefined, but FF with other /r values could decode
        // Just verify no crash
        printf("    all-0xFF: n=%zu\n", n);
        CHECK(true, "all-0xFF: no crash");
    }
    {
        int decoded = 0, failed = 0;
        for (int b = 0; b < 256; ++b) {
            uint8_t c = (uint8_t)b;
            DecodedInstr di;
            size_t n = decode(&c, 1, di);
            if (n > 0) decoded++; else failed++;
        }
        printf("    single-byte scan: %d decoded, %d failed\n", decoded, failed);
        CHECK(decoded > 50, "at least 50 single-byte instructions decode");
        CHECK(failed > 0, "some single bytes fail to decode (need more data)");
    }

    printf("--- REX prefix edge cases ---\n");
    {
        uint8_t c[] = {0x48, 0x90};
        DecodedInstr di; size_t n = decode(c, 2, di);
        CHECK(n == 2, "REX.W + NOP: 2 bytes");
        CHECK(di.rex == 0x48, "REX byte is 0x48");
    }
    {
        int ok = 0;
        for (uint8_t rex = 0x40; rex <= 0x4F; ++rex) {
            uint8_t c[] = {rex, 0x90};
            DecodedInstr di; size_t n = decode(c, 2, di);
            if (n == 2 && di.rex == rex) ok++;
        }
        CHECK(ok == 16, "all 16 REX variants (0x40-0x4F) + NOP decode");
    }
    {
        uint8_t c[] = {0x41, 0x50};
        DecodedInstr di; size_t n = decode(c, 2, di);
        CHECK(n == 2, "PUSH R8 (REX.B + 0x50) decodes");
        CHECK(di.opcode_reg == 0, "opcode_reg base is 0");
        CHECK((di.rex & 0x01) != 0, "REX.B is set");
    }

    printf("--- VEX prefix edge cases ---\n");
    {
        uint8_t c[] = {0xC5, 0xF8, 0x28, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "2-byte VEX VMOVAPS decodes");
        CHECK(di.has_vex, "has_vex set");
    }
    {
        uint8_t c[] = {0xC4, 0xE1, 0x78, 0x28, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "3-byte VEX VMOVAPS decodes");
        CHECK(di.has_vex, "3-byte VEX: has_vex set");
    }
    {
        uint8_t c[] = {0xC5};
        DecodedInstr di; size_t n = decode(c, 1, di);
        CHECK(n == 0, "truncated 2-byte VEX: fails");
    }
    {
        uint8_t c[] = {0xC4, 0xE1};
        DecodedInstr di; size_t n = decode(c, 2, di);
        CHECK(n == 0, "truncated 3-byte VEX: fails");
    }

    printf("--- Encode/decode roundtrip edge cases ---\n");
    {
        uint8_t c[] = {0xCC};
        DecodedInstr di; decode(c, 1, di);
        uint8_t out[16]; size_t n = encode(di, out, sizeof(out));
        CHECK(n == 1 && out[0] == 0xCC, "INT3 roundtrip");
    }
    {
        uint8_t c[] = {0xF0, 0x87, 0x08};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n == 3, "LOCK XCHG decodes");
        uint8_t out[16]; size_t en = encode(di, out, sizeof(out));
        CHECK(en == 3 && memcmp(c, out, 3) == 0, "LOCK XCHG roundtrip");
    }
    {
        uint8_t c[] = {0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n == 9, "9-byte long NOP decodes");
    }
    {
        uint8_t c[] = {0x48, 0x8B, 0x05, 0x78, 0x56, 0x34, 0x12};
        DecodedInstr di; decode(c, sizeof(c), di);
        uint8_t out[16]; size_t en = encode(di, out, sizeof(out));
        CHECK(en == 7 && memcmp(c, out, 7) == 0, "RIP-relative MOV roundtrip");
    }

    printf("--- Instruction class edge cases ---\n");
    {
        Instruction instr;
        size_t n = Instruction::decode_instruction(nullptr, 0, instr);
        CHECK(n == 0, "Instruction::decode null input");
        CHECK(instr.mnemonic == Mnemonic::INVALID, "mnemonic stays INVALID");
        CHECK(instr.operands.empty(), "no operands");
    }
    {
        uint8_t c[] = {0x90};
        Instruction instr; Instruction::decode_instruction(c, 1, instr);
        CHECK(instr.mnemonic != Mnemonic::INVALID, "NOP has valid mnemonic");
    }
    {
        uint8_t c[] = {0x48, 0xB8, 0xEF, 0xBE, 0xAD, 0xDE, 0xEF, 0xBE, 0xAD, 0xDE};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n == 10, "MOV RAX, imm64: 10 bytes");
        CHECK(di.immediate == (int64_t)0xDEADBEEFDEADBEEFULL, "imm64 value");
    }

#ifdef VEDX64_STRINGS
    printf("--- Disassembler edge cases ---\n");
    {
        uint8_t c[] = {0x90};
        char buf[4]; size_t n = disassemble(c, 1, buf, sizeof(buf), 0);
        CHECK(n > 0, "disassemble into tiny buffer");
        CHECK(strlen(buf) < sizeof(buf), "output fits in buffer");
    }
    {
        uint8_t c[] = {0x90};
        char buf[1]; size_t n = disassemble(c, 1, buf, sizeof(buf), 0);
        CHECK(n > 0, "disassemble into 1-byte buffer");
    }
    {
        uint8_t c[] = {0x48, 0xB8, 0xEF, 0xBE, 0xAD, 0xDE, 0xEF, 0xBE, 0xAD, 0xDE};
        char buf[128]; size_t n = disassemble(c, sizeof(c), buf, sizeof(buf), 0);
        CHECK(n == 10, "disassemble MOV RAX,imm64");
        printf("    movabs: %s\n", buf);
    }
#endif // VEDX64_STRINGS

    printf("--- Consecutive decode stress ---\n");
    {
        // Decode a realistic function: push rbp; mov rbp,rsp; sub rsp,0x20;
        // mov [rbp-4],edi; mov eax,[rbp-4]; add eax,1; imul eax,eax;
        // add rsp,0x20; pop rbp; ret
        uint8_t code[] = {0x55, 0x48,0x89,0xE5, 0x48,0x83,0xEC,0x20,
            0x89,0x7D,0xFC, 0x8B,0x45,0xFC, 0x83,0xC0,0x01,
            0x0F,0xAF,0xC0, 0x48,0x83,0xC4,0x20, 0x5D, 0xC3};
        size_t off = 0;
        int count = 0;
        while (off < sizeof(code)) {
            DecodedInstr di;
            size_t n = decode(code + off, sizeof(code) - off, di);
            if (n == 0) break;
            // Verify roundtrip
            uint8_t re[16]; size_t rn = encode(di, re, sizeof(re));
            if (rn != n || memcmp(code + off, re, n) != 0)
                printf("    WARN: roundtrip mismatch at offset %zu\n", off);
            off += n; count++;
        }
        CHECK(count == 10, "decoded 10 instructions from function");
        CHECK(off == sizeof(code), "consumed all bytes");
    }

    printf("--- Extension instruction decode tests ---\n");

    { // ANDN ecx, ecx, eax
        uint8_t c[] = {0xC4, 0xE2, 0x70, 0xF2, 0xC8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "ANDN decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::ANDN, "ANDN mnemonic");
    }
    { // BEXTR eax, ecx, edx
        uint8_t c[] = {0xC4, 0xE2, 0x68, 0xF7, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "BEXTR decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::BEXTR, "BEXTR mnemonic");
    }
    { // BLSI ecx, eax (vvvv=ecx, rm=eax, /3)
        uint8_t c[] = {0xC4, 0xE2, 0x70, 0xF3, 0xD8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "BLSI decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::BLSI, "BLSI mnemonic");
    }
    { // BLSMSK ecx, eax
        uint8_t c[] = {0xC4, 0xE2, 0x70, 0xF3, 0xD0};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "BLSMSK decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::BLSMSK, "BLSMSK mnemonic");
    }
    { // BLSR ecx, eax
        uint8_t c[] = {0xC4, 0xE2, 0x70, 0xF3, 0xC8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "BLSR decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::BLSR, "BLSR mnemonic");
    }
    { // TZCNT ecx, eax
        uint8_t c[] = {0xF3, 0x0F, 0xBC, 0xC8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "TZCNT decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::TZCNT, "TZCNT mnemonic");
    }
    { // BZHI eax, ecx, edx
        uint8_t c[] = {0xC4, 0xE2, 0x68, 0xF5, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "BZHI decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::BZHI, "BZHI mnemonic");
    }
    { // MULX edx, ecx, eax
        uint8_t c[] = {0xC4, 0xE2, 0x73, 0xF6, 0xD0};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "MULX decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::MULX, "MULX mnemonic");
    }
    { // PDEP eax, ecx, edx
        uint8_t c[] = {0xC4, 0xE2, 0x73, 0xF5, 0xC2};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "PDEP decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::PDEP, "PDEP mnemonic");
    }
    { // PEXT eax, ecx, edx
        uint8_t c[] = {0xC4, 0xE2, 0x72, 0xF5, 0xC2};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "PEXT decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::PEXT, "PEXT mnemonic");
    }
    { // RORX eax, ecx, 5
        uint8_t c[] = {0xC4, 0xE3, 0x7B, 0xF0, 0xC1, 0x05};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "RORX decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::RORX, "RORX mnemonic");
    }
    { // SARX eax, ecx, edx
        uint8_t c[] = {0xC4, 0xE2, 0x6A, 0xF7, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "SARX decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::SARX, "SARX mnemonic");
    }
    { // SHLX eax, ecx, edx
        uint8_t c[] = {0xC4, 0xE2, 0x69, 0xF7, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "SHLX decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::SHLX, "SHLX mnemonic");
    }
    { // SHRX eax, ecx, edx
        uint8_t c[] = {0xC4, 0xE2, 0x6B, 0xF7, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "SHRX decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::SHRX, "SHRX mnemonic");
    }
    { // ADCX eax, ecx
        uint8_t c[] = {0x66, 0x0F, 0x38, 0xF6, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "ADCX decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::ADCX, "ADCX mnemonic");
    }
    { // ADOX eax, ecx
        uint8_t c[] = {0xF3, 0x0F, 0x38, 0xF6, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "ADOX decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::ADOX, "ADOX mnemonic");
    }
    { // RDRAND eax
        uint8_t c[] = {0x0F, 0xC7, 0xF0};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "RDRAND decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDRAND, "RDRAND mnemonic");
    }
    { // RDSEED eax
        uint8_t c[] = {0x0F, 0xC7, 0xF8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "RDSEED decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDSEED, "RDSEED mnemonic");
    }
    { // AESENC xmm0, xmm1
        uint8_t c[] = {0x66, 0x0F, 0x38, 0xDC, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "AESENC decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::AESENC, "AESENC mnemonic");
    }
    { // AESDEC xmm0, xmm1
        uint8_t c[] = {0x66, 0x0F, 0x38, 0xDE, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "AESDEC decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::AESDEC, "AESDEC mnemonic");
    }
    { // AESKEYGENASSIST xmm0, xmm1, 0x01
        uint8_t c[] = {0x66, 0x0F, 0x3A, 0xDF, 0xC1, 0x01};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "AESKEYGENASSIST decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::AESKEYGENASSIST, "AESKEYGENASSIST mnemonic");
        CHECK(di.desc->num_operands == 3, "AESKEYGENASSIST has 3 operands");
    }
    { // SHA256RNDS2 xmm0, xmm1
        uint8_t c[] = {0x0F, 0x38, 0xCB, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "SHA256RNDS2 decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::SHA256RNDS2, "SHA256RNDS2 mnemonic");
    }
    { // SHA1RNDS4 xmm0, xmm1, 0x00
        uint8_t c[] = {0x0F, 0x3A, 0xCC, 0xC1, 0x00};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "SHA1RNDS4 decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::SHA1RNDS4, "SHA1RNDS4 mnemonic");
    }
    { // PCLMULQDQ xmm0, xmm1, 0x00
        uint8_t c[] = {0x66, 0x0F, 0x3A, 0x44, 0xC1, 0x00};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "PCLMULQDQ decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::PCLMULQDQ, "PCLMULQDQ mnemonic");
    }
    { // GF2P8MULB xmm0, xmm1
        uint8_t c[] = {0x66, 0x0F, 0x38, 0xCF, 0xC1};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "GF2P8MULB decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::GF2P8MULB, "GF2P8MULB mnemonic");
    }
    { // ENDBR64
        uint8_t c[] = {0xF3, 0x0F, 0x1E, 0xFA};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "ENDBR64 decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::ENDBR64, "ENDBR64 mnemonic");
    }
    { // RDSSPD eax
        uint8_t c[] = {0xF3, 0x0F, 0x1E, 0xC8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "RDSSPD decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDSSPD, "RDSSPD mnemonic");
    }
    { // INCSSPD eax
        uint8_t c[] = {0xF3, 0x0F, 0xAE, 0xE8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "INCSSPD decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::INCSSPD, "INCSSPD mnemonic");
    }
    { // RDFSBASE eax
        uint8_t c[] = {0xF3, 0x0F, 0xAE, 0xC0};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "RDFSBASE decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDFSBASE, "RDFSBASE mnemonic");
    }
    { // WRFSBASE eax
        uint8_t c[] = {0xF3, 0x0F, 0xAE, 0xD0};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "WRFSBASE decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::WRFSBASE, "WRFSBASE mnemonic");
    }
    { // XTEST
        uint8_t c[] = {0x0F, 0x01, 0xD6};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "XTEST decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::XTEST, "XTEST mnemonic");
    }
    { // XEND
        uint8_t c[] = {0x0F, 0x01, 0xD5};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "XEND decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::XEND, "XEND mnemonic");
    }
    { // XABORT 0x42
        uint8_t c[] = {0xC6, 0xF8, 0x42};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "XABORT decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::XABORT, "XABORT mnemonic");
    }
    { // XSAVEC [rax]
        uint8_t c[] = {0x0F, 0xC7, 0x20};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "XSAVEC decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::XSAVEC, "XSAVEC mnemonic");
    }
    { // SERIALIZE
        uint8_t c[] = {0x0F, 0x01, 0xE8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "SERIALIZE decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::SERIALIZE, "SERIALIZE mnemonic");
    }
    { // CLFLUSHOPT [rax]
        uint8_t c[] = {0x66, 0x0F, 0xAE, 0x38};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "CLFLUSHOPT decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::CLFLUSHOPT, "CLFLUSHOPT mnemonic");
    }
    { // MOVDIRI [rax], ecx
        uint8_t c[] = {0x0F, 0x38, 0xF9, 0x08};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "MOVDIRI decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::MOVDIRI, "MOVDIRI mnemonic");
    }
    { // RDPID eax
        uint8_t c[] = {0xF3, 0x0F, 0xC7, 0xF8};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n > 0, "RDPID decode");
        CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDPID, "RDPID mnemonic");
    }

    printf("\n--- Operand Verification ---\n");
    { uint8_t c[] = {0xF3, 0x0F, 0xBC, 0xC8};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc->num_operands == 2, "tzcnt has 2 operands");
      CHECK(di.desc->operands[0].addr == AddrMode::ModRM_Reg, "tzcnt op0 is ModRM_Reg");
      CHECK(di.desc->operands[1].addr == AddrMode::ModRM_RM, "tzcnt op1 is ModRM_RM");
      CHECK(((di.modrm >> 3) & 7) == 1, "tzcnt reg field = ecx");
      CHECK((di.modrm & 7) == 0, "tzcnt rm field = eax");
      CHECK((di.modrm >> 6) == 3, "tzcnt mod = register");
      CHECK(di.length == 4, "tzcnt length = 4"); }
    { uint8_t c[] = {0xC4, 0xE2, 0x68, 0xF2, 0xCB};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::ANDN, "andn decode");
      CHECK(di.has_vex, "andn is VEX");
      CHECK(di.vex_vvvv == 2, "andn vvvv = edx (2)");
      CHECK(((di.modrm >> 3) & 7) == 1, "andn reg = ecx");
      CHECK((di.modrm & 7) == 3, "andn rm = ebx");
      CHECK(di.length == 5, "andn length = 5"); }
    { uint8_t c[] = {0x66, 0x0F, 0x3A, 0xDF, 0xC8, 0x42};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::AESKEYGENASSIST, "aeskeygenassist decode");
      CHECK(di.desc->num_operands == 3, "aeskeygenassist has 3 operands");
      CHECK(di.desc->operands[0].addr == AddrMode::XmmReg, "aeskeygenassist op0 is XmmReg");
      CHECK(di.desc->operands[1].addr == AddrMode::XmmRM, "aeskeygenassist op1 is XmmRM");
      CHECK(di.desc->operands[2].addr == AddrMode::Immediate, "aeskeygenassist op2 is Immediate");
      CHECK(di.immediate == 0x42, "aeskeygenassist imm = 0x42");
      CHECK(di.length == 6, "aeskeygenassist length = 6"); }
    { uint8_t c[] = {0x66, 0x0F, 0x3A, 0x44, 0xD3, 0x11};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::PCLMULQDQ, "pclmulqdq decode");
      CHECK(((di.modrm >> 3) & 7) == 2, "pclmulqdq reg = xmm2");
      CHECK((di.modrm & 7) == 3, "pclmulqdq rm = xmm3");
      CHECK(di.immediate == 0x11, "pclmulqdq imm = 0x11"); }
    { uint8_t c[] = {0x0F, 0x38, 0xCB, 0x08};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::SHA256RNDS2, "sha256rnds2 decode");
      CHECK((di.modrm >> 6) == 0, "sha256rnds2 mod = 0 (memory)");
      CHECK(di.length == 4, "sha256rnds2 length = 4"); }
    { uint8_t c[] = {0x0F, 0xC7, 0xF0};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDRAND, "rdrand decode");
      CHECK(di.desc->num_operands == 1, "rdrand has 1 operand");
      CHECK((di.modrm & 7) == 0, "rdrand rm = eax");
      CHECK(di.length == 3, "rdrand length = 3"); }
    { uint8_t c[] = {0x0F, 0xC7, 0xF9};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDSEED, "rdseed decode");
      CHECK((di.modrm & 7) == 1, "rdseed rm = ecx"); }
    { uint8_t c[] = {0x66, 0x0F, 0x38, 0xF6, 0xCA};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::ADCX, "adcx decode");
      CHECK(di.desc->num_operands == 2, "adcx has 2 operands");
      CHECK(((di.modrm >> 3) & 7) == 1, "adcx reg = ecx");
      CHECK((di.modrm & 7) == 2, "adcx rm = edx"); }
    { uint8_t c[] = {0xC4, 0xE3, 0x7B, 0xF0, 0xC8, 0x07};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::RORX, "rorx decode");
      CHECK(di.has_vex, "rorx is VEX");
      CHECK(di.immediate == 0x07, "rorx imm = 7");
      CHECK(di.length == 6, "rorx length = 6"); }
    { uint8_t c[] = {0xF3, 0x0F, 0x1E, 0xFA};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::ENDBR64, "endbr64 decode");
      CHECK(di.desc->num_operands == 0, "endbr64 has 0 operands"); }
    { uint8_t c[] = {0xC7, 0xF8, 0x10, 0x00, 0x00, 0x00};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::XBEGIN, "xbegin decode");
      CHECK(di.desc->operands[0].addr == AddrMode::RelOffset, "xbegin op0 is RelOffset"); }
    { uint8_t c[] = {0xC6, 0xF8, 0x42};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::XABORT, "xabort decode"); }
    { uint8_t c[] = {0xC4, 0xE2, 0x78, 0xF3, 0x18};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::BLSI, "blsi [rax] decode");
      CHECK((di.modrm >> 6) == 0, "blsi mod = 0 (memory)");
      CHECK((di.modrm & 7) == 0, "blsi rm = [rax]"); }
    { uint8_t c[] = {0x66, 0x0F, 0x38, 0xDC, 0x6B, 0x10};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::AESENC, "aesenc [rbx+0x10] decode");
      CHECK(((di.modrm >> 3) & 7) == 5, "aesenc reg = xmm5");
      CHECK((di.modrm >> 6) == 1, "aesenc mod = 1 (disp8)");
      CHECK(di.displacement == 0x10, "aesenc disp = 0x10");
      CHECK(di.length == 6, "aesenc length = 6"); }
    { uint8_t c[] = {0xF3, 0x0F, 0xAE, 0xC1};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDFSBASE, "rdfsbase ecx decode");
      CHECK((di.modrm & 7) == 1, "rdfsbase rm = ecx");
      CHECK(di.length == 4, "rdfsbase length = 4"); }
    { uint8_t c[] = {0xF3, 0x49, 0x0F, 0xAE, 0xC0};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::RDFSBASE, "rdfsbase r8 decode");
      CHECK((di.rex & 0x08) != 0, "rdfsbase REX.W set");
      CHECK((di.rex & 0x01) != 0, "rdfsbase REX.B set (r8)");
      CHECK(di.length == 5, "rdfsbase r8 length = 5"); }
    { uint8_t c[] = {0x0F, 0x38, 0xF9, 0x08};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::MOVDIRI, "movdiri operands decode");
      CHECK(di.desc->num_operands == 2, "movdiri has 2 operands");
      CHECK(di.desc->operands[0].addr == AddrMode::MemOnly, "movdiri op0 is MemOnly");
      CHECK(di.desc->operands[1].addr == AddrMode::ModRM_Reg, "movdiri op1 is ModRM_Reg");
      CHECK((di.modrm >> 6) == 0, "movdiri mod = 0 (memory)"); }
    { uint8_t c[] = {0xC4, 0xE2, 0x6B, 0xF5, 0xCB};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::PDEP, "pdep decode");
      CHECK(di.has_vex, "pdep is VEX");
      CHECK(di.vex_vvvv == 2, "pdep vvvv = edx (2)");
      CHECK(((di.modrm >> 3) & 7) == 1, "pdep reg = ecx");
      CHECK((di.modrm & 7) == 3, "pdep rm = ebx"); }
    { uint8_t c[] = {0x0F, 0x3A, 0xCC, 0xC8, 0x03};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::SHA1RNDS4, "sha1rnds4 decode");
      CHECK(di.desc->num_operands == 3, "sha1rnds4 has 3 operands");
      CHECK(di.immediate == 3, "sha1rnds4 imm = 3");
      CHECK(di.length == 5, "sha1rnds4 length = 5"); }
    { uint8_t c[] = {0x66, 0x0F, 0xAE, 0x78, 0x40};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::CLFLUSHOPT, "clflushopt [rax+0x40] decode");
      CHECK((di.modrm >> 6) == 1, "clflushopt mod = 1 (disp8)");
      CHECK(di.displacement == 0x40, "clflushopt disp = 0x40");
      CHECK(di.length == 5, "clflushopt length = 5"); }
    { uint8_t c[] = {0xF3, 0x0F, 0xBD, 0x0C, 0x4B};
      DecodedInstr di; decode(c, sizeof(c), di);
      CHECK(di.desc && di.desc->mnemonic == Mnemonic::LZCNT, "lzcnt sib decode");
      CHECK((di.modrm & 7) == 4, "lzcnt rm = 4 (SIB)");
      CHECK(di.sib != 0, "lzcnt has SIB");
      CHECK(di.length == 5, "lzcnt sib length = 5"); }
    printf("\n%d/%d edge case tests passed\n", g_pass, g_pass+g_fail);
    return g_fail > 0 ? 1 : 0;
}
