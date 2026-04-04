// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

//! Rust bindings for the vedx64 x86-64 library.
//!
//! Uses `cxx` for direct C++ interop — no C wrapper layer.

mod bridge;
use bridge::ffi;

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
/// Returns encoded bytes, or None if assembly fails.
pub fn assemble(text: &str) -> Option<Vec<u8>> {
    let v = ffi::assemble(text);
    if v.is_empty() { None } else { Some(v) }
}

/// Assemble multiple instructions with label support.
/// Returns encoded bytes, or None if assembly fails.
pub fn assemble_block(text: &str) -> Option<Vec<u8>> {
    let v = ffi::assemble_block(text);
    if v.is_empty() { None } else { Some(v) }
}

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
