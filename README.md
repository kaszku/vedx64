# vedx64

A comprehensive x86-64 instruction library for C++17.

vedx64 provides a complete toolkit for working with x86-64 machine code: decoding,
encoding, disassembly, emulation, semantic analysis, assembler DSL, intermediate
representation lifting, inline hooking, instruction relocation, control flow analysis,
and language bindings for Python and Rust.

> **Note:** The source code in this repository was generated with the assistance of AI
> (Claude, Anthropic). The instruction tables are derived from the x86 reference XML
> (`coder64.xml`).

## Features

| Module | Description |
|--------|-------------|
| **Decoder** | Decode raw bytes into structured `DecodedInstr` with full prefix, REX, VEX/EVEX, ModRM, SIB, displacement and immediate support |
| **Encoder** | Re-encode a `DecodedInstr` back to machine code bytes |
| **Disassembler** | Format instructions as Intel-syntax text with EVEX masking ({k1}{z}) and rounding ({rn-sae}) decorations |
| **Emulator** | Execute instructions on a virtual `CpuState` with GPRs, RFLAGS, XMM/YMM/ZMM, x87 FPU, and byte-addressable memory. 638 mnemonic handlers |
| **IR Lifter** | Lift instructions to a flat SSA-style intermediate representation with 47 IR opcodes. 831 mnemonic handlers covering ALU, SSE/AVX, BMI, AES-NI, SHA, x87, and system instructions |
| **Assembler DSL** | Chainable `CodeGen` class: `e.mov(rax, 42).add(rax, rcx).ret();` with label support |
| **Text Assembler** | Assemble Intel-syntax text to machine code: 585 mnemonics, memory/SIB, labels, prefixes, segments, XMM/ZMM/MMX, AVX-512 masking, directives (db/dw/dd/dq/times/align/equ), error reporting |
| **Inline Hooking** | Handle-based API to install, enable, disable, and remove function hooks with trampoline generation (Windows + Linux) |
| **Relocation** | Relocate RIP-relative, branch, and call instructions to new addresses. Stolen-byte calculation for hook trampolines |
| **Branch Following** | Classify control flow, build basic blocks, walk CFGs, and resolve forwarding stubs |
| **Operand API** | High-level typed operand representation (register, memory, immediate, relative offset) with `Instruction` wrapper |
| **Encoding ID** | Unique typed enum for each instruction encoding, enabling O(1) table lookups |
| **Python Bindings** | nanobind module exposing decode/encode/disassemble, assemble, emulator, IR lifter, relocation, and branch analysis |
| **Rust Bindings** | `cxx`-bridge crate for direct C++ interop with safe Rust wrappers |
| **Syscall Module** | NT API wrappers for executable memory allocation, thread suspension, and protection changes (Windows) |

## Statistics

| Metric | Value |
|--------|-------|
| Instruction encodings | 1,396 |
| Unique mnemonics | 1,091 |
| Test corpus variants | 2,244 (100% roundtrip) |
| Decode coverage | 100% |
| Assembler DSL methods | 1,580 |
| Text assembler mnemonics | 585 |
| IR lifter handlers | 831 mnemonics |
| Emulator handlers | 638 mnemonics |
| Test suites | 12 (4,500+ assertions) |

## Quick Start

```cpp
#include <vedx64.hpp>
#include <cstdio>

int main() {
    using namespace vedx64;

    // Decode an instruction
    uint8_t code[] = { 0x48, 0x89, 0xC1 }; // mov rcx, rax
    DecodedInstr di;
    size_t len = decode(code, sizeof(code), di);
    printf("length: %zu, mnemonic: %s\n", len, mnemonic_name(di.desc->mnemonic));

    // Disassemble
    char buf[128];
    disassemble(code, sizeof(code), buf, sizeof(buf), 0x401000);
    printf("%s\n", buf); // mov rcx, rax

    // Encode back to bytes
    uint8_t out[15];
    size_t n = encode(di, out, sizeof(out));

    // Assembler DSL
    CodeGen e;
    e.mov(rax, 42).add(rax, rcx).xor_(rdx, rdx).ret();
    // e.code() contains the assembled bytes
}
```

## Emulator Example

```cpp
#include <vedx64.hpp>

int main() {
    using namespace vedx64;

    uint8_t program[] = {
        0x48, 0xC7, 0xC0, 0x0A, 0x00, 0x00, 0x00, // mov rax, 10
        0x48, 0x83, 0xC0, 0x05,                   // add rax, 5
        0xF4,                                       // hlt
    };

    CpuState cpu;
    emu_init(cpu, program, sizeof(program));
    emu_run(cpu);
    // cpu.rax() == 15
}
```

## IR Lifter Example

```cpp
#include <vedx64/ir.hpp>

auto lifted = vedx64::ir::lift(code, len, 0x401000);
if (lifted) {
    for (auto& op : lifted->ops) {
        // op.opcode, op.output, op.inputs[0..2]
    }
}
```

## AVX-512 / EVEX Example

