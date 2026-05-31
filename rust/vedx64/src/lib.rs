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
    pub fn has_evex(&self) -> bool { ffi::decoded_has_evex(&self.inner) }
    /// EVEX opmask register (k0-k7) selected by the aaa field.
    pub fn evex_aaa(&self) -> u8 { ffi::decoded_evex_aaa(&self.inner) }
    pub fn evex_z(&self) -> bool { ffi::decoded_evex_z(&self.inner) }
    pub fn evex_b(&self) -> bool { ffi::decoded_evex_b(&self.inner) }
    pub fn evex_rc(&self) -> u8 { ffi::decoded_evex_rc(&self.inner) }
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

/// True if the IR sequence contains no `Opcode::UNDEF` op.
/// Symbolic-execution callers should refuse to step past `!ir_is_fully_lifted`,
/// since the lifter doesn't model the underlying instruction.
pub fn ir_is_fully_lifted(l: &ffi::IrLifted) -> bool { ffi::ir_is_fully_lifted(l) }

pub use ffi::{FlowResult, SemResult, IrLifted, IndirectBranchInfo};

/// Default action taken on every emulator memory fault.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FaultAction { Abort = 0, Skip = 1, Retry = 2 }

/// An emulator instance that owns its memory image (mirrors the C++ Emu).
pub struct Emulator { inner: cxx::UniquePtr<ffi::Emu> }

impl Emulator {
    pub fn new(mem_size: usize) -> Self { Emulator { inner: ffi::emu_new(mem_size) } }
    pub fn rip(&self) -> u64 { ffi::emu_rip(&self.inner) }
    pub fn set_rip(&mut self, v: u64) { ffi::emu_set_rip(self.inner.pin_mut(), v); }
    pub fn gpr(&self, i: usize) -> u64 { ffi::emu_gpr(&self.inner, i) }
    pub fn set_gpr(&mut self, i: usize, v: u64) { ffi::emu_set_gpr(self.inner.pin_mut(), i, v); }
    pub fn rflags(&self) -> u64 { ffi::emu_rflags(&self.inner) }
    pub fn set_rflags(&mut self, v: u64) { ffi::emu_set_rflags(self.inner.pin_mut(), v); }
    pub fn write_mem(&mut self, offset: usize, data: &[u8]) { ffi::emu_write_mem(self.inner.pin_mut(), offset, data); }
    pub fn read_mem(&self, offset: usize, len: usize) -> Vec<u8> { ffi::emu_read_mem(&self.inner, offset, len) }
    pub fn step(&mut self) -> i32 { ffi::emu_step(self.inner.pin_mut()) }
    pub fn run(&mut self, max_steps: usize) -> usize { ffi::emu_run(self.inner.pin_mut(), max_steps) }
    /// Set the action every subsequent memory fault takes (a per-fault
    /// std::function callback can't cross the cxx boundary, so this is a
    /// fixed default action instead).
    pub fn set_default_fault_action(&mut self, action: FaultAction) {
        ffi::emu_set_default_fault_action(self.inner.pin_mut(), action as u8);
    }
}

/// A symbolic-execution session over a flat code image.
pub struct SymxSession { inner: cxx::UniquePtr<ffi::SymxSession> }

