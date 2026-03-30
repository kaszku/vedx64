// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/core.hpp"
#include "vedx64/instruction.hpp"
#include <cstdio>
#include <cstring>
using namespace vedx64;
static int g_pass=0,g_fail=0;
#define CHECK(c,m) do{if(!(c)){printf("FAIL: %s (line %d)\n",m,__LINE__);g_fail++;}else{g_pass++;}}while(0)

static bool decode_check(const uint8_t* code, size_t len, Mnemonic expected, const char* name) {
    DecodedInstr di; size_t n = decode(code, len, di);
    if (n == 0) { printf("FAIL: %s decode=0\n", name); g_fail++; return false; }
    if (di.desc->mnemonic != expected) { printf("FAIL: %s wrong mnemonic\n", name); g_fail++; return false; }
    g_pass++; return true;
}

static void roundtrip(const uint8_t* code, size_t len, const char* name) {
    DecodedInstr di; size_t n = decode(code, len, di);
    if (n == 0) { printf("FAIL: %s dec=0\n", name); g_fail++; return; }
    uint8_t buf[15]; size_t en = encode(di, buf, sizeof(buf));
    if (en == n && memcmp(code, buf, n) == 0) g_pass++;
    else { printf("FAIL: %s roundtrip (d=%zu e=%zu) orig=", name, n, en);
        for(size_t i=0;i<n;i++) printf("%02X ",code[i]); printf("enc=");
        for(size_t i=0;i<en;i++) printf("%02X ",buf[i]); printf("\n"); g_fail++; }
}

