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

/// Mnemonic-level analysis helpers.
/// Each takes a Mnemonic id (`Instruction::mnemonic_id()`).
pub mod analysis {
    use super::ffi;
    pub fn is_jcc(m: u16) -> bool { ffi::is_jcc(m) }
    pub fn is_cmov(m: u16) -> bool { ffi::is_cmov(m) }
    pub fn is_call(m: u16) -> bool { ffi::is_call(m) }
    pub fn is_ret(m: u16) -> bool { ffi::is_ret(m) }
    pub fn is_unconditional_branch(m: u16) -> bool { ffi::is_unconditional_branch(m) }
    pub fn is_relative_branch(m: u16) -> bool { ffi::is_relative_branch(m) }
    pub fn changes_rip(m: u16) -> bool { ffi::changes_rip(m) }
    pub fn is_arith(m: u16) -> bool { ffi::is_arith(m) }
    pub fn is_logical(m: u16) -> bool { ffi::is_logical(m) }
    pub fn is_shift(m: u16) -> bool { ffi::is_shift(m) }
    /// Returns the 4-bit condition code for a Jcc, or None if not a Jcc.
    pub fn jcc_condition(m: u16) -> Option<u8> {
        let c = ffi::jcc_condition(m);
        if c == 0xFF { None } else { Some(c) }
    }
    /// Same for CMOVcc.
    pub fn cmov_condition(m: u16) -> Option<u8> {
        let c = ffi::cmov_condition(m);
        if c == 0xFF { None } else { Some(c) }
    }
    /// Canonical Jcc mnemonic id for a 4-bit condition code.
    pub fn jcc_for_condition(cc: u8) -> u16 { ffi::jcc_for_condition(cc) }
    /// EFLAGS bitmask (CF=1<<0, PF=1<<1, AF=1<<2, ZF=1<<3, SF=1<<4, OF=1<<5, DF=1<<6, IF=1<<7).
    pub fn sets_eflags(m: u16) -> u8 { ffi::sets_eflags(m) }
    pub fn reads_eflags(m: u16) -> u8 { ffi::reads_eflags(m) }
    /// Implied operand size in bytes (1/2/4/8), or 0 if variable.
    pub fn canonical_size(m: u16) -> u8 { ffi::canonical_size(m) }

    pub fn build_jmp_rel32(disp: i32) -> Vec<u8> { ffi::build_jmp_rel32(disp) }
    pub fn build_jcc_rel32(cc: u8, disp: i32) -> Vec<u8> { ffi::build_jcc_rel32(cc, disp) }
    pub fn build_call_rel32(disp: i32) -> Vec<u8> { ffi::build_call_rel32(disp) }
    pub fn build_mov_imm64(reg_id: u8, imm: u64) -> Vec<u8> { ffi::build_mov_imm64(reg_id, imm) }
}

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
    #[test] fn test_analysis_mnemonic_queries() {
        let ret = decode(&[0xc3]).unwrap().mnemonic_id();
        assert!(analysis::is_ret(ret));
        assert!(!analysis::is_call(ret));
        assert!(analysis::changes_rip(ret));
        let je = decode(&[0x74, 0x00]).unwrap().mnemonic_id();
        assert!(analysis::is_jcc(je));
        assert_eq!(analysis::jcc_condition(je), Some(4)); // CondCode::Z
        assert!(analysis::reads_eflags(je) & (1 << 3) != 0); // ZF
    }
    #[test] fn test_analysis_patchers() {
        let bytes = analysis::build_jmp_rel32(0x12345678);
        assert_eq!(bytes, vec![0xE9, 0x78, 0x56, 0x34, 0x12]);
        let mov = analysis::build_mov_imm64(1 /* RCX */, 0xCAFEBABEu64);
        assert_eq!(mov[0..2], [0x48, 0xB9]);
    }
}
