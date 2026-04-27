// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski


#include "vedx64/analysis.hpp"
#include "vedx64/instruction.hpp"
#include "vedx64/codegen.hpp"
#include <cstdio>
#include <vector>

using namespace vedx64;
using namespace vedx64::analysis;

// A small Win64-style function:
//   55              push rbp
//   48 89 E5        mov  rbp, rsp
//   48 83 EC 20     sub  rsp, 0x20
//   89 7D FC        mov  [rbp-4], edi
//   83 7D FC 00     cmp  dword [rbp-4], 0
//   74 09           jz   $+9         (skips the body if input == 0)
//   8B 45 FC        mov  eax, [rbp-4]
//   0F AF C0        imul eax, eax
//   EB 02           jmp  $+2
//   33 C0           xor  eax, eax
//   48 83 C4 20     add  rsp, 0x20
//   5D              pop  rbp
//   C3              ret
static const uint8_t kSampleCode[] = {
    0x55,
    0x48, 0x89, 0xE5,
    0x48, 0x83, 0xEC, 0x20,
    0x89, 0x7D, 0xFC,
    0x83, 0x7D, 0xFC, 0x00,
    0x74, 0x09,
    0x8B, 0x45, 0xFC,
    0x0F, 0xAF, 0xC0,
    0xEB, 0x02,
    0x33, 0xC0,
    0x48, 0x83, 0xC4, 0x20,
    0x5D,
    0xC3,
};

int main() {
    std::printf("=== analysis.hpp worked example ===\n\n");

    // 1. Skip the prologue and report what we found.
    size_t prologue = skip_prologue(kSampleCode, sizeof(kSampleCode));
    std::printf("Prologue: %zu bytes (push rbp; mov rbp,rsp; sub rsp,0x20)\n", prologue);

    // 2. Walk every instruction, classify it, and dump a one-liner summary.
    std::printf("\n--- Per-instruction classification ---\n");
    size_t off = 0;
    int branches = 0, calls = 0, rets = 0;
    while (off < sizeof(kSampleCode)) {
        DecodedInstr di;
        size_t len = decode(kSampleCode + off, sizeof(kSampleCode) - off, di);
        if (len == 0 || !di.desc) break;
        Mnemonic m = di.desc->mnemonic;

        const char* tag = "seq";
        if (is_jcc(m))                    { tag = "jcc"; ++branches; }
        else if (is_unconditional_branch(m)) { tag = "jmp"; ++branches; }
        else if (is_call(m))               { tag = "call"; ++calls; }
        else if (is_ret(m))                { tag = "ret";  ++rets; }
        else if (is_arith(m))              { tag = "arith"; }
        else if (is_logical(m))            { tag = "logic"; }
        else if (is_shift(m))              { tag = "shift"; }

        EFlagMask def = sets_eflags(m);
        std::printf("  %04zx  [%-5s]  flags-write=0x%02X\n", off, tag, def);
        off += len;
    }
    std::printf("\nTotal: %d branches, %d calls, %d returns\n",
                branches, calls, rets);

    // 3. Demonstrate the patchers: build a 5-byte JMP rel32 in a buffer.
    std::printf("\n--- Patchers ---\n");
    uint8_t scratch[5] = {0};
    patch_jmp_rel32(scratch, 0x12345678);
    std::printf("patch_jmp_rel32(0x12345678) ->");
    for (auto b : scratch) std::printf(" %02X", b);
    std::printf("\n");

    // 4. Caller-state-aware concrete execution scaffold: seed RCX and RDX.
    std::printf("\n--- Sparse GPR seeding ---\n");
    uint64_t gpr[16] = {0};
    apply_sparse_gprs(gpr, {{1 /*RCX*/, 0xCAFE}, {2 /*RDX*/, 0xBEEF}});
    std::printf("RCX = 0x%llX, RDX = 0x%llX\n",
                (unsigned long long)gpr[1], (unsigned long long)gpr[2]);

    // 5. Demand-paged memory: store an x86-64 address and read it back.
    std::printf("\n--- DemandPagedMemory ---\n");
    DemandPagedMemory mem;
    mem.write_u64(0x7FFFFFFF1000ULL, 0xDEADBEEFCAFEBABEULL);
    std::printf("read @0x7FFFFFFF1000 = 0x%llX (pages=%zu)\n",
                (unsigned long long)mem.read_u64(0x7FFFFFFF1000ULL),
                mem.page_count());

    // 6. CodeGen anchor / relocation metadata.
    std::printf("\n--- CodeGen anchors + relocs ---\n");
    CodeGen g;
    g.add_anchor("entry");
    g.mov(rax, (int64_t)1);
    g.add_anchor("after_mov");
    g.emit_rel32_placeholder("forward_target");  // placeholder for later patching
    g.ret();
    for (const auto& a : g.anchors())
        std::printf("anchor %-15s offset=%zu\n", a.name.c_str(), a.offset);
    for (const auto& r : g.relocations())
        std::printf("reloc  %-15s offset=%zu size=%u\n", r.name.c_str(), r.offset, (unsigned)r.size);

    return 0;
}
