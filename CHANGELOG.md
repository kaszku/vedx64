# Changelog

## Unreleased

### Added
- Text assembler: 585 mnemonics, memory operands, labels, prefixes, segments
- AVX-512 masking support ({{k1}}, {{z}}) for vpxord/vpandd/vpord/vpternlogd
- ZMM (zmm0-zmm31) and MMX (mm0-mm7) register support
- Directives: db, dw, dd, dq, times, align, equ
- Label support with two-pass forward/backward resolution
- Error reporting: `assemble(text, error)` with descriptive messages
- Standalone assembler tool (`vedx64_asm`) with REPL mode
- Python bindings: `assemble()`, `assemble_block()`, `assemble_ex()`, `assemble_block_ex()`
- Rust FFI: `assemble()`, `assemble_block()` returning `Option<Vec<u8>>`
- Shift/rotate by CL register
- `movsxd` and `movsx rax, ecx` alias
- `xchg eax, reg` short form encoding (90+rd)
- `leave`, string instruction aliases (movsb/w/q, etc.)

### Fixed
- Memory operand encoding: set Mem.has_base/has_index flags
- Byte/dword opcode selection for dec/inc/neg/not (FE vs FF)
- IMUL 2-operand: use 0F AF instead of 69/F7
- RR pattern operand order: detect G,E vs E,G for correct reg/rm mapping
- String literal case preservation in `db` directive
- Operand size_bits: uint8_t -> uint16_t (supports 256/512)
- All compiler warnings eliminated (was 26)

### Changed
- CMake options default to OFF (was inconsistent)
- README updated with full assembler documentation
- Test count: 4,500+ assertions across 11 suites
