# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Kevin Szkudlapski
# Auto-generated — do not edit

"""Tests for vedx64 Python bindings."""
import vedx64_py as v

def test_table_size():
    assert v.table_size() > 0

def test_mnemonic_name():
    assert v.mnemonic_name(v.Mnemonic.INVALID) == "???"
    assert v.mnemonic_name(v.Mnemonic.ADD) == "add"
    assert v.mnemonic_name(v.Mnemonic.MOV) == "mov"
    assert v.mnemonic_name(v.Mnemonic.NOP) == "nop"

def test_decode_nop():
    di = v.decode(b'\x90')
    assert di is not None
    assert di.length == 1

def test_decode_ret():
    di = v.decode(b'\xc3')
    assert di is not None
    assert di.length == 1
    assert di.mnemonic == v.Mnemonic.RETN
    assert di.mnemonic_name == "retn"  # table mnemonic

def test_decode_add():
    di = v.decode(b'\x01\xc8')  # add eax, ecx
    assert di is not None
    assert di.length == 2
    assert di.mnemonic == v.Mnemonic.ADD
    assert di.has_modrm
    assert di.modrm == 0xc8

def test_decode_rex():
    di = v.decode(b'\x48\x89\xe5')  # mov rbp, rsp
    assert di is not None
    assert di.length == 3
    assert di.rex == 0x48
    assert di.mnemonic == v.Mnemonic.MOV

def test_decode_mov_imm():
    di = v.decode(b'\xb8\x78\x56\x34\x12')  # mov eax, 0x12345678
    assert di is not None
    assert di.length == 5
    assert di.immediate == 0x12345678

def test_decode_invalid():
    result = v.decode(b'')
    assert result is None

def test_encode_roundtrip():
    cases = [
        b'\x90',              # nop
        b'\xc3',              # ret
        b'\x55',              # push rbp
        b'\x01\xc8',          # add eax, ecx
        b'\x48\x89\xe5',      # mov rbp, rsp
        b'\x0f\x31',          # rdtsc
    ]
    for code in cases:
        di = v.decode(code)
        assert di is not None, f"decode failed for {code.hex()}"
        encoded = v.encode(di)
        assert encoded == code, f"roundtrip failed: {code.hex()} -> {encoded.hex()}"

def test_disassemble():
    result = v.disassemble(b'\xc3', 0x1000)
    assert result is not None
    text, length = result
    assert length == 1
    assert "ret" in text

def test_disassemble_all():
    code = b'\x55\x48\x89\xe5\xc3'  # push rbp; mov rbp,rsp; ret
    results = v.disassemble_all(code, 0x401000)
    assert len(results) == 3
    assert results[0][0] == 0x401000  # address
    assert results[1][0] == 0x401001  # address
    assert results[2][0] == 0x401004  # address

def test_enum_values():
    assert v.OpSize.Byte != v.OpSize.Word
    assert v.AddrMode.ModRM_RM != v.AddrMode.Immediate

def test_vex_fields():
    di = v.decode(b'\x90')  # nop - no VEX
    assert di is not None
    assert di.has_vex == False

def test_encoding_id_enum():
    # Just verify the type exists
    assert hasattr(v, 'EncodingId')

def test_operand_types():
    assert v.OperandType.Register != v.OperandType.Memory
    assert v.RegisterClass.GPR64 != v.RegisterClass.XMM

def test_instruction_class():
    instr = v.Instruction.decode_instruction(b'\x48\x89\xe5', 0x1000)
    assert instr is not None
    assert instr.length == 3
    assert instr.address == 0x1000

def test_relocation():
    # nop is relocatable
    assert v.can_relocate(b'\x90') == True
    # nop is not RIP-relative
    assert v.is_rip_relative(b'\x90') == False

def test_branch_flow():
    # ret is a Return flow
    flow = v.classify_flow(b'\xc3', 0x1000)
    assert flow is not None
    assert flow.type == v.FlowType.Return  # BranchFlowType in C++, exposed as FlowType in Python

def test_assemble():
    if not hasattr(v, 'assemble'):
        return  # VEDX64_ASSEMBLER not enabled
    # Single instruction
    nop = v.assemble('nop')
    assert nop == b'\x90', f'nop: {nop}'
    ret = v.assemble('ret')
    assert ret == b'\xc3', f'ret: {ret}'
    # Memory operand
    mov_rm = v.assemble('mov rax, [rbx]')
    assert mov_rm is not None and len(mov_rm) > 0
    # Invalid
    assert v.assemble('foobar') is None

def test_assemble_block():
    if not hasattr(v, 'assemble_block'):
        return
    code = v.assemble_block('push rbp\nmov rbp, rsp\nnop\nret')
    assert code is not None and len(code) >= 4
    # Labels
    labeled = v.assemble_block('jmp end\nnop\nend:\nret')
    assert labeled is not None
    # Roundtrip: assemble then decode
    mov = v.assemble('mov rax, rcx')
    if mov:
        di = v.decode(mov)
        assert di is not None and di.length > 0

if __name__ == "__main__":
    tests = [v for k, v in globals().items() if k.startswith("test_")]
    passed = 0
    for t in tests:
        try:
            t()
            passed += 1
            print(f"  PASS: {t.__name__}")
        except Exception as e:
            print(f"  FAIL: {t.__name__}: {e}")
    print(f"\n{passed}/{len(tests)} tests passed")
