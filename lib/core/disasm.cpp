// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/core.hpp"
#ifdef VEDX64_STRINGS
#include <cstdio>
#include <cstring>

namespace vedx64 {

namespace {

static inline int emit_str(char* buf, int max, const char* s) {
    int i = 0; while (i < max - 1 && s[i]) { buf[i] = s[i]; i++; } buf[i] = 0; return i;
}
static inline int emit_ch(char* buf, int max, char c) {
    if (max < 2) return 0; buf[0] = c; buf[1] = 0; return 1;
}
static inline int emit_2str(char* buf, int max, const char* a, const char* b) {
    int n = emit_str(buf, max, a); return n + emit_str(buf + n, max - n, b);
}
static inline int emit_hex8(char* buf, int max, uint8_t v) {
    if (max < 5) return 0;
    static const char hex[] = "0123456789abcdef";
    buf[0]='0'; buf[1]='x'; buf[2]=hex[v>>4]; buf[3]=hex[v&15]; buf[4]=0; return 4;
}
static inline int emit_hex(char* buf, int max, uint64_t v) {
    static const char h[] = "0123456789abcdef";
    if (max < 4) return 0;
    char tmp[18]; int n = 0;
    if (v == 0) { buf[0]='0'; buf[1]='x'; buf[2]='0'; buf[3]=0; return 3; }
    while (v) { tmp[n++] = h[v & 0xF]; v >>= 4; }
    int off = 0; buf[off++] = '0'; buf[off++] = 'x';
    for (int i = n - 1; i >= 0 && off < max - 1; i--) buf[off++] = tmp[i];
    buf[off] = 0; return off;
}
static inline int emit_3str(char* buf, int max, const char* a, const char* b, const char* c) {
    int n = emit_str(buf, max, a); n += emit_str(buf+n, max-n, b); return n + emit_str(buf+n, max-n, c);
}

static bool has_vex_operand(const InstrDesc& d) {
    for (uint8_t i = 0; i < d.num_operands; ++i)
        if (d.operands[i].addr == AddrMode::VexReg || d.operands[i].addr == AddrMode::XmmVvvv) return true;
    return false;
}

const char* reg8[]  = {"al","cl","dl","bl","spl","bpl","sil","dil","r8b","r9b","r10b","r11b","r12b","r13b","r14b","r15b","r16b","r17b","r18b","r19b","r20b","r21b","r22b","r23b","r24b","r25b","r26b","r27b","r28b","r29b","r30b","r31b"};
const char* reg16[] = {"ax","cx","dx","bx","sp","bp","si","di","r8w","r9w","r10w","r11w","r12w","r13w","r14w","r15w","r16w","r17w","r18w","r19w","r20w","r21w","r22w","r23w","r24w","r25w","r26w","r27w","r28w","r29w","r30w","r31w"};
const char* reg32[] = {"eax","ecx","edx","ebx","esp","ebp","esi","edi","r8d","r9d","r10d","r11d","r12d","r13d","r14d","r15d","r16d","r17d","r18d","r19d","r20d","r21d","r22d","r23d","r24d","r25d","r26d","r27d","r28d","r29d","r30d","r31d"};
const char* reg64[] = {"rax","rcx","rdx","rbx","rsp","rbp","rsi","rdi","r8","r9","r10","r11","r12","r13","r14","r15","r16","r17","r18","r19","r20","r21","r22","r23","r24","r25","r26","r27","r28","r29","r30","r31"};
const char* xmm[]   = {"xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","xmm8","xmm9","xmm10","xmm11","xmm12","xmm13","xmm14","xmm15"};
const char* ymm[]   = {"ymm0","ymm1","ymm2","ymm3","ymm4","ymm5","ymm6","ymm7","ymm8","ymm9","ymm10","ymm11","ymm12","ymm13","ymm14","ymm15"};
const char* mmx_r[] = {"mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"};
const char* seg_r[] = {"es","cs","ss","ds","fs","gs","??","??"};
const char* scale_tab[] = {"1","2","4","8"};

static bool is_default64(Mnemonic m) {
    switch (m) {
    case Mnemonic::PUSH: case Mnemonic::POP:
    case Mnemonic::CALL: case Mnemonic::CALLF:
    case Mnemonic::JMP: case Mnemonic::JMPF:
    case Mnemonic::RETN: case Mnemonic::RETF:
        return true;
    default: return false;
    }
}

const char** reg_table(OpSize sz, bool rex_w, bool opsz, bool def64) {
    switch (sz) {
    case OpSize::Byte: case OpSize::ByteSignExt: return (const char**)reg8;
    case OpSize::Word: return (const char**)reg16;
    case OpSize::Dword: return (const char**)reg32;
    case OpSize::Qword: return (const char**)reg64;
    case OpSize::OpSz:
        if (def64) return opsz ? (const char**)reg16 : (const char**)reg64;
        return opsz ? (const char**)reg16 : (const char**)reg32;
    case OpSize::OpSzQ: return rex_w ? (const char**)reg64 : (opsz ? (const char**)reg16 : (const char**)reg32);
    case OpSize::OpSzDS: return opsz ? (const char**)reg16 : (const char**)reg32;
    default: return (const char**)reg64;
    }
}

int fmt_sib(const DecodedInstr& di, uint8_t mod_, char* buf, int max) {
    uint8_t scale = di.sib >> 6;
    uint8_t index = ((di.sib >> 3) & 7) | ((di.rex & 0x02) ? 8 : 0) | (di.has_rex2 ? (di.rex2_x4 << 4) : 0);
    uint8_t base  = (di.sib & 7) | ((di.rex & 0x01) ? 8 : 0) | (di.has_rex2 ? (di.rex2_b4 << 4) : 0);
    bool has_index = (index != 4); // RSP can't be index
    bool has_base = !(mod_ == 0 && (di.sib & 7) == 5);
    uint8_t rmask = di.has_rex2 ? 31 : 15;
    int off = 0;
    off += emit_ch(buf+off, max-off, '[');
    bool need_plus = false;
    if (has_base) {
        off += emit_str(buf+off, max-off, reg64[base & rmask]);
        need_plus = true;
    }
    if (has_index) {
        if (need_plus) off += emit_ch(buf+off, max-off, '+');
        off += emit_str(buf+off, max-off, reg64[index & rmask]);
        off += emit_ch(buf+off, max-off, '*');
        off += emit_str(buf+off, max-off, scale_tab[scale]);
        need_plus = true;
    }
    if (di.displacement != 0) {
        if (di.displacement > 0) { if (need_plus) off += emit_ch(buf+off, max-off, '+'); off += emit_hex(buf+off, max-off, (uint64_t)di.displacement); }
        else { off += emit_ch(buf+off, max-off, '-'); off += emit_hex(buf+off, max-off, (uint64_t)(-di.displacement)); }
    } else if (!has_base && !has_index) {
        off += emit_str(buf+off, max-off, "0x0");
    }
    off += emit_ch(buf+off, max-off, ']');
    return off;
}

static bool vex_rm_stays_xmm(Mnemonic m) {
    const char* n = mnemonic_name(m);
    return (strstr(n, "broadcast") || strstr(n, "insertf128") || strstr(n, "inserti128") || strstr(n, "extractf128") || strstr(n, "extracti128"));
}

static bool is_bmi_gpr_vvvv(Mnemonic m) {
    switch (m) {
    case Mnemonic::ANDN: case Mnemonic::BEXTR: case Mnemonic::BZHI:
    case Mnemonic::PDEP: case Mnemonic::PEXT: case Mnemonic::MULX:
    case Mnemonic::SARX: case Mnemonic::SHLX: case Mnemonic::SHRX:
    case Mnemonic::BLSI: case Mnemonic::BLSMSK: case Mnemonic::BLSR:
    case Mnemonic::RORX:
        return true;
    default: return false;
    }
}

int fmt_modrm(const DecodedInstr& di, const OperandDesc& op, char* buf, int max, bool is_reg_field, bool opsz, bool def64, bool gpr32_force = false, bool gpr64_force = false) {
    bool rex_w = (di.rex & 0x08) != 0;
    bool rex_r = (di.rex & 0x04) != 0;
    bool rex_b = (di.rex & 0x01) != 0;
    uint8_t mod_ = di.modrm >> 6;
    uint8_t reg = ((di.modrm >> 3) & 7) | (rex_r ? 8 : 0) | (di.has_rex2 ? (di.rex2_r4 << 4) : 0);
    uint8_t rm = (di.modrm & 7) | (rex_b ? 8 : 0) | (di.has_rex2 ? (di.rex2_b4 << 4) : 0);
    uint8_t rmask = di.has_rex2 ? 31 : 15;

    if (is_reg_field) {
        if (op.addr == AddrMode::XmmReg) return emit_str(buf, max, (di.has_vex && di.vex_L >= 1) ? ymm[reg & 15] : xmm[reg & 15]);
        if (op.addr == AddrMode::MmxReg) return emit_str(buf, max, mmx_r[reg & 7]);
        if (op.addr == AddrMode::SegReg) return emit_str(buf, max, seg_r[reg & 7]);
        if (gpr32_force && op.size != OpSize::Byte && op.size != OpSize::ByteSignExt && op.size != OpSize::Word) return emit_str(buf, max, reg32[reg & rmask]);
        if (gpr64_force) return emit_str(buf, max, reg64[reg & rmask]);
        return emit_str(buf, max, reg_table(op.size, rex_w, opsz, def64)[reg & rmask]);
    }

    if (mod_ == 3) {
        if (op.addr == AddrMode::XmmRM || op.addr == AddrMode::XmmRegOnly) {
            bool use_ymm = di.has_vex && di.vex_L >= 1 && !vex_rm_stays_xmm(di.desc->mnemonic);
            return emit_str(buf, max, use_ymm ? ymm[rm & 15] : xmm[rm & 15]);
        }
        if (op.addr == AddrMode::MmxRM || op.addr == AddrMode::MmxRegOnly) return emit_str(buf, max, mmx_r[rm & 7]);
        if (gpr32_force && op.size != OpSize::Byte && op.size != OpSize::ByteSignExt && op.size != OpSize::Word) return emit_str(buf, max, reg32[rm & rmask]);
        if (gpr64_force) return emit_str(buf, max, reg64[rm & rmask]);
        return emit_str(buf, max, reg_table(op.size, rex_w, opsz, def64)[rm & rmask]);
    }

    if ((di.modrm & 7) == 4) {
        return fmt_sib(di, mod_, buf, max);
    }

    if (mod_ == 0 && (di.modrm & 7) == 5) {
        int o=0; o+=emit_str(buf+o,max-o,"[rip");
        if (di.displacement >= 0) { o+=emit_ch(buf+o,max-o,'+'); o+=emit_hex(buf+o,max-o,(uint64_t)di.displacement); }
        else { o+=emit_ch(buf+o,max-o,'-'); o+=emit_hex(buf+o,max-o,(uint64_t)(-di.displacement)); }
        o+=emit_ch(buf+o,max-o,']'); return o;
    }

    const char* base = reg64[rm & 15];
    if (di.displacement == 0 && mod_ == 0) return emit_3str(buf, max, "[", base, "]");
    { int o=0; o+=emit_ch(buf+o,max-o,'['); o+=emit_str(buf+o,max-o,base);
    if (di.displacement >= 0) { o+=emit_ch(buf+o,max-o,'+'); o+=emit_hex(buf+o,max-o,(uint64_t)di.displacement); }
    else { o+=emit_ch(buf+o,max-o,'-'); o+=emit_hex(buf+o,max-o,(uint64_t)(-di.displacement)); }
    o+=emit_ch(buf+o,max-o,']'); return o; }
}

const char* size_prefix(OpSize sz, bool rex_w, bool opsz, bool def64, uint8_t vex_L = 0) {
    switch (sz) {
    case OpSize::Byte: case OpSize::ByteSignExt: return "byte ";
    case OpSize::Word: return "word ";
    case OpSize::Dword: return "dword ";
    case OpSize::Qword: return "qword ";
    case OpSize::OpSz:
        if (rex_w) return "qword ";
        if (def64) return opsz ? "word " : "qword ";
        return opsz ? "word " : "dword ";
    case OpSize::OpSzQ:
        if (rex_w) return "qword ";
        return opsz ? "word " : "dword ";
    case OpSize::OpSzDS: return opsz ? "word " : "dword ";
    case OpSize::Xmm128: return (vex_L >= 1) ? "ymmword " : "xmmword ";
    default: return "";
    }
}

const char* cr_r[] = {"cr0","cr1","cr2","cr3","cr4","cr5","cr6","cr7","cr8","cr9","cr10","cr11","cr12","cr13","cr14","cr15"};
const char* dr_r[] = {"dr0","dr1","dr2","dr3","dr4","dr5","dr6","dr7","dr8","dr9","dr10","dr11","dr12","dr13","dr14","dr15"};

static bool hide_all_fixed(Mnemonic m) {
    switch (m) {
    case Mnemonic::MUL: case Mnemonic::IMUL: case Mnemonic::DIV: case Mnemonic::IDIV:
    case Mnemonic::CMPXCHG: case Mnemonic::CMPXCHG8B: case Mnemonic::CMPXCHG16B:
    case Mnemonic::SYSCALL: case Mnemonic::SYSRET: case Mnemonic::SYSENTER: case Mnemonic::SYSEXIT:
    case Mnemonic::RDTSC: case Mnemonic::RDTSCP: case Mnemonic::RDMSR: case Mnemonic::WRMSR: case Mnemonic::RDPMC:
    case Mnemonic::CPUID: case Mnemonic::GETSEC: case Mnemonic::CLTS: case Mnemonic::XGETBV: case Mnemonic::XSETBV:
    case Mnemonic::MONITOR: case Mnemonic::MWAIT: case Mnemonic::SWAPGS:
    case Mnemonic::FXSAVE: case Mnemonic::XSAVE:
    case Mnemonic::PBLENDVB: case Mnemonic::BLENDVPS: case Mnemonic::BLENDVPD:
    case Mnemonic::LOOP: case Mnemonic::LOOPE: case Mnemonic::LOOPNE: case Mnemonic::LOOPZ: case Mnemonic::LOOPNZ:
    case Mnemonic::JCXZ: case Mnemonic::JECXZ: case Mnemonic::JRCXZ:
    case Mnemonic::XLAT:
    case Mnemonic::LMSW: case Mnemonic::SMSW:
    case Mnemonic::LSS: case Mnemonic::LFS: case Mnemonic::LGS:
    case Mnemonic::PCMPESTRM: case Mnemonic::PCMPESTRI: case Mnemonic::PCMPISTRM: case Mnemonic::PCMPISTRI:
    case Mnemonic::MOVMSKPS: case Mnemonic::MOVMSKPD: case Mnemonic::PMOVMSKB:
        return true;
    default: return false;
    }
}

static bool force_gpr32(Mnemonic m) {
    switch (m) {
    case Mnemonic::CVTSI2SS: case Mnemonic::CVTSI2SD:
    case Mnemonic::CVTSS2SI: case Mnemonic::CVTSD2SI:
    case Mnemonic::CVTTSS2SI: case Mnemonic::CVTTSD2SI:
    case Mnemonic::CRC32:
    case Mnemonic::MOVMSKPS: case Mnemonic::MOVMSKPD: case Mnemonic::PMOVMSKB:
    case Mnemonic::PINSRW: case Mnemonic::PEXTRW:
    case Mnemonic::MOVNTI: case Mnemonic::ADCX:
        return true;
    default: return false;
    }
}

static bool force_gpr64(Mnemonic m) {
    switch (m) {
    case Mnemonic::INVEPT: case Mnemonic::INVVPID: case Mnemonic::INVPCID:
    case Mnemonic::UMONITOR: case Mnemonic::MOVDIR64B:
        return true;
    default: return false;
    }
}

} // anonymous namespace

size_t disassemble(const uint8_t* code, size_t len, char* buf, size_t buf_len_, uint64_t rip) {
    int buf_len = (int)buf_len_;
    DecodedInstr di;
    size_t consumed = decode(code, len, di);
    if (consumed == 0 || !di.desc) {
        snprintf(buf, buf_len, "db 0x%02x", code[0]);
        return 1;
    }

    bool rex_w = (di.rex & 0x08) != 0;
    bool opsz = false;
    for (uint8_t i = 0; i < di.num_prefixes; ++i) if (di.legacy_prefix[i] == 0x66) opsz = true;
    bool def64 = is_default64(di.desc->mnemonic);
    char tmp[256];
    int off = 0;

    for (uint8_t i = 0; i < di.num_prefixes; ++i) {
        uint8_t p = di.legacy_prefix[i];
        if (p == di.desc->prefix) continue; // mandatory, skip
        if (p == 0xF0) off += snprintf(buf+off, buf_len-off, "lock ");
        else if (p == 0xF3 && di.desc->mnemonic != Mnemonic::PAUSE) off += snprintf(buf+off, buf_len-off, "rep ");
        else if (p == 0xF2) off += snprintf(buf+off, buf_len-off, "repne ");
    }

    // NOP: 90 without REX.B
    if (di.desc->mnemonic == Mnemonic::XCHG && di.desc->opcode == 0x90 && di.opcode_reg == 0 && !(di.rex & 0x01)) {
        off += snprintf(buf+off, buf_len-off, "nop");
        return consumed;
    }

    if (di.desc->opcode == 0xCC) {
        off += snprintf(buf+off, buf_len-off, "int3");
        return consumed;
    }
    if (di.desc->mnemonic == Mnemonic::RETN && di.desc->opcode == 0xC3) {
        off += snprintf(buf+off, buf_len-off, "ret");
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::CBW) {
        off += emit_str(buf+off, buf_len-off, rex_w ? "cdqe" : (opsz ? "cbw" : "cwde"));
        return consumed;
    }
    if (di.desc->mnemonic == Mnemonic::CWD) {
        off += emit_str(buf+off, buf_len-off, rex_w ? "cqo" : (opsz ? "cwd" : "cdq"));
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::IRET || di.desc->mnemonic == Mnemonic::IRETD) {
        off += emit_str(buf+off, buf_len-off, rex_w ? "iretq" : (opsz ? "iretw" : "iretd"));
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::SAHF || di.desc->mnemonic == Mnemonic::LAHF || di.desc->mnemonic == Mnemonic::LEAVE) {
        off += emit_str(buf+off, buf_len-off, mnemonic_name(di.desc->mnemonic));
        return consumed;
    }

    if (di.desc->opcode >= 0xD8 && di.desc->opcode <= 0xDF) {
        uint8_t opc = (uint8_t)(di.desc->opcode & 0xFF);
        uint8_t mod_ = di.modrm >> 6;
        uint8_t reg = (di.modrm >> 3) & 7;
        uint8_t rm = di.modrm & 7;
        // FNSTSW AX special case
        if (opc == 0xDF && mod_ == 3 && reg == 4) {
            off += snprintf(buf+off, buf_len-off, "fnstsw ax");
            return consumed;
        }
        // Mnemonic overrides for x87
        const char* fmn = mnemonic_name(di.desc->mnemonic);
        if (di.desc->mnemonic == Mnemonic::FSTP1) fmn = "fstpnce";
        else if (di.desc->mnemonic == Mnemonic::FUCOMIP) fmn = "fucompi";
        else if (di.desc->mnemonic == Mnemonic::FCOMIP) fmn = "fcompi";
        off += emit_str(buf+off, buf_len-off, fmn);
        if (mod_ != 3) {
            // Memory form: determine size prefix
            const char* sz = "dword ";
            if (opc == 0xDC) sz = "qword ";
            else if (opc == 0xDD) {
                if (reg == 4 || reg == 6) sz = "dword ";
                else if (reg == 7) sz = "word ";
                else sz = "qword ";
            }
            else if (opc == 0xDE) sz = "word ";
            else if (opc == 0xDF) {
                if (reg == 4 || reg == 6) sz = "tbyte ";
                else if (reg == 5 || reg == 7) sz = "qword ";
                else sz = "word ";
            }
            else if (opc == 0xDB) {
                if (reg == 5 || reg == 7) sz = "xword ";
                else sz = "dword ";
            }
            else if (opc == 0xD9) {
                if (reg == 4 || reg == 6) sz = "";
                else if (reg == 5 || reg == 7) sz = "word ";
                else sz = "dword ";
            }
            fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
            off += snprintf(buf+off, buf_len-off, " %s%s", sz, tmp);
        } else {
            // Register-register form
            Mnemonic m = di.desc->mnemonic;
            switch (m) {
            // Two operands: st(0), st(N)
            case Mnemonic::FADD: case Mnemonic::FMUL: case Mnemonic::FSUB: case Mnemonic::FSUBR:
            case Mnemonic::FDIV: case Mnemonic::FDIVR:
            case Mnemonic::FCMOVB: case Mnemonic::FCMOVE: case Mnemonic::FCMOVBE: case Mnemonic::FCMOVU:
            case Mnemonic::FCMOVNB: case Mnemonic::FCMOVNE: case Mnemonic::FCMOVNBE: case Mnemonic::FCMOVNU:
                off += snprintf(buf+off, buf_len-off, " st(%d), st(%d)", 0, rm);
                break;
            // Single operand: st(N)
            case Mnemonic::FLD: case Mnemonic::FXCH: case Mnemonic::FFREE: case Mnemonic::FFREEP:
            case Mnemonic::FST: case Mnemonic::FSTP: case Mnemonic::FUCOM: case Mnemonic::FUCOMP:
            case Mnemonic::FADDP: case Mnemonic::FMULP: case Mnemonic::FSUBP: case Mnemonic::FSUBRP:
            case Mnemonic::FDIVP: case Mnemonic::FDIVRP:
            case Mnemonic::FCOM: case Mnemonic::FCOMP:
            case Mnemonic::FCOMI: case Mnemonic::FUCOMI: case Mnemonic::FUCOMIP: case Mnemonic::FCOMIP:
                off += snprintf(buf+off, buf_len-off, " st(%d)", rm);
                break;
            case Mnemonic::FSTP1:
                off += snprintf(buf+off, buf_len-off, " st(%d), st(%d)", 0, rm);
                break;
            // No operands: all other x87 register forms
            default:
                break;
            }
        }
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::ENTER) {
        uint16_t iw = (uint16_t)(di.immediate & 0xFFFF);
        uint8_t ib = (uint8_t)(di.displacement & 0xFF);
        off += snprintf(buf+off, buf_len-off, "enter 0x%X, 0x%X", iw, ib);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::MOV && (di.desc->opcode == 0x8C || di.desc->opcode == 0x8E)) {
        uint8_t sreg = (di.modrm >> 3) & 7;
        off += snprintf(buf+off, buf_len-off, "mov");
        if (di.desc->opcode == 0x8C) { // MOV r/m16, Sreg
            fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
            uint8_t mod_ = di.modrm >> 6;
            if (mod_ != 3) off += snprintf(buf+off, buf_len-off, " word %s, %s", tmp, seg_r[sreg & 7]);
            else off += snprintf(buf+off, buf_len-off, " %s, %s", tmp, seg_r[sreg & 7]);
        } else { // MOV Sreg, r/m16
            fmt_modrm(di, di.desc->operands[1], tmp, sizeof(tmp), false, opsz, def64);
            uint8_t mod_ = di.modrm >> 6;
            if (mod_ != 3) off += snprintf(buf+off, buf_len-off, " %s, word %s", seg_r[sreg & 7], tmp);
            else off += snprintf(buf+off, buf_len-off, " %s, %s", seg_r[sreg & 7], tmp);
        }
        return consumed;
    }

    bool is_string_op = false;
    const char* str_suffix = "";
    uint16_t sopc = di.desc->opcode;
    if (sopc==0xA4||sopc==0xA6||sopc==0xAA||sopc==0xAC||sopc==0xAE||sopc==0x6C||sopc==0x6E) {
        is_string_op = true; str_suffix = "b";
    } else if (sopc==0xA5||sopc==0xA7||sopc==0xAB||sopc==0xAD||sopc==0xAF||sopc==0x6D||sopc==0x6F) {
        is_string_op = true;
        if (rex_w) str_suffix = "q";
        else if (opsz) str_suffix = "w";
        else str_suffix = "d";
    }

    if (di.desc->mnemonic == Mnemonic::MOV && di.desc->opcode >= 0x0F20 && di.desc->opcode <= 0x0F23) {
        uint8_t reg = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        uint8_t rm = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        const char** cr_or_dr = (di.desc->opcode <= 0x0F21) ?
            ((di.desc->opcode == 0x0F20) ? (const char**)cr_r : (const char**)dr_r) :
            ((di.desc->opcode == 0x0F22) ? (const char**)cr_r : (const char**)dr_r);
        if (di.desc->opcode == 0x0F20 || di.desc->opcode == 0x0F21)
            off += snprintf(buf+off, buf_len-off, "mov %s, %s", reg64[rm & 15], cr_or_dr[reg & 15]);
        else
            off += snprintf(buf+off, buf_len-off, "mov %s, %s", cr_or_dr[reg & 15], reg64[rm & 15]);
        return consumed;
    }

    if (di.desc->opcode == 0x0FA0 || di.desc->opcode == 0x0FA1 ||
        di.desc->opcode == 0x0FA8 || di.desc->opcode == 0x0FA9) {
        const char* seg = (di.desc->opcode == 0x0FA0 || di.desc->opcode == 0x0FA1) ? "fs" : "gs";
        const char* opn = (di.desc->opcode == 0x0FA0 || di.desc->opcode == 0x0FA8) ? "push" : "pop";
        off += snprintf(buf+off, buf_len-off, "%s %s", opn, seg);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::ADCX && di.desc->has_modrm && (di.modrm >> 6) != 3) {
        off += snprintf(buf+off, buf_len-off, "adcx");
        uint8_t reg = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        fmt_modrm(di, di.desc->operands[1], tmp, sizeof(tmp), false, opsz, def64);
        off += snprintf(buf+off, buf_len-off, " %s, dword %s", rex_w ? reg64[reg & 15] : reg32[reg & 15], tmp);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::MOVDIR64B && di.desc->has_modrm) {
        uint8_t reg = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        fmt_modrm(di, di.desc->operands[1], tmp, sizeof(tmp), false, opsz, def64);
        off += snprintf(buf+off, buf_len-off, "movdir64b %s, zmmword %s", reg64[reg & 15], tmp);
        return consumed;
    }

    if ((di.desc->mnemonic == Mnemonic::RDPKRU || di.desc->mnemonic == Mnemonic::WRPKRU) &&
        di.num_prefixes > 0) {
        bool has_f3 = false;
        for (uint8_t i = 0; i < di.num_prefixes; ++i) if (di.legacy_prefix[i] == 0xF3) has_f3 = true;
        if (has_f3) {
            off += emit_str(buf+off, buf_len-off, di.desc->mnemonic == Mnemonic::RDPKRU ? "clui" : "stui");
            return consumed;
        }
    }

    if (di.desc->mnemonic == Mnemonic::BNDMK || di.desc->mnemonic == Mnemonic::BNDCL ||
        di.desc->mnemonic == Mnemonic::BNDCU || di.desc->mnemonic == Mnemonic::BNDCN ||
        di.desc->mnemonic == Mnemonic::BNDMOV) {
        uint8_t bnd = ((di.modrm >> 3) & 7);
        off += snprintf(buf+off, buf_len-off, "%s bnd%d, ", mnemonic_name(di.desc->mnemonic), bnd);
        if ((di.modrm >> 6) == 3) {
            if (di.desc->mnemonic == Mnemonic::BNDMOV)
                off += snprintf(buf+off, buf_len-off, "bnd%d", di.modrm & 7);
            else
                off += emit_str(buf+off, buf_len-off, reg64[(di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0)]);
        } else {
            fmt_modrm(di, di.desc->operands[1], tmp, sizeof(tmp), false, opsz, def64);
            if (di.desc->mnemonic == Mnemonic::BNDMOV) off += snprintf(buf+off, buf_len-off, "xmmword %s", tmp);
            else off += emit_str(buf+off, buf_len-off, tmp);
        }
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::SHA256RNDS2) {
        off += snprintf(buf+off, buf_len-off, "sha256rnds2");
        fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), true, opsz, def64);
        off += snprintf(buf+off, buf_len-off, " %s", tmp);
        fmt_modrm(di, di.desc->operands[1], tmp, sizeof(tmp), false, opsz, def64);
        uint8_t mod_ = di.modrm >> 6;
        if (mod_ != 3) off += snprintf(buf+off, buf_len-off, ", xmmword %s, xmm0", tmp);
        else off += snprintf(buf+off, buf_len-off, ", %s, xmm0", tmp);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::XBEGIN && consumed >= 2) {
        int32_t rel = 0; memcpy(&rel, &code[2], 4);
        uint64_t target = rip + 6 + rel; // 2 (opcode) + 4 (rel32) = 6 total
        off += snprintf(buf+off, buf_len-off, "xbegin 0x%llx", (unsigned long long)target);
        consumed = 6; // fix consumed to include rel32
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::XABORT) {
        uint8_t imm = code[consumed]; consumed++;
        off += snprintf(buf+off, buf_len-off, "xabort 0x%02X", imm);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::RDPID) {
        uint8_t rm = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        off += snprintf(buf+off, buf_len-off, "rdpid %s", reg64[rm & 15]);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::INVLPG) {
        fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
        off += snprintf(buf+off, buf_len-off, "invlpg byte %s", tmp);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::XLAT) {
        off += snprintf(buf+off, buf_len-off, "xlatb");
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::CALLF && di.desc->opcode == 0xFF) {
        fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
        off += snprintf(buf+off, buf_len-off, "call %s", tmp);
        return consumed;
    }
    if (di.desc->mnemonic == Mnemonic::JMPF && di.desc->opcode == 0xFF) {
        fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
        off += snprintf(buf+off, buf_len-off, "jmp %s", tmp);
        return consumed;
    }

    if (di.desc->opcode == 0x0F0D) {
        fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
        off += snprintf(buf+off, buf_len-off, "prefetchw byte %s", tmp);
        return consumed;
    }
    if (di.desc->mnemonic == Mnemonic::HINT_NOP || (di.desc->mnemonic == Mnemonic::NOP && di.desc->has_modrm)) {
        if (di.desc->opcode == 0x0F1A && (di.modrm >> 6) != 3) {
            uint8_t reg = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
            fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
            off += snprintf(buf+off, buf_len-off, "bndldx bnd%d, %s", reg, tmp);
            return consumed;
        }
        if (di.desc->opcode == 0x0F1B && (di.modrm >> 6) != 3) {
            uint8_t reg = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
            fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
            off += snprintf(buf+off, buf_len-off, "bndstx %s, bnd%d", tmp, reg);
            return consumed;
        }
        if (di.desc->opcode == 0x0F18) {
            uint8_t reg = (di.modrm >> 3) & 7;
            uint8_t mod_ = di.modrm >> 6;
            if (reg <= 3 && mod_ != 3) {
                const char* pf[] = {"prefetchnta", "prefetcht0", "prefetcht1", "prefetcht2"};
                fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
                off += snprintf(buf+off, buf_len-off, "%s byte %s", pf[reg], tmp);
                return consumed;
            }
        }
        off += snprintf(buf+off, buf_len-off, "nop");
        if (di.desc->has_modrm) {
            fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
            uint8_t mod_ = di.modrm >> 6;
            if (mod_ != 3) off += snprintf(buf+off, buf_len-off, " dword %s", tmp);
            else {
                off += snprintf(buf+off, buf_len-off, " %s", tmp);
                // 0F 19: two-operand NOP form
                if (di.desc->opcode == 0x0F19) {
                    uint8_t nop_reg = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
                    off += snprintf(buf+off, buf_len-off, ", %s", reg32[nop_reg & 15]);
                }
            }
        }
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::FXRSTOR || di.desc->mnemonic == Mnemonic::XRSTOR) {
        off += emit_str(buf+off, buf_len-off, mnemonic_name(di.desc->mnemonic));
        fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64);
        off += snprintf(buf+off, buf_len-off, " %s", tmp);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::VMREAD || di.desc->mnemonic == Mnemonic::VMWRITE) {
        off += emit_str(buf+off, buf_len-off, mnemonic_name(di.desc->mnemonic));
        uint8_t mod_ = di.modrm >> 6;
        uint8_t reg = ((di.modrm >> 3) & 7) | ((di.rex & 0x04) ? 8 : 0);
        uint8_t rm = (di.modrm & 7) | ((di.rex & 0x01) ? 8 : 0);
        if (di.desc->mnemonic == Mnemonic::VMREAD) {
            if (mod_ == 3) off += snprintf(buf+off, buf_len-off, " %s, %s", reg64[rm & 15], reg64[reg & 15]);
            else { fmt_modrm(di, di.desc->operands[0], tmp, sizeof(tmp), false, opsz, def64); off += snprintf(buf+off, buf_len-off, " qword %s, %s", tmp, reg64[reg & 15]); }
        } else {
            if (mod_ == 3) off += snprintf(buf+off, buf_len-off, " %s, %s", reg64[reg & 15], reg64[rm & 15]);
            else { fmt_modrm(di, di.desc->operands[1], tmp, sizeof(tmp), false, opsz, def64); off += snprintf(buf+off, buf_len-off, " %s, qword %s", reg64[reg & 15], tmp); }
        }
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::MFENCE && (di.modrm >> 6) != 3) {
        OperandDesc fake = {AddrMode::MemOnly, OpSize::None, 0xFF};
        fmt_modrm(di, fake, tmp, sizeof(tmp), false, opsz, def64);
        off += snprintf(buf+off, buf_len-off, "xsaveopt %s", tmp);
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::UD) {
        off += snprintf(buf+off, buf_len-off, "ud1");
        return consumed;
    }

    if (di.desc->mnemonic == Mnemonic::LOOPNZ) { off += snprintf(buf+off, buf_len-off, "loopne"); }
    else if (di.desc->mnemonic == Mnemonic::LOOPZ) { off += snprintf(buf+off, buf_len-off, "loope"); }
    else if (di.desc->mnemonic == Mnemonic::JCXZ || di.desc->mnemonic == Mnemonic::JECXZ || di.desc->mnemonic == Mnemonic::JRCXZ) {
        off += snprintf(buf+off, buf_len-off, "jrcxz");
    }
    else {
    const char* mn = mnemonic_name(di.desc->mnemonic);
    bool need_v = di.has_vex && mn[0] != 'v' && mn[0] != 'k' && !has_vex_operand(*di.desc) && !is_bmi_gpr_vvvv(di.desc->mnemonic);
    if (need_v) off += emit_ch(buf+off, buf_len-off, 'v');
    off += emit_str(buf+off, buf_len-off, mn);
    if (di.desc->mnemonic == Mnemonic::PUSHF || di.desc->mnemonic == Mnemonic::POPF) off += emit_ch(buf+off, buf_len-off, 'q');
    if (str_suffix[0]) off += emit_str(buf+off, buf_len-off, str_suffix);
    }
    if (is_string_op) return consumed;

    bool gpr32 = !rex_w && force_gpr32(di.desc->mnemonic);
    bool gpr64 = force_gpr64(di.desc->mnemonic);
    bool first_op = true;
    bool vex_vvvv_emitted = false;
    for (uint8_t i = 0; i < di.desc->num_operands && i < 4; ++i) {
        const auto& op = di.desc->operands[i];
        if (op.addr == AddrMode::None || op.addr == AddrMode::Stack ||
            op.addr == AddrMode::Flags ||
            op.addr == AddrMode::StringSrc || op.addr == AddrMode::StringDst) continue;
        if (op.addr == AddrMode::Fixed && (hide_all_fixed(di.desc->mnemonic) ||
            (di.desc->opcode >= 0xD8 && di.desc->opcode <= 0xDF) ||
            op.fixed_reg >= 8 || (op.size == OpSize::None && op.fixed_reg >= 4))) continue;
        off += emit_str(buf + off, buf_len - off, first_op ? " " : ", ");
        first_op = false;


        switch (op.addr) {
        case AddrMode::VexReg:
            off += emit_str(buf + off, buf_len - off, (rex_w || di.vex_W) ? reg64[di.vex_vvvv & 15] : reg32[di.vex_vvvv & 15]);
            vex_vvvv_emitted = true;
            break;
        case AddrMode::XmmVvvv:
            off += emit_str(buf + off, buf_len - off, (di.vex_L >= 1) ? ymm[di.vex_vvvv & 15] : xmm[di.vex_vvvv & 15]);
            vex_vvvv_emitted = true;
            break;
        case AddrMode::ModRM_Reg:
        case AddrMode::XmmReg:
        case AddrMode::MmxReg:
        case AddrMode::SegReg:
            fmt_modrm(di, op, tmp, sizeof(tmp), true, opsz, def64, gpr32 && op.addr == AddrMode::ModRM_Reg, gpr64 && op.addr == AddrMode::ModRM_Reg);
            off += emit_str(buf + off, buf_len - off, tmp);
            break;
        case AddrMode::ModRM_RM:
        case AddrMode::MemOnly:
        case AddrMode::RegOnly:
        case AddrMode::XmmRM:
        case AddrMode::XmmRegOnly:
        case AddrMode::MmxRM:
        case AddrMode::MmxRegOnly:
            fmt_modrm(di, op, tmp, sizeof(tmp), false, opsz, def64, gpr32 && (op.addr == AddrMode::ModRM_RM || op.addr == AddrMode::RegOnly), gpr64 && (op.addr == AddrMode::ModRM_RM || op.addr == AddrMode::RegOnly));
            if ((di.modrm >> 6) != 3 && op.addr != AddrMode::RegOnly && op.addr != AddrMode::XmmRegOnly && op.addr != AddrMode::MmxRegOnly && di.desc->mnemonic != Mnemonic::LEA && di.desc->mnemonic != Mnemonic::LSS && di.desc->mnemonic != Mnemonic::LFS && di.desc->mnemonic != Mnemonic::LGS) {
                const char* sp = "";
                if (op.addr == AddrMode::XmmRM) {
                    const char* mn = mnemonic_name(di.desc->mnemonic);
                    size_t mnlen = strlen(mn);
                    bool is_ss = mnlen >= 2 && mn[mnlen-2] == 's' && mn[mnlen-1] == 's';
                    bool is_sd = mnlen >= 2 && mn[mnlen-2] == 's' && mn[mnlen-1] == 'd';
                    // Exclude packed integer ops ending in "sd" (pabsd, pminsd, pmaxsd, etc.)
                    if (is_sd && mnlen >= 3 && (mn[0] == 'p' || (mn[0] == 'v' && mn[1] == 'p'))) is_sd = false;
                    Mnemonic m = di.desc->mnemonic;
                    if (m == Mnemonic::MOVQ) sp = "qword ";
                    else if (m == Mnemonic::CVTPS2PD || m == Mnemonic::CVTDQ2PD) sp = "qword ";
                    else if (m == Mnemonic::CVTTPS2PI || m == Mnemonic::CVTPS2PI) sp = "qword ";
                    else if (m == Mnemonic::VPBROADCASTB) sp = "byte ";
                    else if (m == Mnemonic::VPBROADCASTW) sp = "word ";
                    else if (m == Mnemonic::VPBROADCASTD) sp = "dword ";
                    else if (m == Mnemonic::VPBROADCASTQ) sp = "qword ";
                    else if (m == Mnemonic::CVTSI2SS || m == Mnemonic::CVTSI2SD) sp = rex_w ? "qword " : "dword ";
                    else if (m == Mnemonic::CVTSS2SI || m == Mnemonic::CVTTSS2SI) sp = "dword ";
                    else if (m == Mnemonic::CVTSD2SI || m == Mnemonic::CVTTSD2SI) sp = "qword ";
                    else if (m == Mnemonic::CVTSS2SD) sp = "dword ";
                    else if (m == Mnemonic::CVTSD2SS) sp = "qword ";
                    else if (m == Mnemonic::MOVDDUP) sp = "qword ";
                    else if (is_ss) sp = "dword ";
                    else if (is_sd) sp = "qword ";
                    else sp = (di.has_vex && di.vex_L >= 1 && !vex_rm_stays_xmm(di.desc->mnemonic)) ? "ymmword " : "xmmword ";
                }
                else if (op.addr == AddrMode::MmxRM) {
                    Mnemonic m = di.desc->mnemonic;
                    if (m == Mnemonic::PUNPCKLBW || m == Mnemonic::PUNPCKLWD || m == Mnemonic::PUNPCKLDQ)
                        sp = "dword ";
                    else sp = "qword ";
                }
                else sp = size_prefix(op.size, rex_w, opsz, def64, di.vex_L);
                if (sp[0] == '\0') {
                    if (gpr32) sp = rex_w ? "qword " : "dword ";
                    else if (gpr64) sp = "qword ";
                    else for (uint8_t j = 0; j < di.desc->num_operands; ++j) {
                        if (di.desc->operands[j].addr == AddrMode::XmmReg) { sp = (di.has_vex && di.vex_L >= 1) ? "ymmword " : "xmmword "; break; }
                        if (di.desc->operands[j].addr == AddrMode::ModRM_Reg && di.desc->operands[j].size != OpSize::None) { sp = size_prefix(di.desc->operands[j].size, rex_w, opsz, def64, di.vex_L); break; }
                    }
                }
                off += emit_str(buf + off, buf_len - off, sp);
            }
            off += emit_str(buf + off, buf_len - off, tmp);
            break;
        case AddrMode::Immediate:
            off += emit_hex(buf + off, buf_len - off, (uint64_t)di.immediate);
            break;
        case AddrMode::RelOffset: {
            uint64_t target = rip + consumed + di.immediate;
            off += emit_hex(buf + off, buf_len - off, target);
            break;
        }
        case AddrMode::OpcodeReg: {
            uint8_t reg_n = di.opcode_reg | ((di.rex & 0x01) ? 8 : 0);
            off += emit_str(buf + off, buf_len - off, reg_table(op.size, rex_w, opsz, def64)[reg_n & 15]);
            break;
        }
        case AddrMode::Fixed: {
            OpSize fsz = op.size;
            if (fsz == OpSize::None) {
                for (uint8_t j = 0; j < di.desc->num_operands; ++j) {
                    if (j != i && di.desc->operands[j].size != OpSize::None) {
                        fsz = di.desc->operands[j].size;
                        break;
                    }
                }
            }
            off += emit_str(buf + off, buf_len - off, reg_table(fsz, rex_w, opsz, def64)[op.fixed_reg & 15]);
            break;
        }
        case AddrMode::Moffset:
            off += snprintf(buf + off, buf_len - off, "%s[0x%llx]", size_prefix(op.size, rex_w, opsz, def64, di.vex_L), (unsigned long long)di.displacement);
            break;
        default: break;
        }
        if (di.has_vex && !vex_vvvv_emitted && !has_vex_operand(*di.desc)) {
            Mnemonic m = di.desc->mnemonic;
            const char* vmn = mnemonic_name(m);
            bool skip_vvvv = (strstr(vmn, "broadcast") || strstr(vmn, "extractf128") || strstr(vmn, "extracti128") || m == Mnemonic::RORX || m == Mnemonic::ROUNDPS || m == Mnemonic::ROUNDPD);
            bool vvvv_last = (m == Mnemonic::BEXTR || m == Mnemonic::BZHI || m == Mnemonic::SARX || m == Mnemonic::SHLX || m == Mnemonic::SHRX);
            bool vvvv_first = (m == Mnemonic::BLSI || m == Mnemonic::BLSMSK || m == Mnemonic::BLSR);
            if (!skip_vvvv) {
                bool emit_now = false;
                if (vvvv_first && i == 0) emit_now = false;
                else if (vvvv_last && i == 1) emit_now = true;
                else if (!vvvv_last && !vvvv_first && i == 0) emit_now = true;
                if (emit_now) {
                    if (is_bmi_gpr_vvvv(m)) {
                        off += emit_2str(buf + off, buf_len - off, ", ", (rex_w || di.vex_W) ? reg64[di.vex_vvvv & 15] : reg32[di.vex_vvvv & 15]);
                    } else {
                        off += emit_2str(buf + off, buf_len - off, ", ", (di.vex_L >= 1) ? ymm[di.vex_vvvv & 15] : xmm[di.vex_vvvv & 15]);
                    }
                    vex_vvvv_emitted = true;
                }
            } else { vex_vvvv_emitted = true; }
        }
    }

    if (di.desc->opcode == 0xD0 || di.desc->opcode == 0xD1) {
        off += snprintf(buf+off, buf_len-off, ", 1");
    }

    if (di.desc->mnemonic == Mnemonic::PBLENDVB || di.desc->mnemonic == Mnemonic::BLENDVPS || di.desc->mnemonic == Mnemonic::BLENDVPD) {
        off += snprintf(buf+off, buf_len-off, ", xmm0");
    }

    if (di.desc->opcode == 0x0F3A && di.immediate != 0) {
        bool has_imm = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i)
            if (di.desc->operands[i].addr == AddrMode::Immediate) has_imm = true;
        if (!has_imm) off += snprintf(buf+off, buf_len-off, ", 0x%02X", (uint8_t)di.immediate);
    }

    if (di.has_evex && di.evex_aaa != 0) {
        off += snprintf(buf+off, buf_len-off, " {k%u}", di.evex_aaa);
        if (di.evex_z) off += snprintf(buf+off, buf_len-off, " {z}");
    }

    if (di.has_evex && di.evex_b && (di.modrm >> 6) == 3) {
        const char* rc[] = {"rn-sae", "rd-sae", "ru-sae", "rz-sae"};
        off += snprintf(buf+off, buf_len-off, ", {%s}", rc[di.evex_rc & 3]);
    }

    if (di.has_evex && di.evex_b && (di.modrm >> 6) != 3) {
        // Element size: W=0 → 32-bit (float/dword), W=1 → 64-bit (double/qword)
        int elem = di.vex_W ? 8 : 4;
        int vl = (di.vex_L == 2) ? 64 : ((di.vex_L == 1) ? 32 : 16);
        int n = vl / elem;
        off += snprintf(buf+off, buf_len-off, " {1to%d}", n);
    }

    return consumed;
}

} // namespace vedx64
#endif // VEDX64_STRINGS
