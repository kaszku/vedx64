# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Kevin Szkudlapski

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

def test_analysis_mnemonic_queries():
    a = v.analysis
    M = v.Mnemonic
    assert a.is_jcc(M.JE)
    assert a.is_jcc(M.JNE)
    assert not a.is_jcc(M.JMP)
    assert a.is_unconditional_branch(M.JMP)
    assert a.is_call(M.CALL)
    assert a.is_ret(M.RETN)
    assert a.changes_rip(M.HLT)
    assert not a.changes_rip(M.ADD)
    assert a.is_arith(M.ADD)
    assert a.is_logical(M.AND)
    assert a.is_shift(M.SHL)
    # Condition codes
    assert a.jcc_condition(M.JE) == a.CondCode.Z
    assert a.jcc_condition(M.JMP) is None
    assert a.cmov_condition(M.CMOVE) == a.CondCode.Z
    assert a.jcc_for_condition(a.CondCode.Z) == M.JZ
    # EFLAGS
    assert a.sets_eflags(M.ADD) & a.EF_CF
    assert a.sets_eflags(M.ADD) & a.EF_ZF
    assert not (a.sets_eflags(M.INC) & a.EF_CF)  # INC doesn't write CF
    assert a.reads_eflags(M.JE) & a.EF_ZF
    assert a.reads_eflags(M.ADC) & a.EF_CF
    # Sizes
    assert a.canonical_size(M.MOVSB) == 1
    assert a.canonical_size(M.MOVSQ) == 8
    assert a.canonical_size(M.ADD) == 0

def test_analysis_patchers():
    a = v.analysis
    assert a.build_jmp_rel32(0x12345678) == bytes([0xE9, 0x78, 0x56, 0x34, 0x12])
    jcc = a.build_jcc_rel32(int(a.CondCode.Z), -16)
    assert jcc[:2] == bytes([0x0F, 0x84])
    mov = a.build_mov_imm64(1, 0xCAFEBABE_DEADBEEF)  # RCX
    assert mov[:2] == bytes([0x48, 0xB9])
    # Indirect register JMP / CALL
    assert a.build_jmp_reg(0) == bytes([0xFF, 0xE0])  # jmp rax
    assert a.build_jmp_reg(10) == bytes([0x41, 0xFF, 0xE2])  # jmp r10
    assert a.build_call_reg(0) == bytes([0xFF, 0xD0])

def test_analysis_decoded_queries():
    a = v.analysis
    M = v.Mnemonic
    # Count-conditional + interrupt mnemonic queries
    assert a.is_count_conditional_branch(M.JCXZ)
    assert a.is_count_conditional_branch(M.LOOP)
    assert not a.is_count_conditional_branch(M.JE)
    assert a.is_int_or_ud(M.UD2)
    assert a.is_int_or_ud(M.IRET)
    # Indirect branch info
    info = a.indirect_branch_info(b'\xFF\xE0')  # jmp rax
    assert info == {'reg_id': 0, 'is_mem': False}
    assert a.indirect_branch_info(b'\xEB\x05') is None  # rel8 jmp
    # Relative target
    assert a.relative_target(b'\xEB\x05', 0x1000) == 0x1007
    assert a.relative_target(b'\xFF\xE0', 0x1000) is None  # indirect
    # First immediate
    assert a.first_immediate(b'\x48\x83\xEC\x20') == 0x20  # SUB rsp,0x20
    assert a.first_immediate(b'\x90') is None  # nop

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

def test_assemble_ex():
    if not hasattr(v, 'assemble_ex'):
        return
    # Success
    assert v.assemble_ex('nop') == b'\x90'
    # Error with message
    try:
        v.assemble_ex('foobar')
        assert False, 'should have raised'
    except RuntimeError as e:
        assert 'unknown instruction' in str(e)
    try:
        v.assemble_ex('mov rax')
        assert False, 'should have raised'
    except RuntimeError as e:
        assert 'unsupported operand' in str(e)

def test_assemble_block_ex():
    if not hasattr(v, 'assemble_block_ex'):
        return
    code = v.assemble_block_ex('push rbp\nnop\nret')
    assert len(code) >= 3
    try:
        v.assemble_block_ex('push rbp\nfoobar\nret')
        assert False, 'should have raised'
    except RuntimeError as e:
        assert 'line' in str(e)

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
