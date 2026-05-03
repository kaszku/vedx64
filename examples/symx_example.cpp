// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/symx.hpp"
#include "vedx64/ir.hpp"
#include <cstdio>
#include <cstring>

using namespace vedx64;
using namespace vedx64::symx;

int main() {
    std::printf("=== symx example: branching on a symbolic input ===\n\n");

    // A small comparator — simulates: int f(uint64_t x) { return x > 100 ? 1 : 0; }
    //   xor rax, rax        48 31 C0
    //   cmp rdi, 100        48 83 FF 64
    //   setg al             0F 9F C0
    //   ret                 C3
    static const uint8_t prog[] = {
        0x48, 0x31, 0xC0,
        0x48, 0x83, 0xFF, 0x64,
        0x0F, 0x9F, 0xC0,
        0xC3,
    };
    constexpr uint64_t base = 0x400000;

    Config cfg{};
    cfg.max_steps_per_path = 32;
    Engine eng(cfg, [&](uint64_t addr, uint8_t* out, size_t n) {
        if (addr < base || addr + n > base + sizeof(prog)) return false;
        std::memcpy(out, prog + (addr - base), n);
        return true;
    });

    // Leave RDI symbolic — the engine will track RAX as a function of it.
    auto results = eng.run(base, [&](const State& s) { return s.rip == base + sizeof(prog) - 1; });

    std::printf("Finished paths: %zu\n", results.size());
    for (size_t i = 0; i < results.size(); ++i) {
        const State& s = results[i];
        std::printf("\n-- path %zu (depth=%u, %zu PC terms) --\n",
            i, s.fork_depth, s.pc.terms.size());
        std::printf("  RIP = 0x%llX\n", (unsigned long long)s.rip);
        std::printf("  RAX = %s\n", Builder::to_string(s.gpr[0]).c_str());
        std::printf("  ZF  = %s\n", Builder::to_string(s.flags[ZF]).c_str());
    }
    std::printf("\nbuilder arena=%zu nodes, symbols=%zu\n",
        eng.builder().arena_size(), eng.builder().symbol_count());
    return 0;
}
