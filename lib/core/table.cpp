// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski
// Auto-generated — do not edit

#include "vedx64/core.hpp"
#include <cstring>

namespace vedx64 {

#ifdef VEDX64_STRINGS
const char* mnemonic_name(Mnemonic m) {
    switch (m) {
    case Mnemonic::INVALID: return "???";
    case Mnemonic::AAA: return "aaa";
    case Mnemonic::AAD: return "aad";
    case Mnemonic::AAM: return "aam";
    case Mnemonic::AAS: return "aas";
    case Mnemonic::ADC: return "adc";
    case Mnemonic::ADCX: return "adcx";
    case Mnemonic::ADD: return "add";
    case Mnemonic::ADDPD: return "addpd";
    case Mnemonic::ADDPS: return "addps";
    case Mnemonic::ADDSD: return "addsd";
    case Mnemonic::ADDSS: return "addss";
    case Mnemonic::ADDSUBPD: return "addsubpd";
    case Mnemonic::ADDSUBPS: return "addsubps";
    case Mnemonic::ADOX: return "adox";
    case Mnemonic::ADX: return "adx";
    case Mnemonic::AESDEC: return "aesdec";
    case Mnemonic::AESDEC128KL: return "aesdec128kl";
    case Mnemonic::AESDEC256KL: return "aesdec256kl";
    case Mnemonic::AESDECLAST: return "aesdeclast";
    case Mnemonic::AESDECWIDE128KL: return "aesdecwide128kl";
    case Mnemonic::AESDECWIDE256KL: return "aesdecwide256kl";
    case Mnemonic::AESENC: return "aesenc";
    case Mnemonic::AESENC128KL: return "aesenc128kl";
    case Mnemonic::AESENC256KL: return "aesenc256kl";
    case Mnemonic::AESENCLAST: return "aesenclast";
    case Mnemonic::AESENCWIDE128KL: return "aesencwide128kl";
    case Mnemonic::AESENCWIDE256KL: return "aesencwide256kl";
    case Mnemonic::AESIMC: return "aesimc";
    case Mnemonic::AESKEYGENASSIST: return "aeskeygenassist";
    case Mnemonic::AMX: return "amx";
    case Mnemonic::AND: return "and";
    case Mnemonic::ANDN: return "andn";
    case Mnemonic::ANDNPD: return "andnpd";
    case Mnemonic::ANDNPS: return "andnps";
    case Mnemonic::ANDPD: return "andpd";
    case Mnemonic::ANDPS: return "andps";
    case Mnemonic::BEXTR: return "bextr";
    case Mnemonic::BLENDPD: return "blendpd";
    case Mnemonic::BLENDPS: return "blendps";
    case Mnemonic::BLENDVPD: return "blendvpd";
    case Mnemonic::BLENDVPS: return "blendvps";
    case Mnemonic::BLSI: return "blsi";
    case Mnemonic::BLSMSK: return "blsmsk";
    case Mnemonic::BLSR: return "blsr";
    case Mnemonic::BNDCL: return "bndcl";
    case Mnemonic::BNDCN: return "bndcn";
    case Mnemonic::BNDCU: return "bndcu";
    case Mnemonic::BNDLDX: return "bndldx";
    case Mnemonic::BNDMK: return "bndmk";
    case Mnemonic::BNDMOV: return "bndmov";
    case Mnemonic::BNDSTX: return "bndstx";
    case Mnemonic::BOUND: return "bound";
    case Mnemonic::BSF: return "bsf";
    case Mnemonic::BSR: return "bsr";
    case Mnemonic::BSWAP: return "bswap";
    case Mnemonic::BT: return "bt";
    case Mnemonic::BTC: return "btc";
    case Mnemonic::BTR: return "btr";
    case Mnemonic::BTS: return "bts";
    case Mnemonic::BZHI: return "bzhi";
    case Mnemonic::CALL: return "call";
    case Mnemonic::CALLF: return "callf";
    case Mnemonic::CBW: return "cbw";
    case Mnemonic::CDQ: return "cdq";
    case Mnemonic::CDQE: return "cdqe";
    case Mnemonic::CLAC: return "clac";
    case Mnemonic::CLC: return "clc";
    case Mnemonic::CLD: return "cld";
    case Mnemonic::CLFLUSH: return "clflush";
    case Mnemonic::CLFLUSHOPT: return "clflushopt";
    case Mnemonic::CLI: return "cli";
    case Mnemonic::CLRSSBSY: return "clrssbsy";
    case Mnemonic::CLTS: return "clts";
    case Mnemonic::CLUI: return "clui";
    case Mnemonic::CLWB: return "clwb";
    case Mnemonic::CMC: return "cmc";
    case Mnemonic::CMOVA: return "cmova";
    case Mnemonic::CMOVAE: return "cmovae";
    case Mnemonic::CMOVB: return "cmovb";
    case Mnemonic::CMOVBE: return "cmovbe";
    case Mnemonic::CMOVC: return "cmovc";
    case Mnemonic::CMOVE: return "cmove";
    case Mnemonic::CMOVG: return "cmovg";
    case Mnemonic::CMOVGE: return "cmovge";
    case Mnemonic::CMOVL: return "cmovl";
    case Mnemonic::CMOVLE: return "cmovle";
    case Mnemonic::CMOVNA: return "cmovna";
    case Mnemonic::CMOVNAE: return "cmovnae";
    case Mnemonic::CMOVNB: return "cmovnb";
    case Mnemonic::CMOVNBE: return "cmovnbe";
    case Mnemonic::CMOVNC: return "cmovnc";
    case Mnemonic::CMOVNE: return "cmovne";
    case Mnemonic::CMOVNG: return "cmovng";
    case Mnemonic::CMOVNGE: return "cmovnge";
    case Mnemonic::CMOVNL: return "cmovnl";
    case Mnemonic::CMOVNLE: return "cmovnle";
    case Mnemonic::CMOVNO: return "cmovno";
    case Mnemonic::CMOVNP: return "cmovnp";
    case Mnemonic::CMOVNS: return "cmovns";
    case Mnemonic::CMOVNZ: return "cmovnz";
    case Mnemonic::CMOVO: return "cmovo";
    case Mnemonic::CMOVP: return "cmovp";
    case Mnemonic::CMOVPE: return "cmovpe";
    case Mnemonic::CMOVPO: return "cmovpo";
    case Mnemonic::CMOVS: return "cmovs";
    case Mnemonic::CMOVZ: return "cmovz";
    case Mnemonic::CMP: return "cmp";
    case Mnemonic::CMPPD: return "cmppd";
    case Mnemonic::CMPPS: return "cmpps";
    case Mnemonic::CMPS: return "cmps";
    case Mnemonic::CMPSB: return "cmpsb";
    case Mnemonic::CMPSD: return "cmpsd";
    case Mnemonic::CMPSQ: return "cmpsq";
    case Mnemonic::CMPSS: return "cmpss";
    case Mnemonic::CMPSW: return "cmpsw";
    case Mnemonic::CMPXCHG: return "cmpxchg";
    case Mnemonic::CMPXCHG16B: return "cmpxchg16b";
    case Mnemonic::CMPXCHG8B: return "cmpxchg8b";
    case Mnemonic::COMISD: return "comisd";
    case Mnemonic::COMISS: return "comiss";
    case Mnemonic::CPUID: return "cpuid";
    case Mnemonic::CQO: return "cqo";
    case Mnemonic::CRC32: return "crc32";
    case Mnemonic::CVTDQ2PD: return "cvtdq2pd";
    case Mnemonic::CVTDQ2PS: return "cvtdq2ps";
    case Mnemonic::CVTPD2DQ: return "cvtpd2dq";
    case Mnemonic::CVTPD2PI: return "cvtpd2pi";
    case Mnemonic::CVTPD2PS: return "cvtpd2ps";
    case Mnemonic::CVTPI2PD: return "cvtpi2pd";
    case Mnemonic::CVTPI2PS: return "cvtpi2ps";
    case Mnemonic::CVTPS2DQ: return "cvtps2dq";
    case Mnemonic::CVTPS2PD: return "cvtps2pd";
    case Mnemonic::CVTPS2PI: return "cvtps2pi";
    case Mnemonic::CVTSD2SI: return "cvtsd2si";
    case Mnemonic::CVTSD2SS: return "cvtsd2ss";
    case Mnemonic::CVTSI2SD: return "cvtsi2sd";
    case Mnemonic::CVTSI2SS: return "cvtsi2ss";
    case Mnemonic::CVTSS2SD: return "cvtss2sd";
    case Mnemonic::CVTSS2SI: return "cvtss2si";
    case Mnemonic::CVTTPD2DQ: return "cvttpd2dq";
    case Mnemonic::CVTTPD2PI: return "cvttpd2pi";
    case Mnemonic::CVTTPS2DQ: return "cvttps2dq";
    case Mnemonic::CVTTPS2PI: return "cvttps2pi";
    case Mnemonic::CVTTSD2SI: return "cvttsd2si";
    case Mnemonic::CVTTSS2SI: return "cvttss2si";
    case Mnemonic::CWD: return "cwd";
    case Mnemonic::CWDE: return "cwde";
    case Mnemonic::DAA: return "daa";
    case Mnemonic::DAS: return "das";
    case Mnemonic::DEC: return "dec";
    case Mnemonic::DIV: return "div";
    case Mnemonic::DIVPD: return "divpd";
    case Mnemonic::DIVPS: return "divps";
    case Mnemonic::DIVSD: return "divsd";
    case Mnemonic::DIVSS: return "divss";
    case Mnemonic::DPPD: return "dppd";
    case Mnemonic::DPPS: return "dpps";
    case Mnemonic::EMMS: return "emms";
    case Mnemonic::ENCODEKEY128: return "encodekey128";
    case Mnemonic::ENCODEKEY256: return "encodekey256";
    case Mnemonic::ENDBR32: return "endbr32";
    case Mnemonic::ENDBR64: return "endbr64";
    case Mnemonic::ENQCMD: return "enqcmd";
    case Mnemonic::ENQCMDS: return "enqcmds";
    case Mnemonic::ENTER: return "enter";
    case Mnemonic::EXTRACTPS: return "extractps";
    case Mnemonic::EXTRQ: return "extrq";
    case Mnemonic::F2XM1: return "f2xm1";
    case Mnemonic::FABS: return "fabs";
    case Mnemonic::FADD: return "fadd";
    case Mnemonic::FADDP: return "faddp";
    case Mnemonic::FBLD: return "fbld";
    case Mnemonic::FBSTP: return "fbstp";
    case Mnemonic::FCHS: return "fchs";
    case Mnemonic::FCLEX: return "fclex";
    case Mnemonic::FCMOVB: return "fcmovb";
    case Mnemonic::FCMOVBE: return "fcmovbe";
    case Mnemonic::FCMOVE: return "fcmove";
    case Mnemonic::FCMOVNB: return "fcmovnb";
    case Mnemonic::FCMOVNBE: return "fcmovnbe";
    case Mnemonic::FCMOVNE: return "fcmovne";
    case Mnemonic::FCMOVNU: return "fcmovnu";
    case Mnemonic::FCMOVU: return "fcmovu";
    case Mnemonic::FCOM: return "fcom";
    case Mnemonic::FCOM2: return "fcom2";
    case Mnemonic::FCOMI: return "fcomi";
    case Mnemonic::FCOMIP: return "fcomip";
    case Mnemonic::FCOMP: return "fcomp";
    case Mnemonic::FCOMP3: return "fcomp3";
    case Mnemonic::FCOMP5: return "fcomp5";
    case Mnemonic::FCOMPP: return "fcompp";
    case Mnemonic::FCOS: return "fcos";
    case Mnemonic::FDECSTP: return "fdecstp";
    case Mnemonic::FDISI: return "fdisi";
    case Mnemonic::FDIV: return "fdiv";
    case Mnemonic::FDIVP: return "fdivp";
    case Mnemonic::FDIVR: return "fdivr";
    case Mnemonic::FDIVRP: return "fdivrp";
    case Mnemonic::FENI: return "feni";
    case Mnemonic::FFREE: return "ffree";
    case Mnemonic::FFREEP: return "ffreep";
    case Mnemonic::FIADD: return "fiadd";
    case Mnemonic::FICOM: return "ficom";
    case Mnemonic::FICOMP: return "ficomp";
    case Mnemonic::FIDIV: return "fidiv";
    case Mnemonic::FIDIVR: return "fidivr";
    case Mnemonic::FILD: return "fild";
    case Mnemonic::FIMUL: return "fimul";
    case Mnemonic::FINCSTP: return "fincstp";
    case Mnemonic::FINIT: return "finit";
    case Mnemonic::FIST: return "fist";
    case Mnemonic::FISTP: return "fistp";
    case Mnemonic::FISTTP: return "fisttp";
    case Mnemonic::FISUB: return "fisub";
    case Mnemonic::FISUBR: return "fisubr";
    case Mnemonic::FLD: return "fld";
    case Mnemonic::FLD1: return "fld1";
    case Mnemonic::FLDCW: return "fldcw";
    case Mnemonic::FLDENV: return "fldenv";
    case Mnemonic::FLDL2E: return "fldl2e";
    case Mnemonic::FLDL2T: return "fldl2t";
    case Mnemonic::FLDLG2: return "fldlg2";
    case Mnemonic::FLDLN2: return "fldln2";
    case Mnemonic::FLDPI: return "fldpi";
    case Mnemonic::FLDZ: return "fldz";
    case Mnemonic::FMUL: return "fmul";
    case Mnemonic::FMULP: return "fmulp";
    case Mnemonic::FNCLEX: return "fnclex";
    case Mnemonic::FNDISI: return "fndisi";
    case Mnemonic::FNENI: return "fneni";
    case Mnemonic::FNINIT: return "fninit";
    case Mnemonic::FNOP: return "fnop";
    case Mnemonic::FNSAVE: return "fnsave";
    case Mnemonic::FNSETPM: return "fnsetpm";
    case Mnemonic::FNSTCW: return "fnstcw";
    case Mnemonic::FNSTENV: return "fnstenv";
    case Mnemonic::FNSTSW: return "fnstsw";
    case Mnemonic::FPATAN: return "fpatan";
    case Mnemonic::FPREM: return "fprem";
    case Mnemonic::FPREM1: return "fprem1";
    case Mnemonic::FPTAN: return "fptan";
    case Mnemonic::FRNDINT: return "frndint";
    case Mnemonic::FRSTOR: return "frstor";
    case Mnemonic::FSAVE: return "fsave";
    case Mnemonic::FSCALE: return "fscale";
    case Mnemonic::FSETPM: return "fsetpm";
    case Mnemonic::FSIN: return "fsin";
    case Mnemonic::FSINCOS: return "fsincos";
    case Mnemonic::FSQRT: return "fsqrt";
    case Mnemonic::FST: return "fst";
    case Mnemonic::FSTCW: return "fstcw";
    case Mnemonic::FSTENV: return "fstenv";
    case Mnemonic::FSTP: return "fstp";
    case Mnemonic::FSTP1: return "fstp1";
    case Mnemonic::FSTP8: return "fstp8";
    case Mnemonic::FSTP9: return "fstp9";
    case Mnemonic::FSTSW: return "fstsw";
    case Mnemonic::FSUB: return "fsub";
    case Mnemonic::FSUBP: return "fsubp";
    case Mnemonic::FSUBR: return "fsubr";
    case Mnemonic::FSUBRP: return "fsubrp";
    case Mnemonic::FTST: return "ftst";
    case Mnemonic::FUCOM: return "fucom";
    case Mnemonic::FUCOMI: return "fucomi";
    case Mnemonic::FUCOMIP: return "fucomip";
    case Mnemonic::FUCOMP: return "fucomp";
    case Mnemonic::FUCOMPP: return "fucompp";
    case Mnemonic::FWAIT: return "fwait";
    case Mnemonic::FXAM: return "fxam";
    case Mnemonic::FXCH: return "fxch";
    case Mnemonic::FXCH4: return "fxch4";
    case Mnemonic::FXCH7: return "fxch7";
    case Mnemonic::FXRSTOR: return "fxrstor";
    case Mnemonic::FXSAVE: return "fxsave";
    case Mnemonic::FXTRACT: return "fxtract";
    case Mnemonic::FYL2X: return "fyl2x";
    case Mnemonic::FYL2XP1: return "fyl2xp1";
    case Mnemonic::GETSEC: return "getsec";
    case Mnemonic::GF2P8AFFINEINVQB: return "gf2p8affineinvqb";
    case Mnemonic::GF2P8AFFINEQB: return "gf2p8affineqb";
    case Mnemonic::GF2P8MULB: return "gf2p8mulb";
    case Mnemonic::HADDPD: return "haddpd";
    case Mnemonic::HADDPS: return "haddps";
    case Mnemonic::HINT_NOP: return "hint_nop";
    case Mnemonic::HLT: return "hlt";
    case Mnemonic::HRESET: return "hreset";
    case Mnemonic::HSUBPD: return "hsubpd";
    case Mnemonic::HSUBPS: return "hsubps";
    case Mnemonic::ICEBP: return "icebp";
    case Mnemonic::IDIV: return "idiv";
    case Mnemonic::IMUL: return "imul";
    case Mnemonic::IN: return "in";
    case Mnemonic::INC: return "inc";
    case Mnemonic::INCSSPD: return "incsspd";
    case Mnemonic::INCSSPQ: return "incsspq";
    case Mnemonic::INS: return "ins";
    case Mnemonic::INSB: return "insb";
    case Mnemonic::INSD: return "insd";
    case Mnemonic::INSERTPS: return "insertps";
    case Mnemonic::INSERTQ: return "insertq";
    case Mnemonic::INSW: return "insw";
    case Mnemonic::INT: return "int";
    case Mnemonic::INT1: return "int1";
    case Mnemonic::INTO: return "into";
    case Mnemonic::INVD: return "invd";
    case Mnemonic::INVEPT: return "invept";
    case Mnemonic::INVLPG: return "invlpg";
    case Mnemonic::INVPCID: return "invpcid";
    case Mnemonic::INVVPID: return "invvpid";
    case Mnemonic::IRET: return "iret";
    case Mnemonic::IRETD: return "iretd";
    case Mnemonic::IRETQ: return "iretq";
    case Mnemonic::JA: return "ja";
    case Mnemonic::JAE: return "jae";
    case Mnemonic::JB: return "jb";
    case Mnemonic::JBE: return "jbe";
    case Mnemonic::JC: return "jc";
    case Mnemonic::JCXZ: return "jcxz";
    case Mnemonic::JE: return "je";
    case Mnemonic::JECXZ: return "jecxz";
    case Mnemonic::JG: return "jg";
    case Mnemonic::JGE: return "jge";
    case Mnemonic::JL: return "jl";
    case Mnemonic::JLE: return "jle";
    case Mnemonic::JMP: return "jmp";
    case Mnemonic::JMPE: return "jmpe";
    case Mnemonic::JMPF: return "jmpf";
    case Mnemonic::JNA: return "jna";
    case Mnemonic::JNAE: return "jnae";
    case Mnemonic::JNB: return "jnb";
    case Mnemonic::JNBE: return "jnbe";
    case Mnemonic::JNC: return "jnc";
    case Mnemonic::JNE: return "jne";
    case Mnemonic::JNG: return "jng";
    case Mnemonic::JNGE: return "jnge";
    case Mnemonic::JNL: return "jnl";
    case Mnemonic::JNLE: return "jnle";
    case Mnemonic::JNO: return "jno";
    case Mnemonic::JNP: return "jnp";
    case Mnemonic::JNS: return "jns";
    case Mnemonic::JNZ: return "jnz";
    case Mnemonic::JO: return "jo";
    case Mnemonic::JP: return "jp";
    case Mnemonic::JPE: return "jpe";
    case Mnemonic::JPO: return "jpo";
    case Mnemonic::JRCXZ: return "jrcxz";
    case Mnemonic::JS: return "js";
    case Mnemonic::JZ: return "jz";
    case Mnemonic::KADDB: return "kaddb";
    case Mnemonic::KADDD: return "kaddd";
    case Mnemonic::KADDQ: return "kaddq";
    case Mnemonic::KADDW: return "kaddw";
    case Mnemonic::KANDB: return "kandb";
    case Mnemonic::KANDD: return "kandd";
    case Mnemonic::KANDNB: return "kandnb";
    case Mnemonic::KANDND: return "kandnd";
    case Mnemonic::KANDNQ: return "kandnq";
    case Mnemonic::KANDNW: return "kandnw";
    case Mnemonic::KANDQ: return "kandq";
    case Mnemonic::KANDW: return "kandw";
    case Mnemonic::KMOVB: return "kmovb";
    case Mnemonic::KMOVD: return "kmovd";
    case Mnemonic::KMOVQ: return "kmovq";
    case Mnemonic::KMOVW: return "kmovw";
    case Mnemonic::KNOTB: return "knotb";
    case Mnemonic::KNOTD: return "knotd";
    case Mnemonic::KNOTQ: return "knotq";
    case Mnemonic::KNOTW: return "knotw";
    case Mnemonic::KORB: return "korb";
    case Mnemonic::KORD: return "kord";
    case Mnemonic::KORQ: return "korq";
    case Mnemonic::KORTESTB: return "kortestb";
    case Mnemonic::KORTESTD: return "kortestd";
    case Mnemonic::KORTESTQ: return "kortestq";
    case Mnemonic::KORTESTW: return "kortestw";
    case Mnemonic::KORW: return "korw";
    case Mnemonic::KSHIFTLB: return "kshiftlb";
    case Mnemonic::KSHIFTLD: return "kshiftld";
    case Mnemonic::KSHIFTLQ: return "kshiftlq";
    case Mnemonic::KSHIFTLW: return "kshiftlw";
    case Mnemonic::KSHIFTRB: return "kshiftrb";
    case Mnemonic::KSHIFTRD: return "kshiftrd";
    case Mnemonic::KSHIFTRQ: return "kshiftrq";
    case Mnemonic::KSHIFTRW: return "kshiftrw";
    case Mnemonic::KTESTB: return "ktestb";
    case Mnemonic::KTESTD: return "ktestd";
    case Mnemonic::KTESTQ: return "ktestq";
    case Mnemonic::KTESTW: return "ktestw";
    case Mnemonic::KUNPCKBW: return "kunpckbw";
    case Mnemonic::KUNPCKDQ: return "kunpckdq";
    case Mnemonic::KUNPCKWD: return "kunpckwd";
    case Mnemonic::KXNORB: return "kxnorb";
    case Mnemonic::KXNORD: return "kxnord";
    case Mnemonic::KXNORQ: return "kxnorq";
    case Mnemonic::KXNORW: return "kxnorw";
    case Mnemonic::KXORB: return "kxorb";
    case Mnemonic::KXORD: return "kxord";
    case Mnemonic::KXORQ: return "kxorq";
    case Mnemonic::KXORW: return "kxorw";
    case Mnemonic::LAHF: return "lahf";
    case Mnemonic::LAR: return "lar";
    case Mnemonic::LDDQU: return "lddqu";
    case Mnemonic::LDMXCSR: return "ldmxcsr";
    case Mnemonic::LDS: return "lds";
    case Mnemonic::LDTILECFG: return "ldtilecfg";
    case Mnemonic::LEA: return "lea";
    case Mnemonic::LEAVE: return "leave";
    case Mnemonic::LES: return "les";
    case Mnemonic::LFENCE: return "lfence";
    case Mnemonic::LFS: return "lfs";
    case Mnemonic::LGDT: return "lgdt";
    case Mnemonic::LGS: return "lgs";
    case Mnemonic::LIDT: return "lidt";
    case Mnemonic::LLDT: return "lldt";
    case Mnemonic::LMSW: return "lmsw";
    case Mnemonic::LODS: return "lods";
    case Mnemonic::LODSB: return "lodsb";
    case Mnemonic::LODSD: return "lodsd";
    case Mnemonic::LODSQ: return "lodsq";
    case Mnemonic::LODSW: return "lodsw";
    case Mnemonic::LOOP: return "loop";
    case Mnemonic::LOOPE: return "loope";
    case Mnemonic::LOOPNE: return "loopne";
    case Mnemonic::LOOPNZ: return "loopnz";
    case Mnemonic::LOOPZ: return "loopz";
    case Mnemonic::LSL: return "lsl";
    case Mnemonic::LSS: return "lss";
    case Mnemonic::LTR: return "ltr";
    case Mnemonic::LZCNT: return "lzcnt";
    case Mnemonic::MASKMOVDQU: return "maskmovdqu";
    case Mnemonic::MASKMOVQ: return "maskmovq";
    case Mnemonic::MAXPD: return "maxpd";
    case Mnemonic::MAXPS: return "maxps";
    case Mnemonic::MAXSD: return "maxsd";
    case Mnemonic::MAXSS: return "maxss";
    case Mnemonic::MFENCE: return "mfence";
    case Mnemonic::MINPD: return "minpd";
    case Mnemonic::MINPS: return "minps";
    case Mnemonic::MINSD: return "minsd";
    case Mnemonic::MINSS: return "minss";
    case Mnemonic::MONITOR: return "monitor";
    case Mnemonic::MOV: return "mov";
    case Mnemonic::MOVAPD: return "movapd";
    case Mnemonic::MOVAPS: return "movaps";
    case Mnemonic::MOVBE: return "movbe";
    case Mnemonic::MOVD: return "movd";
    case Mnemonic::MOVDDUP: return "movddup";
    case Mnemonic::MOVDIR64B: return "movdir64b";
    case Mnemonic::MOVDIRI: return "movdiri";
    case Mnemonic::MOVDQ2Q: return "movdq2q";
    case Mnemonic::MOVDQA: return "movdqa";
    case Mnemonic::MOVDQU: return "movdqu";
    case Mnemonic::MOVHLPS: return "movhlps";
    case Mnemonic::MOVHPD: return "movhpd";
    case Mnemonic::MOVHPS: return "movhps";
    case Mnemonic::MOVLHPS: return "movlhps";
    case Mnemonic::MOVLPD: return "movlpd";
    case Mnemonic::MOVLPS: return "movlps";
    case Mnemonic::MOVMSKPD: return "movmskpd";
    case Mnemonic::MOVMSKPS: return "movmskps";
    case Mnemonic::MOVNTDQ: return "movntdq";
    case Mnemonic::MOVNTDQA: return "movntdqa";
    case Mnemonic::MOVNTI: return "movnti";
    case Mnemonic::MOVNTPD: return "movntpd";
    case Mnemonic::MOVNTPS: return "movntps";
    case Mnemonic::MOVNTQ: return "movntq";
    case Mnemonic::MOVQ: return "movq";
    case Mnemonic::MOVQ2DQ: return "movq2dq";
    case Mnemonic::MOVS: return "movs";
    case Mnemonic::MOVSB: return "movsb";
    case Mnemonic::MOVSD: return "movsd";
    case Mnemonic::MOVSHDUP: return "movshdup";
    case Mnemonic::MOVSLDUP: return "movsldup";
    case Mnemonic::MOVSQ: return "movsq";
    case Mnemonic::MOVSS: return "movss";
    case Mnemonic::MOVSW: return "movsw";
    case Mnemonic::MOVSX: return "movsx";
    case Mnemonic::MOVSXD: return "movsxd";
    case Mnemonic::MOVUPD: return "movupd";
    case Mnemonic::MOVUPS: return "movups";
    case Mnemonic::MOVZX: return "movzx";
    case Mnemonic::MPSADBW: return "mpsadbw";
    case Mnemonic::MUL: return "mul";
    case Mnemonic::MULPD: return "mulpd";
    case Mnemonic::MULPS: return "mulps";
    case Mnemonic::MULSD: return "mulsd";
    case Mnemonic::MULSS: return "mulss";
    case Mnemonic::MULX: return "mulx";
    case Mnemonic::MWAIT: return "mwait";
    case Mnemonic::NEG: return "neg";
    case Mnemonic::NOP: return "nop";
    case Mnemonic::NOT: return "not";
    case Mnemonic::OR: return "or";
    case Mnemonic::ORPD: return "orpd";
    case Mnemonic::ORPS: return "orps";
    case Mnemonic::OUT: return "out";
    case Mnemonic::OUTS: return "outs";
    case Mnemonic::OUTSB: return "outsb";
    case Mnemonic::OUTSD: return "outsd";
    case Mnemonic::OUTSW: return "outsw";
    case Mnemonic::PABSB: return "pabsb";
    case Mnemonic::PABSD: return "pabsd";
    case Mnemonic::PABSW: return "pabsw";
    case Mnemonic::PACKSSDW: return "packssdw";
    case Mnemonic::PACKSSWB: return "packsswb";
    case Mnemonic::PACKUSDW: return "packusdw";
    case Mnemonic::PACKUSWB: return "packuswb";
    case Mnemonic::PADDB: return "paddb";
    case Mnemonic::PADDD: return "paddd";
    case Mnemonic::PADDQ: return "paddq";
    case Mnemonic::PADDSB: return "paddsb";
    case Mnemonic::PADDSW: return "paddsw";
    case Mnemonic::PADDUSB: return "paddusb";
    case Mnemonic::PADDUSW: return "paddusw";
    case Mnemonic::PADDW: return "paddw";
    case Mnemonic::PALIGNR: return "palignr";
    case Mnemonic::PAND: return "pand";
    case Mnemonic::PANDN: return "pandn";
    case Mnemonic::PAUSE: return "pause";
    case Mnemonic::PAVGB: return "pavgb";
    case Mnemonic::PAVGW: return "pavgw";
    case Mnemonic::PBLENDVB: return "pblendvb";
    case Mnemonic::PBLENDW: return "pblendw";
    case Mnemonic::PCLMULQDQ: return "pclmulqdq";
    case Mnemonic::PCMPEQB: return "pcmpeqb";
    case Mnemonic::PCMPEQD: return "pcmpeqd";
    case Mnemonic::PCMPEQQ: return "pcmpeqq";
    case Mnemonic::PCMPEQW: return "pcmpeqw";
    case Mnemonic::PCMPESTRI: return "pcmpestri";
    case Mnemonic::PCMPESTRM: return "pcmpestrm";
    case Mnemonic::PCMPGTB: return "pcmpgtb";
    case Mnemonic::PCMPGTD: return "pcmpgtd";
    case Mnemonic::PCMPGTQ: return "pcmpgtq";
    case Mnemonic::PCMPGTW: return "pcmpgtw";
    case Mnemonic::PCMPISTRI: return "pcmpistri";
    case Mnemonic::PCMPISTRM: return "pcmpistrm";
    case Mnemonic::PCONFIG: return "pconfig";
    case Mnemonic::PDEP: return "pdep";
    case Mnemonic::PEXT: return "pext";
    case Mnemonic::PEXTRB: return "pextrb";
    case Mnemonic::PEXTRD: return "pextrd";
    case Mnemonic::PEXTRQ: return "pextrq";
    case Mnemonic::PEXTRW: return "pextrw";
    case Mnemonic::PHADDD: return "phaddd";
    case Mnemonic::PHADDSW: return "phaddsw";
    case Mnemonic::PHADDW: return "phaddw";
    case Mnemonic::PHMINPOSUW: return "phminposuw";
    case Mnemonic::PHSUBD: return "phsubd";
    case Mnemonic::PHSUBSW: return "phsubsw";
    case Mnemonic::PHSUBW: return "phsubw";
    case Mnemonic::PINSRB: return "pinsrb";
    case Mnemonic::PINSRD: return "pinsrd";
    case Mnemonic::PINSRQ: return "pinsrq";
    case Mnemonic::PINSRW: return "pinsrw";
    case Mnemonic::PMADDUBSW: return "pmaddubsw";
    case Mnemonic::PMADDWD: return "pmaddwd";
    case Mnemonic::PMAXSB: return "pmaxsb";
    case Mnemonic::PMAXSD: return "pmaxsd";
    case Mnemonic::PMAXSW: return "pmaxsw";
    case Mnemonic::PMAXUB: return "pmaxub";
    case Mnemonic::PMAXUD: return "pmaxud";
    case Mnemonic::PMAXUW: return "pmaxuw";
    case Mnemonic::PMINSB: return "pminsb";
    case Mnemonic::PMINSD: return "pminsd";
    case Mnemonic::PMINSW: return "pminsw";
    case Mnemonic::PMINUB: return "pminub";
    case Mnemonic::PMINUD: return "pminud";
    case Mnemonic::PMINUW: return "pminuw";
    case Mnemonic::PMOVMSKB: return "pmovmskb";
    case Mnemonic::PMOVSXBD: return "pmovsxbd";
    case Mnemonic::PMOVSXBQ: return "pmovsxbq";
    case Mnemonic::PMOVSXBW: return "pmovsxbw";
    case Mnemonic::PMOVSXDQ: return "pmovsxdq";
    case Mnemonic::PMOVSXWD: return "pmovsxwd";
    case Mnemonic::PMOVSXWQ: return "pmovsxwq";
    case Mnemonic::PMOVZXBD: return "pmovzxbd";
    case Mnemonic::PMOVZXBQ: return "pmovzxbq";
    case Mnemonic::PMOVZXBW: return "pmovzxbw";
    case Mnemonic::PMOVZXDQ: return "pmovzxdq";
    case Mnemonic::PMOVZXWD: return "pmovzxwd";
    case Mnemonic::PMOVZXWQ: return "pmovzxwq";
    case Mnemonic::PMULDQ: return "pmuldq";
    case Mnemonic::PMULHRSW: return "pmulhrsw";
    case Mnemonic::PMULHUW: return "pmulhuw";
    case Mnemonic::PMULHW: return "pmulhw";
    case Mnemonic::PMULLD: return "pmulld";
    case Mnemonic::PMULLW: return "pmullw";
    case Mnemonic::PMULUDQ: return "pmuludq";
    case Mnemonic::POP: return "pop";
    case Mnemonic::POPA: return "popa";
    case Mnemonic::POPAD: return "popad";
    case Mnemonic::POPCNT: return "popcnt";
    case Mnemonic::POPF: return "popf";
    case Mnemonic::POPFD: return "popfd";
    case Mnemonic::POPFQ: return "popfq";
    case Mnemonic::POR: return "por";
    case Mnemonic::PREFETCHNTA: return "prefetchnta";
    case Mnemonic::PREFETCHT0: return "prefetcht0";
    case Mnemonic::PREFETCHT1: return "prefetcht1";
    case Mnemonic::PREFETCHT2: return "prefetcht2";
    case Mnemonic::PREFETCHW: return "prefetchw";
    case Mnemonic::PSADBW: return "psadbw";
    case Mnemonic::PSHUFB: return "pshufb";
    case Mnemonic::PSHUFD: return "pshufd";
    case Mnemonic::PSHUFHW: return "pshufhw";
    case Mnemonic::PSHUFLW: return "pshuflw";
    case Mnemonic::PSHUFW: return "pshufw";
    case Mnemonic::PSIGNB: return "psignb";
    case Mnemonic::PSIGND: return "psignd";
    case Mnemonic::PSIGNW: return "psignw";
    case Mnemonic::PSLLD: return "pslld";
    case Mnemonic::PSLLDQ: return "pslldq";
    case Mnemonic::PSLLQ: return "psllq";
    case Mnemonic::PSLLW: return "psllw";
    case Mnemonic::PSRAD: return "psrad";
    case Mnemonic::PSRAW: return "psraw";
    case Mnemonic::PSRLD: return "psrld";
    case Mnemonic::PSRLDQ: return "psrldq";
    case Mnemonic::PSRLQ: return "psrlq";
    case Mnemonic::PSRLW: return "psrlw";
    case Mnemonic::PSUBB: return "psubb";
    case Mnemonic::PSUBD: return "psubd";
    case Mnemonic::PSUBQ: return "psubq";
    case Mnemonic::PSUBSB: return "psubsb";
    case Mnemonic::PSUBSW: return "psubsw";
    case Mnemonic::PSUBUSB: return "psubusb";
    case Mnemonic::PSUBUSW: return "psubusw";
    case Mnemonic::PSUBW: return "psubw";
    case Mnemonic::PTEST: return "ptest";
    case Mnemonic::PTWRITE: return "ptwrite";
    case Mnemonic::PUNPCKHBW: return "punpckhbw";
    case Mnemonic::PUNPCKHDQ: return "punpckhdq";
    case Mnemonic::PUNPCKHQDQ: return "punpckhqdq";
    case Mnemonic::PUNPCKHWD: return "punpckhwd";
    case Mnemonic::PUNPCKLBW: return "punpcklbw";
    case Mnemonic::PUNPCKLDQ: return "punpckldq";
    case Mnemonic::PUNPCKLQDQ: return "punpcklqdq";
    case Mnemonic::PUNPCKLWD: return "punpcklwd";
    case Mnemonic::PUSH: return "push";
    case Mnemonic::PUSHA: return "pusha";
    case Mnemonic::PUSHAD: return "pushad";
    case Mnemonic::PUSHF: return "pushf";
    case Mnemonic::PUSHFD: return "pushfd";
    case Mnemonic::PUSHFQ: return "pushfq";
    case Mnemonic::PXOR: return "pxor";
    case Mnemonic::RCL: return "rcl";
    case Mnemonic::RCPPS: return "rcpps";
    case Mnemonic::RCPSS: return "rcpss";
    case Mnemonic::RCR: return "rcr";
    case Mnemonic::RDFSBASE: return "rdfsbase";
    case Mnemonic::RDGSBASE: return "rdgsbase";
    case Mnemonic::RDMSR: return "rdmsr";
    case Mnemonic::RDPID: return "rdpid";
    case Mnemonic::RDPKRU: return "rdpkru";
    case Mnemonic::RDPMC: return "rdpmc";
    case Mnemonic::RDRAND: return "rdrand";
    case Mnemonic::RDSEED: return "rdseed";
    case Mnemonic::RDSSPD: return "rdsspd";
    case Mnemonic::RDSSPQ: return "rdsspq";
    case Mnemonic::RDTSC: return "rdtsc";
    case Mnemonic::RDTSCP: return "rdtscp";
    case Mnemonic::RETF: return "retf";
    case Mnemonic::RETN: return "retn";
    case Mnemonic::ROL: return "rol";
    case Mnemonic::ROR: return "ror";
    case Mnemonic::RORX: return "rorx";
    case Mnemonic::ROUNDPD: return "roundpd";
    case Mnemonic::ROUNDPS: return "roundps";
    case Mnemonic::ROUNDSD: return "roundsd";
    case Mnemonic::ROUNDSS: return "roundss";
    case Mnemonic::RSM: return "rsm";
    case Mnemonic::RSQRTPS: return "rsqrtps";
    case Mnemonic::RSQRTSS: return "rsqrtss";
    case Mnemonic::RSTORSSP: return "rstorssp";
    case Mnemonic::SAHF: return "sahf";
    case Mnemonic::SAL: return "sal";
    case Mnemonic::SALC: return "salc";
    case Mnemonic::SAR: return "sar";
    case Mnemonic::SARX: return "sarx";
    case Mnemonic::SAVEPREVSSP: return "saveprevssp";
    case Mnemonic::SBB: return "sbb";
    case Mnemonic::SCAS: return "scas";
    case Mnemonic::SCASB: return "scasb";
    case Mnemonic::SCASD: return "scasd";
    case Mnemonic::SCASQ: return "scasq";
    case Mnemonic::SCASW: return "scasw";
    case Mnemonic::SENDUIPI: return "senduipi";
    case Mnemonic::SERIALIZE: return "serialize";
    case Mnemonic::SETA: return "seta";
    case Mnemonic::SETAE: return "setae";
    case Mnemonic::SETALC: return "setalc";
    case Mnemonic::SETB: return "setb";
    case Mnemonic::SETBE: return "setbe";
    case Mnemonic::SETC: return "setc";
    case Mnemonic::SETE: return "sete";
    case Mnemonic::SETG: return "setg";
    case Mnemonic::SETGE: return "setge";
    case Mnemonic::SETL: return "setl";
    case Mnemonic::SETLE: return "setle";
    case Mnemonic::SETNA: return "setna";
    case Mnemonic::SETNAE: return "setnae";
    case Mnemonic::SETNB: return "setnb";
    case Mnemonic::SETNBE: return "setnbe";
    case Mnemonic::SETNC: return "setnc";
    case Mnemonic::SETNE: return "setne";
    case Mnemonic::SETNG: return "setng";
    case Mnemonic::SETNGE: return "setnge";
    case Mnemonic::SETNL: return "setnl";
    case Mnemonic::SETNLE: return "setnle";
    case Mnemonic::SETNO: return "setno";
    case Mnemonic::SETNP: return "setnp";
    case Mnemonic::SETNS: return "setns";
    case Mnemonic::SETNZ: return "setnz";
    case Mnemonic::SETO: return "seto";
    case Mnemonic::SETP: return "setp";
    case Mnemonic::SETPE: return "setpe";
    case Mnemonic::SETPO: return "setpo";
    case Mnemonic::SETS: return "sets";
    case Mnemonic::SETSSBSY: return "setssbsy";
    case Mnemonic::SETZ: return "setz";
    case Mnemonic::SFENCE: return "sfence";
    case Mnemonic::SGDT: return "sgdt";
    case Mnemonic::SHA1MSG1: return "sha1msg1";
    case Mnemonic::SHA1MSG2: return "sha1msg2";
    case Mnemonic::SHA1NEXTE: return "sha1nexte";
    case Mnemonic::SHA1RNDS4: return "sha1rnds4";
    case Mnemonic::SHA256MSG1: return "sha256msg1";
    case Mnemonic::SHA256MSG2: return "sha256msg2";
    case Mnemonic::SHA256RNDS2: return "sha256rnds2";
    case Mnemonic::SHL: return "shl";
    case Mnemonic::SHLD: return "shld";
    case Mnemonic::SHLX: return "shlx";
    case Mnemonic::SHR: return "shr";
    case Mnemonic::SHRD: return "shrd";
    case Mnemonic::SHRX: return "shrx";
    case Mnemonic::SHUFPD: return "shufpd";
    case Mnemonic::SHUFPS: return "shufps";
    case Mnemonic::SIDT: return "sidt";
    case Mnemonic::SLDT: return "sldt";
    case Mnemonic::SMSW: return "smsw";
    case Mnemonic::SQRTPD: return "sqrtpd";
    case Mnemonic::SQRTPS: return "sqrtps";
    case Mnemonic::SQRTSD: return "sqrtsd";
    case Mnemonic::SQRTSS: return "sqrtss";
    case Mnemonic::STAC: return "stac";
    case Mnemonic::STC: return "stc";
    case Mnemonic::STD: return "std";
    case Mnemonic::STI: return "sti";
    case Mnemonic::STMXCSR: return "stmxcsr";
    case Mnemonic::STOS: return "stos";
    case Mnemonic::STOSB: return "stosb";
    case Mnemonic::STOSD: return "stosd";
    case Mnemonic::STOSQ: return "stosq";
    case Mnemonic::STOSW: return "stosw";
    case Mnemonic::STR: return "str";
    case Mnemonic::STTILECFG: return "sttilecfg";
    case Mnemonic::STUI: return "stui";
    case Mnemonic::SUB: return "sub";
    case Mnemonic::SUBPD: return "subpd";
    case Mnemonic::SUBPS: return "subps";
    case Mnemonic::SUBSD: return "subsd";
    case Mnemonic::SUBSS: return "subss";
    case Mnemonic::SWAPGS: return "swapgs";
    case Mnemonic::SYSCALL: return "syscall";
    case Mnemonic::SYSENTER: return "sysenter";
    case Mnemonic::SYSEXIT: return "sysexit";
    case Mnemonic::SYSRET: return "sysret";
    case Mnemonic::TDPBF16PS: return "tdpbf16ps";
    case Mnemonic::TDPBSSD: return "tdpbssd";
    case Mnemonic::TDPBSUD: return "tdpbsud";
    case Mnemonic::TDPBUSD: return "tdpbusd";
    case Mnemonic::TDPBUUD: return "tdpbuud";
    case Mnemonic::TDPFP16PS: return "tdpfp16ps";
    case Mnemonic::TEST: return "test";
    case Mnemonic::TESTUI: return "testui";
    case Mnemonic::TILELOADD: return "tileloadd";
    case Mnemonic::TILELOADDT1: return "tileloaddt1";
    case Mnemonic::TILERELEASE: return "tilerelease";
    case Mnemonic::TILESTORED: return "tilestored";
    case Mnemonic::TILEZERO: return "tilezero";
    case Mnemonic::TPAUSE: return "tpause";
    case Mnemonic::TZCNT: return "tzcnt";
    case Mnemonic::UCOMISD: return "ucomisd";
    case Mnemonic::UCOMISS: return "ucomiss";
    case Mnemonic::UD: return "ud";
    case Mnemonic::UD2: return "ud2";
    case Mnemonic::UIRET: return "uiret";
    case Mnemonic::UMONITOR: return "umonitor";
    case Mnemonic::UMWAIT: return "umwait";
    case Mnemonic::UNPCKHPD: return "unpckhpd";
    case Mnemonic::UNPCKHPS: return "unpckhps";
    case Mnemonic::UNPCKLPD: return "unpcklpd";
    case Mnemonic::UNPCKLPS: return "unpcklps";
    case Mnemonic::VBLENDMPD: return "vblendmpd";
    case Mnemonic::VBLENDMPS: return "vblendmps";
    case Mnemonic::VBROADCASTF128: return "vbroadcastf128";
    case Mnemonic::VBROADCASTF32X2: return "vbroadcastf32x2";
    case Mnemonic::VBROADCASTF32X4: return "vbroadcastf32x4";
    case Mnemonic::VBROADCASTF32X8: return "vbroadcastf32x8";
    case Mnemonic::VBROADCASTF64X2: return "vbroadcastf64x2";
    case Mnemonic::VBROADCASTF64X4: return "vbroadcastf64x4";
    case Mnemonic::VBROADCASTI32X2: return "vbroadcasti32x2";
    case Mnemonic::VBROADCASTI32X4: return "vbroadcasti32x4";
    case Mnemonic::VBROADCASTI32X8: return "vbroadcasti32x8";
    case Mnemonic::VBROADCASTI64X2: return "vbroadcasti64x2";
    case Mnemonic::VBROADCASTI64X4: return "vbroadcasti64x4";
    case Mnemonic::VBROADCASTSD: return "vbroadcastsd";
    case Mnemonic::VBROADCASTSS: return "vbroadcastss";
    case Mnemonic::VCOMPRESSPD: return "vcompresspd";
    case Mnemonic::VCOMPRESSPS: return "vcompressps";
    case Mnemonic::VCVTNEEBF162PS: return "vcvtneebf162ps";
    case Mnemonic::VCVTNEEPH2PS: return "vcvtneeph2ps";
    case Mnemonic::VCVTNEOBF162PS: return "vcvtneobf162ps";
    case Mnemonic::VCVTNEOPH2PS: return "vcvtneoph2ps";
    case Mnemonic::VCVTNEPS2BF16: return "vcvtneps2bf16";
    case Mnemonic::VCVTPD2QQ: return "vcvtpd2qq";
    case Mnemonic::VCVTPD2UDQ: return "vcvtpd2udq";
    case Mnemonic::VCVTPD2UQQ: return "vcvtpd2uqq";
    case Mnemonic::VCVTPS2QQ: return "vcvtps2qq";
    case Mnemonic::VCVTPS2UDQ: return "vcvtps2udq";
    case Mnemonic::VCVTPS2UQQ: return "vcvtps2uqq";
    case Mnemonic::VCVTQQ2PD: return "vcvtqq2pd";
    case Mnemonic::VCVTQQ2PS: return "vcvtqq2ps";
    case Mnemonic::VCVTSD2USI: return "vcvtsd2usi";
    case Mnemonic::VCVTSS2USI: return "vcvtss2usi";
    case Mnemonic::VCVTTPD2QQ: return "vcvttpd2qq";
    case Mnemonic::VCVTTPD2UDQ: return "vcvttpd2udq";
    case Mnemonic::VCVTTPD2UQQ: return "vcvttpd2uqq";
    case Mnemonic::VCVTTPS2QQ: return "vcvttps2qq";
    case Mnemonic::VCVTTPS2UDQ: return "vcvttps2udq";
    case Mnemonic::VCVTTPS2UQQ: return "vcvttps2uqq";
    case Mnemonic::VCVTTSD2USI: return "vcvttsd2usi";
    case Mnemonic::VCVTTSS2USI: return "vcvttss2usi";
    case Mnemonic::VCVTUDQ2PD: return "vcvtudq2pd";
    case Mnemonic::VCVTUDQ2PS: return "vcvtudq2ps";
    case Mnemonic::VCVTUQQ2PD: return "vcvtuqq2pd";
    case Mnemonic::VCVTUQQ2PS: return "vcvtuqq2ps";
    case Mnemonic::VCVTUSI2SD: return "vcvtusi2sd";
    case Mnemonic::VCVTUSI2SS: return "vcvtusi2ss";
    case Mnemonic::VDBPSADBW: return "vdbpsadbw";
    case Mnemonic::VERR: return "verr";
    case Mnemonic::VERW: return "verw";
    case Mnemonic::VEXPANDPD: return "vexpandpd";
    case Mnemonic::VEXPANDPS: return "vexpandps";
    case Mnemonic::VEXTRACTF128: return "vextractf128";
    case Mnemonic::VEXTRACTF32X4: return "vextractf32x4";
    case Mnemonic::VEXTRACTF32X8: return "vextractf32x8";
    case Mnemonic::VEXTRACTF64X2: return "vextractf64x2";
    case Mnemonic::VEXTRACTF64X4: return "vextractf64x4";
    case Mnemonic::VEXTRACTI128: return "vextracti128";
    case Mnemonic::VEXTRACTI32X4: return "vextracti32x4";
    case Mnemonic::VEXTRACTI32X8: return "vextracti32x8";
    case Mnemonic::VEXTRACTI64X2: return "vextracti64x2";
    case Mnemonic::VEXTRACTI64X4: return "vextracti64x4";
    case Mnemonic::VFIXUPIMMPD: return "vfixupimmpd";
    case Mnemonic::VFIXUPIMMPS: return "vfixupimmps";
    case Mnemonic::VFIXUPIMMSD: return "vfixupimmsd";
    case Mnemonic::VFIXUPIMMSS: return "vfixupimmss";
    case Mnemonic::VFMADD132PD: return "vfmadd132pd";
    case Mnemonic::VFMADD132PS: return "vfmadd132ps";
    case Mnemonic::VFMADD132SD: return "vfmadd132sd";
    case Mnemonic::VFMADD132SS: return "vfmadd132ss";
    case Mnemonic::VFMADD213PD: return "vfmadd213pd";
    case Mnemonic::VFMADD213PS: return "vfmadd213ps";
    case Mnemonic::VFMADD213SD: return "vfmadd213sd";
    case Mnemonic::VFMADD213SS: return "vfmadd213ss";
    case Mnemonic::VFMADD231PD: return "vfmadd231pd";
    case Mnemonic::VFMADD231PS: return "vfmadd231ps";
    case Mnemonic::VFMADD231SD: return "vfmadd231sd";
    case Mnemonic::VFMADD231SS: return "vfmadd231ss";
    case Mnemonic::VFMADDSUB132PD: return "vfmaddsub132pd";
    case Mnemonic::VFMADDSUB132PS: return "vfmaddsub132ps";
    case Mnemonic::VFMADDSUB213PD: return "vfmaddsub213pd";
    case Mnemonic::VFMADDSUB213PS: return "vfmaddsub213ps";
    case Mnemonic::VFMADDSUB231PD: return "vfmaddsub231pd";
    case Mnemonic::VFMADDSUB231PS: return "vfmaddsub231ps";
    case Mnemonic::VFMSUB132PD: return "vfmsub132pd";
    case Mnemonic::VFMSUB132PS: return "vfmsub132ps";
    case Mnemonic::VFMSUB132SD: return "vfmsub132sd";
    case Mnemonic::VFMSUB132SS: return "vfmsub132ss";
    case Mnemonic::VFMSUB213PD: return "vfmsub213pd";
    case Mnemonic::VFMSUB213PS: return "vfmsub213ps";
    case Mnemonic::VFMSUB213SD: return "vfmsub213sd";
    case Mnemonic::VFMSUB213SS: return "vfmsub213ss";
    case Mnemonic::VFMSUB231PD: return "vfmsub231pd";
    case Mnemonic::VFMSUB231PS: return "vfmsub231ps";
    case Mnemonic::VFMSUB231SD: return "vfmsub231sd";
    case Mnemonic::VFMSUB231SS: return "vfmsub231ss";
    case Mnemonic::VFMSUBADD132PD: return "vfmsubadd132pd";
    case Mnemonic::VFMSUBADD132PS: return "vfmsubadd132ps";
    case Mnemonic::VFMSUBADD213PD: return "vfmsubadd213pd";
    case Mnemonic::VFMSUBADD213PS: return "vfmsubadd213ps";
    case Mnemonic::VFMSUBADD231PD: return "vfmsubadd231pd";
    case Mnemonic::VFMSUBADD231PS: return "vfmsubadd231ps";
    case Mnemonic::VFNMADD132PD: return "vfnmadd132pd";
    case Mnemonic::VFNMADD132PS: return "vfnmadd132ps";
    case Mnemonic::VFNMADD132SD: return "vfnmadd132sd";
    case Mnemonic::VFNMADD132SS: return "vfnmadd132ss";
    case Mnemonic::VFNMADD213PD: return "vfnmadd213pd";
    case Mnemonic::VFNMADD213PS: return "vfnmadd213ps";
    case Mnemonic::VFNMADD213SD: return "vfnmadd213sd";
    case Mnemonic::VFNMADD213SS: return "vfnmadd213ss";
    case Mnemonic::VFNMADD231PD: return "vfnmadd231pd";
    case Mnemonic::VFNMADD231PS: return "vfnmadd231ps";
    case Mnemonic::VFNMADD231SD: return "vfnmadd231sd";
    case Mnemonic::VFNMADD231SS: return "vfnmadd231ss";
    case Mnemonic::VFNMSUB132PD: return "vfnmsub132pd";
    case Mnemonic::VFNMSUB132PS: return "vfnmsub132ps";
    case Mnemonic::VFNMSUB132SD: return "vfnmsub132sd";
    case Mnemonic::VFNMSUB132SS: return "vfnmsub132ss";
    case Mnemonic::VFNMSUB213PD: return "vfnmsub213pd";
    case Mnemonic::VFNMSUB213PS: return "vfnmsub213ps";
    case Mnemonic::VFNMSUB213SD: return "vfnmsub213sd";
    case Mnemonic::VFNMSUB213SS: return "vfnmsub213ss";
    case Mnemonic::VFNMSUB231PD: return "vfnmsub231pd";
    case Mnemonic::VFNMSUB231PS: return "vfnmsub231ps";
    case Mnemonic::VFNMSUB231SD: return "vfnmsub231sd";
    case Mnemonic::VFNMSUB231SS: return "vfnmsub231ss";
    case Mnemonic::VFPCLASSPD: return "vfpclasspd";
    case Mnemonic::VFPCLASSPS: return "vfpclassps";
    case Mnemonic::VFPCLASSSD: return "vfpclasssd";
    case Mnemonic::VFPCLASSSS: return "vfpclassss";
    case Mnemonic::VGATHERDPD: return "vgatherdpd";
    case Mnemonic::VGATHERDPS: return "vgatherdps";
    case Mnemonic::VGATHERQPD: return "vgatherqpd";
    case Mnemonic::VGATHERQPS: return "vgatherqps";
    case Mnemonic::VGETEXPPD: return "vgetexppd";
    case Mnemonic::VGETEXPPS: return "vgetexpps";
    case Mnemonic::VGETEXPSD: return "vgetexpsd";
    case Mnemonic::VGETEXPSS: return "vgetexpss";
    case Mnemonic::VGETMANTPD: return "vgetmantpd";
    case Mnemonic::VGETMANTPS: return "vgetmantps";
    case Mnemonic::VGETMANTSD: return "vgetmantsd";
    case Mnemonic::VGETMANTSS: return "vgetmantss";
    case Mnemonic::VINSERTF128: return "vinsertf128";
    case Mnemonic::VINSERTF32X4: return "vinsertf32x4";
    case Mnemonic::VINSERTF32X8: return "vinsertf32x8";
    case Mnemonic::VINSERTF64X2: return "vinsertf64x2";
    case Mnemonic::VINSERTF64X4: return "vinsertf64x4";
    case Mnemonic::VINSERTI128: return "vinserti128";
    case Mnemonic::VINSERTI32X4: return "vinserti32x4";
    case Mnemonic::VINSERTI32X8: return "vinserti32x8";
    case Mnemonic::VINSERTI64X2: return "vinserti64x2";
    case Mnemonic::VINSERTI64X4: return "vinserti64x4";
    case Mnemonic::VMASKMOVPD: return "vmaskmovpd";
    case Mnemonic::VMASKMOVPS: return "vmaskmovps";
    case Mnemonic::VMCALL: return "vmcall";
    case Mnemonic::VMCLEAR: return "vmclear";
    case Mnemonic::VMFUNC: return "vmfunc";
    case Mnemonic::VMLAUNCH: return "vmlaunch";
    case Mnemonic::VMOVDQA32: return "vmovdqa32";
    case Mnemonic::VMOVDQA64: return "vmovdqa64";
    case Mnemonic::VMOVDQU16: return "vmovdqu16";
    case Mnemonic::VMOVDQU32: return "vmovdqu32";
    case Mnemonic::VMOVDQU64: return "vmovdqu64";
    case Mnemonic::VMOVDQU8: return "vmovdqu8";
    case Mnemonic::VMPTRLD: return "vmptrld";
    case Mnemonic::VMPTRST: return "vmptrst";
    case Mnemonic::VMREAD: return "vmread";
    case Mnemonic::VMRESUME: return "vmresume";
    case Mnemonic::VMWRITE: return "vmwrite";
    case Mnemonic::VMXOFF: return "vmxoff";
    case Mnemonic::VMXON: return "vmxon";
    case Mnemonic::VPALIGNR: return "vpalignr";
    case Mnemonic::VPANDD: return "vpandd";
    case Mnemonic::VPANDND: return "vpandnd";
    case Mnemonic::VPANDNQ: return "vpandnq";
    case Mnemonic::VPANDQ: return "vpandq";
    case Mnemonic::VPBLENDMB: return "vpblendmb";
    case Mnemonic::VPBLENDMD: return "vpblendmd";
    case Mnemonic::VPBLENDMQ: return "vpblendmq";
    case Mnemonic::VPBLENDMW: return "vpblendmw";
    case Mnemonic::VPBROADCASTB: return "vpbroadcastb";
    case Mnemonic::VPBROADCASTD: return "vpbroadcastd";
    case Mnemonic::VPBROADCASTQ: return "vpbroadcastq";
    case Mnemonic::VPBROADCASTW: return "vpbroadcastw";
    case Mnemonic::VPCOMPRESSD: return "vpcompressd";
    case Mnemonic::VPCOMPRESSQ: return "vpcompressq";
    case Mnemonic::VPCONFLICTD: return "vpconflictd";
    case Mnemonic::VPCONFLICTQ: return "vpconflictq";
    case Mnemonic::VPDPBUSD: return "vpdpbusd";
    case Mnemonic::VPDPBUSDS: return "vpdpbusds";
    case Mnemonic::VPDPWSSD: return "vpdpwssd";
    case Mnemonic::VPDPWSSDS: return "vpdpwssds";
    case Mnemonic::VPERM2F128: return "vperm2f128";
    case Mnemonic::VPERM2I128: return "vperm2i128";
    case Mnemonic::VPERMB: return "vpermb";
    case Mnemonic::VPERMD: return "vpermd";
    case Mnemonic::VPERMI2D: return "vpermi2d";
    case Mnemonic::VPERMI2PD: return "vpermi2pd";
    case Mnemonic::VPERMI2PS: return "vpermi2ps";
    case Mnemonic::VPERMI2Q: return "vpermi2q";
    case Mnemonic::VPERMILPD: return "vpermilpd";
    case Mnemonic::VPERMILPS: return "vpermilps";
    case Mnemonic::VPERMPD: return "vpermpd";
    case Mnemonic::VPERMPS: return "vpermps";
    case Mnemonic::VPERMQ: return "vpermq";
    case Mnemonic::VPERMT2D: return "vpermt2d";
    case Mnemonic::VPERMT2PD: return "vpermt2pd";
    case Mnemonic::VPERMT2PS: return "vpermt2ps";
    case Mnemonic::VPERMT2Q: return "vpermt2q";
    case Mnemonic::VPERMW: return "vpermw";
    case Mnemonic::VPEXPANDD: return "vpexpandd";
    case Mnemonic::VPEXPANDQ: return "vpexpandq";
    case Mnemonic::VPGATHERDD: return "vpgatherdd";
    case Mnemonic::VPGATHERDQ: return "vpgatherdq";
    case Mnemonic::VPGATHERQD: return "vpgatherqd";
    case Mnemonic::VPGATHERQQ: return "vpgatherqq";
    case Mnemonic::VPLZCNTD: return "vplzcntd";
    case Mnemonic::VPLZCNTQ: return "vplzcntq";
    case Mnemonic::VPMADD52HUQ: return "vpmadd52huq";
    case Mnemonic::VPMADD52LUQ: return "vpmadd52luq";
    case Mnemonic::VPORD: return "vpord";
    case Mnemonic::VPORQ: return "vporq";
    case Mnemonic::VPROLD: return "vprold";
    case Mnemonic::VPROLQ: return "vprolq";
    case Mnemonic::VPROLVD: return "vprolvd";
    case Mnemonic::VPROLVQ: return "vprolvq";
    case Mnemonic::VPRORD: return "vprord";
    case Mnemonic::VPRORQ: return "vprorq";
    case Mnemonic::VPRORVD: return "vprorvd";
    case Mnemonic::VPRORVQ: return "vprorvq";
    case Mnemonic::VPSCATTERDD: return "vpscatterdd";
    case Mnemonic::VPSCATTERDQ: return "vpscatterdq";
    case Mnemonic::VPSCATTERQD: return "vpscatterqd";
    case Mnemonic::VPSCATTERQQ: return "vpscatterqq";
    case Mnemonic::VPTERNLOGD: return "vpternlogd";
    case Mnemonic::VPTERNLOGQ: return "vpternlogq";
    case Mnemonic::VPTESTMB: return "vptestmb";
    case Mnemonic::VPTESTMD: return "vptestmd";
    case Mnemonic::VPTESTMQ: return "vptestmq";
    case Mnemonic::VPTESTMW: return "vptestmw";
    case Mnemonic::VPTESTNMB: return "vptestnmb";
    case Mnemonic::VPTESTNMD: return "vptestnmd";
    case Mnemonic::VPTESTNMQ: return "vptestnmq";
    case Mnemonic::VPTESTNMW: return "vptestnmw";
    case Mnemonic::VPXORD: return "vpxord";
    case Mnemonic::VPXORQ: return "vpxorq";
    case Mnemonic::VRANGEPD: return "vrangepd";
    case Mnemonic::VRANGEPS: return "vrangeps";
    case Mnemonic::VRANGESD: return "vrangesd";
    case Mnemonic::VRANGESS: return "vrangess";
    case Mnemonic::VREDUCEPD: return "vreducepd";
    case Mnemonic::VREDUCEPS: return "vreduceps";
    case Mnemonic::VREDUCESD: return "vreducesd";
    case Mnemonic::VREDUCESS: return "vreducess";
    case Mnemonic::VRNDSCALEPD: return "vrndscalepd";
    case Mnemonic::VRNDSCALEPS: return "vrndscaleps";
    case Mnemonic::VRNDSCALESD: return "vrndscalesd";
    case Mnemonic::VRNDSCALESS: return "vrndscaless";
    case Mnemonic::VSCALEFPD: return "vscalefpd";
    case Mnemonic::VSCALEFPS: return "vscalefps";
    case Mnemonic::VSCALEFSD: return "vscalefsd";
    case Mnemonic::VSCALEFSS: return "vscalefss";
    case Mnemonic::VSCATTERDPD: return "vscatterdpd";
    case Mnemonic::VSCATTERDPS: return "vscatterdps";
    case Mnemonic::VSCATTERQPD: return "vscatterqpd";
    case Mnemonic::VSCATTERQPS: return "vscatterqps";
    case Mnemonic::VZEROALL: return "vzeroall";
    case Mnemonic::VZEROUPPER: return "vzeroupper";
    case Mnemonic::WAIT: return "wait";
    case Mnemonic::WBINVD: return "wbinvd";
    case Mnemonic::WBNOINVD: return "wbnoinvd";
    case Mnemonic::WRFSBASE: return "wrfsbase";
    case Mnemonic::WRGSBASE: return "wrgsbase";
    case Mnemonic::WRMSR: return "wrmsr";
    case Mnemonic::WRPKRU: return "wrpkru";
    case Mnemonic::WRSSD: return "wrssd";
    case Mnemonic::WRSSQ: return "wrssq";
    case Mnemonic::WRUSSD: return "wrussd";
    case Mnemonic::WRUSSQ: return "wrussq";
    case Mnemonic::XABORT: return "xabort";
    case Mnemonic::XADD: return "xadd";
    case Mnemonic::XBEGIN: return "xbegin";
    case Mnemonic::XCHG: return "xchg";
    case Mnemonic::XEND: return "xend";
    case Mnemonic::XGETBV: return "xgetbv";
    case Mnemonic::XLAT: return "xlat";
    case Mnemonic::XLATB: return "xlatb";
    case Mnemonic::XOR: return "xor";
    case Mnemonic::XORPD: return "xorpd";
    case Mnemonic::XORPS: return "xorps";
    case Mnemonic::XRSTOR: return "xrstor";
    case Mnemonic::XRSTORS: return "xrstors";
    case Mnemonic::XSAVE: return "xsave";
    case Mnemonic::XSAVEC: return "xsavec";
    case Mnemonic::XSAVEOPT: return "xsaveopt";
    case Mnemonic::XSAVES: return "xsaves";
    case Mnemonic::XSETBV: return "xsetbv";
    case Mnemonic::XTEST: return "xtest";
    default: return "???";
    }
}
#endif // VEDX64_STRINGS

const InstrDesc g_instr_table[] = {
    /*    0 */ {Mnemonic::ADD, 0x0000, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    1 */ {Mnemonic::ADD, 0x0001, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    2 */ {Mnemonic::ADD, 0x0002, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    3 */ {Mnemonic::ADD, 0x0003, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    4 */ {Mnemonic::ADD, 0x0004, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    5 */ {Mnemonic::ADD, 0x0005, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    6 */ {Mnemonic::PUSH, 0x0006, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    7 */ {Mnemonic::POP, 0x0007, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    8 */ {Mnemonic::OR, 0x0008, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*    9 */ {Mnemonic::OR, 0x0009, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   10 */ {Mnemonic::OR, 0x000A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   11 */ {Mnemonic::OR, 0x000B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   12 */ {Mnemonic::OR, 0x000C, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   13 */ {Mnemonic::OR, 0x000D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   14 */ {Mnemonic::PUSH, 0x000E, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   15 */ {Mnemonic::ADC, 0x0010, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   16 */ {Mnemonic::ADC, 0x0011, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   17 */ {Mnemonic::ADC, 0x0012, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   18 */ {Mnemonic::ADC, 0x0013, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   19 */ {Mnemonic::ADC, 0x0014, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   20 */ {Mnemonic::ADC, 0x0015, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   21 */ {Mnemonic::PUSH, 0x0016, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   22 */ {Mnemonic::POP, 0x0017, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   23 */ {Mnemonic::SBB, 0x0018, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   24 */ {Mnemonic::SBB, 0x0019, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   25 */ {Mnemonic::SBB, 0x001A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   26 */ {Mnemonic::SBB, 0x001B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   27 */ {Mnemonic::SBB, 0x001C, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   28 */ {Mnemonic::SBB, 0x001D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   29 */ {Mnemonic::PUSH, 0x001E, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x03}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   30 */ {Mnemonic::POP, 0x001F, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x03}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   31 */ {Mnemonic::AND, 0x0020, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   32 */ {Mnemonic::AND, 0x0021, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   33 */ {Mnemonic::AND, 0x0022, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   34 */ {Mnemonic::AND, 0x0023, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   35 */ {Mnemonic::AND, 0x0024, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   36 */ {Mnemonic::AND, 0x0025, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   37 */ {Mnemonic::DAA, 0x0027, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   38 */ {Mnemonic::SUB, 0x0028, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   39 */ {Mnemonic::SUB, 0x0029, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   40 */ {Mnemonic::SUB, 0x002A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   41 */ {Mnemonic::SUB, 0x002B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   42 */ {Mnemonic::SUB, 0x002C, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   43 */ {Mnemonic::SUB, 0x002D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   44 */ {Mnemonic::DAS, 0x002F, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   45 */ {Mnemonic::XOR, 0x0030, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   46 */ {Mnemonic::XOR, 0x0031, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   47 */ {Mnemonic::XOR, 0x0032, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   48 */ {Mnemonic::XOR, 0x0033, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   49 */ {Mnemonic::XOR, 0x0034, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   50 */ {Mnemonic::XOR, 0x0035, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   51 */ {Mnemonic::AAA, 0x0037, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   52 */ {Mnemonic::CMP, 0x0038, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   53 */ {Mnemonic::CMP, 0x0039, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   54 */ {Mnemonic::CMP, 0x003A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   55 */ {Mnemonic::CMP, 0x003B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   56 */ {Mnemonic::CMP, 0x003C, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   57 */ {Mnemonic::CMP, 0x003D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   58 */ {Mnemonic::AAS, 0x003F, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   59 */ {Mnemonic::PUSH, 0x0050, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::OpcodeReg, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   60 */ {Mnemonic::PUSH, 0x0050, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::OpcodeReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   61 */ {Mnemonic::POP, 0x0058, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::OpcodeReg, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   62 */ {Mnemonic::POP, 0x0058, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::OpcodeReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   63 */ {Mnemonic::PUSHA, 0x0060, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}}},
    /*   64 */ {Mnemonic::PUSHAD, 0x0060, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}}},
    /*   65 */ {Mnemonic::POPA, 0x0061, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x07}, {AddrMode::Fixed, OpSize::None, 0x06}, {AddrMode::Fixed, OpSize::None, 0x05}, {AddrMode::Fixed, OpSize::None, 0x03}}},
    /*   66 */ {Mnemonic::POPAD, 0x0061, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x07}, {AddrMode::Fixed, OpSize::None, 0x06}, {AddrMode::Fixed, OpSize::None, 0x05}, {AddrMode::Fixed, OpSize::None, 0x03}}},
    /*   67 */ {Mnemonic::BOUND, 0x0062, 0xFF, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSz, 0xFF}, {AddrMode::MemOnly, OpSize::BoundPair, 0xFF}, {AddrMode::None, OpSize::OpSz, 0xFF}}},
    /*   68 */ {Mnemonic::MOVSXD, 0x0063, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   69 */ {Mnemonic::PUSH, 0x0068, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   70 */ {Mnemonic::IMUL, 0x0069, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   71 */ {Mnemonic::PUSH, 0x006A, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   72 */ {Mnemonic::IMUL, 0x006B, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   73 */ {Mnemonic::INS, 0x006C, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   74 */ {Mnemonic::INS, 0x006D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   75 */ {Mnemonic::INS, 0x006D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   76 */ {Mnemonic::OUTS, 0x006E, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   77 */ {Mnemonic::OUTS, 0x006F, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   78 */ {Mnemonic::OUTS, 0x006F, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   79 */ {Mnemonic::JO, 0x0070, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   80 */ {Mnemonic::JNO, 0x0071, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   81 */ {Mnemonic::JB, 0x0072, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   82 */ {Mnemonic::JNB, 0x0073, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   83 */ {Mnemonic::JZ, 0x0074, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   84 */ {Mnemonic::JNZ, 0x0075, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   85 */ {Mnemonic::JBE, 0x0076, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   86 */ {Mnemonic::JNBE, 0x0077, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   87 */ {Mnemonic::JS, 0x0078, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   88 */ {Mnemonic::JNS, 0x0079, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   89 */ {Mnemonic::JP, 0x007A, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   90 */ {Mnemonic::JNP, 0x007B, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   91 */ {Mnemonic::JL, 0x007C, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   92 */ {Mnemonic::JNL, 0x007D, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   93 */ {Mnemonic::JLE, 0x007E, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   94 */ {Mnemonic::JNLE, 0x007F, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   95 */ {Mnemonic::ADD, 0x0080, 0x00, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   96 */ {Mnemonic::OR, 0x0080, 0x01, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   97 */ {Mnemonic::ADC, 0x0080, 0x02, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   98 */ {Mnemonic::SBB, 0x0080, 0x03, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*   99 */ {Mnemonic::AND, 0x0080, 0x04, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  100 */ {Mnemonic::SUB, 0x0080, 0x05, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  101 */ {Mnemonic::XOR, 0x0080, 0x06, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  102 */ {Mnemonic::CMP, 0x0080, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  103 */ {Mnemonic::ADD, 0x0081, 0x00, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  104 */ {Mnemonic::OR, 0x0081, 0x01, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  105 */ {Mnemonic::ADC, 0x0081, 0x02, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  106 */ {Mnemonic::SBB, 0x0081, 0x03, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  107 */ {Mnemonic::AND, 0x0081, 0x04, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  108 */ {Mnemonic::SUB, 0x0081, 0x05, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  109 */ {Mnemonic::XOR, 0x0081, 0x06, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  110 */ {Mnemonic::CMP, 0x0081, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  111 */ {Mnemonic::ADD, 0x0082, 0x00, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  112 */ {Mnemonic::OR, 0x0082, 0x01, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  113 */ {Mnemonic::ADC, 0x0082, 0x02, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  114 */ {Mnemonic::SBB, 0x0082, 0x03, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  115 */ {Mnemonic::AND, 0x0082, 0x04, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  116 */ {Mnemonic::SUB, 0x0082, 0x05, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  117 */ {Mnemonic::XOR, 0x0082, 0x06, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  118 */ {Mnemonic::CMP, 0x0082, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  119 */ {Mnemonic::ADD, 0x0083, 0x00, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  120 */ {Mnemonic::OR, 0x0083, 0x01, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  121 */ {Mnemonic::ADC, 0x0083, 0x02, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  122 */ {Mnemonic::SBB, 0x0083, 0x03, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  123 */ {Mnemonic::AND, 0x0083, 0x04, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  124 */ {Mnemonic::SUB, 0x0083, 0x05, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  125 */ {Mnemonic::XOR, 0x0083, 0x06, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  126 */ {Mnemonic::CMP, 0x0083, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  127 */ {Mnemonic::TEST, 0x0084, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  128 */ {Mnemonic::TEST, 0x0085, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  129 */ {Mnemonic::XCHG, 0x0086, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  130 */ {Mnemonic::XCHG, 0x0087, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  131 */ {Mnemonic::MOV, 0x0088, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  132 */ {Mnemonic::MOV, 0x0089, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  133 */ {Mnemonic::MOV, 0x008A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  134 */ {Mnemonic::MOV, 0x008B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  135 */ {Mnemonic::MOV, 0x008C, 0xFF, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  136 */ {Mnemonic::LEA, 0x008D, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  137 */ {Mnemonic::MOV, 0x008E, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::Word, 0xFF}, {AddrMode::ModRM_RM, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  138 */ {Mnemonic::POP, 0x008F, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  139 */ {Mnemonic::POP, 0x008F, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  140 */ {Mnemonic::XCHG, 0x0090, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::OpcodeReg, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  141 */ {Mnemonic::NOP, 0x0090, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  142 */ {Mnemonic::PAUSE, 0x0090, 0xFF, 0xF3, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  143 */ {Mnemonic::CBW, 0x0098, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  144 */ {Mnemonic::CWDE, 0x0098, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  145 */ {Mnemonic::CBW, 0x0098, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  146 */ {Mnemonic::CWD, 0x0099, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  147 */ {Mnemonic::CDQ, 0x0099, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  148 */ {Mnemonic::CWD, 0x0099, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  149 */ {Mnemonic::CALLF, 0x009A, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::FarPtr, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  150 */ {Mnemonic::FWAIT, 0x009B, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  151 */ {Mnemonic::PUSHF, 0x009C, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  152 */ {Mnemonic::PUSHFD, 0x009C, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  153 */ {Mnemonic::PUSHF, 0x009C, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  154 */ {Mnemonic::POPF, 0x009D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  155 */ {Mnemonic::POPFD, 0x009D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  156 */ {Mnemonic::POPF, 0x009D, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  157 */ {Mnemonic::SAHF, 0x009E, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  158 */ {Mnemonic::LAHF, 0x009F, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  159 */ {Mnemonic::MOV, 0x00A0, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Moffset, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  160 */ {Mnemonic::MOV, 0x00A1, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Moffset, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  161 */ {Mnemonic::MOV, 0x00A2, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Moffset, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  162 */ {Mnemonic::MOV, 0x00A3, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Moffset, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  163 */ {Mnemonic::MOVS, 0x00A4, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  164 */ {Mnemonic::MOVS, 0x00A5, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  165 */ {Mnemonic::MOVS, 0x00A5, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  166 */ {Mnemonic::MOVS, 0x00A5, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  167 */ {Mnemonic::CMPS, 0x00A6, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  168 */ {Mnemonic::CMPS, 0x00A7, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  169 */ {Mnemonic::CMPS, 0x00A7, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  170 */ {Mnemonic::CMPS, 0x00A7, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  171 */ {Mnemonic::TEST, 0x00A8, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  172 */ {Mnemonic::TEST, 0x00A9, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  173 */ {Mnemonic::STOS, 0x00AA, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  174 */ {Mnemonic::STOS, 0x00AB, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  175 */ {Mnemonic::STOS, 0x00AB, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::Fixed, OpSize::OpSz, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  176 */ {Mnemonic::STOS, 0x00AB, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  177 */ {Mnemonic::LODS, 0x00AC, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  178 */ {Mnemonic::LODS, 0x00AD, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  179 */ {Mnemonic::LODS, 0x00AD, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::OpSz, 0x00}, {AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  180 */ {Mnemonic::LODS, 0x00AD, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  181 */ {Mnemonic::SCAS, 0x00AE, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  182 */ {Mnemonic::SCAS, 0x00AF, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  183 */ {Mnemonic::SCAS, 0x00AF, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::Fixed, OpSize::OpSz, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  184 */ {Mnemonic::SCAS, 0x00AF, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  185 */ {Mnemonic::MOV, 0x00B0, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::OpcodeReg, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  186 */ {Mnemonic::MOV, 0x00B8, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::OpcodeReg, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  187 */ {Mnemonic::ROL, 0x00C0, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  188 */ {Mnemonic::ROR, 0x00C0, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  189 */ {Mnemonic::RCL, 0x00C0, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  190 */ {Mnemonic::RCR, 0x00C0, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  191 */ {Mnemonic::SHL, 0x00C0, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  192 */ {Mnemonic::SHR, 0x00C0, 0x05, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  193 */ {Mnemonic::SAL, 0x00C0, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  194 */ {Mnemonic::SAR, 0x00C0, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  195 */ {Mnemonic::ROL, 0x00C1, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  196 */ {Mnemonic::ROR, 0x00C1, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  197 */ {Mnemonic::RCL, 0x00C1, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  198 */ {Mnemonic::RCR, 0x00C1, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  199 */ {Mnemonic::SHL, 0x00C1, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  200 */ {Mnemonic::SHR, 0x00C1, 0x05, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  201 */ {Mnemonic::SAL, 0x00C1, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  202 */ {Mnemonic::SAR, 0x00C1, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  203 */ {Mnemonic::RETN, 0x00C2, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  204 */ {Mnemonic::RETN, 0x00C3, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  205 */ {Mnemonic::LES, 0x00C4, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::ModRM_Reg, OpSize::OpSz, 0xFF}, {AddrMode::MemOnly, OpSize::FarPtr, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  206 */ {Mnemonic::LDS, 0x00C5, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Word, 0x03}, {AddrMode::ModRM_Reg, OpSize::OpSz, 0xFF}, {AddrMode::MemOnly, OpSize::FarPtr, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  207 */ {Mnemonic::MOV, 0x00C6, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  208 */ {Mnemonic::MOV, 0x00C7, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  209 */ {Mnemonic::ENTER, 0x00C8, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::OpSz, 0x05}, {AddrMode::Immediate, OpSize::Word, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /*  210 */ {Mnemonic::ENTER, 0x00C8, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x05}, {AddrMode::Immediate, OpSize::Word, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /*  211 */ {Mnemonic::LEAVE, 0x00C9, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::OpSz, 0x05}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  212 */ {Mnemonic::LEAVE, 0x00C9, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x05}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  213 */ {Mnemonic::RETF, 0x00CA, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Immediate, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  214 */ {Mnemonic::RETF, 0x00CB, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  215 */ {Mnemonic::INT, 0x00CC, 0xFF, 0x00, 0xFF, false, false, 0, 3, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  216 */ {Mnemonic::INT, 0x00CD, 0xFF, 0x00, 0xFF, false, false, 0, 3, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  217 */ {Mnemonic::INTO, 0x00CE, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  218 */ {Mnemonic::IRET, 0x00CF, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  219 */ {Mnemonic::IRETD, 0x00CF, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  220 */ {Mnemonic::IRET, 0x00CF, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  221 */ {Mnemonic::ROL, 0x00D0, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  222 */ {Mnemonic::ROR, 0x00D0, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  223 */ {Mnemonic::RCL, 0x00D0, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  224 */ {Mnemonic::RCR, 0x00D0, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  225 */ {Mnemonic::SHL, 0x00D0, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  226 */ {Mnemonic::SHR, 0x00D0, 0x05, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  227 */ {Mnemonic::SAL, 0x00D0, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  228 */ {Mnemonic::SAR, 0x00D0, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  229 */ {Mnemonic::ROL, 0x00D1, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  230 */ {Mnemonic::ROR, 0x00D1, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  231 */ {Mnemonic::RCL, 0x00D1, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  232 */ {Mnemonic::RCR, 0x00D1, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  233 */ {Mnemonic::SHL, 0x00D1, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  234 */ {Mnemonic::SHR, 0x00D1, 0x05, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  235 */ {Mnemonic::SAL, 0x00D1, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  236 */ {Mnemonic::SAR, 0x00D1, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  237 */ {Mnemonic::ROL, 0x00D2, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  238 */ {Mnemonic::ROR, 0x00D2, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  239 */ {Mnemonic::RCL, 0x00D2, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  240 */ {Mnemonic::RCR, 0x00D2, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  241 */ {Mnemonic::SHL, 0x00D2, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  242 */ {Mnemonic::SHR, 0x00D2, 0x05, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  243 */ {Mnemonic::SAL, 0x00D2, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  244 */ {Mnemonic::SAR, 0x00D2, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  245 */ {Mnemonic::ROL, 0x00D3, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  246 */ {Mnemonic::ROR, 0x00D3, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  247 */ {Mnemonic::RCL, 0x00D3, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  248 */ {Mnemonic::RCR, 0x00D3, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  249 */ {Mnemonic::SHL, 0x00D3, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  250 */ {Mnemonic::SHR, 0x00D3, 0x05, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  251 */ {Mnemonic::SAL, 0x00D3, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  252 */ {Mnemonic::SAR, 0x00D3, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  253 */ {Mnemonic::AAM, 0x00D4, 0xFF, 0x00, 0x0A, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  254 */ {Mnemonic::AMX, 0x00D4, 0xFF, 0x00, 0xFF, false, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  255 */ {Mnemonic::AAD, 0x00D5, 0xFF, 0x00, 0x0A, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  256 */ {Mnemonic::ADX, 0x00D5, 0xFF, 0x00, 0xFF, false, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  257 */ {Mnemonic::SALC, 0x00D6, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  258 */ {Mnemonic::XLAT, 0x00D7, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  259 */ {Mnemonic::FADD, 0x00D8, 0x00, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  260 */ {Mnemonic::FMUL, 0x00D8, 0x01, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  261 */ {Mnemonic::FCOM, 0x00D8, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  262 */ {Mnemonic::FCOM, 0x00D8, 0x02, 0x00, 0xD1, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  263 */ {Mnemonic::FCOMP, 0x00D8, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  264 */ {Mnemonic::FCOMP, 0x00D8, 0x03, 0x00, 0xD9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  265 */ {Mnemonic::FSUB, 0x00D8, 0x04, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  266 */ {Mnemonic::FSUBR, 0x00D8, 0x05, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  267 */ {Mnemonic::FDIV, 0x00D8, 0x06, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  268 */ {Mnemonic::FDIVR, 0x00D8, 0x07, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  269 */ {Mnemonic::FLD, 0x00D9, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  270 */ {Mnemonic::FXCH, 0x00D9, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  271 */ {Mnemonic::FXCH, 0x00D9, 0x01, 0x00, 0xC9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  272 */ {Mnemonic::FST, 0x00D9, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  273 */ {Mnemonic::FNOP, 0x00D9, 0x02, 0x00, 0xD0, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  274 */ {Mnemonic::FSTP, 0x00D9, 0x03, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  275 */ {Mnemonic::FSTP1, 0x00D9, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  276 */ {Mnemonic::FSTP1, 0x00D9, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  277 */ {Mnemonic::FLDENV, 0x00D9, 0x04, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  278 */ {Mnemonic::FCHS, 0x00D9, 0x04, 0x00, 0xE0, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  279 */ {Mnemonic::FABS, 0x00D9, 0x04, 0x00, 0xE1, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  280 */ {Mnemonic::FTST, 0x00D9, 0x04, 0x00, 0xE4, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  281 */ {Mnemonic::FXAM, 0x00D9, 0x04, 0x00, 0xE5, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  282 */ {Mnemonic::FLDCW, 0x00D9, 0x05, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  283 */ {Mnemonic::FLD1, 0x00D9, 0x05, 0x00, 0xE8, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  284 */ {Mnemonic::FLDL2T, 0x00D9, 0x05, 0x00, 0xE9, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  285 */ {Mnemonic::FLDL2E, 0x00D9, 0x05, 0x00, 0xEA, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  286 */ {Mnemonic::FLDPI, 0x00D9, 0x05, 0x00, 0xEB, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  287 */ {Mnemonic::FLDLG2, 0x00D9, 0x05, 0x00, 0xEC, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  288 */ {Mnemonic::FLDLN2, 0x00D9, 0x05, 0x00, 0xED, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  289 */ {Mnemonic::FLDZ, 0x00D9, 0x05, 0x00, 0xEE, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  290 */ {Mnemonic::FNSTENV, 0x00D9, 0x06, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  291 */ {Mnemonic::FSTENV, 0x00D9, 0x06, 0x9B, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  292 */ {Mnemonic::F2XM1, 0x00D9, 0x06, 0x00, 0xF0, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  293 */ {Mnemonic::FYL2X, 0x00D9, 0x06, 0x00, 0xF1, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  294 */ {Mnemonic::FPTAN, 0x00D9, 0x06, 0x00, 0xF2, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  295 */ {Mnemonic::FPATAN, 0x00D9, 0x06, 0x00, 0xF3, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  296 */ {Mnemonic::FXTRACT, 0x00D9, 0x06, 0x00, 0xF4, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  297 */ {Mnemonic::FPREM1, 0x00D9, 0x06, 0x00, 0xF5, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  298 */ {Mnemonic::FDECSTP, 0x00D9, 0x06, 0x00, 0xF6, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  299 */ {Mnemonic::FINCSTP, 0x00D9, 0x06, 0x00, 0xF7, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  300 */ {Mnemonic::FNSTCW, 0x00D9, 0x07, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  301 */ {Mnemonic::FSTCW, 0x00D9, 0x07, 0x9B, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  302 */ {Mnemonic::FPREM, 0x00D9, 0x07, 0x00, 0xF8, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  303 */ {Mnemonic::FYL2XP1, 0x00D9, 0x07, 0x00, 0xF9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  304 */ {Mnemonic::FSQRT, 0x00D9, 0x07, 0x00, 0xFA, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  305 */ {Mnemonic::FSINCOS, 0x00D9, 0x07, 0x00, 0xFB, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  306 */ {Mnemonic::FRNDINT, 0x00D9, 0x07, 0x00, 0xFC, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  307 */ {Mnemonic::FSCALE, 0x00D9, 0x07, 0x00, 0xFD, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  308 */ {Mnemonic::FSIN, 0x00D9, 0x07, 0x00, 0xFE, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  309 */ {Mnemonic::FCOS, 0x00D9, 0x07, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  310 */ {Mnemonic::FIADD, 0x00DA, 0x00, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  311 */ {Mnemonic::FCMOVB, 0x00DA, 0x00, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  312 */ {Mnemonic::FIMUL, 0x00DA, 0x01, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  313 */ {Mnemonic::FCMOVE, 0x00DA, 0x01, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  314 */ {Mnemonic::FICOM, 0x00DA, 0x02, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  315 */ {Mnemonic::FCMOVBE, 0x00DA, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  316 */ {Mnemonic::FICOMP, 0x00DA, 0x03, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  317 */ {Mnemonic::FCMOVU, 0x00DA, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  318 */ {Mnemonic::FISUB, 0x00DA, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  319 */ {Mnemonic::FISUBR, 0x00DA, 0x05, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  320 */ {Mnemonic::FUCOMPP, 0x00DA, 0x05, 0x00, 0xE9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  321 */ {Mnemonic::FIDIV, 0x00DA, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  322 */ {Mnemonic::FIDIVR, 0x00DA, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  323 */ {Mnemonic::FILD, 0x00DB, 0x00, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  324 */ {Mnemonic::FCMOVNB, 0x00DB, 0x00, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  325 */ {Mnemonic::FISTTP, 0x00DB, 0x01, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  326 */ {Mnemonic::FCMOVNE, 0x00DB, 0x01, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  327 */ {Mnemonic::FIST, 0x00DB, 0x02, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  328 */ {Mnemonic::FCMOVNBE, 0x00DB, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  329 */ {Mnemonic::FISTP, 0x00DB, 0x03, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  330 */ {Mnemonic::FCMOVNU, 0x00DB, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  331 */ {Mnemonic::FNENI, 0x00DB, 0x04, 0x00, 0xE0, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  332 */ {Mnemonic::FENI, 0x00DB, 0x04, 0x9B, 0xE0, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  333 */ {Mnemonic::FNENI, 0x00DB, 0x04, 0x00, 0xE0, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  334 */ {Mnemonic::FNDISI, 0x00DB, 0x04, 0x00, 0xE1, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  335 */ {Mnemonic::FDISI, 0x00DB, 0x04, 0x9B, 0xE1, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  336 */ {Mnemonic::FNDISI, 0x00DB, 0x04, 0x00, 0xE1, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  337 */ {Mnemonic::FNCLEX, 0x00DB, 0x04, 0x00, 0xE2, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  338 */ {Mnemonic::FCLEX, 0x00DB, 0x04, 0x9B, 0xE2, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  339 */ {Mnemonic::FNINIT, 0x00DB, 0x04, 0x00, 0xE3, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  340 */ {Mnemonic::FINIT, 0x00DB, 0x04, 0x9B, 0xE3, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  341 */ {Mnemonic::FNSETPM, 0x00DB, 0x04, 0x00, 0xE4, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  342 */ {Mnemonic::FSETPM, 0x00DB, 0x04, 0x9B, 0xE4, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  343 */ {Mnemonic::FNSETPM, 0x00DB, 0x04, 0x00, 0xE4, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  344 */ {Mnemonic::FLD, 0x00DB, 0x05, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  345 */ {Mnemonic::FUCOMI, 0x00DB, 0x05, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  346 */ {Mnemonic::FCOMI, 0x00DB, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  347 */ {Mnemonic::FSTP, 0x00DB, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  348 */ {Mnemonic::FADD, 0x00DC, 0x00, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  349 */ {Mnemonic::FADD, 0x00DC, 0x00, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  350 */ {Mnemonic::FMUL, 0x00DC, 0x01, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  351 */ {Mnemonic::FMUL, 0x00DC, 0x01, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  352 */ {Mnemonic::FCOM, 0x00DC, 0x02, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  353 */ {Mnemonic::FCOM2, 0x00DC, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  354 */ {Mnemonic::FCOM2, 0x00DC, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  355 */ {Mnemonic::FCOMP, 0x00DC, 0x03, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  356 */ {Mnemonic::FCOMP3, 0x00DC, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  357 */ {Mnemonic::FCOMP3, 0x00DC, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  358 */ {Mnemonic::FSUB, 0x00DC, 0x04, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  359 */ {Mnemonic::FSUBR, 0x00DC, 0x04, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  360 */ {Mnemonic::FSUBR, 0x00DC, 0x05, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  361 */ {Mnemonic::FSUB, 0x00DC, 0x05, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  362 */ {Mnemonic::FDIV, 0x00DC, 0x06, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  363 */ {Mnemonic::FDIVR, 0x00DC, 0x06, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  364 */ {Mnemonic::FDIVR, 0x00DC, 0x07, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  365 */ {Mnemonic::FDIV, 0x00DC, 0x07, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  366 */ {Mnemonic::FLD, 0x00DD, 0x00, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  367 */ {Mnemonic::FFREE, 0x00DD, 0x00, 0x00, 0xFF, true, false, -1, 1, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  368 */ {Mnemonic::FISTTP, 0x00DD, 0x01, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  369 */ {Mnemonic::FXCH4, 0x00DD, 0x01, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  370 */ {Mnemonic::FXCH4, 0x00DD, 0x01, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  371 */ {Mnemonic::FST, 0x00DD, 0x02, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  372 */ {Mnemonic::FST, 0x00DD, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  373 */ {Mnemonic::FSTP, 0x00DD, 0x03, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  374 */ {Mnemonic::FSTP, 0x00DD, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  375 */ {Mnemonic::FRSTOR, 0x00DD, 0x04, 0x00, 0xFF, true, false, 1, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x03}}},
    /*  376 */ {Mnemonic::FUCOM, 0x00DD, 0x04, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  377 */ {Mnemonic::FUCOM, 0x00DD, 0x04, 0x00, 0xE1, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  378 */ {Mnemonic::FUCOMP, 0x00DD, 0x05, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  379 */ {Mnemonic::FUCOMP, 0x00DD, 0x05, 0x00, 0xE9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  380 */ {Mnemonic::FNSAVE, 0x00DD, 0x06, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}}},
    /*  381 */ {Mnemonic::FSAVE, 0x00DD, 0x06, 0x9B, 0xFF, true, false, 0, 4, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}}},
    /*  382 */ {Mnemonic::FNSTSW, 0x00DD, 0x07, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  383 */ {Mnemonic::FSTSW, 0x00DD, 0x07, 0x9B, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  384 */ {Mnemonic::FIADD, 0x00DE, 0x00, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  385 */ {Mnemonic::FADDP, 0x00DE, 0x00, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  386 */ {Mnemonic::FADDP, 0x00DE, 0x00, 0x00, 0xC1, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  387 */ {Mnemonic::FIMUL, 0x00DE, 0x01, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  388 */ {Mnemonic::FMULP, 0x00DE, 0x01, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  389 */ {Mnemonic::FMULP, 0x00DE, 0x01, 0x00, 0xC9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  390 */ {Mnemonic::FICOM, 0x00DE, 0x02, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  391 */ {Mnemonic::FCOMP5, 0x00DE, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  392 */ {Mnemonic::FCOMP5, 0x00DE, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  393 */ {Mnemonic::FICOMP, 0x00DE, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  394 */ {Mnemonic::FCOMPP, 0x00DE, 0x03, 0x00, 0xD9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  395 */ {Mnemonic::FISUB, 0x00DE, 0x04, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  396 */ {Mnemonic::FSUBRP, 0x00DE, 0x04, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  397 */ {Mnemonic::FSUBRP, 0x00DE, 0x04, 0x00, 0xE1, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  398 */ {Mnemonic::FISUBR, 0x00DE, 0x05, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  399 */ {Mnemonic::FSUBP, 0x00DE, 0x05, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  400 */ {Mnemonic::FSUBP, 0x00DE, 0x05, 0x00, 0xE9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  401 */ {Mnemonic::FIDIV, 0x00DE, 0x06, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  402 */ {Mnemonic::FDIVRP, 0x00DE, 0x06, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  403 */ {Mnemonic::FDIVRP, 0x00DE, 0x06, 0x00, 0xF1, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  404 */ {Mnemonic::FIDIVR, 0x00DE, 0x07, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  405 */ {Mnemonic::FDIVP, 0x00DE, 0x07, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  406 */ {Mnemonic::FDIVP, 0x00DE, 0x07, 0x00, 0xF9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  407 */ {Mnemonic::FILD, 0x00DF, 0x00, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  408 */ {Mnemonic::FFREEP, 0x00DF, 0x00, 0x00, 0xFF, true, false, -1, 1, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  409 */ {Mnemonic::FISTTP, 0x00DF, 0x01, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  410 */ {Mnemonic::FXCH7, 0x00DF, 0x01, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  411 */ {Mnemonic::FXCH7, 0x00DF, 0x01, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  412 */ {Mnemonic::FIST, 0x00DF, 0x02, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  413 */ {Mnemonic::FSTP8, 0x00DF, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  414 */ {Mnemonic::FSTP8, 0x00DF, 0x02, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  415 */ {Mnemonic::FISTP, 0x00DF, 0x03, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  416 */ {Mnemonic::FSTP9, 0x00DF, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  417 */ {Mnemonic::FSTP9, 0x00DF, 0x03, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  418 */ {Mnemonic::FBLD, 0x00DF, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  419 */ {Mnemonic::FNSTSW, 0x00DF, 0x04, 0x00, 0xE0, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  420 */ {Mnemonic::FSTSW, 0x00DF, 0x04, 0x9B, 0xE0, true, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  421 */ {Mnemonic::FILD, 0x00DF, 0x05, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  422 */ {Mnemonic::FUCOMIP, 0x00DF, 0x05, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  423 */ {Mnemonic::FBSTP, 0x00DF, 0x06, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  424 */ {Mnemonic::FCOMIP, 0x00DF, 0x06, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  425 */ {Mnemonic::FISTP, 0x00DF, 0x07, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  426 */ {Mnemonic::LOOPNZ, 0x00E0, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  427 */ {Mnemonic::LOOPNZ, 0x00E0, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  428 */ {Mnemonic::LOOPZ, 0x00E1, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  429 */ {Mnemonic::LOOPZ, 0x00E1, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  430 */ {Mnemonic::LOOP, 0x00E2, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  431 */ {Mnemonic::LOOP, 0x00E2, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  432 */ {Mnemonic::JCXZ, 0x00E3, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  433 */ {Mnemonic::JECXZ, 0x00E3, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  434 */ {Mnemonic::IN, 0x00E4, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  435 */ {Mnemonic::IN, 0x00E5, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::OpSz, 0x00}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  436 */ {Mnemonic::OUT, 0x00E6, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  437 */ {Mnemonic::OUT, 0x00E7, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::OpSz, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  438 */ {Mnemonic::CALL, 0x00E8, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  439 */ {Mnemonic::JMP, 0x00E9, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  440 */ {Mnemonic::JMPF, 0x00EA, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::None, OpSize::FarPtr, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  441 */ {Mnemonic::JMP, 0x00EB, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::ByteSignExt, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  442 */ {Mnemonic::IN, 0x00EC, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  443 */ {Mnemonic::IN, 0x00ED, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::OpSz, 0x00}, {AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  444 */ {Mnemonic::OUT, 0x00EE, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  445 */ {Mnemonic::OUT, 0x00EF, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::Fixed, OpSize::OpSz, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  446 */ {Mnemonic::INT1, 0x00F1, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  447 */ {Mnemonic::HLT, 0x00F4, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  448 */ {Mnemonic::CMC, 0x00F5, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  449 */ {Mnemonic::TEST, 0x00F6, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  450 */ {Mnemonic::TEST, 0x00F6, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  451 */ {Mnemonic::NOT, 0x00F6, 0x02, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  452 */ {Mnemonic::NEG, 0x00F6, 0x03, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  453 */ {Mnemonic::MUL, 0x00F6, 0x04, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  454 */ {Mnemonic::IMUL, 0x00F6, 0x05, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  455 */ {Mnemonic::DIV, 0x00F6, 0x06, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}}},
    /*  456 */ {Mnemonic::IDIV, 0x00F6, 0x07, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::Fixed, OpSize::Byte, 0x04}, {AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}}},
    /*  457 */ {Mnemonic::TEST, 0x00F7, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  458 */ {Mnemonic::TEST, 0x00F7, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  459 */ {Mnemonic::NOT, 0x00F7, 0x02, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  460 */ {Mnemonic::NEG, 0x00F7, 0x03, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  461 */ {Mnemonic::MUL, 0x00F7, 0x04, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  462 */ {Mnemonic::IMUL, 0x00F7, 0x05, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  463 */ {Mnemonic::DIV, 0x00F7, 0x06, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  464 */ {Mnemonic::IDIV, 0x00F7, 0x07, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  465 */ {Mnemonic::CLC, 0x00F8, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  466 */ {Mnemonic::STC, 0x00F9, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  467 */ {Mnemonic::CLI, 0x00FA, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  468 */ {Mnemonic::STI, 0x00FB, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  469 */ {Mnemonic::CLD, 0x00FC, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  470 */ {Mnemonic::STD, 0x00FD, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  471 */ {Mnemonic::INC, 0x00FE, 0x00, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  472 */ {Mnemonic::DEC, 0x00FE, 0x01, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  473 */ {Mnemonic::INC, 0x00FF, 0x00, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  474 */ {Mnemonic::DEC, 0x00FF, 0x01, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  475 */ {Mnemonic::CALL, 0x00FF, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  476 */ {Mnemonic::CALL, 0x00FF, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  477 */ {Mnemonic::CALLF, 0x00FF, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  478 */ {Mnemonic::JMP, 0x00FF, 0x04, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  479 */ {Mnemonic::JMP, 0x00FF, 0x04, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  480 */ {Mnemonic::JMPF, 0x00FF, 0x05, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  481 */ {Mnemonic::PUSH, 0x00FF, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  482 */ {Mnemonic::PUSH, 0x00FF, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  483 */ {Mnemonic::SLDT, 0x0F00, 0x00, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  484 */ {Mnemonic::STR, 0x0F00, 0x01, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  485 */ {Mnemonic::LLDT, 0x0F00, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  486 */ {Mnemonic::LTR, 0x0F00, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  487 */ {Mnemonic::VERR, 0x0F00, 0x04, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  488 */ {Mnemonic::VERW, 0x0F00, 0x05, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  489 */ {Mnemonic::JMPE, 0x0F00, 0x06, 0x00, 0xFF, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  490 */ {Mnemonic::SGDT, 0x0F01, 0x00, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::PseudoDesc, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  491 */ {Mnemonic::VMCALL, 0x0F01, 0x00, 0x00, 0xC1, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  492 */ {Mnemonic::VMLAUNCH, 0x0F01, 0x00, 0x00, 0xC2, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  493 */ {Mnemonic::VMRESUME, 0x0F01, 0x00, 0x00, 0xC3, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  494 */ {Mnemonic::VMXOFF, 0x0F01, 0x00, 0x00, 0xC4, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  495 */ {Mnemonic::SIDT, 0x0F01, 0x01, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::PseudoDesc, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  496 */ {Mnemonic::MONITOR, 0x0F01, 0x01, 0x00, 0xC8, true, false, 0, 3, 0, {{AddrMode::None, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Dword, 0x01}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  497 */ {Mnemonic::MWAIT, 0x0F01, 0x01, 0x00, 0xC9, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::Dword, 0x01}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  498 */ {Mnemonic::LGDT, 0x0F01, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::PseudoDesc, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  499 */ {Mnemonic::XGETBV, 0x0F01, 0x02, 0x00, 0xD0, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::Dword, 0x01}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  500 */ {Mnemonic::XSETBV, 0x0F01, 0x02, 0x00, 0xD1, true, false, 0, 4, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Dword, 0x01}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::Fixed, OpSize::Dword, 0x00}}},
    /*  501 */ {Mnemonic::LIDT, 0x0F01, 0x03, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::PseudoDesc, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  502 */ {Mnemonic::SMSW, 0x0F01, 0x04, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  503 */ {Mnemonic::LMSW, 0x0F01, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x00}, {AddrMode::ModRM_RM, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  504 */ {Mnemonic::INVLPG, 0x0F01, 0x07, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  505 */ {Mnemonic::SWAPGS, 0x0F01, 0x07, 0x00, 0xF8, true, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x05}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  506 */ {Mnemonic::RDTSCP, 0x0F01, 0x07, 0x00, 0xF9, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::Fixed, OpSize::Dword, 0x01}, {AddrMode::Fixed, OpSize::None, 0x0A}}},
    /*  507 */ {Mnemonic::LAR, 0x0F02, 0xFF, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  508 */ {Mnemonic::LSL, 0x0F03, 0xFF, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  509 */ {Mnemonic::SYSCALL, 0x0F05, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Qword, 0x01}, {AddrMode::Fixed, OpSize::Qword, 0x0B}, {AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::Dword, 0xFF}}},
    /*  510 */ {Mnemonic::CLTS, 0x0F06, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  511 */ {Mnemonic::SYSRET, 0x0F07, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::Fixed, OpSize::Qword, 0x0B}, {AddrMode::Fixed, OpSize::Qword, 0x01}}},
    /*  512 */ {Mnemonic::INVD, 0x0F08, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  513 */ {Mnemonic::WBINVD, 0x0F09, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  514 */ {Mnemonic::UD2, 0x0F0B, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  515 */ {Mnemonic::NOP, 0x0F0D, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  516 */ {Mnemonic::MOVUPS, 0x0F10, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  517 */ {Mnemonic::MOVSS, 0x0F10, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  518 */ {Mnemonic::MOVUPD, 0x0F10, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  519 */ {Mnemonic::MOVSD, 0x0F10, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  520 */ {Mnemonic::MOVUPS, 0x0F11, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  521 */ {Mnemonic::MOVSS, 0x0F11, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  522 */ {Mnemonic::MOVUPD, 0x0F11, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  523 */ {Mnemonic::MOVSD, 0x0F11, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  524 */ {Mnemonic::MOVHLPS, 0x0F12, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::XmmRegOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  525 */ {Mnemonic::MOVLPS, 0x0F12, 0xFF, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  526 */ {Mnemonic::MOVLPD, 0x0F12, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  527 */ {Mnemonic::MOVDDUP, 0x0F12, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::XmmRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  528 */ {Mnemonic::MOVSLDUP, 0x0F12, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::XmmRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  529 */ {Mnemonic::MOVLPS, 0x0F13, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  530 */ {Mnemonic::MOVLPD, 0x0F13, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  531 */ {Mnemonic::UNPCKLPS, 0x0F14, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  532 */ {Mnemonic::UNPCKLPD, 0x0F14, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  533 */ {Mnemonic::UNPCKHPS, 0x0F15, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  534 */ {Mnemonic::UNPCKHPD, 0x0F15, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  535 */ {Mnemonic::MOVLHPS, 0x0F16, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::XmmRegOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  536 */ {Mnemonic::MOVHPS, 0x0F16, 0xFF, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  537 */ {Mnemonic::MOVHPD, 0x0F16, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  538 */ {Mnemonic::MOVSHDUP, 0x0F16, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::XmmRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  539 */ {Mnemonic::MOVHPS, 0x0F17, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  540 */ {Mnemonic::MOVHPD, 0x0F17, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  541 */ {Mnemonic::HINT_NOP, 0x0F18, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  542 */ {Mnemonic::PREFETCHNTA, 0x0F18, 0x00, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  543 */ {Mnemonic::PREFETCHT0, 0x0F18, 0x01, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  544 */ {Mnemonic::PREFETCHT1, 0x0F18, 0x02, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  545 */ {Mnemonic::PREFETCHT2, 0x0F18, 0x03, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  546 */ {Mnemonic::HINT_NOP, 0x0F18, 0x04, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  547 */ {Mnemonic::HINT_NOP, 0x0F18, 0x05, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  548 */ {Mnemonic::HINT_NOP, 0x0F18, 0x06, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  549 */ {Mnemonic::HINT_NOP, 0x0F18, 0x07, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  550 */ {Mnemonic::HINT_NOP, 0x0F19, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  551 */ {Mnemonic::HINT_NOP, 0x0F1A, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  552 */ {Mnemonic::HINT_NOP, 0x0F1B, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  553 */ {Mnemonic::HINT_NOP, 0x0F1C, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  554 */ {Mnemonic::HINT_NOP, 0x0F1D, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  555 */ {Mnemonic::HINT_NOP, 0x0F1E, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  556 */ {Mnemonic::HINT_NOP, 0x0F1F, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  557 */ {Mnemonic::NOP, 0x0F1F, 0x00, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  558 */ {Mnemonic::HINT_NOP, 0x0F1F, 0x01, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  559 */ {Mnemonic::HINT_NOP, 0x0F1F, 0x02, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  560 */ {Mnemonic::HINT_NOP, 0x0F1F, 0x03, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  561 */ {Mnemonic::HINT_NOP, 0x0F1F, 0x04, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  562 */ {Mnemonic::HINT_NOP, 0x0F1F, 0x05, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  563 */ {Mnemonic::HINT_NOP, 0x0F1F, 0x06, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  564 */ {Mnemonic::HINT_NOP, 0x0F1F, 0x07, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSz, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  565 */ {Mnemonic::MOV, 0x0F20, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::RegOnly, OpSize::Dword, 0xFF}, {AddrMode::CtrlReg, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  566 */ {Mnemonic::MOV, 0x0F20, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::CtrlReg, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  567 */ {Mnemonic::MOV, 0x0F20, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::RegOnly, OpSize::Qword, 0xFF}, {AddrMode::CtrlReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  568 */ {Mnemonic::MOV, 0x0F20, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::Qword, 0xFF}, {AddrMode::CtrlReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  569 */ {Mnemonic::MOV, 0x0F21, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::RegOnly, OpSize::Dword, 0xFF}, {AddrMode::DbgReg, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  570 */ {Mnemonic::MOV, 0x0F21, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::DbgReg, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  571 */ {Mnemonic::MOV, 0x0F21, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::RegOnly, OpSize::Qword, 0xFF}, {AddrMode::DbgReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  572 */ {Mnemonic::MOV, 0x0F21, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::Qword, 0xFF}, {AddrMode::DbgReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  573 */ {Mnemonic::MOV, 0x0F22, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::CtrlReg, OpSize::Dword, 0xFF}, {AddrMode::RegOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  574 */ {Mnemonic::MOV, 0x0F22, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::CtrlReg, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  575 */ {Mnemonic::MOV, 0x0F22, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::CtrlReg, OpSize::Qword, 0xFF}, {AddrMode::RegOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  576 */ {Mnemonic::MOV, 0x0F22, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::CtrlReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  577 */ {Mnemonic::MOV, 0x0F23, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::DbgReg, OpSize::Dword, 0xFF}, {AddrMode::RegOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  578 */ {Mnemonic::MOV, 0x0F23, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::DbgReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  579 */ {Mnemonic::MOV, 0x0F23, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::DbgReg, OpSize::Qword, 0xFF}, {AddrMode::RegOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  580 */ {Mnemonic::MOV, 0x0F23, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::DbgReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  581 */ {Mnemonic::MOV, 0x0F24, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::RegOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  582 */ {Mnemonic::MOV, 0x0F24, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  583 */ {Mnemonic::MOV, 0x0F26, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::RegOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  584 */ {Mnemonic::MOV, 0x0F26, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  585 */ {Mnemonic::MOVAPS, 0x0F28, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  586 */ {Mnemonic::MOVAPD, 0x0F28, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  587 */ {Mnemonic::MOVAPS, 0x0F29, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  588 */ {Mnemonic::MOVAPD, 0x0F29, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  589 */ {Mnemonic::CVTPI2PS, 0x0F2A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::MmxRM, OpSize::Mmx64, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  590 */ {Mnemonic::CVTSI2SS, 0x0F2A, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::ModRM_RM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  591 */ {Mnemonic::CVTPI2PD, 0x0F2A, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::MmxRM, OpSize::Mmx64, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  592 */ {Mnemonic::CVTSI2SD, 0x0F2A, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::ModRM_RM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  593 */ {Mnemonic::MOVNTPS, 0x0F2B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  594 */ {Mnemonic::MOVNTPD, 0x0F2B, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  595 */ {Mnemonic::CVTTPS2PI, 0x0F2C, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Mmx64, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  596 */ {Mnemonic::CVTTSS2SI, 0x0F2C, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  597 */ {Mnemonic::CVTTPD2PI, 0x0F2C, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Mmx64, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  598 */ {Mnemonic::CVTTSD2SI, 0x0F2C, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  599 */ {Mnemonic::CVTPS2PI, 0x0F2D, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Mmx64, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  600 */ {Mnemonic::CVTSS2SI, 0x0F2D, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  601 */ {Mnemonic::CVTPD2PI, 0x0F2D, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Mmx64, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  602 */ {Mnemonic::CVTSD2SI, 0x0F2D, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  603 */ {Mnemonic::UCOMISS, 0x0F2E, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  604 */ {Mnemonic::UCOMISD, 0x0F2E, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  605 */ {Mnemonic::COMISS, 0x0F2F, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  606 */ {Mnemonic::COMISD, 0x0F2F, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  607 */ {Mnemonic::WRMSR, 0x0F30, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x02}}},
    /*  608 */ {Mnemonic::RDTSC, 0x0F31, 0xFF, 0x00, 0xFF, false, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::Fixed, OpSize::None, 0x0A}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  609 */ {Mnemonic::RDMSR, 0x0F32, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  610 */ {Mnemonic::RDPMC, 0x0F33, 0xFF, 0x00, 0xFF, false, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  611 */ {Mnemonic::SYSENTER, 0x0F34, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::Fixed, OpSize::Dword, 0x04}, {AddrMode::Fixed, OpSize::None, 0xAE}, {AddrMode::Fixed, OpSize::None, 0xAF}}},
    /*  612 */ {Mnemonic::SYSENTER, 0x0F34, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::Fixed, OpSize::Qword, 0x04}, {AddrMode::Fixed, OpSize::None, 0xAE}, {AddrMode::Fixed, OpSize::None, 0xAF}}},
    /*  613 */ {Mnemonic::SYSEXIT, 0x0F35, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::Fixed, OpSize::None, 0x04}, {AddrMode::Fixed, OpSize::None, 0xAE}, {AddrMode::Fixed, OpSize::None, 0x01}}},
    /*  614 */ {Mnemonic::GETSEC, 0x0F37, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  615 */ {Mnemonic::PSHUFB, 0x0F38, 0xFF, 0x00, 0x00, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  616 */ {Mnemonic::PSHUFB, 0x0F38, 0xFF, 0x66, 0x00, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  617 */ {Mnemonic::PHADDW, 0x0F38, 0xFF, 0x00, 0x01, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  618 */ {Mnemonic::PHADDW, 0x0F38, 0xFF, 0x66, 0x01, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  619 */ {Mnemonic::PHADDD, 0x0F38, 0xFF, 0x00, 0x02, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  620 */ {Mnemonic::PHADDD, 0x0F38, 0xFF, 0x66, 0x02, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  621 */ {Mnemonic::PHADDSW, 0x0F38, 0xFF, 0x00, 0x03, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  622 */ {Mnemonic::PHADDSW, 0x0F38, 0xFF, 0x66, 0x03, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  623 */ {Mnemonic::PMADDUBSW, 0x0F38, 0xFF, 0x00, 0x04, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  624 */ {Mnemonic::PMADDUBSW, 0x0F38, 0xFF, 0x66, 0x04, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  625 */ {Mnemonic::PHSUBW, 0x0F38, 0xFF, 0x00, 0x05, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  626 */ {Mnemonic::PHSUBW, 0x0F38, 0xFF, 0x66, 0x05, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  627 */ {Mnemonic::PHSUBD, 0x0F38, 0xFF, 0x00, 0x06, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  628 */ {Mnemonic::PHSUBD, 0x0F38, 0xFF, 0x66, 0x06, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  629 */ {Mnemonic::PHSUBSW, 0x0F38, 0xFF, 0x00, 0x07, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  630 */ {Mnemonic::PHSUBSW, 0x0F38, 0xFF, 0x66, 0x07, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  631 */ {Mnemonic::PSIGNB, 0x0F38, 0xFF, 0x00, 0x08, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  632 */ {Mnemonic::PSIGNB, 0x0F38, 0xFF, 0x66, 0x08, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  633 */ {Mnemonic::PSIGNW, 0x0F38, 0xFF, 0x00, 0x09, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  634 */ {Mnemonic::PSIGNW, 0x0F38, 0xFF, 0x66, 0x09, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  635 */ {Mnemonic::PSIGND, 0x0F38, 0xFF, 0x00, 0x0A, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  636 */ {Mnemonic::PSIGND, 0x0F38, 0xFF, 0x66, 0x0A, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  637 */ {Mnemonic::PMULHRSW, 0x0F38, 0xFF, 0x00, 0x0B, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  638 */ {Mnemonic::PMULHRSW, 0x0F38, 0xFF, 0x66, 0x0B, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  639 */ {Mnemonic::PBLENDVB, 0x0F38, 0xFF, 0x66, 0x10, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  640 */ {Mnemonic::BLENDVPS, 0x0F38, 0xFF, 0x66, 0x14, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  641 */ {Mnemonic::BLENDVPD, 0x0F38, 0xFF, 0x66, 0x15, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  642 */ {Mnemonic::PTEST, 0x0F38, 0xFF, 0x66, 0x17, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  643 */ {Mnemonic::PABSB, 0x0F38, 0xFF, 0x00, 0x1C, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  644 */ {Mnemonic::PABSB, 0x0F38, 0xFF, 0x66, 0x1C, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  645 */ {Mnemonic::PABSW, 0x0F38, 0xFF, 0x00, 0x1D, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  646 */ {Mnemonic::PABSW, 0x0F38, 0xFF, 0x66, 0x1D, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  647 */ {Mnemonic::PABSD, 0x0F38, 0xFF, 0x00, 0x1E, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  648 */ {Mnemonic::PABSD, 0x0F38, 0xFF, 0x66, 0x1E, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  649 */ {Mnemonic::PMOVSXBW, 0x0F38, 0xFF, 0x66, 0x20, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  650 */ {Mnemonic::PMOVSXBD, 0x0F38, 0xFF, 0x66, 0x21, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  651 */ {Mnemonic::PMOVSXBQ, 0x0F38, 0xFF, 0x66, 0x22, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  652 */ {Mnemonic::PMOVSXWD, 0x0F38, 0xFF, 0x66, 0x23, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  653 */ {Mnemonic::PMOVSXWQ, 0x0F38, 0xFF, 0x66, 0x24, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  654 */ {Mnemonic::PMOVSXDQ, 0x0F38, 0xFF, 0x66, 0x25, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  655 */ {Mnemonic::PMULDQ, 0x0F38, 0xFF, 0x66, 0x28, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  656 */ {Mnemonic::PCMPEQQ, 0x0F38, 0xFF, 0x66, 0x29, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  657 */ {Mnemonic::MOVNTDQA, 0x0F38, 0xFF, 0x66, 0x2A, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  658 */ {Mnemonic::PACKUSDW, 0x0F38, 0xFF, 0x66, 0x2B, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  659 */ {Mnemonic::PMOVZXBW, 0x0F38, 0xFF, 0x66, 0x30, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  660 */ {Mnemonic::PMOVZXBD, 0x0F38, 0xFF, 0x66, 0x31, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  661 */ {Mnemonic::PMOVZXBQ, 0x0F38, 0xFF, 0x66, 0x32, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  662 */ {Mnemonic::PMOVZXWD, 0x0F38, 0xFF, 0x66, 0x33, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  663 */ {Mnemonic::PMOVZXWQ, 0x0F38, 0xFF, 0x66, 0x34, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  664 */ {Mnemonic::PMOVZXDQ, 0x0F38, 0xFF, 0x66, 0x35, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  665 */ {Mnemonic::PCMPGTQ, 0x0F38, 0xFF, 0x66, 0x37, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  666 */ {Mnemonic::PMINSB, 0x0F38, 0xFF, 0x66, 0x38, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  667 */ {Mnemonic::PMINSD, 0x0F38, 0xFF, 0x66, 0x39, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  668 */ {Mnemonic::PMINUW, 0x0F38, 0xFF, 0x66, 0x3A, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  669 */ {Mnemonic::PMINUD, 0x0F38, 0xFF, 0x66, 0x3B, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  670 */ {Mnemonic::PMAXSB, 0x0F38, 0xFF, 0x66, 0x3C, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  671 */ {Mnemonic::PMAXSD, 0x0F38, 0xFF, 0x66, 0x3D, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  672 */ {Mnemonic::PMAXUW, 0x0F38, 0xFF, 0x66, 0x3E, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  673 */ {Mnemonic::PMAXUD, 0x0F38, 0xFF, 0x66, 0x3F, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  674 */ {Mnemonic::PMULLD, 0x0F38, 0xFF, 0x66, 0x40, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  675 */ {Mnemonic::PHMINPOSUW, 0x0F38, 0xFF, 0x66, 0x41, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  676 */ {Mnemonic::INVEPT, 0x0F38, 0xFF, 0x66, 0x80, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Dword, 0xFF}, {AddrMode::MemOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  677 */ {Mnemonic::INVEPT, 0x0F38, 0xFF, 0x66, 0x80, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Qword, 0xFF}, {AddrMode::MemOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  678 */ {Mnemonic::INVVPID, 0x0F38, 0xFF, 0x66, 0x81, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Dword, 0xFF}, {AddrMode::MemOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  679 */ {Mnemonic::INVVPID, 0x0F38, 0xFF, 0x66, 0x81, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Qword, 0xFF}, {AddrMode::MemOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  680 */ {Mnemonic::MOVBE, 0x0F38, 0xFF, 0x00, 0xF0, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  681 */ {Mnemonic::CRC32, 0x0F38, 0xFF, 0xF2, 0xF0, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  682 */ {Mnemonic::MOVBE, 0x0F38, 0xFF, 0x00, 0xF1, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  683 */ {Mnemonic::CRC32, 0x0F38, 0xFF, 0xF2, 0xF1, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  684 */ {Mnemonic::ROUNDPS, 0x0F3A, 0xFF, 0x66, 0x08, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  685 */ {Mnemonic::ROUNDPD, 0x0F3A, 0xFF, 0x66, 0x09, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  686 */ {Mnemonic::ROUNDSS, 0x0F3A, 0xFF, 0x66, 0x0A, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  687 */ {Mnemonic::ROUNDSD, 0x0F3A, 0xFF, 0x66, 0x0B, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  688 */ {Mnemonic::BLENDPS, 0x0F3A, 0xFF, 0x66, 0x0C, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  689 */ {Mnemonic::BLENDPD, 0x0F3A, 0xFF, 0x66, 0x0D, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  690 */ {Mnemonic::PBLENDW, 0x0F3A, 0xFF, 0x66, 0x0E, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  691 */ {Mnemonic::PALIGNR, 0x0F3A, 0xFF, 0x00, 0x0F, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  692 */ {Mnemonic::PALIGNR, 0x0F3A, 0xFF, 0x66, 0x0F, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  693 */ {Mnemonic::PEXTRB, 0x0F3A, 0xFF, 0x66, 0x14, true, false, 1, 3, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  694 */ {Mnemonic::PEXTRW, 0x0F3A, 0xFF, 0x66, 0x15, true, false, 1, 3, 0, {{AddrMode::MemOnly, OpSize::Word, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  695 */ {Mnemonic::PEXTRD, 0x0F3A, 0xFF, 0x66, 0x16, true, false, 0, 3, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  696 */ {Mnemonic::EXTRACTPS, 0x0F3A, 0xFF, 0x66, 0x17, true, false, 0, 3, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  697 */ {Mnemonic::PINSRB, 0x0F3A, 0xFF, 0x66, 0x20, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  698 */ {Mnemonic::INSERTPS, 0x0F3A, 0xFF, 0x66, 0x21, true, false, 1, 3, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRegOnly, OpSize::PackedSingle, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  699 */ {Mnemonic::PINSRD, 0x0F3A, 0xFF, 0x66, 0x22, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  700 */ {Mnemonic::DPPS, 0x0F3A, 0xFF, 0x66, 0x40, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  701 */ {Mnemonic::DPPD, 0x0F3A, 0xFF, 0x66, 0x41, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  702 */ {Mnemonic::MPSADBW, 0x0F3A, 0xFF, 0x66, 0x42, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  703 */ {Mnemonic::PCMPESTRM, 0x0F3A, 0xFF, 0x66, 0x60, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /*  704 */ {Mnemonic::PCMPESTRI, 0x0F3A, 0xFF, 0x66, 0x61, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /*  705 */ {Mnemonic::PCMPISTRM, 0x0F3A, 0xFF, 0x66, 0x62, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /*  706 */ {Mnemonic::PCMPISTRI, 0x0F3A, 0xFF, 0x66, 0x63, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /*  707 */ {Mnemonic::CMOVO, 0x0F40, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  708 */ {Mnemonic::CMOVNO, 0x0F41, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  709 */ {Mnemonic::CMOVB, 0x0F42, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  710 */ {Mnemonic::CMOVNB, 0x0F43, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  711 */ {Mnemonic::CMOVZ, 0x0F44, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  712 */ {Mnemonic::CMOVNZ, 0x0F45, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  713 */ {Mnemonic::CMOVBE, 0x0F46, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  714 */ {Mnemonic::CMOVNBE, 0x0F47, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  715 */ {Mnemonic::CMOVS, 0x0F48, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  716 */ {Mnemonic::CMOVNS, 0x0F49, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  717 */ {Mnemonic::CMOVP, 0x0F4A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  718 */ {Mnemonic::CMOVNP, 0x0F4B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  719 */ {Mnemonic::CMOVL, 0x0F4C, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  720 */ {Mnemonic::CMOVNL, 0x0F4D, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  721 */ {Mnemonic::CMOVLE, 0x0F4E, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  722 */ {Mnemonic::CMOVNLE, 0x0F4F, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  723 */ {Mnemonic::MOVMSKPS, 0x0F50, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::XmmRegOnly, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  724 */ {Mnemonic::MOVMSKPD, 0x0F50, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::XmmRegOnly, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  725 */ {Mnemonic::SQRTPS, 0x0F51, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  726 */ {Mnemonic::SQRTSS, 0x0F51, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  727 */ {Mnemonic::SQRTPD, 0x0F51, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  728 */ {Mnemonic::SQRTSD, 0x0F51, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  729 */ {Mnemonic::RSQRTPS, 0x0F52, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  730 */ {Mnemonic::RSQRTSS, 0x0F52, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  731 */ {Mnemonic::RCPPS, 0x0F53, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  732 */ {Mnemonic::RCPSS, 0x0F53, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  733 */ {Mnemonic::ANDPS, 0x0F54, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  734 */ {Mnemonic::ANDPD, 0x0F54, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  735 */ {Mnemonic::ANDNPS, 0x0F55, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  736 */ {Mnemonic::ANDNPD, 0x0F55, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  737 */ {Mnemonic::ORPS, 0x0F56, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  738 */ {Mnemonic::ORPD, 0x0F56, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  739 */ {Mnemonic::XORPS, 0x0F57, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  740 */ {Mnemonic::XORPD, 0x0F57, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  741 */ {Mnemonic::ADDPS, 0x0F58, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  742 */ {Mnemonic::ADDSS, 0x0F58, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  743 */ {Mnemonic::ADDPD, 0x0F58, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  744 */ {Mnemonic::ADDSD, 0x0F58, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  745 */ {Mnemonic::MULPS, 0x0F59, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  746 */ {Mnemonic::MULSS, 0x0F59, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  747 */ {Mnemonic::MULPD, 0x0F59, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  748 */ {Mnemonic::MULSD, 0x0F59, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  749 */ {Mnemonic::CVTPS2PD, 0x0F5A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  750 */ {Mnemonic::CVTPD2PS, 0x0F5A, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  751 */ {Mnemonic::CVTSS2SD, 0x0F5A, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  752 */ {Mnemonic::CVTSD2SS, 0x0F5A, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  753 */ {Mnemonic::CVTDQ2PS, 0x0F5B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  754 */ {Mnemonic::CVTPS2DQ, 0x0F5B, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  755 */ {Mnemonic::CVTTPS2DQ, 0x0F5B, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  756 */ {Mnemonic::SUBPS, 0x0F5C, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  757 */ {Mnemonic::SUBSS, 0x0F5C, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  758 */ {Mnemonic::SUBPD, 0x0F5C, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  759 */ {Mnemonic::SUBSD, 0x0F5C, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  760 */ {Mnemonic::MINPS, 0x0F5D, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  761 */ {Mnemonic::MINSS, 0x0F5D, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  762 */ {Mnemonic::MINPD, 0x0F5D, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  763 */ {Mnemonic::MINSD, 0x0F5D, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  764 */ {Mnemonic::DIVPS, 0x0F5E, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  765 */ {Mnemonic::DIVSS, 0x0F5E, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  766 */ {Mnemonic::DIVPD, 0x0F5E, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  767 */ {Mnemonic::DIVSD, 0x0F5E, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  768 */ {Mnemonic::MAXPS, 0x0F5F, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  769 */ {Mnemonic::MAXSS, 0x0F5F, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  770 */ {Mnemonic::MAXPD, 0x0F5F, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  771 */ {Mnemonic::MAXSD, 0x0F5F, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  772 */ {Mnemonic::PUNPCKLBW, 0x0F60, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  773 */ {Mnemonic::PUNPCKLBW, 0x0F60, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  774 */ {Mnemonic::PUNPCKLWD, 0x0F61, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  775 */ {Mnemonic::PUNPCKLWD, 0x0F61, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  776 */ {Mnemonic::PUNPCKLDQ, 0x0F62, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  777 */ {Mnemonic::PUNPCKLDQ, 0x0F62, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  778 */ {Mnemonic::PACKSSWB, 0x0F63, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  779 */ {Mnemonic::PACKSSWB, 0x0F63, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  780 */ {Mnemonic::PCMPGTB, 0x0F64, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  781 */ {Mnemonic::PCMPGTB, 0x0F64, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  782 */ {Mnemonic::PCMPGTW, 0x0F65, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  783 */ {Mnemonic::PCMPGTW, 0x0F65, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  784 */ {Mnemonic::PCMPGTD, 0x0F66, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  785 */ {Mnemonic::PCMPGTD, 0x0F66, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  786 */ {Mnemonic::PACKUSWB, 0x0F67, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  787 */ {Mnemonic::PACKUSWB, 0x0F67, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  788 */ {Mnemonic::PUNPCKHBW, 0x0F68, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  789 */ {Mnemonic::PUNPCKHBW, 0x0F68, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  790 */ {Mnemonic::PUNPCKHWD, 0x0F69, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  791 */ {Mnemonic::PUNPCKHWD, 0x0F69, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  792 */ {Mnemonic::PUNPCKHDQ, 0x0F6A, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  793 */ {Mnemonic::PUNPCKHDQ, 0x0F6A, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  794 */ {Mnemonic::PACKSSDW, 0x0F6B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  795 */ {Mnemonic::PACKSSDW, 0x0F6B, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  796 */ {Mnemonic::PUNPCKLQDQ, 0x0F6C, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  797 */ {Mnemonic::PUNPCKHQDQ, 0x0F6D, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  798 */ {Mnemonic::MOVD, 0x0F6E, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  799 */ {Mnemonic::MOVD, 0x0F6E, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  800 */ {Mnemonic::MOVD, 0x0F6E, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  801 */ {Mnemonic::MOVD, 0x0F6E, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  802 */ {Mnemonic::MOVQ, 0x0F6F, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  803 */ {Mnemonic::MOVDQA, 0x0F6F, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  804 */ {Mnemonic::MOVDQU, 0x0F6F, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  805 */ {Mnemonic::PSHUFW, 0x0F70, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  806 */ {Mnemonic::PSHUFLW, 0x0F70, 0xFF, 0xF2, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  807 */ {Mnemonic::PSHUFHW, 0x0F70, 0xFF, 0xF3, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  808 */ {Mnemonic::PSHUFD, 0x0F70, 0xFF, 0x66, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  809 */ {Mnemonic::PSRLW, 0x0F71, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  810 */ {Mnemonic::PSRLW, 0x0F71, 0x02, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  811 */ {Mnemonic::PSRAW, 0x0F71, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  812 */ {Mnemonic::PSRAW, 0x0F71, 0x04, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  813 */ {Mnemonic::PSLLW, 0x0F71, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  814 */ {Mnemonic::PSLLW, 0x0F71, 0x06, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  815 */ {Mnemonic::PSRLD, 0x0F72, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  816 */ {Mnemonic::PSRLD, 0x0F72, 0x02, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  817 */ {Mnemonic::PSRAD, 0x0F72, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  818 */ {Mnemonic::PSRAD, 0x0F72, 0x04, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  819 */ {Mnemonic::PSLLD, 0x0F72, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  820 */ {Mnemonic::PSLLD, 0x0F72, 0x06, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  821 */ {Mnemonic::PSRLQ, 0x0F73, 0x02, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  822 */ {Mnemonic::PSRLQ, 0x0F73, 0x02, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  823 */ {Mnemonic::PSRLDQ, 0x0F73, 0x03, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  824 */ {Mnemonic::PSLLQ, 0x0F73, 0x06, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  825 */ {Mnemonic::PSLLQ, 0x0F73, 0x06, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  826 */ {Mnemonic::PSLLDQ, 0x0F73, 0x07, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  827 */ {Mnemonic::PCMPEQB, 0x0F74, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  828 */ {Mnemonic::PCMPEQB, 0x0F74, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  829 */ {Mnemonic::PCMPEQW, 0x0F75, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  830 */ {Mnemonic::PCMPEQW, 0x0F75, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  831 */ {Mnemonic::PCMPEQD, 0x0F76, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  832 */ {Mnemonic::PCMPEQD, 0x0F76, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  833 */ {Mnemonic::EMMS, 0x0F77, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  834 */ {Mnemonic::VMREAD, 0x0F78, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  835 */ {Mnemonic::VMREAD, 0x0F78, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Qword, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  836 */ {Mnemonic::VMWRITE, 0x0F79, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Dword, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  837 */ {Mnemonic::VMWRITE, 0x0F79, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Qword, 0xFF}, {AddrMode::ModRM_RM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  838 */ {Mnemonic::HADDPD, 0x0F7C, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  839 */ {Mnemonic::HADDPS, 0x0F7C, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  840 */ {Mnemonic::HSUBPD, 0x0F7D, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  841 */ {Mnemonic::HSUBPS, 0x0F7D, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  842 */ {Mnemonic::MOVD, 0x0F7E, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  843 */ {Mnemonic::MOVD, 0x0F7E, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  844 */ {Mnemonic::MOVD, 0x0F7E, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  845 */ {Mnemonic::MOVD, 0x0F7E, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  846 */ {Mnemonic::MOVQ, 0x0F7E, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::XmmRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  847 */ {Mnemonic::MOVQ, 0x0F7F, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  848 */ {Mnemonic::MOVDQA, 0x0F7F, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  849 */ {Mnemonic::MOVDQU, 0x0F7F, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  850 */ {Mnemonic::JO, 0x0F80, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  851 */ {Mnemonic::JNO, 0x0F81, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  852 */ {Mnemonic::JB, 0x0F82, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  853 */ {Mnemonic::JNB, 0x0F83, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  854 */ {Mnemonic::JZ, 0x0F84, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  855 */ {Mnemonic::JNZ, 0x0F85, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  856 */ {Mnemonic::JBE, 0x0F86, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  857 */ {Mnemonic::JNBE, 0x0F87, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  858 */ {Mnemonic::JS, 0x0F88, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  859 */ {Mnemonic::JNS, 0x0F89, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  860 */ {Mnemonic::JP, 0x0F8A, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  861 */ {Mnemonic::JNP, 0x0F8B, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  862 */ {Mnemonic::JL, 0x0F8C, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  863 */ {Mnemonic::JNL, 0x0F8D, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  864 */ {Mnemonic::JLE, 0x0F8E, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  865 */ {Mnemonic::JNLE, 0x0F8F, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::OpSzDS, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  866 */ {Mnemonic::SETO, 0x0F90, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  867 */ {Mnemonic::SETNO, 0x0F91, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  868 */ {Mnemonic::SETB, 0x0F92, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  869 */ {Mnemonic::SETNB, 0x0F93, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  870 */ {Mnemonic::SETZ, 0x0F94, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  871 */ {Mnemonic::SETNZ, 0x0F95, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  872 */ {Mnemonic::SETBE, 0x0F96, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  873 */ {Mnemonic::SETNBE, 0x0F97, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  874 */ {Mnemonic::SETS, 0x0F98, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  875 */ {Mnemonic::SETNS, 0x0F99, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  876 */ {Mnemonic::SETP, 0x0F9A, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  877 */ {Mnemonic::SETNP, 0x0F9B, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  878 */ {Mnemonic::SETL, 0x0F9C, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  879 */ {Mnemonic::SETNL, 0x0F9D, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  880 */ {Mnemonic::SETLE, 0x0F9E, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  881 */ {Mnemonic::SETNLE, 0x0F9F, 0xFF, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  882 */ {Mnemonic::PUSH, 0x0FA0, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x04}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  883 */ {Mnemonic::POP, 0x0FA1, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x04}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  884 */ {Mnemonic::CPUID, 0x0FA2, 0xFF, 0x00, 0xFF, false, false, 0, 4, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::Dword, 0x01}, {AddrMode::Fixed, OpSize::Dword, 0x02}}},
    /*  885 */ {Mnemonic::BT, 0x0FA3, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  886 */ {Mnemonic::SHLD, 0x0FA4, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  887 */ {Mnemonic::SHLD, 0x0FA5, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  888 */ {Mnemonic::PUSH, 0x0FA8, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Word, 0x05}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  889 */ {Mnemonic::POP, 0x0FA9, 0xFF, 0x00, 0xFF, false, false, 0, 2, 0, {{AddrMode::Fixed, OpSize::Word, 0x05}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  890 */ {Mnemonic::RSM, 0x0FAA, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::None, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  891 */ {Mnemonic::BTS, 0x0FAB, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  892 */ {Mnemonic::SHRD, 0x0FAC, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  893 */ {Mnemonic::SHRD, 0x0FAD, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x01}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  894 */ {Mnemonic::FXSAVE, 0x0FAE, 0x00, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}}},
    /*  895 */ {Mnemonic::FXSAVE, 0x0FAE, 0x00, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}}},
    /*  896 */ {Mnemonic::FXRSTOR, 0x0FAE, 0x01, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x03}}},
    /*  897 */ {Mnemonic::FXRSTOR, 0x0FAE, 0x01, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x03}}},
    /*  898 */ {Mnemonic::LDMXCSR, 0x0FAE, 0x02, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  899 */ {Mnemonic::STMXCSR, 0x0FAE, 0x03, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  900 */ {Mnemonic::XSAVE, 0x0FAE, 0x04, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::None, 0x00}}},
    /*  901 */ {Mnemonic::XSAVE, 0x0FAE, 0x04, 0x00, 0xFF, true, false, 0, 4, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::None, 0x00}}},
    /*  902 */ {Mnemonic::LFENCE, 0x0FAE, 0x05, 0x00, 0xFF, true, false, -1, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  903 */ {Mnemonic::XRSTOR, 0x0FAE, 0x05, 0x00, 0xFF, true, false, 1, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x03}}},
    /*  904 */ {Mnemonic::XRSTOR, 0x0FAE, 0x05, 0x00, 0xFF, true, false, 1, 4, 0, {{AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::Fixed, OpSize::None, 0x01}, {AddrMode::Fixed, OpSize::None, 0x02}, {AddrMode::Fixed, OpSize::None, 0x03}}},
    /*  905 */ {Mnemonic::MFENCE, 0x0FAE, 0x06, 0x00, 0xFF, true, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  906 */ {Mnemonic::SFENCE, 0x0FAE, 0x07, 0x00, 0xFF, true, false, -1, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  907 */ {Mnemonic::CLFLUSH, 0x0FAE, 0x07, 0x00, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  908 */ {Mnemonic::IMUL, 0x0FAF, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  909 */ {Mnemonic::CMPXCHG, 0x0FB0, 0xFF, 0x00, 0xFF, true, true, 0, 3, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::Fixed, OpSize::Byte, 0x00}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  910 */ {Mnemonic::CMPXCHG, 0x0FB1, 0xFF, 0x00, 0xFF, true, true, 0, 3, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Fixed, OpSize::None, 0x00}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  911 */ {Mnemonic::LSS, 0x0FB2, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Word, 0x02}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  912 */ {Mnemonic::BTR, 0x0FB3, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  913 */ {Mnemonic::LFS, 0x0FB4, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Word, 0x04}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  914 */ {Mnemonic::LGS, 0x0FB5, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::Fixed, OpSize::Word, 0x05}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  915 */ {Mnemonic::MOVZX, 0x0FB6, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  916 */ {Mnemonic::MOVZX, 0x0FB7, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  917 */ {Mnemonic::JMPE, 0x0FB8, 0xFF, 0x00, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  918 */ {Mnemonic::POPCNT, 0x0FB8, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  919 */ {Mnemonic::UD, 0x0FB9, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  920 */ {Mnemonic::BT, 0x0FBA, 0x04, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  921 */ {Mnemonic::BTS, 0x0FBA, 0x05, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  922 */ {Mnemonic::BTR, 0x0FBA, 0x06, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  923 */ {Mnemonic::BTC, 0x0FBA, 0x07, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  924 */ {Mnemonic::BTC, 0x0FBB, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  925 */ {Mnemonic::BSF, 0x0FBC, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  926 */ {Mnemonic::BSR, 0x0FBD, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  927 */ {Mnemonic::MOVSX, 0x0FBE, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  928 */ {Mnemonic::MOVSX, 0x0FBF, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::Word, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  929 */ {Mnemonic::XADD, 0x0FC0, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::Byte, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  930 */ {Mnemonic::XADD, 0x0FC1, 0xFF, 0x00, 0xFF, true, true, 0, 2, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  931 */ {Mnemonic::CMPPS, 0x0FC2, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  932 */ {Mnemonic::CMPSS, 0x0FC2, 0xFF, 0xF3, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::ScalarSingle, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarSingle, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  933 */ {Mnemonic::CMPPD, 0x0FC2, 0xFF, 0x66, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  934 */ {Mnemonic::CMPSD, 0x0FC2, 0xFF, 0xF2, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::ScalarDouble, 0xFF}, {AddrMode::XmmRM, OpSize::ScalarDouble, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  935 */ {Mnemonic::MOVNTI, 0x0FC3, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  936 */ {Mnemonic::PINSRW, 0x0FC4, 0xFF, 0x00, 0xFF, true, false, -1, 3, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::RegOnly, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  937 */ {Mnemonic::PINSRW, 0x0FC4, 0xFF, 0x66, 0xFF, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::RegOnly, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  938 */ {Mnemonic::PEXTRW, 0x0FC5, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  939 */ {Mnemonic::PEXTRW, 0x0FC5, 0xFF, 0x66, 0xFF, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  940 */ {Mnemonic::SHUFPS, 0x0FC6, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  941 */ {Mnemonic::SHUFPD, 0x0FC6, 0xFF, 0x66, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  942 */ {Mnemonic::CMPXCHG8B, 0x0FC7, 0x01, 0x00, 0xFF, true, true, 0, 4, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::Fixed, OpSize::Dword, 0x03}}},
    /*  943 */ {Mnemonic::CMPXCHG8B, 0x0FC7, 0x01, 0x00, 0xFF, true, true, 0, 4, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::Fixed, OpSize::Dword, 0x00}, {AddrMode::Fixed, OpSize::Dword, 0x02}, {AddrMode::Fixed, OpSize::Dword, 0x03}}},
    /*  944 */ {Mnemonic::VMPTRLD, 0x0FC7, 0x06, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  945 */ {Mnemonic::VMCLEAR, 0x0FC7, 0x06, 0x66, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  946 */ {Mnemonic::VMXON, 0x0FC7, 0x06, 0xF3, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  947 */ {Mnemonic::VMPTRST, 0x0FC7, 0x07, 0x00, 0xFF, true, false, 0, 1, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  948 */ {Mnemonic::BSWAP, 0x0FC8, 0xFF, 0x00, 0xFF, false, false, 0, 1, 0, {{AddrMode::OpcodeReg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  949 */ {Mnemonic::ADDSUBPD, 0x0FD0, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  950 */ {Mnemonic::ADDSUBPS, 0x0FD0, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedSingle, 0xFF}, {AddrMode::XmmRM, OpSize::PackedSingle, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  951 */ {Mnemonic::PSRLW, 0x0FD1, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  952 */ {Mnemonic::PSRLW, 0x0FD1, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  953 */ {Mnemonic::PSRLD, 0x0FD2, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  954 */ {Mnemonic::PSRLD, 0x0FD2, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  955 */ {Mnemonic::PSRLQ, 0x0FD3, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  956 */ {Mnemonic::PSRLQ, 0x0FD3, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  957 */ {Mnemonic::PADDQ, 0x0FD4, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  958 */ {Mnemonic::PADDQ, 0x0FD4, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  959 */ {Mnemonic::PMULLW, 0x0FD5, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  960 */ {Mnemonic::PMULLW, 0x0FD5, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  961 */ {Mnemonic::MOVQ, 0x0FD6, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::Qword, 0xFF}, {AddrMode::XmmReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  962 */ {Mnemonic::MOVQ2DQ, 0x0FD6, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  963 */ {Mnemonic::MOVDQ2Q, 0x0FD6, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::XmmRegOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  964 */ {Mnemonic::PMOVMSKB, 0x0FD7, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  965 */ {Mnemonic::PMOVMSKB, 0x0FD7, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  966 */ {Mnemonic::PSUBUSB, 0x0FD8, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  967 */ {Mnemonic::PSUBUSB, 0x0FD8, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  968 */ {Mnemonic::PSUBUSW, 0x0FD9, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  969 */ {Mnemonic::PSUBUSW, 0x0FD9, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  970 */ {Mnemonic::PMINUB, 0x0FDA, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  971 */ {Mnemonic::PMINUB, 0x0FDA, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  972 */ {Mnemonic::PAND, 0x0FDB, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  973 */ {Mnemonic::PAND, 0x0FDB, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  974 */ {Mnemonic::PADDUSB, 0x0FDC, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  975 */ {Mnemonic::PADDUSB, 0x0FDC, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  976 */ {Mnemonic::PADDUSW, 0x0FDD, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  977 */ {Mnemonic::PADDUSW, 0x0FDD, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  978 */ {Mnemonic::PMAXUB, 0x0FDE, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  979 */ {Mnemonic::PMAXUB, 0x0FDE, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  980 */ {Mnemonic::PANDN, 0x0FDF, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  981 */ {Mnemonic::PANDN, 0x0FDF, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  982 */ {Mnemonic::PAVGB, 0x0FE0, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  983 */ {Mnemonic::PAVGB, 0x0FE0, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  984 */ {Mnemonic::PSRAW, 0x0FE1, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  985 */ {Mnemonic::PSRAW, 0x0FE1, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  986 */ {Mnemonic::PSRAD, 0x0FE2, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  987 */ {Mnemonic::PSRAD, 0x0FE2, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  988 */ {Mnemonic::PAVGW, 0x0FE3, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  989 */ {Mnemonic::PAVGW, 0x0FE3, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  990 */ {Mnemonic::PMULHUW, 0x0FE4, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  991 */ {Mnemonic::PMULHUW, 0x0FE4, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  992 */ {Mnemonic::PMULHW, 0x0FE5, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  993 */ {Mnemonic::PMULHW, 0x0FE5, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  994 */ {Mnemonic::CVTPD2DQ, 0x0FE6, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  995 */ {Mnemonic::CVTTPD2DQ, 0x0FE6, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::PackedDouble, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  996 */ {Mnemonic::CVTDQ2PD, 0x0FE6, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::PackedDouble, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  997 */ {Mnemonic::MOVNTQ, 0x0FE7, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  998 */ {Mnemonic::MOVNTDQ, 0x0FE7, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::MemOnly, OpSize::Xmm128, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /*  999 */ {Mnemonic::PSUBSB, 0x0FE8, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1000 */ {Mnemonic::PSUBSB, 0x0FE8, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1001 */ {Mnemonic::PSUBSW, 0x0FE9, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1002 */ {Mnemonic::PSUBSW, 0x0FE9, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1003 */ {Mnemonic::PMINSW, 0x0FEA, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1004 */ {Mnemonic::PMINSW, 0x0FEA, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1005 */ {Mnemonic::POR, 0x0FEB, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1006 */ {Mnemonic::POR, 0x0FEB, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1007 */ {Mnemonic::PADDSB, 0x0FEC, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1008 */ {Mnemonic::PADDSB, 0x0FEC, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1009 */ {Mnemonic::PADDSW, 0x0FED, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1010 */ {Mnemonic::PADDSW, 0x0FED, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1011 */ {Mnemonic::PMAXSW, 0x0FEE, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1012 */ {Mnemonic::PMAXSW, 0x0FEE, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1013 */ {Mnemonic::PXOR, 0x0FEF, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1014 */ {Mnemonic::PXOR, 0x0FEF, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1015 */ {Mnemonic::LDDQU, 0x0FF0, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1016 */ {Mnemonic::PSLLW, 0x0FF1, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1017 */ {Mnemonic::PSLLW, 0x0FF1, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1018 */ {Mnemonic::PSLLD, 0x0FF2, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1019 */ {Mnemonic::PSLLD, 0x0FF2, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1020 */ {Mnemonic::PSLLQ, 0x0FF3, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1021 */ {Mnemonic::PSLLQ, 0x0FF3, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1022 */ {Mnemonic::PMULUDQ, 0x0FF4, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1023 */ {Mnemonic::PMULUDQ, 0x0FF4, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1024 */ {Mnemonic::PMADDWD, 0x0FF5, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1025 */ {Mnemonic::PMADDWD, 0x0FF5, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1026 */ {Mnemonic::PSADBW, 0x0FF6, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1027 */ {Mnemonic::PSADBW, 0x0FF6, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1028 */ {Mnemonic::MASKMOVQ, 0x0FF7, 0xFF, 0x00, 0xFF, true, false, 0, 3, 0, {{AddrMode::None, OpSize::Qword, 0xFF}, {AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRegOnly, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1029 */ {Mnemonic::MASKMOVDQU, 0x0FF7, 0xFF, 0x66, 0xFF, true, false, 0, 3, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRegOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1030 */ {Mnemonic::PSUBB, 0x0FF8, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1031 */ {Mnemonic::PSUBB, 0x0FF8, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1032 */ {Mnemonic::PSUBW, 0x0FF9, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1033 */ {Mnemonic::PSUBW, 0x0FF9, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1034 */ {Mnemonic::PSUBD, 0x0FFA, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1035 */ {Mnemonic::PSUBD, 0x0FFA, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1036 */ {Mnemonic::PSUBQ, 0x0FFB, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1037 */ {Mnemonic::PSUBQ, 0x0FFB, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1038 */ {Mnemonic::PADDB, 0x0FFC, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1039 */ {Mnemonic::PADDB, 0x0FFC, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1040 */ {Mnemonic::PADDW, 0x0FFD, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1041 */ {Mnemonic::PADDW, 0x0FFD, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1042 */ {Mnemonic::PADDD, 0x0FFE, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::MmxReg, OpSize::Qword, 0xFF}, {AddrMode::MmxRM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1043 */ {Mnemonic::PADDD, 0x0FFE, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1044 */ {Mnemonic::VFMADD132PS, 0x0F38, 0xFF, 0x66, 0x98, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1045 */ {Mnemonic::VFMADD213PS, 0x0F38, 0xFF, 0x66, 0xA8, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1046 */ {Mnemonic::VFMADD231PS, 0x0F38, 0xFF, 0x66, 0xB8, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1047 */ {Mnemonic::VFMADD132SS, 0x0F38, 0xFF, 0x66, 0x99, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1048 */ {Mnemonic::VFMADD213SS, 0x0F38, 0xFF, 0x66, 0xA9, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1049 */ {Mnemonic::VFMADD231SS, 0x0F38, 0xFF, 0x66, 0xB9, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1050 */ {Mnemonic::VFMSUB132PS, 0x0F38, 0xFF, 0x66, 0x9A, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1051 */ {Mnemonic::VFMSUB213PS, 0x0F38, 0xFF, 0x66, 0xAA, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1052 */ {Mnemonic::VFMSUB231PS, 0x0F38, 0xFF, 0x66, 0xBA, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1053 */ {Mnemonic::VFMSUB132SS, 0x0F38, 0xFF, 0x66, 0x9B, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1054 */ {Mnemonic::VFMSUB213SS, 0x0F38, 0xFF, 0x66, 0xAB, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1055 */ {Mnemonic::VFMSUB231SS, 0x0F38, 0xFF, 0x66, 0xBB, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1056 */ {Mnemonic::VFNMADD132PS, 0x0F38, 0xFF, 0x66, 0x9C, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1057 */ {Mnemonic::VFNMADD213PS, 0x0F38, 0xFF, 0x66, 0xAC, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1058 */ {Mnemonic::VFNMADD231PS, 0x0F38, 0xFF, 0x66, 0xBC, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1059 */ {Mnemonic::VFNMADD132SS, 0x0F38, 0xFF, 0x66, 0x9D, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1060 */ {Mnemonic::VFNMADD213SS, 0x0F38, 0xFF, 0x66, 0xAD, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1061 */ {Mnemonic::VFNMADD231SS, 0x0F38, 0xFF, 0x66, 0xBD, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1062 */ {Mnemonic::VFNMSUB132PS, 0x0F38, 0xFF, 0x66, 0x9E, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1063 */ {Mnemonic::VFNMSUB213PS, 0x0F38, 0xFF, 0x66, 0xAE, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1064 */ {Mnemonic::VFNMSUB231PS, 0x0F38, 0xFF, 0x66, 0xBE, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1065 */ {Mnemonic::VFNMSUB132SS, 0x0F38, 0xFF, 0x66, 0x9F, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1066 */ {Mnemonic::VFNMSUB213SS, 0x0F38, 0xFF, 0x66, 0xAF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1067 */ {Mnemonic::VFNMSUB231SS, 0x0F38, 0xFF, 0x66, 0xBF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1068 */ {Mnemonic::VFMADDSUB132PS, 0x0F38, 0xFF, 0x66, 0x96, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1069 */ {Mnemonic::VFMADDSUB213PS, 0x0F38, 0xFF, 0x66, 0xA6, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1070 */ {Mnemonic::VFMADDSUB231PS, 0x0F38, 0xFF, 0x66, 0xB6, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1071 */ {Mnemonic::VFMSUBADD132PS, 0x0F38, 0xFF, 0x66, 0x97, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1072 */ {Mnemonic::VFMSUBADD213PS, 0x0F38, 0xFF, 0x66, 0xA7, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1073 */ {Mnemonic::VFMSUBADD231PS, 0x0F38, 0xFF, 0x66, 0xB7, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1074 */ {Mnemonic::VBROADCASTSS, 0x0F38, 0xFF, 0x66, 0x18, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1075 */ {Mnemonic::VBROADCASTSD, 0x0F38, 0xFF, 0x66, 0x19, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1076 */ {Mnemonic::VBROADCASTF128, 0x0F38, 0xFF, 0x66, 0x1A, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1077 */ {Mnemonic::VPBROADCASTD, 0x0F38, 0xFF, 0x66, 0x58, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1078 */ {Mnemonic::VPBROADCASTQ, 0x0F38, 0xFF, 0x66, 0x59, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1079 */ {Mnemonic::VPBROADCASTB, 0x0F38, 0xFF, 0x66, 0x78, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1080 */ {Mnemonic::VPBROADCASTW, 0x0F38, 0xFF, 0x66, 0x79, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1081 */ {Mnemonic::VPERMILPS, 0x0F38, 0xFF, 0x66, 0x0C, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1082 */ {Mnemonic::VPERMILPD, 0x0F38, 0xFF, 0x66, 0x0D, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1083 */ {Mnemonic::VPERMD, 0x0F38, 0xFF, 0x66, 0x36, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1084 */ {Mnemonic::VPERM2F128, 0x0F3A, 0xFF, 0x66, 0x06, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1085 */ {Mnemonic::VPERM2I128, 0x0F3A, 0xFF, 0x66, 0x46, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1086 */ {Mnemonic::VINSERTF128, 0x0F3A, 0xFF, 0x66, 0x18, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1087 */ {Mnemonic::VINSERTI128, 0x0F3A, 0xFF, 0x66, 0x38, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1088 */ {Mnemonic::VPERMQ, 0x0F3A, 0xFF, 0x66, 0x00, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1089 */ {Mnemonic::VEXTRACTF128, 0x0F3A, 0xFF, 0x66, 0x19, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1090 */ {Mnemonic::VEXTRACTI128, 0x0F3A, 0xFF, 0x66, 0x39, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1091 */ {Mnemonic::ANDN, 0x0F38, 0xFF, 0x00, 0xF2, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1092 */ {Mnemonic::BEXTR, 0x0F38, 0xFF, 0x00, 0xF7, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1093 */ {Mnemonic::BLSI, 0x0F38, 0x03, 0x00, 0xF3, true, false, 0, 2, 0, {{AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1094 */ {Mnemonic::BLSMSK, 0x0F38, 0x02, 0x00, 0xF3, true, false, 0, 2, 0, {{AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1095 */ {Mnemonic::BLSR, 0x0F38, 0x01, 0x00, 0xF3, true, false, 0, 2, 0, {{AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1096 */ {Mnemonic::TZCNT, 0x0FBC, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1097 */ {Mnemonic::LZCNT, 0x0FBD, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1098 */ {Mnemonic::BZHI, 0x0F38, 0xFF, 0x00, 0xF5, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1099 */ {Mnemonic::MULX, 0x0F38, 0xFF, 0xF2, 0xF6, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1100 */ {Mnemonic::PDEP, 0x0F38, 0xFF, 0xF2, 0xF5, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1101 */ {Mnemonic::PEXT, 0x0F38, 0xFF, 0xF3, 0xF5, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1102 */ {Mnemonic::RORX, 0x0F3A, 0xFF, 0xF2, 0xF0, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1103 */ {Mnemonic::SARX, 0x0F38, 0xFF, 0xF3, 0xF7, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1104 */ {Mnemonic::SHLX, 0x0F38, 0xFF, 0x66, 0xF7, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1105 */ {Mnemonic::SHRX, 0x0F38, 0xFF, 0xF2, 0xF7, true, false, 0, 3, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::VexReg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1106 */ {Mnemonic::ADCX, 0x0F38, 0xFF, 0x66, 0xF6, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1107 */ {Mnemonic::ADOX, 0x0F38, 0xFF, 0xF3, 0xF6, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1108 */ {Mnemonic::RDRAND, 0x0FC7, 0x06, 0x00, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1109 */ {Mnemonic::RDSEED, 0x0FC7, 0x07, 0x00, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1110 */ {Mnemonic::AESDEC, 0x0F38, 0xFF, 0x66, 0xDE, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1111 */ {Mnemonic::AESDECLAST, 0x0F38, 0xFF, 0x66, 0xDF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1112 */ {Mnemonic::AESENC, 0x0F38, 0xFF, 0x66, 0xDC, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1113 */ {Mnemonic::AESENCLAST, 0x0F38, 0xFF, 0x66, 0xDD, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1114 */ {Mnemonic::AESIMC, 0x0F38, 0xFF, 0x66, 0xDB, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1115 */ {Mnemonic::AESKEYGENASSIST, 0x0F3A, 0xFF, 0x66, 0xDF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1116 */ {Mnemonic::PCLMULQDQ, 0x0F3A, 0xFF, 0x66, 0x44, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1117 */ {Mnemonic::SHA1MSG1, 0x0F38, 0xFF, 0x00, 0xC9, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1118 */ {Mnemonic::SHA1MSG2, 0x0F38, 0xFF, 0x00, 0xCA, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1119 */ {Mnemonic::SHA1NEXTE, 0x0F38, 0xFF, 0x00, 0xC8, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1120 */ {Mnemonic::SHA1RNDS4, 0x0F3A, 0xFF, 0x00, 0xCC, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1121 */ {Mnemonic::SHA256MSG1, 0x0F38, 0xFF, 0x00, 0xCC, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1122 */ {Mnemonic::SHA256MSG2, 0x0F38, 0xFF, 0x00, 0xCD, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1123 */ {Mnemonic::SHA256RNDS2, 0x0F38, 0xFF, 0x00, 0xCB, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1124 */ {Mnemonic::RDFSBASE, 0x0FAE, 0x00, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1125 */ {Mnemonic::RDGSBASE, 0x0FAE, 0x01, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1126 */ {Mnemonic::WRFSBASE, 0x0FAE, 0x02, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1127 */ {Mnemonic::WRGSBASE, 0x0FAE, 0x03, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1128 */ {Mnemonic::XEND, 0x0F01, 0xFF, 0x00, 0xD5, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1129 */ {Mnemonic::XABORT, 0x00C6, 0xFF, 0x00, 0xF8, false, false, 0, 1, 0, {{AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1130 */ {Mnemonic::XTEST, 0x0F01, 0xFF, 0x00, 0xD6, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1131 */ {Mnemonic::XSAVEC, 0x0FC7, 0x04, 0x00, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1132 */ {Mnemonic::XSAVEOPT, 0x0FAE, 0x06, 0x00, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1133 */ {Mnemonic::XSAVES, 0x0FC7, 0x05, 0x00, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1134 */ {Mnemonic::XRSTORS, 0x0FC7, 0x03, 0x00, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1135 */ {Mnemonic::ENDBR32, 0x0F1E, 0xFF, 0xF3, 0xFB, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1136 */ {Mnemonic::ENDBR64, 0x0F1E, 0xFF, 0xF3, 0xFA, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1137 */ {Mnemonic::CLFLUSHOPT, 0x0FAE, 0x07, 0x66, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1138 */ {Mnemonic::CLWB, 0x0FAE, 0x06, 0x66, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1139 */ {Mnemonic::INVPCID, 0x0F38, 0xFF, 0x66, 0x82, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1140 */ {Mnemonic::SERIALIZE, 0x0F01, 0xFF, 0x00, 0xE8, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1141 */ {Mnemonic::WBNOINVD, 0x0F09, 0xFF, 0xF3, 0xFF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1142 */ {Mnemonic::RDPID, 0x0FC7, 0x07, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1143 */ {Mnemonic::WRPKRU, 0x0F01, 0xFF, 0x00, 0xEF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1144 */ {Mnemonic::RDPKRU, 0x0F01, 0xFF, 0x00, 0xEE, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1145 */ {Mnemonic::MOVDIRI, 0x0F38, 0xFF, 0x00, 0xF9, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::OpSzQ, 0xFF}, {AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1146 */ {Mnemonic::STAC, 0x0F01, 0xFF, 0x00, 0xCB, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1147 */ {Mnemonic::CLAC, 0x0F01, 0xFF, 0x00, 0xCA, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1148 */ {Mnemonic::GF2P8AFFINEINVQB, 0x0F3A, 0xFF, 0x66, 0xCF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1149 */ {Mnemonic::GF2P8AFFINEQB, 0x0F3A, 0xFF, 0x66, 0xCE, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1150 */ {Mnemonic::GF2P8MULB, 0x0F38, 0xFF, 0x66, 0xCF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::XmmRM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1151 */ {Mnemonic::TPAUSE, 0x0FAE, 0x06, 0x66, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1152 */ {Mnemonic::UMONITOR, 0x0FAE, 0x06, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1153 */ {Mnemonic::UMWAIT, 0x0FAE, 0x06, 0xF2, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1154 */ {Mnemonic::MOVDIR64B, 0x0F38, 0xFF, 0x66, 0xF8, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1155 */ {Mnemonic::PREFETCHW, 0x0F0D, 0x01, 0x00, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1156 */ {Mnemonic::XBEGIN, 0x00C7, 0xFF, 0x00, 0xF8, false, false, 0, 1, 0, {{AddrMode::RelOffset, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1157 */ {Mnemonic::VMFUNC, 0x0F01, 0xFF, 0x00, 0xD4, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1158 */ {Mnemonic::PTWRITE, 0x0FAE, 0x04, 0xF3, 0xFF, true, false, 0, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1159 */ {Mnemonic::INCSSPD, 0x0FAE, 0x05, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1160 */ {Mnemonic::INCSSPQ, 0x0FAE, 0x05, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1161 */ {Mnemonic::RDSSPD, 0x0F1E, 0x01, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1162 */ {Mnemonic::RDSSPQ, 0x0F1E, 0x01, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1163 */ {Mnemonic::CLRSSBSY, 0x0FAE, 0x06, 0xF3, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1164 */ {Mnemonic::RSTORSSP, 0x0F01, 0x05, 0xF3, 0xFF, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1165 */ {Mnemonic::SAVEPREVSSP, 0x0F01, 0xFF, 0xF3, 0xEA, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1166 */ {Mnemonic::SETSSBSY, 0x0F01, 0xFF, 0xF3, 0xE8, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1167 */ {Mnemonic::WRSSD, 0x0F38, 0xFF, 0x00, 0xF6, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1168 */ {Mnemonic::WRSSQ, 0x0F38, 0xFF, 0x00, 0xF6, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1169 */ {Mnemonic::WRUSSD, 0x0F38, 0xFF, 0x66, 0xF5, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Dword, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1170 */ {Mnemonic::WRUSSQ, 0x0F38, 0xFF, 0x66, 0xF5, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::Qword, 0xFF}, {AddrMode::ModRM_Reg, OpSize::Qword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1171 */ {Mnemonic::CLUI, 0x0F01, 0xFF, 0xF3, 0xEE, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1172 */ {Mnemonic::STUI, 0x0F01, 0xFF, 0xF3, 0xEF, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1173 */ {Mnemonic::TESTUI, 0x0F01, 0xFF, 0xF3, 0xED, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1174 */ {Mnemonic::UIRET, 0x0F01, 0xFF, 0xF3, 0xEC, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1175 */ {Mnemonic::SENDUIPI, 0x0FC7, 0x06, 0xF3, 0xFF, true, false, -1, 1, 0, {{AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1176 */ {Mnemonic::BNDLDX, 0x0F1A, 0xFF, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1177 */ {Mnemonic::BNDSTX, 0x0F1B, 0xFF, 0x00, 0xFF, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1178 */ {Mnemonic::BNDMK, 0x0F1B, 0xFF, 0xF3, 0xFF, true, false, 1, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1179 */ {Mnemonic::BNDCL, 0x0F1A, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1180 */ {Mnemonic::BNDCU, 0x0F1A, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1181 */ {Mnemonic::BNDCN, 0x0F1B, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1182 */ {Mnemonic::BNDMOV, 0x0F1A, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::Xmm128, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1183 */ {Mnemonic::AESENC128KL, 0x0F38, 0xFF, 0xF3, 0xDC, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1184 */ {Mnemonic::AESDEC128KL, 0x0F38, 0xFF, 0xF3, 0xDD, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1185 */ {Mnemonic::AESENC256KL, 0x0F38, 0xFF, 0xF3, 0xDE, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1186 */ {Mnemonic::AESDEC256KL, 0x0F38, 0xFF, 0xF3, 0xDF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::Xmm128, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1187 */ {Mnemonic::AESENCWIDE128KL, 0x0F38, 0x00, 0xF3, 0xD8, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1188 */ {Mnemonic::AESDECWIDE128KL, 0x0F38, 0x01, 0xF3, 0xD8, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1189 */ {Mnemonic::AESENCWIDE256KL, 0x0F38, 0x02, 0xF3, 0xD8, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1190 */ {Mnemonic::AESDECWIDE256KL, 0x0F38, 0x03, 0xF3, 0xD8, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1191 */ {Mnemonic::ENCODEKEY128, 0x0F38, 0xFF, 0xF3, 0xFA, true, false, -1, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Dword, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1192 */ {Mnemonic::ENCODEKEY256, 0x0F38, 0xFF, 0xF3, 0xFB, true, false, -1, 2, 0, {{AddrMode::ModRM_Reg, OpSize::Dword, 0xFF}, {AddrMode::ModRM_RM, OpSize::Dword, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1193 */ {Mnemonic::ENQCMD, 0x0F38, 0xFF, 0xF2, 0xF8, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1194 */ {Mnemonic::ENQCMDS, 0x0F38, 0xFF, 0xF3, 0xF8, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1195 */ {Mnemonic::HRESET, 0x0F3A, 0x00, 0xF3, 0xF0, true, false, -1, 1, 0, {{AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1196 */ {Mnemonic::VPDPBUSD, 0x0F38, 0xFF, 0x66, 0x50, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1197 */ {Mnemonic::VPDPBUSDS, 0x0F38, 0xFF, 0x66, 0x51, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1198 */ {Mnemonic::VPDPWSSD, 0x0F38, 0xFF, 0x66, 0x52, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1199 */ {Mnemonic::VPDPWSSDS, 0x0F38, 0xFF, 0x66, 0x53, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1200 */ {Mnemonic::VPMADD52LUQ, 0x0F38, 0xFF, 0x66, 0xB4, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1201 */ {Mnemonic::VPMADD52HUQ, 0x0F38, 0xFF, 0x66, 0xB5, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1202 */ {Mnemonic::PCONFIG, 0x0F01, 0xFF, 0x00, 0xC5, false, false, 0, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1203 */ {Mnemonic::VPANDD, 0x0F00 | 0xDB, 0xFF, 0x66, 0xFF, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1204 */ {Mnemonic::VPANDQ, 0x0F00 | 0xDB, 0xFF, 0x66, 0xFF, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1205 */ {Mnemonic::VPANDND, 0x0F00 | 0xDF, 0xFF, 0x66, 0xFF, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1206 */ {Mnemonic::VPANDNQ, 0x0F00 | 0xDF, 0xFF, 0x66, 0xFF, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1207 */ {Mnemonic::VPORD, 0x0F00 | 0xEB, 0xFF, 0x66, 0xFF, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1208 */ {Mnemonic::VPORQ, 0x0F00 | 0xEB, 0xFF, 0x66, 0xFF, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1209 */ {Mnemonic::VPXORD, 0x0F00 | 0xEF, 0xFF, 0x66, 0xFF, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1210 */ {Mnemonic::VPXORQ, 0x0F00 | 0xEF, 0xFF, 0x66, 0xFF, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1211 */ {Mnemonic::VPROLVD, 0x0F38, 0xFF, 0x66, 0x15, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1212 */ {Mnemonic::VPROLVQ, 0x0F38, 0xFF, 0x66, 0x15, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1213 */ {Mnemonic::VPRORVD, 0x0F38, 0xFF, 0x66, 0x14, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1214 */ {Mnemonic::VPRORVQ, 0x0F38, 0xFF, 0x66, 0x14, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1215 */ {Mnemonic::VPROLD, 0x0F3A, 0xFF, 0x66, 0x72, true, false, 0, 3, -1, {{AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1216 */ {Mnemonic::VPROLQ, 0x0F3A, 0xFF, 0x66, 0x72, true, false, 0, 3, 1, {{AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1217 */ {Mnemonic::VPRORD, 0x0F3A, 0xFF, 0x66, 0x72, true, false, 0, 3, -1, {{AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1218 */ {Mnemonic::VPRORQ, 0x0F3A, 0xFF, 0x66, 0x72, true, false, 0, 3, 1, {{AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1219 */ {Mnemonic::VBLENDMPS, 0x0F38, 0xFF, 0x66, 0x65, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1220 */ {Mnemonic::VBLENDMPD, 0x0F38, 0xFF, 0x66, 0x65, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1221 */ {Mnemonic::VPBLENDMD, 0x0F38, 0xFF, 0x66, 0x64, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1222 */ {Mnemonic::VPBLENDMQ, 0x0F38, 0xFF, 0x66, 0x64, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1223 */ {Mnemonic::VPBLENDMB, 0x0F38, 0xFF, 0x66, 0x66, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1224 */ {Mnemonic::VPBLENDMW, 0x0F38, 0xFF, 0x66, 0x66, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1225 */ {Mnemonic::VPERMI2D, 0x0F38, 0xFF, 0x66, 0x76, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1226 */ {Mnemonic::VPERMI2Q, 0x0F38, 0xFF, 0x66, 0x76, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1227 */ {Mnemonic::VPERMI2PS, 0x0F38, 0xFF, 0x66, 0x77, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1228 */ {Mnemonic::VPERMI2PD, 0x0F38, 0xFF, 0x66, 0x77, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1229 */ {Mnemonic::VPERMT2D, 0x0F38, 0xFF, 0x66, 0x7E, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1230 */ {Mnemonic::VPERMT2Q, 0x0F38, 0xFF, 0x66, 0x7E, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1231 */ {Mnemonic::VPERMT2PS, 0x0F38, 0xFF, 0x66, 0x7F, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1232 */ {Mnemonic::VPERMT2PD, 0x0F38, 0xFF, 0x66, 0x7F, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1233 */ {Mnemonic::VPERMB, 0x0F38, 0xFF, 0x66, 0x8D, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1234 */ {Mnemonic::VPERMW, 0x0F38, 0xFF, 0x66, 0x8D, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1235 */ {Mnemonic::VPTERNLOGD, 0x0F3A, 0xFF, 0x66, 0x25, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1236 */ {Mnemonic::VPTERNLOGQ, 0x0F3A, 0xFF, 0x66, 0x25, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1237 */ {Mnemonic::VPTESTMD, 0x0F38, 0xFF, 0x66, 0x27, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1238 */ {Mnemonic::VPTESTMQ, 0x0F38, 0xFF, 0x66, 0x27, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1239 */ {Mnemonic::VPTESTMB, 0x0F38, 0xFF, 0x66, 0x26, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1240 */ {Mnemonic::VPTESTMW, 0x0F38, 0xFF, 0x66, 0x26, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1241 */ {Mnemonic::VPTESTNMD, 0x0F38, 0xFF, 0x66, 0x27, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1242 */ {Mnemonic::VPTESTNMQ, 0x0F38, 0xFF, 0x66, 0x27, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1243 */ {Mnemonic::VPTESTNMB, 0x0F38, 0xFF, 0x66, 0x26, true, false, 0, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1244 */ {Mnemonic::VPTESTNMW, 0x0F38, 0xFF, 0x66, 0x26, true, false, 0, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1245 */ {Mnemonic::VCOMPRESSPS, 0x0F38, 0xFF, 0x66, 0x8A, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1246 */ {Mnemonic::VCOMPRESSPD, 0x0F38, 0xFF, 0x66, 0x8A, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1247 */ {Mnemonic::VPCOMPRESSD, 0x0F38, 0xFF, 0x66, 0x8B, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1248 */ {Mnemonic::VPCOMPRESSQ, 0x0F38, 0xFF, 0x66, 0x8B, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1249 */ {Mnemonic::VEXPANDPS, 0x0F38, 0xFF, 0x66, 0x88, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1250 */ {Mnemonic::VEXPANDPD, 0x0F38, 0xFF, 0x66, 0x88, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1251 */ {Mnemonic::VPEXPANDD, 0x0F38, 0xFF, 0x66, 0x89, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1252 */ {Mnemonic::VPEXPANDQ, 0x0F38, 0xFF, 0x66, 0x89, true, false, 0, 2, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1253 */ {Mnemonic::VFIXUPIMMPS, 0x0F3A, 0xFF, 0x66, 0x54, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1254 */ {Mnemonic::VFIXUPIMMPD, 0x0F3A, 0xFF, 0x66, 0x54, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1255 */ {Mnemonic::VFIXUPIMMSS, 0x0F3A, 0xFF, 0x66, 0x55, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1256 */ {Mnemonic::VFIXUPIMMSD, 0x0F3A, 0xFF, 0x66, 0x55, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1257 */ {Mnemonic::VGETEXPPS, 0x0F38, 0xFF, 0x66, 0x42, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1258 */ {Mnemonic::VGETEXPPD, 0x0F38, 0xFF, 0x66, 0x42, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1259 */ {Mnemonic::VGETEXPSS, 0x0F38, 0xFF, 0x66, 0x43, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1260 */ {Mnemonic::VGETEXPSD, 0x0F38, 0xFF, 0x66, 0x43, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1261 */ {Mnemonic::VSCALEFPS, 0x0F38, 0xFF, 0x66, 0x2C, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1262 */ {Mnemonic::VSCALEFPD, 0x0F38, 0xFF, 0x66, 0x2C, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1263 */ {Mnemonic::VSCALEFSS, 0x0F38, 0xFF, 0x66, 0x2D, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1264 */ {Mnemonic::VSCALEFSD, 0x0F38, 0xFF, 0x66, 0x2D, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1265 */ {Mnemonic::VGETMANTPS, 0x0F3A, 0xFF, 0x66, 0x26, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1266 */ {Mnemonic::VGETMANTPD, 0x0F3A, 0xFF, 0x66, 0x26, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1267 */ {Mnemonic::VGETMANTSS, 0x0F3A, 0xFF, 0x66, 0x27, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1268 */ {Mnemonic::VGETMANTSD, 0x0F3A, 0xFF, 0x66, 0x27, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1269 */ {Mnemonic::VRNDSCALEPS, 0x0F3A, 0xFF, 0x66, 0x08, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1270 */ {Mnemonic::VRNDSCALEPD, 0x0F3A, 0xFF, 0x66, 0x09, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1271 */ {Mnemonic::VRNDSCALESS, 0x0F3A, 0xFF, 0x66, 0x0A, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1272 */ {Mnemonic::VRNDSCALESD, 0x0F3A, 0xFF, 0x66, 0x0B, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1273 */ {Mnemonic::VRANGEPS, 0x0F3A, 0xFF, 0x66, 0x50, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1274 */ {Mnemonic::VRANGEPD, 0x0F3A, 0xFF, 0x66, 0x50, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1275 */ {Mnemonic::VRANGESS, 0x0F3A, 0xFF, 0x66, 0x51, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1276 */ {Mnemonic::VRANGESD, 0x0F3A, 0xFF, 0x66, 0x51, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1277 */ {Mnemonic::VREDUCEPS, 0x0F3A, 0xFF, 0x66, 0x56, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1278 */ {Mnemonic::VREDUCEPD, 0x0F3A, 0xFF, 0x66, 0x56, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1279 */ {Mnemonic::VREDUCESS, 0x0F3A, 0xFF, 0x66, 0x57, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1280 */ {Mnemonic::VREDUCESD, 0x0F3A, 0xFF, 0x66, 0x57, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1281 */ {Mnemonic::VFPCLASSPS, 0x0F3A, 0xFF, 0x66, 0x66, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1282 */ {Mnemonic::VFPCLASSPD, 0x0F3A, 0xFF, 0x66, 0x66, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1283 */ {Mnemonic::VFPCLASSSS, 0x0F3A, 0xFF, 0x66, 0x67, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1284 */ {Mnemonic::VFPCLASSSD, 0x0F3A, 0xFF, 0x66, 0x67, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1285 */ {Mnemonic::VDBPSADBW, 0x0F3A, 0xFF, 0x66, 0x42, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1286 */ {Mnemonic::VPCONFLICTD, 0x0F38, 0xFF, 0x66, 0xC4, true, false, 0, 2, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1287 */ {Mnemonic::VPCONFLICTQ, 0x0F38, 0xFF, 0x66, 0xC4, true, false, 0, 2, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1288 */ {Mnemonic::VPLZCNTD, 0x0F38, 0xFF, 0x66, 0x44, true, false, 0, 2, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1289 */ {Mnemonic::VPLZCNTQ, 0x0F38, 0xFF, 0x66, 0x44, true, false, 0, 2, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1290 */ {Mnemonic::KADDB, 0x0F00 | 0x4A, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1291 */ {Mnemonic::KADDD, 0x0F00 | 0x4A, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1292 */ {Mnemonic::KADDQ, 0x0F00 | 0x4A, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1293 */ {Mnemonic::KADDW, 0x0F00 | 0x4A, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1294 */ {Mnemonic::KANDB, 0x0F00 | 0x41, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1295 */ {Mnemonic::KANDNB, 0x0F00 | 0x42, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1296 */ {Mnemonic::KANDND, 0x0F00 | 0x42, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1297 */ {Mnemonic::KANDNQ, 0x0F00 | 0x42, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1298 */ {Mnemonic::KORB, 0x0F00 | 0x45, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1299 */ {Mnemonic::KXORB, 0x0F00 | 0x47, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1300 */ {Mnemonic::KXNORB, 0x0F00 | 0x46, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1301 */ {Mnemonic::KXNORW, 0x0F00 | 0x46, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1302 */ {Mnemonic::KXNORD, 0x0F00 | 0x46, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1303 */ {Mnemonic::KXNORQ, 0x0F00 | 0x46, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1304 */ {Mnemonic::KNOTB, 0x0F00 | 0x44, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1305 */ {Mnemonic::KORTESTB, 0x0F00 | 0x98, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1306 */ {Mnemonic::KTESTB, 0x0F00 | 0x99, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1307 */ {Mnemonic::KTESTW, 0x0F00 | 0x99, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1308 */ {Mnemonic::KTESTD, 0x0F00 | 0x99, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1309 */ {Mnemonic::KTESTQ, 0x0F00 | 0x99, 0xFF, 0x00, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1310 */ {Mnemonic::KSHIFTLB, 0x0F3A, 0xFF, 0x66, 0x32, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1311 */ {Mnemonic::KSHIFTLD, 0x0F3A, 0xFF, 0x66, 0x33, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1312 */ {Mnemonic::KSHIFTLQ, 0x0F3A, 0xFF, 0x66, 0x33, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1313 */ {Mnemonic::KSHIFTRB, 0x0F3A, 0xFF, 0x66, 0x30, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1314 */ {Mnemonic::KSHIFTRD, 0x0F3A, 0xFF, 0x66, 0x31, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1315 */ {Mnemonic::KSHIFTRQ, 0x0F3A, 0xFF, 0x66, 0x31, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1316 */ {Mnemonic::LDTILECFG, 0x0F38, 0x00, 0x00, 0x49, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1317 */ {Mnemonic::STTILECFG, 0x0F38, 0x00, 0x66, 0x49, true, false, 1, 1, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1318 */ {Mnemonic::TILELOADD, 0x0F38, 0xFF, 0xF2, 0x4B, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1319 */ {Mnemonic::TILELOADDT1, 0x0F38, 0xFF, 0x66, 0x4B, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1320 */ {Mnemonic::TILESTORED, 0x0F38, 0xFF, 0xF3, 0x4B, true, false, 1, 2, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1321 */ {Mnemonic::TILEZERO, 0x0F38, 0xFF, 0xF2, 0x49, true, false, -1, 1, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1322 */ {Mnemonic::TILERELEASE, 0x0F38, 0x00, 0x00, 0x49, true, false, -1, 0, 0, {{AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1323 */ {Mnemonic::TDPBF16PS, 0x0F38, 0xFF, 0xF3, 0x5C, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1324 */ {Mnemonic::TDPBSSD, 0x0F38, 0xFF, 0xF2, 0x5E, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1325 */ {Mnemonic::TDPBSUD, 0x0F38, 0xFF, 0xF3, 0x5E, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1326 */ {Mnemonic::TDPBUSD, 0x0F38, 0xFF, 0x66, 0x5E, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1327 */ {Mnemonic::TDPBUUD, 0x0F38, 0xFF, 0x00, 0x5E, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1328 */ {Mnemonic::TDPFP16PS, 0x0F38, 0xFF, 0xF2, 0x5C, true, false, -1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1329 */ {Mnemonic::VMOVDQA32, 0x0F6F, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1330 */ {Mnemonic::VMOVDQA64, 0x0F6F, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1331 */ {Mnemonic::VMOVDQU8, 0x0F6F, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1332 */ {Mnemonic::VMOVDQU16, 0x0F6F, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1333 */ {Mnemonic::VMOVDQU32, 0x0F6F, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1334 */ {Mnemonic::VMOVDQU64, 0x0F6F, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1335 */ {Mnemonic::VCVTPS2UDQ, 0x0F79, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1336 */ {Mnemonic::VCVTPD2UDQ, 0x0F79, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1337 */ {Mnemonic::VCVTTPS2UDQ, 0x0F78, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1338 */ {Mnemonic::VCVTTPD2UDQ, 0x0F78, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1339 */ {Mnemonic::VCVTUDQ2PS, 0x0F7A, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1340 */ {Mnemonic::VCVTUDQ2PD, 0x0F7A, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1341 */ {Mnemonic::VCVTUSI2SS, 0x0F7B, 0xFF, 0xF3, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1342 */ {Mnemonic::VCVTUSI2SD, 0x0F7B, 0xFF, 0xF2, 0xFF, true, false, 0, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::ModRM_RM, OpSize::OpSzQ, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1343 */ {Mnemonic::VCVTSS2USI, 0x0F79, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1344 */ {Mnemonic::VCVTSD2USI, 0x0F79, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1345 */ {Mnemonic::VCVTTSS2USI, 0x0F78, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1346 */ {Mnemonic::VCVTTSD2USI, 0x0F78, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::ModRM_Reg, OpSize::OpSzQ, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1347 */ {Mnemonic::VINSERTF32X4, 0x0F3A, 0xFF, 0x66, 0x18, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1348 */ {Mnemonic::VINSERTF64X2, 0x0F3A, 0xFF, 0x66, 0x18, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1349 */ {Mnemonic::VINSERTI32X4, 0x0F3A, 0xFF, 0x66, 0x38, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1350 */ {Mnemonic::VINSERTI64X2, 0x0F3A, 0xFF, 0x66, 0x38, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1351 */ {Mnemonic::VINSERTF32X8, 0x0F3A, 0xFF, 0x66, 0x1A, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1352 */ {Mnemonic::VINSERTF64X4, 0x0F3A, 0xFF, 0x66, 0x1A, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1353 */ {Mnemonic::VINSERTI32X8, 0x0F3A, 0xFF, 0x66, 0x3A, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1354 */ {Mnemonic::VINSERTI64X4, 0x0F3A, 0xFF, 0x66, 0x3A, true, false, 0, 4, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}}},
    /* 1355 */ {Mnemonic::VEXTRACTF32X4, 0x0F3A, 0xFF, 0x66, 0x19, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1356 */ {Mnemonic::VEXTRACTF64X2, 0x0F3A, 0xFF, 0x66, 0x19, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1357 */ {Mnemonic::VEXTRACTI32X4, 0x0F3A, 0xFF, 0x66, 0x39, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1358 */ {Mnemonic::VEXTRACTI64X2, 0x0F3A, 0xFF, 0x66, 0x39, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1359 */ {Mnemonic::VEXTRACTF32X8, 0x0F3A, 0xFF, 0x66, 0x1B, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1360 */ {Mnemonic::VEXTRACTF64X4, 0x0F3A, 0xFF, 0x66, 0x1B, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1361 */ {Mnemonic::VEXTRACTI32X8, 0x0F3A, 0xFF, 0x66, 0x3B, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1362 */ {Mnemonic::VEXTRACTI64X4, 0x0F3A, 0xFF, 0x66, 0x3B, true, false, 0, 3, 0, {{AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::Immediate, OpSize::Byte, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1363 */ {Mnemonic::VBROADCASTF32X2, 0x0F38, 0xFF, 0x66, 0x19, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1364 */ {Mnemonic::VBROADCASTF32X4, 0x0F38, 0xFF, 0x66, 0x1A, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1365 */ {Mnemonic::VBROADCASTF32X8, 0x0F38, 0xFF, 0x66, 0x1B, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1366 */ {Mnemonic::VBROADCASTF64X2, 0x0F38, 0xFF, 0x66, 0x1A, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1367 */ {Mnemonic::VBROADCASTF64X4, 0x0F38, 0xFF, 0x66, 0x1B, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1368 */ {Mnemonic::VBROADCASTI32X2, 0x0F38, 0xFF, 0x66, 0x59, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1369 */ {Mnemonic::VBROADCASTI32X4, 0x0F38, 0xFF, 0x66, 0x5A, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1370 */ {Mnemonic::VBROADCASTI32X8, 0x0F38, 0xFF, 0x66, 0x5B, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1371 */ {Mnemonic::VBROADCASTI64X2, 0x0F38, 0xFF, 0x66, 0x5A, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1372 */ {Mnemonic::VBROADCASTI64X4, 0x0F38, 0xFF, 0x66, 0x5B, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1373 */ {Mnemonic::VCVTQQ2PS, 0x0F5B, 0xFF, 0x00, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1374 */ {Mnemonic::VCVTQQ2PD, 0x0FE6, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1375 */ {Mnemonic::VCVTUQQ2PS, 0x0F7A, 0xFF, 0xF2, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1376 */ {Mnemonic::VCVTUQQ2PD, 0x0F7A, 0xFF, 0xF3, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1377 */ {Mnemonic::VCVTPS2QQ, 0x0F7B, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1378 */ {Mnemonic::VCVTPD2QQ, 0x0F7B, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1379 */ {Mnemonic::VCVTPS2UQQ, 0x0F79, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1380 */ {Mnemonic::VCVTPD2UQQ, 0x0F79, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1381 */ {Mnemonic::VCVTTPS2QQ, 0x0F7A, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1382 */ {Mnemonic::VCVTTPD2QQ, 0x0F7A, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1383 */ {Mnemonic::VCVTTPS2UQQ, 0x0F78, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1384 */ {Mnemonic::VCVTTPD2UQQ, 0x0F78, 0xFF, 0x66, 0xFF, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1385 */ {Mnemonic::VCVTNEEBF162PS, 0x0F38, 0xFF, 0x00, 0xB0, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1386 */ {Mnemonic::VCVTNEOBF162PS, 0x0F38, 0xFF, 0xF3, 0xB0, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1387 */ {Mnemonic::VCVTNEEPH2PS, 0x0F38, 0xFF, 0x66, 0xB0, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1388 */ {Mnemonic::VCVTNEOPH2PS, 0x0F38, 0xFF, 0xF2, 0xB0, true, false, 1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1389 */ {Mnemonic::VCVTNEPS2BF16, 0x0F38, 0xFF, 0xF3, 0x72, true, false, 0, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1390 */ {Mnemonic::VMASKMOVPS, 0x0F38, 0xFF, 0x66, 0x2C, true, false, 1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1391 */ {Mnemonic::VMASKMOVPS, 0x0F38, 0xFF, 0x66, 0x2E, true, false, 1, 3, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1392 */ {Mnemonic::VMASKMOVPD, 0x0F38, 0xFF, 0x66, 0x2D, true, false, 1, 3, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1393 */ {Mnemonic::VMASKMOVPD, 0x0F38, 0xFF, 0x66, 0x2F, true, false, 1, 3, 0, {{AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1394 */ {Mnemonic::INSERTQ, 0x0F79, 0xFF, 0xF2, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1395 */ {Mnemonic::EXTRQ, 0x0F79, 0xFF, 0x66, 0xFF, true, false, -1, 2, 0, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::XmmRM, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1396 */ {Mnemonic::VGATHERDPS, 0x0F38, 0xFF, 0x66, 0x92, true, false, 1, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1397 */ {Mnemonic::VGATHERDPD, 0x0F38, 0xFF, 0x66, 0x92, true, false, 1, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1398 */ {Mnemonic::VGATHERQPS, 0x0F38, 0xFF, 0x66, 0x93, true, false, 1, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1399 */ {Mnemonic::VGATHERQPD, 0x0F38, 0xFF, 0x66, 0x93, true, false, 1, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1400 */ {Mnemonic::VPGATHERDD, 0x0F38, 0xFF, 0x66, 0x90, true, false, 1, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1401 */ {Mnemonic::VPGATHERDQ, 0x0F38, 0xFF, 0x66, 0x90, true, false, 1, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1402 */ {Mnemonic::VPGATHERQD, 0x0F38, 0xFF, 0x66, 0x91, true, false, 1, 3, -1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
    /* 1403 */ {Mnemonic::VPGATHERQQ, 0x0F38, 0xFF, 0x66, 0x91, true, false, 1, 3, 1, {{AddrMode::XmmReg, OpSize::None, 0xFF}, {AddrMode::MemOnly, OpSize::None, 0xFF}, {AddrMode::XmmVvvv, OpSize::None, 0xFF}, {AddrMode::None, OpSize::None, 0xFF}}},
};
const size_t g_instr_table_size = 1404;


// Opcode lookup: map[map_id][byte] → first table index; chain[i] → next
uint16_t g_opcode_map[4][256];
uint16_t g_opcode_chain[1404];

static struct OpcodeMapInit {
    OpcodeMapInit() {
        memset(g_opcode_map, 0xFF, sizeof(g_opcode_map));
        memset(g_opcode_chain, 0xFF, sizeof(g_opcode_chain));
        // Insert each entry into its opcode slot chain (prepend)
        for (int i = (int)g_instr_table_size - 1; i >= 0; --i) {
            const auto& d = g_instr_table[i];
            uint8_t map_id, byte;
            if (d.opcode == 0x0F38) { map_id = 2; byte = d.sec_opcd; }
            else if (d.opcode == 0x0F3A) { map_id = 3; byte = d.sec_opcd; }
            else if ((d.opcode & 0xFF00) == 0x0F00) { map_id = 1; byte = d.opcode & 0xFF; }
            else { map_id = 0; byte = d.opcode & 0xFF; }
            // For 0F38/0F3A maps, skip entries where sec_opcd is 0xFF (wildcard)
            if ((map_id == 2 || map_id == 3) && byte == 0xFF) continue;
            g_opcode_chain[i] = g_opcode_map[map_id][byte];
            g_opcode_map[map_id][byte] = (uint16_t)i;
        }
    }
} g_opcode_map_init;

} // namespace vedx64