```cpp
#include <vedx64/core.hpp>

// VPTERNLOGD XMM0{k1}{z}, XMM1, XMM2, 0xFF
uint8_t code[] = { 0x62, 0xF3, 0x75, 0x89, 0x25, 0xC2, 0xFF };
vedx64::DecodedInstr di;
vedx64::decode(code, sizeof(code), di);

// EVEX fields
di.has_evex;    // true
di.evex_aaa;    // 1 (mask register k1)
di.evex_z;      // true (zeroing-masking)
di.vex_vvvv;    // 1 (XMM1)
di.vex_L;       // 0 (128-bit), 1 (256-bit), 2 (512-bit)

// Disassembles as: vpternlogd xmm0, xmm1, xmm2, 0xff {k1} {z}
```

## Hooking Example

```cpp
#include <vedx64/hook.hpp>

vedx64::hook::Config cfg;
vedx64::hook::initialize(cfg);

vedx64::hook::HookHandle h;
using Fn = int(int);
Fn* original = nullptr;
vedx64::hook::install<Fn>(target_fn, my_detour, &original, &h);
vedx64::hook::enable(h);
// target_fn now calls my_detour; original points to the trampoline
```

## Text Assembler

Assemble x86-64 from Intel-syntax text. 585 mnemonics with full addressing
modes, XMM/ZMM/MMX registers, AVX-512 masking, labels, prefixes, and directives.

```cpp
#include <vedx64/assembler.hpp>

// Single instruction
auto bytes = vedx64::assemble("mov rax, [rbx+rcx*4+8]");

// Block with labels
auto code = vedx64::assemble_block(R"(
    xor eax, eax
    cmp ecx, 0
    je done
loop:
    add eax, 1
    dec ecx
    jnz loop
done:
    ret
)");

// Memory, segments, prefixes
assemble("mov rax, fs:[rbx+8]");       // segment override
assemble("lock add [rax], rcx");        // lock prefix
assemble("rep movsb");                   // rep prefix
assemble("lea rax, [rip+0x1000]");       // RIP-relative

// SIMD registers
assemble("movaps xmm0, [rax]");         // XMM + memory
assemble("movq mm0, mm1");              // MMX
assemble("vpxord zmm0 {k1} {z}, zmm1, zmm2"); // AVX-512 masked

// Directives
assemble("db 0x90, 0x90");              // raw bytes
assemble("dd 0xDEADBEEF");              // 32-bit data
assemble("times 16 nop");               // repeat

// Error reporting
std::string err;
assemble("mov rax", err);  // err = "unsupported operand combination for: mov"
```

Block assembly supports labels, `equ` constants, `align`, and comments:

```cpp
auto code = vedx64::assemble_block(R"(
    size equ 0x20
    push rbp
    mov rbp, rsp
    sub rsp, size        ; use equ constant
    align 16
loop:
    nop
    jmp loop             ; backward label
)");
```

## API Reference

| Header | Description |
|--------|-------------|
| `vedx64.hpp` | Master include (pulls in all enabled modules) |
| `vedx64/core.hpp` | `DecodedInstr`, `InstrDesc`, `decode()`, `encode()`, `disassemble()`, `mnemonic_name()` |
| `vedx64/mnemonic.hpp` | `Mnemonic` enum with 1091 values |
| `vedx64/operand.hpp` | `Operand` tagged union (Register, Immediate, Memory, RelativeOffset), `OperandType`, `RegisterClass` |
| `vedx64/instruction.hpp` | `Instruction` high-level wrapper with `from_decoded()`, `decode_instruction()`, typed operand vector |
| `vedx64/encoding_id.hpp` | `EncodingId` typed enum (1396 unique encodings) |
| `vedx64/codegen.hpp` | `CodeGen` assembler DSL with `Reg`, `Xmm`, `Mem` operand types and label support |
| `vedx64/assembler.hpp` | `assemble()`, `assemble_block()` — 585 mnemonics, memory/SIB, labels, XMM/ZMM/MMX, AVX-512 `{k}{z}`, directives, error reporting |
| `vedx64/ir.hpp` | `ir::Opcode` (47 opcodes), `VarNode` (GPR/XMM/Temp/RAM/Const), `Op`, `Lifted`, `lift()`, `execute()` |
| `vedx64/emu.hpp` | `CpuState` (GPR, RFLAGS, XMM/YMM, x87, memory), `emu_init()`, `emu_step()`, `emu_run()`, `StepResult` |
| `vedx64/relocation.hpp` | `can_relocate()`, `is_rip_relative()`, `relocate_instruction()`, `relocate_block()`, `calc_stolen_bytes()` |
| `vedx64/branch_follow.hpp` | `FlowInfo`, `BasicBlock`, `classify_flow()`, `walk_basic_block()`, `walk_cfg()` |
| `vedx64/hook.hpp` | `initialize()`, `install()`, `enable()`, `disable()`, `remove()`, `dump_hook()`, `Config`, `HookHandle` |
| `vedx64/syscall.hpp` | NT API wrappers: `alloc_executable()`, `suspend_threads()`, `resume_threads()` (Windows) |

## Building

Requires C++17 and CMake 3.15+.

