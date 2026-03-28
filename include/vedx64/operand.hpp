// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>

namespace vedx64 {

/// x86-64 control registers.
enum class ControlReg : uint8_t {
    CR0 = 0,
    CR2 = 2,
    CR3 = 3,
    CR4 = 4,
    CR8 = 8,
};

/// x86-64 debug registers.
enum class DebugReg : uint8_t {
    DR0 = 0,
    DR1 = 1,
    DR2 = 2,
    DR3 = 3,
    DR4 = 4,
    DR5 = 5,
    DR6 = 6,
    DR7 = 7,
};

/// x86-64 segment registers.
enum class SegmentReg : uint8_t {
    ES = 0,
    CS = 1,
    SS = 2,
    DS = 3,
    FS = 4,
    GS = 5,
};

/// Legacy prefix types.
enum class PrefixType : uint8_t {
    None,
    Lock,
    RepNE,
    Rep,
    OpSize,
    AddrSize,
    SegES,
    SegCS,
    SegSS,
    SegDS,
    SegFS,
    SegGS,
};

/// Discriminator for the Operand tagged union.
enum class OperandType : uint8_t {
    None,
    Register,
    Immediate,
    Memory,
    RelativeOffset,
};

/// Register file classification.
enum class RegisterClass : uint8_t {
    GPR8,
    GPR8H,
    GPR16,
    GPR32,
    GPR64,
    XMM,
    MMX,
    Segment,
    Control,
    Debug,
    Flags,
    IP,
};

/// Tagged union representing a decoded x86-64 operand (16 bytes).
///
/// Layout: [type:8][flags:8][data:48][value:64] = 128 bits = 16 bytes
/// The `flags` byte packs reg_class / size / scale / segment / rip_relative.
struct Operand {
    OperandType type = OperandType::None;

    union {
        struct {
            RegisterClass reg_class;
            uint8_t reg_id;     ///< 0-15 (or 0-7 for legacy)
            uint16_t size_bits;  ///< 8, 16, 32, 64, 128, 256, 512
        } reg;

        struct {
            uint8_t size;       ///< 1, 2, 4, 8 bytes
            bool is_signed;
            uint8_t _pad[5];
            int64_t value;
        } imm;

        struct {
            uint8_t base_reg;   ///< 0-15, 0xFF = none
            uint8_t index_reg;  ///< 0-15, 0xFF = none
            uint8_t flags;      ///< [1:0]=scale_log2, [2]=rip_relative
            uint8_t segment;    ///< segment override (0=default)
            uint8_t size_bytes;  ///< operand size in bytes
            uint8_t _pad[2];
            int64_t disp;       ///< displacement (sign-extended)
        } mem;

        struct {
            uint8_t size;       ///< 1, 2, 4 bytes (encoding size)
            uint8_t _pad[6];
            int64_t offset;     ///< signed offset from next instruction
        } rel;
    };

    static Operand make_reg(RegisterClass cls, uint8_t id, uint16_t size_bits) {
        Operand op{};
        op.type = OperandType::Register;
        op.reg.reg_class = cls;
        op.reg.reg_id = id;
        op.reg.size_bits = size_bits;
        return op;
    }

    static Operand make_imm(int64_t value, uint8_t size, bool is_signed = true) {
        Operand op{};
        op.type = OperandType::Immediate;
        op.imm.value = value;
        op.imm.size = size;
        op.imm.is_signed = is_signed;
        return op;
    }

    static Operand make_mem(uint8_t base, uint8_t index, uint8_t scale,
                            int64_t disp, uint8_t size_bytes,
                            bool rip_rel = false, uint8_t seg = 0) {
        Operand op{};
        op.type = OperandType::Memory;
        op.mem.base_reg = base;
        op.mem.index_reg = index;
        // Pack scale as log2: 1→0, 2→1, 4→2, 8→3
        uint8_t sl = 0;
        if (scale == 2) sl = 1; else if (scale == 4) sl = 2; else if (scale == 8) sl = 3;
        op.mem.flags = sl | (rip_rel ? 0x04 : 0);
        op.mem.disp = disp;
        op.mem.size_bytes = size_bytes;
        op.mem.segment = seg;
        return op;
    }

    static Operand make_rel(int64_t offset, uint8_t size) {
        Operand op{};
        op.type = OperandType::RelativeOffset;
        op.rel.offset = offset;
        op.rel.size = size;
        return op;
    }

    bool is_register() const { return type == OperandType::Register; }
    bool is_immediate() const { return type == OperandType::Immediate; }
    bool is_memory() const { return type == OperandType::Memory; }
    bool is_relative() const { return type == OperandType::RelativeOffset; }

    /// Get memory scale (1, 2, 4, 8) from packed flags.
    uint8_t mem_scale() const { return 1u << (mem.flags & 0x03); }
    /// Check if memory operand is RIP-relative.
    bool mem_rip_relative() const { return (mem.flags & 0x04) != 0; }
};
static_assert(sizeof(Operand) <= 24, "Operand must be at most 24 bytes");

} // namespace vedx64
