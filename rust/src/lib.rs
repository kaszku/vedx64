// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

//! Rust bindings for the vedx64 x86-64 library.
//!
//! Uses `cxx` for direct C++ interop — no C wrapper layer.

#[cxx::bridge(namespace = "vedx64::bridge")]
mod ffi {
    #[derive(Debug, Clone)]
    struct FlowResult {
        flow_type: u8,
        address: u64,
        target: u64,
        is_indirect: bool,
        has_fallthrough: bool,
        length: u8,
    }

    #[derive(Debug, Clone)]
    struct SemResult {
        flags_read: u8,
        flags_written: u8,
        flags_undefined: u8,
        flow: u8,
        category: u8,
        ring: u8,
        lock_valid: bool,
        is_privileged: bool,
    }

    #[derive(Debug, Clone, Default)]
    struct IrVarNode {
        space: u8,
        offset: u16,
        size: u8,
        value: i64,
    }

    #[derive(Debug, Clone)]
    struct IrOp {
        opcode: u8,
        output: IrVarNode,
        input0: IrVarNode,
        input1: IrVarNode,
        input2: IrVarNode,
        num_inputs: u8,
    }

    #[derive(Debug)]
    struct IrLifted {
        ops: Vec<IrOp>,
        address: u64,
        length: u8,
    }

    unsafe extern "C++" {
        include!("vedx64_bridge.h");

        type Decoded;
        type Emu;

        // Core
        fn decode(code: &[u8]) -> UniquePtr<Decoded>;
        fn encode(d: &Decoded) -> Vec<u8>;
        fn decoded_length(d: &Decoded) -> u8;
        fn decoded_mnemonic(d: &Decoded) -> u16;
        fn decoded_num_operands(d: &Decoded) -> u8;
        fn decoded_rex(d: &Decoded) -> u8;
        fn decoded_modrm(d: &Decoded) -> u8;
        fn decoded_sib(d: &Decoded) -> u8;
        fn decoded_displacement(d: &Decoded) -> i64;
        fn decoded_immediate(d: &Decoded) -> i64;
        fn decoded_has_vex(d: &Decoded) -> bool;
        fn decoded_vex_vvvv(d: &Decoded) -> u8;
        fn decoded_vex_l(d: &Decoded) -> u8;
        fn decoded_vex_w(d: &Decoded) -> bool;

        fn disassemble(code: &[u8], rip: u64) -> String;
        fn mnemonic_name(m: u16) -> String;
        fn table_size() -> usize;

        // Assembler
        fn assemble(text: &str) -> Vec<u8>;
        fn assemble_block(text: &str) -> Vec<u8>;

        // Relocation
        fn can_relocate(code: &[u8]) -> bool;
        fn is_rip_relative(code: &[u8]) -> bool;

        // Branch following
        fn classify_flow(code: &[u8], addr: u64) -> FlowResult;

        // Semantics
        fn get_semantics(d: &Decoded) -> SemResult;

        // Emulator
        fn emu_new(mem_size: usize) -> UniquePtr<Emu>;
        fn emu_step(e: Pin<&mut Emu>) -> i32;
        fn emu_run(e: Pin<&mut Emu>, max_steps: usize) -> usize;
        fn emu_rip(e: &Emu) -> u64;
        fn emu_set_rip(e: Pin<&mut Emu>, v: u64);
        fn emu_gpr(e: &Emu, i: usize) -> u64;
        fn emu_set_gpr(e: Pin<&mut Emu>, i: usize, v: u64);
        fn emu_rflags(e: &Emu) -> u64;
        fn emu_set_rflags(e: Pin<&mut Emu>, v: u64);
        fn emu_write_mem(e: Pin<&mut Emu>, offset: usize, data: &[u8]);
        fn emu_read_mem(e: &Emu, offset: usize, len: usize) -> Vec<u8>;

        // IR
        fn ir_lift(code: &[u8], addr: u64) -> UniquePtr<IrLifted>;
    }
}

// ---- High-level safe Rust API ----

/// A decoded x86-64 instruction.
pub struct Instruction {
    inner: cxx::UniquePtr<ffi::Decoded>,
}

impl Instruction {
    /// Instruction length in bytes.
    pub fn length(&self) -> u8 { ffi::decoded_length(&self.inner) }

    /// Mnemonic value (u16).
    pub fn mnemonic_id(&self) -> u16 { ffi::decoded_mnemonic(&self.inner) }

