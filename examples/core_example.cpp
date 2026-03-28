// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/core.hpp"
#include <cstdio>
#include <cstring>

using namespace vedx64;

#ifdef VEDX64_STRINGS
void example_disassemble() {
    printf("=== Disassembly Example ===\n");

    // x86-64 function prologue + body + epilogue
    uint8_t code[] = {
        0x55,                         // push rbp
        0x48, 0x89, 0xE5,             // mov rbp, rsp
        0x48, 0x83, 0xEC, 0x20,       // sub rsp, 0x20
        0x89, 0x7D, 0xFC,             // mov [rbp-4], edi
        0x8B, 0x45, 0xFC,             // mov eax, [rbp-4]
        0x83, 0xC0, 0x01,             // add eax, 1
        0x0F, 0xAF, 0xC0,             // imul eax, eax
        0x48, 0x83, 0xC4, 0x20,       // add rsp, 0x20
        0x5D,                         // pop rbp
        0xC3,                         // ret
    };

    uint64_t rip = 0x401000;
    size_t offset = 0;
    while (offset < sizeof(code)) {
        char buf[256];
        size_t n = disassemble(&code[offset], sizeof(code) - offset, buf, sizeof(buf), rip + offset);
        if (n == 0) break;

        printf("  %016llx: ", (unsigned long long)(rip + offset));
        for (size_t i = 0; i < n && i < 8; ++i)
            printf("%02X ", code[offset + i]);
        for (size_t i = n; i < 8; ++i)
            printf("   ");
        printf("  %s\n", buf);
        offset += n;
    }
}
#endif // VEDX64_STRINGS

void example_encode() {
    printf("\n=== Encoding Example ===\n");

    // Decode an instruction, then re-encode it
    uint8_t original[] = {0x48, 0x83, 0xEC, 0x20}; // sub rsp, 0x20
    DecodedInstr di;
    size_t n = decode(original, sizeof(original), di);

    if (n > 0 && di.desc) {
#ifdef VEDX64_STRINGS
        printf("  Decoded: %s (length=%zu)\n", mnemonic_name(di.desc->mnemonic), n);
#endif // VEDX64_STRINGS
        printf("  REX=0x%02X, ModRM=0x%02X, imm=%lld\n", di.rex, di.modrm, (long long)di.immediate);

        uint8_t encoded[16];
        size_t enc_len = encode(di, encoded, sizeof(encoded));
        printf("  Re-encoded (%zu bytes): ", enc_len);
        for (size_t i = 0; i < enc_len; ++i)
            printf("%02X ", encoded[i]);
        printf("\n");
        printf("  Match: %s\n", (enc_len == n && memcmp(original, encoded, n) == 0) ? "YES" : "NO");
    }
}

void example_table_stats() {
    printf("\n=== Instruction Table Stats ===\n");
    printf("  Total instructions: %zu\n", g_instr_table_size);

    size_t one_byte = 0, two_byte = 0, with_ext = 0;
    for (size_t i = 0; i < g_instr_table_size; ++i) {
        if (g_instr_table[i].opcode > 0xFF) two_byte++;
        else one_byte++;
        if (g_instr_table[i].opcd_ext != 0xFF) with_ext++;
    }
    printf("  One-byte opcodes: %zu\n", one_byte);
    printf("  Two-byte opcodes: %zu\n", two_byte);
    printf("  With opcode extension: %zu\n", with_ext);
}

void example_avx512() {
    printf("\n=== AVX-512 / EVEX Example ===\n");

    // VPTERNLOGD XMM0{k1}{z}, XMM1, XMM2, 0xFF
    uint8_t evex_code[] = {0x62, 0xF3, 0x75, 0x89, 0x25, 0xC2, 0xFF};
    DecodedInstr di;
    size_t n = decode(evex_code, sizeof(evex_code), di);
    if (n > 0 && di.desc) {
#ifdef VEDX64_STRINGS
        char buf[128]; disassemble(evex_code, sizeof(evex_code), buf, sizeof(buf), 0);
        printf("  Disasm: %s\n", buf);
#endif
        printf("  EVEX: %s, mask=k%u, zeroing=%s, vvvv=%u\n",
               di.has_evex ? "yes" : "no", di.evex_aaa, di.evex_z ? "yes" : "no", di.vex_vvvv);

        // Roundtrip encode
        uint8_t re[15]; size_t en = encode(di, re, sizeof(re));
        printf("  Roundtrip: %s (%zu bytes)\n", (en == n && memcmp(evex_code, re, n) == 0) ? "OK" : "MISMATCH", en);
    }
}

int main() {
#ifdef VEDX64_STRINGS
    example_disassemble();
#endif // VEDX64_STRINGS
    example_encode();
    example_table_stats();
    example_avx512();
    return 0;
}
