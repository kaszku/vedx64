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
        include!("vedx64_bridge.hpp");

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

