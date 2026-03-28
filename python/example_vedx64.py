# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Kevin Szkudlapski

#!/usr/bin/env python3
"""vedx64 Python bindings example.

Usage:
    # Set PYTHONPATH to the directory containing vedx64_py.pyd/.so
    PYTHONPATH=build/Release python python/example_vedx64.py
"""
import sys

try:
    import vedx64_py as v
except ImportError:
    print("Error: vedx64_py module not found.")
    print("Build with -DVEDX64_PYTHON=ON and set PYTHONPATH to the build directory.")
    sys.exit(1)


def example_decode():
    """Decode individual instructions."""
    print("=== Decode ===\n")

    cases = [
        (b"\x90",                       "nop"),
        (b"\xc3",                       "ret"),
        (b"\x55",                       "push rbp"),
        (b"\x48\x89\xe5",              "mov rbp, rsp"),
        (b"\x48\x83\xec\x20",          "sub rsp, 0x20"),
        (b"\x48\x01\xc8",              "add rax, rcx"),
        (b"\x0f\x05",                   "syscall"),
        (b"\xb8\x78\x56\x34\x12",      "mov eax, 0x12345678"),
        (b"\x0f\xaf\xc1",              "imul eax, ecx"),
        (b"\xf0\x0f\xc1\x08",          "lock xadd [rax], ecx"),
    ]

    for code, expected in cases:
        di = v.decode(code)
        if di is None:
            print(f"  {code.hex():20s}  DECODE FAILED  (expected: {expected})")
            continue
        name = v.mnemonic_name(di.mnemonic)
        hex_str = code.hex()
        print(f"  {hex_str:20s}  len={di.length}  mnem={name:12s}  "
              f"rex=0x{di.rex:02x}  modrm=0x{di.modrm:02x}  imm={di.immediate}")


def example_encode_roundtrip():
    """Encode/decode roundtrip verification."""
    print("\n=== Encode/Decode Roundtrip ===\n")

    instructions = [
        b"\x90",                       # nop
        b"\xc3",                       # ret
        b"\x55",                       # push rbp
        b"\x5d",                       # pop rbp
        b"\x01\xc8",                   # add eax, ecx
        b"\x48\x89\xe5",              # mov rbp, rsp
        b"\x48\x83\xec\x20",          # sub rsp, 0x20
        b"\xb8\x78\x56\x34\x12",      # mov eax, 0x12345678
        b"\x0f\x31",                   # rdtsc
        b"\xcc",                       # int3
    ]

    passed = 0
    for code in instructions:
        di = v.decode(code)
        if di is None:
            print(f"  {code.hex()}  DECODE FAILED")
            continue
        encoded = v.encode(di)
        ok = encoded == code
        status = "OK" if ok else "MISMATCH"
        print(f"  {code.hex():20s} -> {encoded.hex():20s}  {status}")
        if ok:
            passed += 1

    print(f"\n  {passed}/{len(instructions)} roundtrips passed")


def example_disassemble():
    """Disassemble a function."""
    print("\n=== Disassemble Function ===\n")

    # A small function: push rbp; mov rbp,rsp; sub rsp,0x20;
    #   mov [rbp-4],edi; mov eax,[rbp-4]; add eax,1; imul eax,eax;
    #   add rsp,0x20; pop rbp; ret
    code = bytes([
        0x55,
        0x48, 0x89, 0xE5,
        0x48, 0x83, 0xEC, 0x20,
        0x89, 0x7D, 0xFC,
        0x8B, 0x45, 0xFC,
        0x83, 0xC0, 0x01,
        0x0F, 0xAF, 0xC0,
        0x48, 0x83, 0xC4, 0x20,
        0x5D,
        0xC3,
    ])

    base = 0x401000
    for addr, text, length in v.disassemble_all(code, base):
        # Find the instruction bytes for this entry
        offset = addr - base
        insn_bytes = code[offset:offset + length]
        hex_str = " ".join(f"{b:02x}" for b in insn_bytes)
        print(f"  {addr:016x}: {hex_str:24s} {text}")


def example_mnemonic_enum():
    """Inspect mnemonic enum values."""
    print("\n=== Mnemonic Enum ===\n")

    mnemonics = [
        v.Mnemonic.ADD, v.Mnemonic.SUB, v.Mnemonic.MOV,
        v.Mnemonic.PUSH, v.Mnemonic.POP, v.Mnemonic.CALL,
        v.Mnemonic.RETN, v.Mnemonic.NOP, v.Mnemonic.SYSCALL,
        v.Mnemonic.IMUL, v.Mnemonic.XOR, v.Mnemonic.LEA,
    ]
    for m in mnemonics:
        print(f"  Mnemonic.{str(m):30s} = {int(m):4d}  name={v.mnemonic_name(m)}")


