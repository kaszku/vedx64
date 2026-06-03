// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <functional>
#include "vedx64/core.hpp"

namespace vedx64 {
namespace branch {

/// What the transfer does (semantic effect).
enum class Effect : uint8_t {
    None,
    Jump,
    ConditionalJump,
    Call,
    Return,
    Interrupt,
    SysTransfer,
    Unknown,
};

/// How the transfer is encoded (the recognized shape). Exhaustive.
enum class Form : uint8_t {
    None,              ///< not a control transfer
    RelNear,           ///< E9/EB/E8/7x/0F8x — pc-relative immediate
    CountConditional,  ///< LOOP/LOOPE/LOOPNE/JrCXZ
    IndirectReg,       ///< FF /4, FF /2 register form (jmp/call reg)
    IndirectMem,       ///< FF /4, FF /2 memory form (GOT/IAT/literal/table)
    FarDirect,         ///< EA/9A ptr16:16/32 (legacy, #UD in long mode)
    FarIndirect,       ///< FF /5, FF /3 m16:64
    ReturnNear,        ///< C3 / C2 iw
    ReturnFar,         ///< CB / CA iw
    ReturnInterrupt,   ///< IRET/IRETD/IRETQ
    Interrupt,         ///< INT/INT1/INT3/INTO/UD0/UD1/UD2
    Syscall,           ///< SYSCALL/SYSRET/SYSENTER/SYSEXIT
    PushImmRet,        ///< push imm8/imm32 ; ret
    PushRegRet,        ///< push reg ; ret
    PushSplitImm64Ret, ///< push imm32 ; mov [rsp+4], imm32 ; ret
    MovRegBranch,      ///< mov reg, imm ; jmp/call reg
    LeaRegBranch,      ///< lea reg, [rip+d] ; jmp/call reg
    MovLeaPushRet,     ///< mov/lea reg, X ; push reg ; ret
    LoadMemBranch,     ///< mov reg, [mem] ; jmp/call reg (literal pool / GOT into reg)
    ComputedRegBranch, ///< mov reg,imm ; <imm arith on reg>* ; jmp/call reg
};

/// How the destination address is obtained.
enum class Resolution : uint8_t {
    Concrete,          ///< `target` holds the final destination VA
    MemoryPointer,     ///< destination = *(ptr_size bytes at `slot`)
    RegisterDynamic,   ///< depends on register `reg`; value not static
    FarPointer,        ///< `selector` : `target`
    StackDynamic,      ///< RET with no recoverable preceding push
    Unknown,           ///< could not be determined
};

/// Effective-address description of a memory operand (where a pointer
/// lives, or a jump-table element). Mirrors the decoder's modrm/sib model.
struct MemRef {
    uint8_t  base   = 0xFF;   ///< base reg id (0xFF if none).
    uint8_t  index  = 0xFF;   ///< index reg id (0xFF if none).
    uint8_t  scale  = 1;      ///< 1/2/4/8.
    int64_t  disp   = 0;      ///< displacement.
    bool     rip_relative = false;
    uint8_t  segment = 0xFF;  ///< segment override prefix byte (0xFF if none).
    bool     static_addr = false; ///< true when abs_addr is statically known.
    uint64_t abs_addr = 0;    ///< absolute EA when static_addr (RIP-rel / pure-disp).
};

/// The result of recognizing a control transfer at an address.
struct BranchPattern {
    bool       valid        = false;
    Effect     effect       = Effect::None;
    Form       form         = Form::None;
    Resolution resolution   = Resolution::Unknown;

    uint64_t   address      = 0;   ///< VA of the first instruction.
    uint32_t   total_length = 0;   ///< bytes across ALL instructions in the idiom.
    uint8_t    insn_count   = 0;   ///< number of instructions consumed.
    uint8_t    insn_len[6]  = {}; ///< per-instruction lengths (idioms are short).

    bool       is_conditional  = false;
    bool       is_far          = false;
    bool       pushes_return   = false; ///< call-like (CALL, mov+call, ...).
    bool       has_fallthrough = true;  ///< can execution continue past it?

    // ---- destination ----
    uint64_t   target    = 0;     ///< Concrete: final VA. FarPointer: offset.
    uint16_t   selector  = 0;     ///< FarPointer selector.
    uint8_t    reg       = 0xFF;  ///< RegisterDynamic: the deciding register.
    MemRef     slot;              ///< MemoryPointer: where the pointer/element lives.
    uint8_t    ptr_size  = 8;     ///< bytes to read at the slot (2/4/8).

    // ---- optional one-level resolution via read_mem ----
    bool       resolved     = false; ///< final_target was read through memory.
    uint64_t   final_target = 0;
    uint8_t    chain_depth  = 0;     ///< # forwarding hops followed (0 by default).
};

/// Provide instruction bytes / memory bytes at a virtual address.
/// Return false if the range is unavailable.
using ReadCode = std::function<bool(uint64_t addr, uint8_t* out, size_t n)>;
using ReadMem  = std::function<bool(uint64_t addr, uint8_t* out, size_t n)>;

struct Options {
    uint8_t max_idiom_insns = 6;     ///< bound the multi-instruction scan.
    bool    fold_arith      = true;  ///< recognize ComputedRegBranch.
    bool    follow_memory   = true;  ///< use read_mem to resolve MemoryPointer (one level).
    bool    follow_chains   = false; ///< also chase JMP->JMP / stub->stub chains.
    uint8_t max_chain_depth = 8;
};

/// Recognize the transfer idiom that STARTS at `address`. `read_code` must
/// cover the idiom bytes. If `read_mem` is provided and opt.follow_memory is
/// set, a statically-addressed MemoryPointer is read one level to fill
/// final_target.
BranchPattern recognize(uint64_t address, const ReadCode& read_code,
                        const Options& opt = {}, const ReadMem& read_mem = {});

/// Convenience overload over a flat buffer; `address` is the VA of code[0].
BranchPattern recognize(const uint8_t* code, size_t len, uint64_t address,
                        const Options& opt = {}, const ReadMem& read_mem = {});

/// Resolve (or re-resolve) the final target of an already-recognized pattern
/// by reading its memory slot. Follows forwarding chains only when
/// opt.follow_chains is set. Returns true if final_target was produced.
bool resolve_target(BranchPattern& p, const ReadMem& read_mem,
                    const Options& opt = {});

} // namespace branch
} // namespace vedx64