int main() {
    printf("=== AVX-512 / VNNI / AMX / APX Tests ===\n\n");

    printf("--- EVEX prefix fields ---\n");
    {
        uint8_t c[] = {0x62, 0xF3, 0x75, 0x89, 0x25, 0xC2, 0xFF};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n == 7, "VPTERNLOGD length=7");
        CHECK(di.has_evex, "has_evex");
        CHECK(di.has_vex, "has_vex (compat)");
        CHECK(di.evex_aaa == 1, "evex_aaa=k1");
        CHECK(di.evex_z, "evex_z=true");
        CHECK(di.desc->mnemonic == Mnemonic::VPTERNLOGD, "mnemonic=VPTERNLOGD");
        CHECK((uint8_t)di.immediate == 0xFF, "imm=0xFF");
        CHECK(di.vex_vvvv == 1, "vvvv=1");
        CHECK(di.desc->num_operands == 4, "4 operands");
    }

    printf("--- EVEX roundtrip ---\n");
    { uint8_t c[] = {0x62, 0xF3, 0x75, 0x89, 0x25, 0xC2, 0xFF}; roundtrip(c, 7, "VPTERNLOGD"); }

    printf("--- VNNI ---\n");
    { uint8_t c[] = {0x62, 0xF2, 0x75, 0x08, 0x50, 0xC2}; decode_check(c, 6, Mnemonic::VPDPBUSD, "VPDPBUSD"); }
    { uint8_t c[] = {0x62, 0xF2, 0x75, 0x08, 0x51, 0xC2}; decode_check(c, 6, Mnemonic::VPDPBUSDS, "VPDPBUSDS"); }
    { uint8_t c[] = {0x62, 0xF2, 0x75, 0x08, 0x52, 0xC2}; decode_check(c, 6, Mnemonic::VPDPWSSD, "VPDPWSSD"); }
    { uint8_t c[] = {0x62, 0xF2, 0x75, 0x08, 0x53, 0xC2}; decode_check(c, 6, Mnemonic::VPDPWSSDS, "VPDPWSSDS"); }

    printf("--- AVX-512 logical ---\n");
    { uint8_t c[] = {0x62, 0xF1, 0x7D, 0x08, 0xEF, 0xC0}; DecodedInstr di; size_t n = decode(c, 6, di);
      CHECK(n == 6, "VPXORD decode length"); CHECK(di.has_evex, "VPXORD has_evex");
      CHECK(di.desc->mnemonic == Mnemonic::VPXORD, "VPXORD mnemonic (not PXOR)"); }
    { uint8_t c[] = {0x62, 0xF1, 0xF5, 0x08, 0xDB, 0xC2}; DecodedInstr di; size_t n = decode(c, 6, di);
      CHECK(n == 6, "VPANDQ decode length"); CHECK(di.has_evex, "VPANDQ has_evex"); CHECK(di.vex_W, "VPANDQ W=1");
      // D/Q variant determined by W bit at runtime; decoder returns VPANDD (first match)
      CHECK(di.desc->mnemonic == Mnemonic::VPANDQ, "VPANDQ mnemonic (W=1 selects Q)"); }

    printf("--- AVX-512 blend ---\n");
    {
        uint8_t c[] = {0x62, 0xF2, 0x75, 0x0A, 0x65, 0xC2};
        DecodedInstr di; decode(c, sizeof(c), di);
        CHECK(di.desc->mnemonic == Mnemonic::VBLENDMPS, "VBLENDMPS mnemonic");
        CHECK(di.evex_aaa == 2, "VBLENDMPS mask=k2");
        CHECK(di.evex_z == false, "VBLENDMPS no zeroing");
    }

    printf("--- compress/expand ---\n");
    { uint8_t c[] = {0x62, 0xF2, 0x7D, 0x09, 0x8A, 0xC1}; decode_check(c, 6, Mnemonic::VCOMPRESSPS, "VCOMPRESSPS"); }
    { uint8_t c[] = {0x62, 0xF2, 0x7D, 0x09, 0x88, 0xC1}; decode_check(c, 6, Mnemonic::VEXPANDPS, "VEXPANDPS"); }

    printf("--- VexReg/XmmVvvv operands ---\n");
    {
        uint8_t c[] = {0xC4, 0xE2, 0x70, 0xF2, 0xC2};
        DecodedInstr di; decode(c, sizeof(c), di);
        CHECK(di.desc->mnemonic == Mnemonic::ANDN, "ANDN mnemonic");
        CHECK(di.desc->num_operands == 3, "ANDN 3 ops");
        CHECK(di.desc->operands[1].addr == AddrMode::VexReg, "ANDN op[1]=VexReg");
        auto instr = Instruction::from_decoded(di, c, 0);
        CHECK(instr.operands.size() == 3, "ANDN Instruction 3 operands");
    }

    {
        uint8_t c[] = {0xC4, 0xE2, 0x71, 0x98, 0xC2};
        DecodedInstr di; decode(c, sizeof(c), di);
        CHECK(di.desc->mnemonic == Mnemonic::VFMADD132PS, "VFMADD132PS");
        CHECK(di.desc->num_operands == 3, "FMA 3 ops");
        CHECK(di.desc->operands[1].addr == AddrMode::XmmVvvv, "FMA op[1]=XmmVvvv");
    }

    printf("--- REX2 (APX) ---\n");
    {
        uint8_t c[] = {0xD5, 0x10, 0x01, 0xC0};
        DecodedInstr di; size_t n = decode(c, sizeof(c), di);
        CHECK(n == 4, "REX2 ADD length=4");
        CHECK(di.has_rex2, "has_rex2");
        CHECK(di.rex2_r4 == 1, "rex2_r4=1");
        CHECK(di.rex2_x4 == 0, "rex2_x4=0");
        CHECK(di.rex2_b4 == 0, "rex2_b4=0");
    }

#ifdef VEDX64_STRINGS
    printf("--- Disassembly ---\n");
    {
        uint8_t c[] = {0x62, 0xF3, 0x75, 0x89, 0x25, 0xC2, 0xFF};
        char buf[128];
        disassemble(c, sizeof(c), buf, sizeof(buf), 0);
        CHECK(strstr(buf, "vpternlogd") != nullptr, "disasm vpternlogd");
        CHECK(strstr(buf, "{k1}") != nullptr, "disasm {k1}");
        CHECK(strstr(buf, "{z}") != nullptr, "disasm {z}");
    }
#endif

    printf("\n%d/%d avx512 tests passed\n", g_pass, g_pass+g_fail);
    return g_fail > 0 ? 1 : 0;
}
