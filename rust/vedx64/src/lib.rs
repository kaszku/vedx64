// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

//! Rust bindings for the vedx64 x86-64 library.
//!
//! Uses `cxx` for direct C++ interop — no C wrapper layer.

mod bridge;
use bridge::ffi;

/// A decoded x86-64 instruction.
pub struct Instruction { inner: cxx::UniquePtr<ffi::Decoded> }

impl Instruction {
    pub fn length(&self) -> u8 { ffi::decoded_length(&self.inner) }
    pub fn mnemonic_id(&self) -> u16 { ffi::decoded_mnemonic(&self.inner) }
    pub fn mnemonic_name(&self) -> String { ffi::mnemonic_name(self.mnemonic_id()) }
    pub fn encode(&self) -> Vec<u8> { ffi::encode(&self.inner) }
    pub fn rex(&self) -> u8 { ffi::decoded_rex(&self.inner) }
    pub fn modrm(&self) -> u8 { ffi::decoded_modrm(&self.inner) }
    pub fn sib(&self) -> u8 { ffi::decoded_sib(&self.inner) }
    pub fn displacement(&self) -> i64 { ffi::decoded_displacement(&self.inner) }
    pub fn immediate(&self) -> i64 { ffi::decoded_immediate(&self.inner) }
    pub fn num_operands(&self) -> u8 { ffi::decoded_num_operands(&self.inner) }
    pub fn has_vex(&self) -> bool { ffi::decoded_has_vex(&self.inner) }
    pub fn vex_vvvv(&self) -> u8 { ffi::decoded_vex_vvvv(&self.inner) }
    pub fn vex_l(&self) -> u8 { ffi::decoded_vex_l(&self.inner) }
    pub fn vex_w(&self) -> bool { ffi::decoded_vex_w(&self.inner) }
    pub fn semantics(&self) -> ffi::SemResult { ffi::get_semantics(&self.inner) }
}

impl std::fmt::Debug for Instruction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "Instruction({} len={})", self.mnemonic_name(), self.length())
    }
}

pub fn decode(code: &[u8]) -> Option<Instruction> {
    let ptr = ffi::decode(code);
    if ptr.is_null() { None } else { Some(Instruction { inner: ptr }) }
}
pub fn disassemble(code: &[u8], rip: u64) -> Option<String> {
    let s = ffi::disassemble(code, rip);
    if s.is_empty() { None } else { Some(s) }
}
pub fn table_size() -> usize { ffi::table_size() }
pub fn assemble(text: &str) -> Option<Vec<u8>> {
    let v = ffi::assemble(text);
    if v.is_empty() { None } else { Some(v) }
}
pub fn assemble_block(text: &str) -> Option<Vec<u8>> {
    let v = ffi::assemble_block(text);
    if v.is_empty() { None } else { Some(v) }
}
pub fn can_relocate(code: &[u8]) -> bool { ffi::can_relocate(code) }
pub fn is_rip_relative(code: &[u8]) -> bool { ffi::is_rip_relative(code) }
pub fn classify_flow(code: &[u8], addr: u64) -> ffi::FlowResult { ffi::classify_flow(code, addr) }
pub fn ir_lift(code: &[u8], addr: u64) -> Option<cxx::UniquePtr<ffi::IrLifted>> {
    let ptr = ffi::ir_lift(code, addr);
    if ptr.is_null() { None } else { Some(ptr) }
}

pub use ffi::{FlowResult, SemResult, IrLifted};

#[cfg(test)]
mod tests {
    use super::*;
    #[test] fn test_decode_nop() { assert_eq!(decode(&[0x90]).expect("nop").length(), 1); }
    #[test] fn test_table_size() { assert!(table_size() > 0); }
    #[test] fn test_roundtrip() {
        let code = [0x48u8, 0x89, 0xc1];
        assert_eq!(decode(&code).unwrap().encode(), code);
    }
    #[test] fn test_disassemble() { assert!(disassemble(&[0xc3], 0).unwrap().contains("ret")); }
    #[test] fn test_flow() { assert_eq!(classify_flow(&[0xc3], 0x1000).length, 1); }
}
