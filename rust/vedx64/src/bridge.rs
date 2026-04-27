// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#[cxx::bridge(namespace = "vedx64::bridge")]
pub mod ffi {
    #[derive(Debug, Clone)]
    struct FlowResult {
        flow_type: u8,
        address: u64,
        target: u64,
        is_indirect: bool,
        has_fallthrough: bool,
        length: u8,
    }

    #[derive(Debug, Clone, Copy)]
    struct IndirectBranchInfo {
        reg_id: u8,
        is_mem: bool,
        valid: bool,
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

    unsafe extern "C++" {
        include!("vedx64_bridge.hpp");

        type Decoded;
        type Emu;
        type IrLifted;

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

        // Analysis (mnemonic queries take Mnemonic-as-u16; see Instruction::mnemonic_id())
        fn is_jcc(m: u16) -> bool;
        fn is_cmov(m: u16) -> bool;
        fn is_call(m: u16) -> bool;
        fn is_ret(m: u16) -> bool;
        fn is_unconditional_branch(m: u16) -> bool;
        fn is_relative_branch(m: u16) -> bool;
        fn changes_rip(m: u16) -> bool;
        fn is_arith(m: u16) -> bool;
        fn is_logical(m: u16) -> bool;
        fn is_shift(m: u16) -> bool;
        fn jcc_condition(m: u16) -> u8;
        fn cmov_condition(m: u16) -> u8;
        fn jcc_for_condition(cc: u8) -> u16;
        fn sets_eflags(m: u16) -> u8;
        fn reads_eflags(m: u16) -> u8;
        fn canonical_size(m: u16) -> u8;
        fn build_jmp_rel32(disp: i32) -> Vec<u8>;
        fn build_jcc_rel32(cc: u8, disp: i32) -> Vec<u8>;
        fn build_call_rel32(disp: i32) -> Vec<u8>;
        fn build_mov_imm64(reg_id: u8, imm: u64) -> Vec<u8>;
        fn build_jmp_reg(reg_id: u8) -> Vec<u8>;
        fn build_call_reg(reg_id: u8) -> Vec<u8>;
        fn is_count_conditional_branch(m: u16) -> bool;
        fn is_int_or_ud(m: u16) -> bool;
        fn indirect_branch_info(d: &Decoded) -> IndirectBranchInfo;
        fn has_relative_target(d: &Decoded) -> bool;
        fn relative_target(d: &Decoded, insn_va: u64) -> u64;
        fn has_first_immediate(d: &Decoded) -> bool;
        fn first_immediate(d: &Decoded) -> i64;

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
        fn ir_lifted_address(l: &IrLifted) -> u64;
        fn ir_lifted_length(l: &IrLifted) -> u8;
        fn ir_lifted_op_count(l: &IrLifted) -> usize;
        fn ir_lifted_op_opcode(l: &IrLifted, i: usize) -> u8;
        fn ir_lifted_op_num_inputs(l: &IrLifted, i: usize) -> u8;
    }
}