    /// Mnemonic name as string.
    pub fn mnemonic_name(&self) -> String { ffi::mnemonic_name(self.mnemonic_id()) }

    /// Re-encode to bytes.
    pub fn encode(&self) -> Vec<u8> { ffi::encode(&self.inner) }

    /// REX prefix byte.
    pub fn rex(&self) -> u8 { ffi::decoded_rex(&self.inner) }

    /// ModRM byte.
    pub fn modrm(&self) -> u8 { ffi::decoded_modrm(&self.inner) }

    /// SIB byte.
    pub fn sib(&self) -> u8 { ffi::decoded_sib(&self.inner) }

    /// Displacement value.
    pub fn displacement(&self) -> i64 { ffi::decoded_displacement(&self.inner) }

    /// Immediate value.
    pub fn immediate(&self) -> i64 { ffi::decoded_immediate(&self.inner) }

    /// Number of operands.
    pub fn num_operands(&self) -> u8 { ffi::decoded_num_operands(&self.inner) }

    /// VEX-encoded.
    pub fn has_vex(&self) -> bool { ffi::decoded_has_vex(&self.inner) }
    pub fn vex_vvvv(&self) -> u8 { ffi::decoded_vex_vvvv(&self.inner) }
    pub fn vex_l(&self) -> u8 { ffi::decoded_vex_l(&self.inner) }
    pub fn vex_w(&self) -> bool { ffi::decoded_vex_w(&self.inner) }

    /// Get semantics for this instruction.
    pub fn semantics(&self) -> ffi::SemResult { ffi::get_semantics(&self.inner) }
}

impl std::fmt::Debug for Instruction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Instruction({} len={})", self.mnemonic_name(), self.length())
    }
}

/// Decode one instruction from bytes.
pub fn decode(code: &[u8]) -> Option<Instruction> {
    let ptr = ffi::decode(code);
    if ptr.is_null() { None } else { Some(Instruction { inner: ptr }) }
}

/// Disassemble one instruction, returns the text.
pub fn disassemble(code: &[u8], rip: u64) -> Option<String> {
    let s = ffi::disassemble(code, rip);
    if s.is_empty() { None } else { Some(s) }
}

/// Instruction table size.
pub fn table_size() -> usize { ffi::table_size() }

/// Assemble a single instruction from Intel syntax text.
/// Returns encoded bytes, or empty Vec if assembly fails.
pub fn assemble(text: &str) -> Vec<u8> { ffi::assemble(text) }

/// Assemble multiple instructions with label support.
/// Returns encoded bytes, or empty Vec if assembly fails.
pub fn assemble_block(text: &str) -> Vec<u8> { ffi::assemble_block(text) }

/// Check if an instruction can be safely relocated.
pub fn can_relocate(code: &[u8]) -> bool { ffi::can_relocate(code) }

/// Check if an instruction uses RIP-relative addressing.
pub fn is_rip_relative(code: &[u8]) -> bool { ffi::is_rip_relative(code) }

/// Classify control flow of an instruction.
pub fn classify_flow(code: &[u8], addr: u64) -> ffi::FlowResult { ffi::classify_flow(code, addr) }

/// Lift an instruction to IR.
pub fn ir_lift(code: &[u8], addr: u64) -> Option<cxx::UniquePtr<ffi::IrLifted>> {
    let ptr = ffi::ir_lift(code, addr);
    if ptr.is_null() { None } else { Some(ptr) }
}

pub use ffi::{FlowResult, SemResult, IrVarNode, IrOp, IrLifted};

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_decode_nop() {
        let instr = decode(&[0x90]).expect("decode NOP");
        assert_eq!(instr.length(), 1);
    }

    #[test]
    fn test_table_size() {
        assert!(table_size() > 0);
    }

    #[test]
    fn test_roundtrip() {
        let code = [0x48u8, 0x89, 0xc1]; // mov rcx, rax
        let instr = decode(&code).expect("decode");
        assert_eq!(instr.encode(), code);
    }

    #[test]
    fn test_disassemble() {
        let text = disassemble(&[0xc3], 0).expect("disasm");
        assert!(text.contains("ret"));
    }

    #[test]
    fn test_flow() {
        let flow = classify_flow(&[0xc3], 0x1000);
        assert_eq!(flow.length, 1);
    }
}