impl SymxSession {
    /// Build a session: `code` is the instruction image, `base` its VA,
    /// and `entry` the RIP to start the seed state at.
    pub fn new(code: &[u8], base: u64, entry: u64) -> Self {
        SymxSession { inner: ffi::symx_new(code, base, entry) }
    }
    /// Fix a concrete value for GPR `reg` (0..15) in the seed state.
    pub fn set_gpr_concrete(&mut self, reg: u8, value: u64) {
        ffi::symx_set_gpr_concrete(self.inner.pin_mut(), reg, value);
    }
    /// Straight-line run of up to `max_instructions`; returns count executed.
    pub fn run_block(&mut self, max_instructions: usize) -> usize {
        ffi::symx_run_block(self.inner.pin_mut(), max_instructions)
    }
    pub fn rip(&self) -> u64 { ffi::symx_rip(&self.inner) }
    /// Concrete value of GPR `reg` if it resolved to a constant, else None.
    pub fn gpr_const(&self, reg: u8) -> Option<u64> {
        if ffi::symx_gpr_is_const(&self.inner, reg) {
            Some(ffi::symx_gpr_const_value(&self.inner, reg))
        } else { None }
    }
    /// Human-readable rendering of GPR `reg`'s symbolic expression.
    pub fn gpr_str(&self, reg: u8) -> String { ffi::symx_gpr_str(&self.inner, reg) }
    /// True when the Solver is SMT-backed (built with VEDX64_Z3).
    pub fn solver_is_smt_backed() -> bool { ffi::symx_solver_is_smt_backed() }
}

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
    pub fn build_jmp_reg(reg_id: u8) -> Vec<u8> { ffi::build_jmp_reg(reg_id) }
    pub fn build_call_reg(reg_id: u8) -> Vec<u8> { ffi::build_call_reg(reg_id) }
    pub fn is_count_conditional_branch(m: u16) -> bool { ffi::is_count_conditional_branch(m) }
    pub fn is_int_or_ud(m: u16) -> bool { ffi::is_int_or_ud(m) }

    /// Indirect-branch info; None if `i` is not an indirect JMP/CALL.
    pub fn indirect_branch_info(i: &super::Instruction) -> Option<ffi::IndirectBranchInfo> {
        let info = ffi::indirect_branch_info(&i.inner);
        if info.valid { Some(info) } else { None }
    }
    /// Absolute target VA for a relative branch / call. None for indirect / non-branch.
    pub fn relative_target(i: &super::Instruction, insn_va: u64) -> Option<u64> {
        if ffi::has_relative_target(&i.inner) {
            Some(ffi::relative_target(&i.inner, insn_va))
        } else { None }
    }
    /// First immediate operand value, if any.
    pub fn first_immediate(i: &super::Instruction) -> Option<i64> {
        if ffi::has_first_immediate(&i.inner) {
            Some(ffi::first_immediate(&i.inner))
        } else { None }
    }
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
        // jmp rax → FF E0 (2 bytes); jmp r10 → 41 FF E2 (3 bytes).
        assert_eq!(analysis::build_jmp_reg(0), vec![0xFF, 0xE0]);
        assert_eq!(analysis::build_jmp_reg(10), vec![0x41, 0xFF, 0xE2]);
    }
    #[test] fn test_analysis_indirect_branch_info() {
        // JMP rax → FF E0
        let i = decode(&[0xFF, 0xE0]).unwrap();
        let info = analysis::indirect_branch_info(&i).unwrap();
        assert_eq!(info.reg_id, 0);
        assert!(!info.is_mem);
        // JMP rel8 → not indirect
        let i = decode(&[0xEB, 0x05]).unwrap();
        assert!(analysis::indirect_branch_info(&i).is_none());
        assert_eq!(analysis::relative_target(&i, 0x1000), Some(0x1007));
    }
    #[test] fn test_ir_is_fully_lifted() {
        // Long NOP — modeled.
        let l = ir_lift(&[0x0F, 0x1F, 0x00], 0).expect("lift long-nop");
        assert!(ir_is_fully_lifted(&l));
        // AAA (0x37) — has flag side effects but the lifter doesn't model
        // them. After the SymExec-fix it emits UNDEF, not silent NOP.
        let u = ir_lift(&[0x37], 0).expect("lift AAA");
        assert!(!ir_is_fully_lifted(&u));
    }
    #[test] fn test_analysis_first_immediate() {
        // SUB rsp, 0x20 → 48 83 EC 20
        let i = decode(&[0x48, 0x83, 0xEC, 0x20]).unwrap();
        assert_eq!(analysis::first_immediate(&i), Some(0x20));
        // NOP → no immediate
        let i = decode(&[0x90]).unwrap();
        assert_eq!(analysis::first_immediate(&i), None);
    }
    #[test] fn test_emu_memory_roundtrip() {
        // mov eax, 0x12345678 (B8 78 56 34 12) at offset 0, then run 1 step.
        let mut e = Emulator::new(0x1000);
        let code = [0xB8u8, 0x78, 0x56, 0x34, 0x12];
        e.write_mem(0, &code);
        assert_eq!(e.read_mem(0, 5), code.to_vec());
        e.set_rip(0);
        let _ = e.step();
        assert_eq!(e.gpr(0) & 0xFFFFFFFF, 0x12345678);
    }
    #[test] fn test_emu_fault_action_skip() {
        // Skip lets a faulting access be abandoned instead of aborting.
        let mut e = Emulator::new(0x100);
        e.set_default_fault_action(FaultAction::Skip);
        // mov rax, [rbx] with rbx pointing way out of bounds.
        let code = [0x48u8, 0x8B, 0x03];
        e.write_mem(0, &code);
        e.set_gpr(3 /* rbx */, 0xDEAD_0000_0000);
        e.set_rip(0);
        // With Skip the step does not hard-fault (result != MemFault==4).
        let r = e.step();
        assert_ne!(r, 4);
    }
    #[test] fn test_symx_run_block_concrete() {
        // mov eax, 5 ; add eax, 3  →  eax should resolve to 8.
        let code = [0xB8u8, 0x05, 0x00, 0x00, 0x00, 0x83, 0xC0, 0x03];
        let mut s = SymxSession::new(&code, 0x1000, 0x1000);
        let n = s.run_block(8);
        assert!(n >= 2, "expected >=2 instructions, ran {}", n);
        assert_eq!(s.gpr_const(0).map(|v| v & 0xFFFFFFFF), Some(8));
    }
    #[test] fn test_decoded_evex_fields() {
        // Plain NOP is not EVEX-encoded; the new accessors read defaults.
        let i = decode(&[0x90]).unwrap();
        assert!(!i.has_evex());
        assert_eq!(i.evex_aaa(), 0);
        assert!(!i.evex_z());
    }
    #[test] fn test_symx_symbolic_input() {
        // add eax, 3 with a non-fixed RAX stays symbolic (not a constant).
        let code = [0x83u8, 0xC0, 0x03];
        let mut s = SymxSession::new(&code, 0x2000, 0x2000);
        let _ = s.run_block(4);
        assert!(s.gpr_const(0).is_none(), "symbolic RAX should not be constant");
        // The solver is the constant-folding stub unless built with Z3.
        let _ = SymxSession::solver_is_smt_backed();
    }
}