def example_instruction_table():
    """Query the instruction table."""
    print("\n=== Instruction Table ===\n")
    print(f"  Total instruction descriptors: {v.table_size()}")


def example_batch_decode():
    """Decode and analyze a batch of instructions."""
    print("\n=== Batch Decode & Analysis ===\n")

    # Various instruction categories
    categories = {
        "Arithmetic":   [b"\x01\xc8", b"\x29\xc8", b"\x0f\xaf\xc1"],
        "Data move":    [b"\x48\x89\xe5", b"\x55", b"\x5d"],
        "Control flow": [b"\xc3", b"\xe8\x00\x01\x00\x00", b"\x0f\x05"],
        "SSE":          [b"\x0f\x58\xc8", b"\x66\x0f\x58\xc8"],
        "System":       [b"\x0f\xa2", b"\x0f\x31"],
    }

    for category, insns in categories.items():
        print(f"  {category}:")
        for code in insns:
            di = v.decode(code)
            if di:
                result = v.disassemble(code)
                text = result[0] if result else "???"
                print(f"    {code.hex():16s} {text}")
        print()


def example_instruction_api():
    """High-level Instruction API."""
    print("\n=== Instruction API ===\n")

    code = b"\x48\x89\xe5"  # mov rbp, rsp
    instr = v.Instruction.decode_instruction(code, 0x401000)
    if instr:
        print(f"  address=0x{instr.address:x}  length={instr.length}  operands={len(instr.operands)}")
        for i, op in enumerate(instr.operands):
            print(f"    operand[{i}]: type={op.type}  reg={op.reg}")


def example_relocation():
    """Relocation analysis."""
    print("\n=== Relocation ===\n")

    cases = [
        (b"\x90",                       "nop"),
        (b"\xc3",                       "ret"),
        (b"\x48\x8b\x05\x00\x10\x00\x00", "mov rax, [rip+0x1000]"),
        (b"\xe9\xfb\xff\xff\xff",       "jmp rel32"),
    ]

    for code, desc in cases:
        can = v.can_relocate(code)
        rip = v.is_rip_relative(code)
        print(f"  {desc:35s}  relocatable={can}  rip_relative={rip}")


def example_branch_analysis():
    """Branch/flow classification."""
    print("\n=== Branch Analysis ===\n")

    cases = [
        (b"\x90",                       0x1000, "nop"),
        (b"\xc3",                       0x1000, "ret"),
        (b"\xe8\x00\x01\x00\x00",       0x1000, "call rel32"),
        (b"\x0f\x84\x00\x01\x00\x00",   0x1000, "jz rel32"),
        (b"\xeb\x10",                   0x1000, "jmp rel8"),
    ]

    for code, rip, desc in cases:
        flow = v.classify_flow(code, rip)
        if flow:
            print(f"  {desc:20s}  type={flow.type}  target=0x{flow.target:x}  fallthrough=0x{flow.fallthrough:x}")
        else:
            print(f"  {desc:20s}  (decode failed)")


def example_assemble():
    """Assemble instructions from text."""
    if not hasattr(v, 'assemble'):
        print("\n=== Text Assembler (not available) ===")
        return
    print("\n=== Text Assembler ===\n")
    tests = [
        "nop", "ret", "push rbp", "mov rbp, rsp",
        "mov rax, [rbx+rcx*4+8]", "add dword [rsp+8], 1",
        "lock add [rax], rcx", "rep movsb",
    ]
    for text in tests:
        result = v.assemble(text)
        if result:
            hexs = ' '.join(f'{b:02X}' for b in result)
            print(f"  {text:35s} -> {hexs}")
        else:
            print(f"  {text:35s} -> FAILED")

    # Block with labels
    print("\n  Block with labels:")
    code = v.assemble_block("""
        xor eax, eax
        cmp ecx, 0
        je done
    loop:
        add eax, 1
        dec ecx
        jnz loop
    done:
        ret
    """)
    if code:
        print(f"  {len(code)} bytes: {' '.join(f'{b:02X}' for b in code)}")


if __name__ == "__main__":
    example_decode()
    example_encode_roundtrip()
    example_disassemble()
    example_mnemonic_enum()
    example_instruction_table()
    example_batch_decode()
    example_instruction_api()
    example_relocation()
    example_branch_analysis()
    example_assemble()