```bash
# Configure with all features
cmake -B build -DCMAKE_BUILD_TYPE=Release \
    -DVEDX64_STRINGS=ON \
    -DVEDX64_TESTS=ON \
    -DVEDX64_EXAMPLES=ON

# Build
cmake --build build --config Release

# Test
ctest --test-dir build -C Release --output-on-failure
```

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `VEDX64_STRINGS` | `OFF` | Enable string functions (`disassemble`, `mnemonic_name`). ~31% smaller binary when off |
| `VEDX64_EMU` | `OFF` | CPU emulator |
| `VEDX64_IR` | `OFF` | IR lifter |
| `VEDX64_HOOK` | `OFF` | Inline hooking |
| `VEDX64_ASSEMBLER` | `OFF` | Text assembler |
| `VEDX64_CODEGEN` | `OFF` | CodeGen assembler DSL |
| `VEDX64_TESTS` | `OFF` | Build test executables |
| `VEDX64_EXAMPLES` | `OFF` | Build example executables |
| `VEDX64_PYTHON` | `OFF` | Build Python nanobind module |

## Python Bindings

Build with `-DVEDX64_PYTHON=ON` (requires [nanobind](https://github.com/wjakob/nanobind)).

```python
import vedx64_py as v

# Decode and disassemble
di = v.decode(b'\x48\x89\xc1')
text, length = v.disassemble(b'\x48\x89\xc1', rip=0x401000)
print(text)  # mov rcx, rax

# Encode roundtrip
encoded = v.encode(di)
assert encoded == b'\x48\x89\xc1'

# Disassemble a block
for addr, text, length in v.disassemble_all(code, rip=0x401000):
    print(f'{addr:#x}: {text}')

# Emulator, IR, relocation, branch analysis also available
```

## Rust Bindings

The `rust/` directory contains a Cargo crate that uses [cxx](https://cxx.rs) for
direct C++ interop (no C wrapper layer).

```rust
use vedx64::{decode, disassemble, classify_flow};

let instr = decode(&[0x48, 0x89, 0xC1]).unwrap();
println!("{} ({} bytes)", instr.mnemonic_name(), instr.length());

let text = disassemble(&[0x48, 0x89, 0xC1], 0x401000).unwrap();
println!("{}", text); // mov rcx, rax

let flow = classify_flow(&[0xC3], 0x401000);
println!("flow type: {}", flow.flow_type); // Return
```

## Platform Support

| Platform | Decode/Encode/Disasm | Emulator | Hooking | Syscall |
|----------|---------------------|----------|---------|---------|
| Windows x86-64 | Full | Full | Full (NT syscalls) | Full |
| Windows ARM64 (emulation) | Full | Full | Analysis only (live hooks skipped) | Full |
| Linux x86-64 | Full | Full | mmap/mprotect fallbacks | N/A |

## Instruction Coverage

vedx64 covers the full x86-64 instruction set as defined by the Intel SDM,
including extensions:

- **General purpose**: All integer ALU, data movement, control flow, string, I/O, flag, and system instructions
- **SSE/SSE2/SSE3/SSSE3/SSE4.1/SSE4.2**: Full SIMD coverage
- **AVX/AVX2**: 256-bit VEX-encoded instructions with ymm register support
- **FMA3**: Fused multiply-add (132/213/231 forms)
- **BMI1/BMI2**: Bit manipulation (ANDN, BEXTR, BLSI, BZHI, PDEP, PEXT, SARX, SHRX, etc.)
- **AES-NI**: AES encryption/decryption rounds and key generation
- **SHA**: SHA-1 and SHA-256 acceleration
- **PCLMULQDQ**: Carry-less multiplication
- **ADX**: Multi-precision add (ADCX, ADOX)
- **CET**: Control-flow enforcement (ENDBR32, ENDBR64, shadow stack)
- **TSX**: Transactional memory (XBEGIN, XEND, XABORT, XTEST)
- **RDRAND/RDSEED**: Hardware random number generation
- **FSGSBASE**: FS/GS base register access
- **x87 FPU**: Floating-point unit (memory forms)
- **AVX-512**: Foundation, BW, DQ, VL, CD — EVEX encoding with opmask (k1-k7), zeroing, broadcast, embedded rounding
- **VNNI**: Vector neural network (VPDPBUSD, VPDPWSSD, etc.)
- **AVX-IFMA**: Integer fused multiply-add (VPMADD52LUQ, VPMADD52HUQ)
- **AVX-NE-CONVERT**: BF16/FP16 non-exception conversions
- **AMX**: Advanced matrix extensions (tile load/store, dot product)
- **APX**: REX2 prefix for extended GPRs (R16-R31)
- **MPX**: Memory protection extensions (deprecated but decodable)
- **Key Locker**: AES key locker instructions
- **GFNI**: Galois field arithmetic

## Project Structure

```
include/vedx64/       C++ headers (public API)
lib/                  C++ implementation files
tests/                Test executables (11 suites)
examples/             Example programs (10 examples)
python/               Python nanobind binding
rust/                 Rust cxx binding crate
tools/                instrgen corpus generator, standalone disassembler, standalone assembler
```

## License

See LICENSE file for details.
