// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/branch_resolve.hpp"
#include <cstdio>
#include <cstdint>
#include <cstring>

using namespace vedx64;
using namespace vedx64::branch;

static int g_pass = 0, g_fail = 0;
#define CHECK(cond, msg) do { if (cond) ++g_pass; else { ++g_fail; std::printf("FAIL: %s\n", msg); } } while (0)

static BranchPattern rec(const uint8_t* b, size_t n, uint64_t addr = 0x1000) {
    return recognize(b, n, addr);
}

int main() {
    // ---- direct relative ----
    { uint8_t b[] = {0xE9, 0x00, 0x01, 0x00, 0x00}; // jmp rel32 (+0x100)
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::RelNear && p.effect == Effect::Jump, "jmp rel32 form");
      CHECK(p.resolution == Resolution::Concrete && p.target == 0x1105, "jmp rel32 target");
      CHECK(p.total_length == 5 && p.insn_count == 1, "jmp rel32 size");
      CHECK(!p.has_fallthrough, "jmp rel32 no fallthrough"); }

    { uint8_t b[] = {0xEB, 0x10}; // jmp rel8 (+0x10)
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::RelNear && p.target == 0x1012, "jmp rel8 target"); }

    { uint8_t b[] = {0xE8, 0x00, 0x00, 0x00, 0x00}; // call rel32
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.effect == Effect::Call && p.pushes_return, "call rel32 effect");
      CHECK(p.has_fallthrough && p.target == 0x1005, "call rel32 fallthrough+target"); }

    { uint8_t b[] = {0x0F, 0x84, 0x00, 0x00, 0x00, 0x00}; // je rel32
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.effect == Effect::ConditionalJump && p.is_conditional, "je cond");
      CHECK(p.target == 0x1006 && p.has_fallthrough, "je target+fallthrough"); }

    { uint8_t b[] = {0x74, 0x05}; // je rel8
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.is_conditional && p.target == 0x1007, "je rel8 target"); }

    // ---- count-conditional ----
    { uint8_t b[] = {0xE2, 0xFE}; // loop -2
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::CountConditional && p.is_conditional, "loop form");
      CHECK(p.resolution == Resolution::Concrete && p.target == 0x1000, "loop target"); }

    { uint8_t b[] = {0xE3, 0x05}; // jrcxz +5
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::CountConditional && p.target == 0x1007, "jrcxz target"); }

    // ---- indirect register ----
    { uint8_t b[] = {0xFF, 0xE0}; // jmp rax
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::IndirectReg && p.effect == Effect::Jump, "jmp rax form");
      CHECK(p.resolution == Resolution::RegisterDynamic && p.reg == 0, "jmp rax reg"); }

    { uint8_t b[] = {0x41, 0xFF, 0xE2}; // jmp r10
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::IndirectReg && p.reg == 10, "jmp r10 reg"); }

    { uint8_t b[] = {0xFF, 0xD0}; // call rax
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::IndirectReg && p.effect == Effect::Call, "call rax"); }

    // ---- indirect memory (literal pool / RIP-relative) ----
    { uint8_t b[] = {0xFF, 0x25, 0x00, 0x00, 0x00, 0x00}; // jmp [rip+0]
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::IndirectMem, "jmp [rip] form");
      CHECK(p.resolution == Resolution::MemoryPointer, "jmp [rip] memptr");
      CHECK(p.slot.rip_relative && p.slot.static_addr && p.slot.abs_addr == 0x1006, "jmp [rip] slot");
      CHECK(p.total_length == 6, "jmp [rip] size"); }

    // resolve through read_mem.
    { uint8_t b[] = {0xFF, 0x25, 0x00, 0x00, 0x00, 0x00};
      ReadMem rm = [](uint64_t a, uint8_t* o, size_t n) -> bool {
          if (a == 0x1006 && n == 8) { uint64_t v = 0xCAFEF00D; std::memcpy(o, &v, 8); return true; }
          return false; };
      auto p = recognize(b, sizeof b, 0x1000, {}, rm);
      CHECK(p.resolved && p.final_target == 0xCAFEF00D, "jmp [rip] resolved"); }

    // ---- far indirect ----
    { uint8_t b[] = {0xFF, 0x2D, 0x00, 0x00, 0x00, 0x00}; // jmp far [rip+0]
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::FarIndirect && p.is_far, "jmp far form"); }

    // ---- returns ----
    { uint8_t b[] = {0xC3}; auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::ReturnNear && p.effect == Effect::Return, "ret near");
      CHECK(p.resolution == Resolution::StackDynamic && !p.has_fallthrough, "ret stackdyn"); }
    { uint8_t b[] = {0xC2, 0x08, 0x00}; auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::ReturnNear && p.total_length == 3, "ret imm16"); }
    { uint8_t b[] = {0xCB}; auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::ReturnFar && p.is_far, "retf"); }

    // ---- interrupt / syscall ----
    { uint8_t b[] = {0xCD, 0x80}; auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::Interrupt && p.effect == Effect::Interrupt, "int 0x80"); }
    { uint8_t b[] = {0x0F, 0x0B}; auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::Interrupt && !p.has_fallthrough, "ud2"); }
    { uint8_t b[] = {0x0F, 0x05}; auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::Syscall && p.effect == Effect::SysTransfer, "syscall"); }

    // ---- push imm ; ret ----
    { uint8_t b[] = {0x68, 0xEF, 0xBE, 0xAD, 0xDE, 0xC3}; // push 0xDEADBEEF ; ret
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::PushImmRet && p.effect == Effect::Jump, "push imm ret form");
      CHECK(p.resolution == Resolution::Concrete && p.target == 0xFFFFFFFFDEADBEEFull, "push imm ret target (sext)");
      CHECK(p.insn_count == 2 && p.total_length == 6, "push imm ret size"); }

    // ---- push imm32 ; mov [rsp+4], imm32 ; ret ----
    { uint8_t b[] = {0x68, 0x78, 0x56, 0x34, 0x12,
                     0xC7, 0x44, 0x24, 0x04, 0xEF, 0xBE, 0xAD, 0xDE,
                     0xC3};
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::PushSplitImm64Ret, "split push form");
      CHECK(p.target == 0xDEADBEEF12345678ull, "split push target");
      CHECK(p.insn_count == 3, "split push count"); }

    // ---- push reg ; ret ----
    { uint8_t b[] = {0x50, 0xC3}; // push rax ; ret
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::PushRegRet, "push reg ret form");
      CHECK(p.resolution == Resolution::RegisterDynamic && p.reg == 0, "push reg ret reg"); }

    // ---- mov reg, imm64 ; jmp reg ----
    { uint8_t b[] = {0x48, 0xB8, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0xFF, 0xE0};
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::MovRegBranch && p.effect == Effect::Jump, "mov imm64 jmp form");
      CHECK(p.resolution == Resolution::Concrete && p.target == 0x1122334455667788ull, "mov imm64 jmp target");
      CHECK(p.insn_count == 2 && p.total_length == 12, "mov imm64 jmp size"); }

    // ---- mov reg, imm32 ; call reg (zero-extended) ----
    { uint8_t b[] = {0xB8, 0x00, 0x10, 0x00, 0x00, 0xFF, 0xD0};
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::MovRegBranch && p.effect == Effect::Call, "mov imm32 call form");
      CHECK(p.target == 0x1000 && p.pushes_return, "mov imm32 call target"); }

    // ---- lea reg, [rip+d] ; jmp reg ----
    { uint8_t b[] = {0x48, 0x8D, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0}; // lea rax,[rip+0]; jmp rax
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::LeaRegBranch, "lea jmp form");
      CHECK(p.resolution == Resolution::Concrete && p.target == 0x1007, "lea jmp target (rip+len)"); }

    // ---- mov reg, imm64 ; push reg ; ret ----
    { uint8_t b[] = {0x48, 0xB8, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0xC3};
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::MovLeaPushRet, "mov push ret form");
      CHECK(p.resolution == Resolution::Concrete && p.target == 0x2000, "mov push ret target");
      CHECK(p.insn_count == 3 && !p.has_fallthrough, "mov push ret count"); }

    // ---- mov reg, [rip+d] ; jmp reg (literal-pool into reg) ----
    { uint8_t b[] = {0x48, 0x8B, 0x05, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xE0};
      ReadMem rm = [](uint64_t a, uint8_t* o, size_t n) -> bool {
          if (a == 0x1007 && n == 8) { uint64_t v = 0xABCD1234; std::memcpy(o, &v, 8); return true; }
          return false; };
      auto p = recognize(b, sizeof b, 0x1000, {}, rm);
      CHECK(p.valid && p.form == Form::LoadMemBranch && p.resolution == Resolution::MemoryPointer, "load mem branch form");
      CHECK(p.slot.static_addr && p.slot.abs_addr == 0x1007, "load mem branch slot");
      CHECK(p.resolved && p.final_target == 0xABCD1234, "load mem branch resolved"); }

    // ---- computed: mov eax,imm ; add eax,imm ; jmp rax ----
    { uint8_t b[] = {0xB8, 0x00, 0x10, 0x00, 0x00,   // mov eax, 0x1000
                     0x83, 0xC0, 0x10,               // add eax, 0x10
                     0xFF, 0xE0};                    // jmp rax
      auto p = rec(b, sizeof b);
      CHECK(p.valid && p.form == Form::ComputedRegBranch, "computed form");
      CHECK(p.resolution == Resolution::Concrete && p.target == 0x1010, "computed target");
      CHECK(p.insn_count == 3, "computed count"); }

    // ---- negative: a plain mov that is NOT followed by a branch ----
    { uint8_t b[] = {0x48, 0x89, 0xC3}; // mov rbx, rax
      auto p = rec(b, sizeof b);
      CHECK(!p.valid, "non-branch mov rejected"); }

    std::printf("\n%d/%d branch_resolve checks passed\n", g_pass, g_pass + g_fail);
    return g_fail > 0 ? 1 : 0;
}
