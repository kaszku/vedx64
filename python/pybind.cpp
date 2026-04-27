// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/core.hpp"
#include "vedx64/operand.hpp"
#include "vedx64/instruction.hpp"
#include "vedx64/encoding_id.hpp"
#endif
#ifdef VEDX64_EMU
#include "vedx64/emu.hpp"
#endif
#ifdef VEDX64_IR
#include "vedx64/ir.hpp"
#endif
#include "vedx64/relocation.hpp"
#include "vedx64/branch_follow.hpp"
#include "vedx64/semantics.hpp"
#include "vedx64/analysis.hpp"
#ifdef VEDX64_ASSEMBLER
#include "vedx64/assembler.hpp"
#endif
#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <cstring>

namespace nb = nanobind;
using namespace vedx64;

NB_MODULE(vedx64_py, m) {
    m.doc() = "x86-64 instruction toolkit: decoder, encoder, disassembler, assembler, emulator, and more";

    nb::enum_<Mnemonic>(m, "Mnemonic")
        .value("INVALID", Mnemonic::INVALID)
        .value("AAA", Mnemonic::AAA)
        .value("AAD", Mnemonic::AAD)
        .value("AAM", Mnemonic::AAM)
        .value("AAS", Mnemonic::AAS)
        .value("ADC", Mnemonic::ADC)
        .value("ADD", Mnemonic::ADD)
        .value("ADDPD", Mnemonic::ADDPD)
        .value("ADDPS", Mnemonic::ADDPS)
        .value("ADDSD", Mnemonic::ADDSD)
        .value("ADDSS", Mnemonic::ADDSS)
        .value("ADDSUBPD", Mnemonic::ADDSUBPD)
        .value("ADDSUBPS", Mnemonic::ADDSUBPS)
        .value("ADX", Mnemonic::ADX)
        .value("AMX", Mnemonic::AMX)
        .value("AND", Mnemonic::AND)
        .value("ANDNPD", Mnemonic::ANDNPD)
        .value("ANDNPS", Mnemonic::ANDNPS)
        .value("ANDPD", Mnemonic::ANDPD)
        .value("ANDPS", Mnemonic::ANDPS)
        .value("BLENDPD", Mnemonic::BLENDPD)
        .value("BLENDPS", Mnemonic::BLENDPS)
        .value("BLENDVPD", Mnemonic::BLENDVPD)
        .value("BLENDVPS", Mnemonic::BLENDVPS)
        .value("BOUND", Mnemonic::BOUND)
        .value("BSF", Mnemonic::BSF)
        .value("BSR", Mnemonic::BSR)
        .value("BSWAP", Mnemonic::BSWAP)
        .value("BT", Mnemonic::BT)
        .value("BTC", Mnemonic::BTC)
        .value("BTR", Mnemonic::BTR)
        .value("BTS", Mnemonic::BTS)
        .value("CALL", Mnemonic::CALL)
        .value("CALLF", Mnemonic::CALLF)
        .value("CBW", Mnemonic::CBW)
        .value("CDQ", Mnemonic::CDQ)
        .value("CDQE", Mnemonic::CDQE)
        .value("CLC", Mnemonic::CLC)
        .value("CLD", Mnemonic::CLD)
        .value("CLFLUSH", Mnemonic::CLFLUSH)
        .value("CLI", Mnemonic::CLI)
        .value("CLTS", Mnemonic::CLTS)
        .value("CMC", Mnemonic::CMC)
        .value("CMOVA", Mnemonic::CMOVA)
        .value("CMOVAE", Mnemonic::CMOVAE)
        .value("CMOVB", Mnemonic::CMOVB)
        .value("CMOVBE", Mnemonic::CMOVBE)
        .value("CMOVC", Mnemonic::CMOVC)
        .value("CMOVE", Mnemonic::CMOVE)
        .value("CMOVG", Mnemonic::CMOVG)
        .value("CMOVGE", Mnemonic::CMOVGE)
        .value("CMOVL", Mnemonic::CMOVL)
        .value("CMOVLE", Mnemonic::CMOVLE)
        .value("CMOVNA", Mnemonic::CMOVNA)
        .value("CMOVNAE", Mnemonic::CMOVNAE)
        .value("CMOVNB", Mnemonic::CMOVNB)
        .value("CMOVNBE", Mnemonic::CMOVNBE)
        .value("CMOVNC", Mnemonic::CMOVNC)
        .value("CMOVNE", Mnemonic::CMOVNE)
        .value("CMOVNG", Mnemonic::CMOVNG)
        .value("CMOVNGE", Mnemonic::CMOVNGE)
        .value("CMOVNL", Mnemonic::CMOVNL)
        .value("CMOVNLE", Mnemonic::CMOVNLE)
        .value("CMOVNO", Mnemonic::CMOVNO)
        .value("CMOVNP", Mnemonic::CMOVNP)
        .value("CMOVNS", Mnemonic::CMOVNS)
        .value("CMOVNZ", Mnemonic::CMOVNZ)
        .value("CMOVO", Mnemonic::CMOVO)
        .value("CMOVP", Mnemonic::CMOVP)
        .value("CMOVPE", Mnemonic::CMOVPE)
        .value("CMOVPO", Mnemonic::CMOVPO)
        .value("CMOVS", Mnemonic::CMOVS)
        .value("CMOVZ", Mnemonic::CMOVZ)
        .value("CMP", Mnemonic::CMP)
        .value("CMPPD", Mnemonic::CMPPD)
        .value("CMPPS", Mnemonic::CMPPS)
        .value("CMPS", Mnemonic::CMPS)
        .value("CMPSB", Mnemonic::CMPSB)
        .value("CMPSD", Mnemonic::CMPSD)
        .value("CMPSQ", Mnemonic::CMPSQ)
        .value("CMPSS", Mnemonic::CMPSS)
        .value("CMPSW", Mnemonic::CMPSW)
        .value("CMPXCHG", Mnemonic::CMPXCHG)
        .value("CMPXCHG16B", Mnemonic::CMPXCHG16B)
        .value("CMPXCHG8B", Mnemonic::CMPXCHG8B)
        .value("COMISD", Mnemonic::COMISD)
        .value("COMISS", Mnemonic::COMISS)
        .value("CPUID", Mnemonic::CPUID)
        .value("CQO", Mnemonic::CQO)
        .value("CRC32", Mnemonic::CRC32)
        .value("CVTDQ2PD", Mnemonic::CVTDQ2PD)
        .value("CVTDQ2PS", Mnemonic::CVTDQ2PS)
        .value("CVTPD2DQ", Mnemonic::CVTPD2DQ)
        .value("CVTPD2PI", Mnemonic::CVTPD2PI)
        .value("CVTPD2PS", Mnemonic::CVTPD2PS)
        .value("CVTPI2PD", Mnemonic::CVTPI2PD)
        .value("CVTPI2PS", Mnemonic::CVTPI2PS)
        .value("CVTPS2DQ", Mnemonic::CVTPS2DQ)
        .value("CVTPS2PD", Mnemonic::CVTPS2PD)
        .value("CVTPS2PI", Mnemonic::CVTPS2PI)
        .value("CVTSD2SI", Mnemonic::CVTSD2SI)
        .value("CVTSD2SS", Mnemonic::CVTSD2SS)
        .value("CVTSI2SD", Mnemonic::CVTSI2SD)
        .value("CVTSI2SS", Mnemonic::CVTSI2SS)
        .value("CVTSS2SD", Mnemonic::CVTSS2SD)
        .value("CVTSS2SI", Mnemonic::CVTSS2SI)
        .value("CVTTPD2DQ", Mnemonic::CVTTPD2DQ)
        .value("CVTTPD2PI", Mnemonic::CVTTPD2PI)
        .value("CVTTPS2DQ", Mnemonic::CVTTPS2DQ)
        .value("CVTTPS2PI", Mnemonic::CVTTPS2PI)
        .value("CVTTSD2SI", Mnemonic::CVTTSD2SI)
        .value("CVTTSS2SI", Mnemonic::CVTTSS2SI)
        .value("CWD", Mnemonic::CWD)
        .value("CWDE", Mnemonic::CWDE)
        .value("DAA", Mnemonic::DAA)
        .value("DAS", Mnemonic::DAS)
        .value("DEC", Mnemonic::DEC)
        .value("DIV", Mnemonic::DIV)
        .value("DIVPD", Mnemonic::DIVPD)
        .value("DIVPS", Mnemonic::DIVPS)
        .value("DIVSD", Mnemonic::DIVSD)
        .value("DIVSS", Mnemonic::DIVSS)
        .value("DPPD", Mnemonic::DPPD)
        .value("DPPS", Mnemonic::DPPS)
        .value("EMMS", Mnemonic::EMMS)
        .value("ENTER", Mnemonic::ENTER)
        .value("EXTRACTPS", Mnemonic::EXTRACTPS)
        .value("F2XM1", Mnemonic::F2XM1)
        .value("FABS", Mnemonic::FABS)
        .value("FADD", Mnemonic::FADD)
        .value("FADDP", Mnemonic::FADDP)
        .value("FBLD", Mnemonic::FBLD)
        .value("FBSTP", Mnemonic::FBSTP)
        .value("FCHS", Mnemonic::FCHS)
        .value("FCLEX", Mnemonic::FCLEX)
        .value("FCMOVB", Mnemonic::FCMOVB)
        .value("FCMOVBE", Mnemonic::FCMOVBE)
        .value("FCMOVE", Mnemonic::FCMOVE)
        .value("FCMOVNB", Mnemonic::FCMOVNB)
        .value("FCMOVNBE", Mnemonic::FCMOVNBE)
        .value("FCMOVNE", Mnemonic::FCMOVNE)
        .value("FCMOVNU", Mnemonic::FCMOVNU)
        .value("FCMOVU", Mnemonic::FCMOVU)
        .value("FCOM", Mnemonic::FCOM)
        .value("FCOM2", Mnemonic::FCOM2)
        .value("FCOMI", Mnemonic::FCOMI)
        .value("FCOMIP", Mnemonic::FCOMIP)
        .value("FCOMP", Mnemonic::FCOMP)
        .value("FCOMP3", Mnemonic::FCOMP3)
        .value("FCOMP5", Mnemonic::FCOMP5)
        .value("FCOMPP", Mnemonic::FCOMPP)
        .value("FCOS", Mnemonic::FCOS)
        .value("FDECSTP", Mnemonic::FDECSTP)
        .value("FDISI", Mnemonic::FDISI)
        .value("FDIV", Mnemonic::FDIV)
        .value("FDIVP", Mnemonic::FDIVP)
        .value("FDIVR", Mnemonic::FDIVR)
        .value("FDIVRP", Mnemonic::FDIVRP)
        .value("FENI", Mnemonic::FENI)
        .value("FFREE", Mnemonic::FFREE)
        .value("FFREEP", Mnemonic::FFREEP)
        .value("FIADD", Mnemonic::FIADD)
        .value("FICOM", Mnemonic::FICOM)
        .value("FICOMP", Mnemonic::FICOMP)
        .value("FIDIV", Mnemonic::FIDIV)
        .value("FIDIVR", Mnemonic::FIDIVR)
        .value("FILD", Mnemonic::FILD)
        .value("FIMUL", Mnemonic::FIMUL)
        .value("FINCSTP", Mnemonic::FINCSTP)
        .value("FINIT", Mnemonic::FINIT)
        .value("FIST", Mnemonic::FIST)
        .value("FISTP", Mnemonic::FISTP)
        .value("FISTTP", Mnemonic::FISTTP)
        .value("FISUB", Mnemonic::FISUB)
        .value("FISUBR", Mnemonic::FISUBR)
        .value("FLD", Mnemonic::FLD)
        .value("FLD1", Mnemonic::FLD1)
        .value("FLDCW", Mnemonic::FLDCW)
        .value("FLDENV", Mnemonic::FLDENV)
        .value("FLDL2E", Mnemonic::FLDL2E)
        .value("FLDL2T", Mnemonic::FLDL2T)
        .value("FLDLG2", Mnemonic::FLDLG2)
        .value("FLDLN2", Mnemonic::FLDLN2)
        .value("FLDPI", Mnemonic::FLDPI)
        .value("FLDZ", Mnemonic::FLDZ)
        .value("FMUL", Mnemonic::FMUL)
        .value("FMULP", Mnemonic::FMULP)
        .value("FNCLEX", Mnemonic::FNCLEX)
        .value("FNDISI", Mnemonic::FNDISI)
        .value("FNENI", Mnemonic::FNENI)
        .value("FNINIT", Mnemonic::FNINIT)
        .value("FNOP", Mnemonic::FNOP)
        .value("FNSAVE", Mnemonic::FNSAVE)
        .value("FNSETPM", Mnemonic::FNSETPM)
        .value("FNSTCW", Mnemonic::FNSTCW)
        .value("FNSTENV", Mnemonic::FNSTENV)
        .value("FNSTSW", Mnemonic::FNSTSW)
        .value("FPATAN", Mnemonic::FPATAN)
        .value("FPREM", Mnemonic::FPREM)
        .value("FPREM1", Mnemonic::FPREM1)
        .value("FPTAN", Mnemonic::FPTAN)
        .value("FRNDINT", Mnemonic::FRNDINT)
        .value("FRSTOR", Mnemonic::FRSTOR)
        .value("FSAVE", Mnemonic::FSAVE)
        .value("FSCALE", Mnemonic::FSCALE)
        .value("FSETPM", Mnemonic::FSETPM)
        .value("FSIN", Mnemonic::FSIN)
        .value("FSINCOS", Mnemonic::FSINCOS)
        .value("FSQRT", Mnemonic::FSQRT)
        .value("FST", Mnemonic::FST)
        .value("FSTCW", Mnemonic::FSTCW)
        .value("FSTENV", Mnemonic::FSTENV)
        .value("FSTP", Mnemonic::FSTP)
        .value("FSTP1", Mnemonic::FSTP1)
        .value("FSTP8", Mnemonic::FSTP8)
        .value("FSTP9", Mnemonic::FSTP9)
        .value("FSTSW", Mnemonic::FSTSW)
        .value("FSUB", Mnemonic::FSUB)
        .value("FSUBP", Mnemonic::FSUBP)
        .value("FSUBR", Mnemonic::FSUBR)
        .value("FSUBRP", Mnemonic::FSUBRP)
        .value("FTST", Mnemonic::FTST)
        .value("FUCOM", Mnemonic::FUCOM)
        .value("FUCOMI", Mnemonic::FUCOMI)
        .value("FUCOMIP", Mnemonic::FUCOMIP)
        .value("FUCOMP", Mnemonic::FUCOMP)
        .value("FUCOMPP", Mnemonic::FUCOMPP)
        .value("FWAIT", Mnemonic::FWAIT)
        .value("FXAM", Mnemonic::FXAM)
        .value("FXCH", Mnemonic::FXCH)
        .value("FXCH4", Mnemonic::FXCH4)
        .value("FXCH7", Mnemonic::FXCH7)
        .value("FXRSTOR", Mnemonic::FXRSTOR)
        .value("FXSAVE", Mnemonic::FXSAVE)
        .value("FXTRACT", Mnemonic::FXTRACT)
        .value("FYL2X", Mnemonic::FYL2X)
        .value("FYL2XP1", Mnemonic::FYL2XP1)
        .value("GETSEC", Mnemonic::GETSEC)
        .value("HADDPD", Mnemonic::HADDPD)
        .value("HADDPS", Mnemonic::HADDPS)
        .value("HINT_NOP", Mnemonic::HINT_NOP)
        .value("HLT", Mnemonic::HLT)
        .value("HSUBPD", Mnemonic::HSUBPD)
        .value("HSUBPS", Mnemonic::HSUBPS)
        .value("ICEBP", Mnemonic::ICEBP)
        .value("IDIV", Mnemonic::IDIV)
        .value("IMUL", Mnemonic::IMUL)
        .value("IN", Mnemonic::IN)
        .value("INC", Mnemonic::INC)
        .value("INS", Mnemonic::INS)
        .value("INSB", Mnemonic::INSB)
        .value("INSD", Mnemonic::INSD)
        .value("INSERTPS", Mnemonic::INSERTPS)
        .value("INSW", Mnemonic::INSW)
        .value("INT", Mnemonic::INT)
        .value("INT1", Mnemonic::INT1)
        .value("INTO", Mnemonic::INTO)
        .value("INVD", Mnemonic::INVD)
        .value("INVEPT", Mnemonic::INVEPT)
        .value("INVLPG", Mnemonic::INVLPG)
        .value("INVVPID", Mnemonic::INVVPID)
        .value("IRET", Mnemonic::IRET)
        .value("IRETD", Mnemonic::IRETD)
        .value("IRETQ", Mnemonic::IRETQ)
        .value("JA", Mnemonic::JA)
        .value("JAE", Mnemonic::JAE)
        .value("JB", Mnemonic::JB)
        .value("JBE", Mnemonic::JBE)
        .value("JC", Mnemonic::JC)
        .value("JCXZ", Mnemonic::JCXZ)
        .value("JE", Mnemonic::JE)
        .value("JECXZ", Mnemonic::JECXZ)
        .value("JG", Mnemonic::JG)
        .value("JGE", Mnemonic::JGE)
        .value("JL", Mnemonic::JL)
        .value("JLE", Mnemonic::JLE)
        .value("JMP", Mnemonic::JMP)
        .value("JMPE", Mnemonic::JMPE)
        .value("JMPF", Mnemonic::JMPF)
        .value("JNA", Mnemonic::JNA)
        .value("JNAE", Mnemonic::JNAE)
        .value("JNB", Mnemonic::JNB)
        .value("JNBE", Mnemonic::JNBE)
        .value("JNC", Mnemonic::JNC)
        .value("JNE", Mnemonic::JNE)
        .value("JNG", Mnemonic::JNG)
        .value("JNGE", Mnemonic::JNGE)
        .value("JNL", Mnemonic::JNL)
        .value("JNLE", Mnemonic::JNLE)
        .value("JNO", Mnemonic::JNO)
        .value("JNP", Mnemonic::JNP)
        .value("JNS", Mnemonic::JNS)
        .value("JNZ", Mnemonic::JNZ)
        .value("JO", Mnemonic::JO)
        .value("JP", Mnemonic::JP)
        .value("JPE", Mnemonic::JPE)
        .value("JPO", Mnemonic::JPO)
        .value("JRCXZ", Mnemonic::JRCXZ)
        .value("JS", Mnemonic::JS)
        .value("JZ", Mnemonic::JZ)
        .value("LAHF", Mnemonic::LAHF)
        .value("LAR", Mnemonic::LAR)
        .value("LDDQU", Mnemonic::LDDQU)
        .value("LDMXCSR", Mnemonic::LDMXCSR)
        .value("LDS", Mnemonic::LDS)
        .value("LEA", Mnemonic::LEA)
        .value("LEAVE", Mnemonic::LEAVE)
        .value("LES", Mnemonic::LES)
        .value("LFENCE", Mnemonic::LFENCE)
        .value("LFS", Mnemonic::LFS)
        .value("LGDT", Mnemonic::LGDT)
        .value("LGS", Mnemonic::LGS)
        .value("LIDT", Mnemonic::LIDT)
        .value("LLDT", Mnemonic::LLDT)
        .value("LMSW", Mnemonic::LMSW)
        .value("LODS", Mnemonic::LODS)
        .value("LODSB", Mnemonic::LODSB)
        .value("LODSD", Mnemonic::LODSD)
        .value("LODSQ", Mnemonic::LODSQ)
        .value("LODSW", Mnemonic::LODSW)
        .value("LOOP", Mnemonic::LOOP)
        .value("LOOPE", Mnemonic::LOOPE)
        .value("LOOPNE", Mnemonic::LOOPNE)
        .value("LOOPNZ", Mnemonic::LOOPNZ)
        .value("LOOPZ", Mnemonic::LOOPZ)
        .value("LSL", Mnemonic::LSL)
        .value("LSS", Mnemonic::LSS)
        .value("LTR", Mnemonic::LTR)
        .value("MASKMOVDQU", Mnemonic::MASKMOVDQU)
        .value("MASKMOVQ", Mnemonic::MASKMOVQ)
        .value("MAXPD", Mnemonic::MAXPD)
        .value("MAXPS", Mnemonic::MAXPS)
        .value("MAXSD", Mnemonic::MAXSD)
        .value("MAXSS", Mnemonic::MAXSS)
        .value("MFENCE", Mnemonic::MFENCE)
        .value("MINPD", Mnemonic::MINPD)
        .value("MINPS", Mnemonic::MINPS)
        .value("MINSD", Mnemonic::MINSD)
        .value("MINSS", Mnemonic::MINSS)
        .value("MONITOR", Mnemonic::MONITOR)
        .value("MOV", Mnemonic::MOV)
        .value("MOVAPD", Mnemonic::MOVAPD)
        .value("MOVAPS", Mnemonic::MOVAPS)
        .value("MOVBE", Mnemonic::MOVBE)
        .value("MOVD", Mnemonic::MOVD)
        .value("MOVDDUP", Mnemonic::MOVDDUP)
        .value("MOVDQ2Q", Mnemonic::MOVDQ2Q)
        .value("MOVDQA", Mnemonic::MOVDQA)
        .value("MOVDQU", Mnemonic::MOVDQU)
        .value("MOVHLPS", Mnemonic::MOVHLPS)
        .value("MOVHPD", Mnemonic::MOVHPD)
        .value("MOVHPS", Mnemonic::MOVHPS)
        .value("MOVLHPS", Mnemonic::MOVLHPS)
        .value("MOVLPD", Mnemonic::MOVLPD)
        .value("MOVLPS", Mnemonic::MOVLPS)
        .value("MOVMSKPD", Mnemonic::MOVMSKPD)
        .value("MOVMSKPS", Mnemonic::MOVMSKPS)
        .value("MOVNTDQ", Mnemonic::MOVNTDQ)
        .value("MOVNTDQA", Mnemonic::MOVNTDQA)
        .value("MOVNTI", Mnemonic::MOVNTI)
        .value("MOVNTPD", Mnemonic::MOVNTPD)
        .value("MOVNTPS", Mnemonic::MOVNTPS)
        .value("MOVNTQ", Mnemonic::MOVNTQ)
        .value("MOVQ", Mnemonic::MOVQ)
        .value("MOVQ2DQ", Mnemonic::MOVQ2DQ)
        .value("MOVS", Mnemonic::MOVS)
        .value("MOVSB", Mnemonic::MOVSB)
        .value("MOVSD", Mnemonic::MOVSD)
        .value("MOVSHDUP", Mnemonic::MOVSHDUP)
        .value("MOVSLDUP", Mnemonic::MOVSLDUP)
        .value("MOVSQ", Mnemonic::MOVSQ)
        .value("MOVSS", Mnemonic::MOVSS)
        .value("MOVSW", Mnemonic::MOVSW)
        .value("MOVSX", Mnemonic::MOVSX)
        .value("MOVSXD", Mnemonic::MOVSXD)
        .value("MOVUPD", Mnemonic::MOVUPD)
        .value("MOVUPS", Mnemonic::MOVUPS)
        .value("MOVZX", Mnemonic::MOVZX)
        .value("MPSADBW", Mnemonic::MPSADBW)
        .value("MUL", Mnemonic::MUL)
        .value("MULPD", Mnemonic::MULPD)
        .value("MULPS", Mnemonic::MULPS)
        .value("MULSD", Mnemonic::MULSD)
        .value("MULSS", Mnemonic::MULSS)
        .value("MWAIT", Mnemonic::MWAIT)
        .value("NEG", Mnemonic::NEG)
        .value("NOP", Mnemonic::NOP)
        .value("NOT", Mnemonic::NOT)
        .value("OR", Mnemonic::OR)
        .value("ORPD", Mnemonic::ORPD)
        .value("ORPS", Mnemonic::ORPS)
        .value("OUT", Mnemonic::OUT)
        .value("OUTS", Mnemonic::OUTS)
        .value("OUTSB", Mnemonic::OUTSB)
        .value("OUTSD", Mnemonic::OUTSD)
        .value("OUTSW", Mnemonic::OUTSW)
        .value("PABSB", Mnemonic::PABSB)
        .value("PABSD", Mnemonic::PABSD)
        .value("PABSW", Mnemonic::PABSW)
        .value("PACKSSDW", Mnemonic::PACKSSDW)
        .value("PACKSSWB", Mnemonic::PACKSSWB)
        .value("PACKUSDW", Mnemonic::PACKUSDW)
        .value("PACKUSWB", Mnemonic::PACKUSWB)
        .value("PADDB", Mnemonic::PADDB)
        .value("PADDD", Mnemonic::PADDD)
        .value("PADDQ", Mnemonic::PADDQ)
        .value("PADDSB", Mnemonic::PADDSB)
        .value("PADDSW", Mnemonic::PADDSW)
        .value("PADDUSB", Mnemonic::PADDUSB)
        .value("PADDUSW", Mnemonic::PADDUSW)
        .value("PADDW", Mnemonic::PADDW)
        .value("PALIGNR", Mnemonic::PALIGNR)
        .value("PAND", Mnemonic::PAND)
        .value("PANDN", Mnemonic::PANDN)
        .value("PAUSE", Mnemonic::PAUSE)
        .value("PAVGB", Mnemonic::PAVGB)
        .value("PAVGW", Mnemonic::PAVGW)
        .value("PBLENDVB", Mnemonic::PBLENDVB)
        .value("PBLENDW", Mnemonic::PBLENDW)
        .value("PCMPEQB", Mnemonic::PCMPEQB)
        .value("PCMPEQD", Mnemonic::PCMPEQD)
        .value("PCMPEQQ", Mnemonic::PCMPEQQ)
        .value("PCMPEQW", Mnemonic::PCMPEQW)
        .value("PCMPESTRI", Mnemonic::PCMPESTRI)
        .value("PCMPESTRM", Mnemonic::PCMPESTRM)
        .value("PCMPGTB", Mnemonic::PCMPGTB)
        .value("PCMPGTD", Mnemonic::PCMPGTD)
        .value("PCMPGTQ", Mnemonic::PCMPGTQ)
        .value("PCMPGTW", Mnemonic::PCMPGTW)
        .value("PCMPISTRI", Mnemonic::PCMPISTRI)
        .value("PCMPISTRM", Mnemonic::PCMPISTRM)
        .value("PEXTRB", Mnemonic::PEXTRB)
        .value("PEXTRD", Mnemonic::PEXTRD)
        .value("PEXTRQ", Mnemonic::PEXTRQ)
        .value("PEXTRW", Mnemonic::PEXTRW)
        .value("PHADDD", Mnemonic::PHADDD)
        .value("PHADDSW", Mnemonic::PHADDSW)
        .value("PHADDW", Mnemonic::PHADDW)
        .value("PHMINPOSUW", Mnemonic::PHMINPOSUW)
        .value("PHSUBD", Mnemonic::PHSUBD)
        .value("PHSUBSW", Mnemonic::PHSUBSW)
        .value("PHSUBW", Mnemonic::PHSUBW)
        .value("PINSRB", Mnemonic::PINSRB)
        .value("PINSRD", Mnemonic::PINSRD)
        .value("PINSRQ", Mnemonic::PINSRQ)
        .value("PINSRW", Mnemonic::PINSRW)
        .value("PMADDUBSW", Mnemonic::PMADDUBSW)
        .value("PMADDWD", Mnemonic::PMADDWD)
        .value("PMAXSB", Mnemonic::PMAXSB)
        .value("PMAXSD", Mnemonic::PMAXSD)
        .value("PMAXSW", Mnemonic::PMAXSW)
        .value("PMAXUB", Mnemonic::PMAXUB)
        .value("PMAXUD", Mnemonic::PMAXUD)
        .value("PMAXUW", Mnemonic::PMAXUW)
        .value("PMINSB", Mnemonic::PMINSB)
        .value("PMINSD", Mnemonic::PMINSD)
        .value("PMINSW", Mnemonic::PMINSW)
        .value("PMINUB", Mnemonic::PMINUB)
        .value("PMINUD", Mnemonic::PMINUD)
        .value("PMINUW", Mnemonic::PMINUW)
        .value("PMOVMSKB", Mnemonic::PMOVMSKB)
        .value("PMOVSXBD", Mnemonic::PMOVSXBD)
        .value("PMOVSXBQ", Mnemonic::PMOVSXBQ)
        .value("PMOVSXBW", Mnemonic::PMOVSXBW)
        .value("PMOVSXDQ", Mnemonic::PMOVSXDQ)
        .value("PMOVSXWD", Mnemonic::PMOVSXWD)
        .value("PMOVSXWQ", Mnemonic::PMOVSXWQ)
        .value("PMOVZXBD", Mnemonic::PMOVZXBD)
        .value("PMOVZXBQ", Mnemonic::PMOVZXBQ)
        .value("PMOVZXBW", Mnemonic::PMOVZXBW)
        .value("PMOVZXDQ", Mnemonic::PMOVZXDQ)
        .value("PMOVZXWD", Mnemonic::PMOVZXWD)
        .value("PMOVZXWQ", Mnemonic::PMOVZXWQ)
        .value("PMULDQ", Mnemonic::PMULDQ)
        .value("PMULHRSW", Mnemonic::PMULHRSW)
        .value("PMULHUW", Mnemonic::PMULHUW)
        .value("PMULHW", Mnemonic::PMULHW)
        .value("PMULLD", Mnemonic::PMULLD)
        .value("PMULLW", Mnemonic::PMULLW)
        .value("PMULUDQ", Mnemonic::PMULUDQ)
        .value("POP", Mnemonic::POP)
        .value("POPA", Mnemonic::POPA)
        .value("POPAD", Mnemonic::POPAD)
        .value("POPCNT", Mnemonic::POPCNT)
        .value("POPF", Mnemonic::POPF)
        .value("POPFD", Mnemonic::POPFD)
        .value("POPFQ", Mnemonic::POPFQ)
        .value("POR", Mnemonic::POR)
        .value("PREFETCHNTA", Mnemonic::PREFETCHNTA)
        .value("PREFETCHT0", Mnemonic::PREFETCHT0)
        .value("PREFETCHT1", Mnemonic::PREFETCHT1)
        .value("PREFETCHT2", Mnemonic::PREFETCHT2)
        .value("PSADBW", Mnemonic::PSADBW)
        .value("PSHUFB", Mnemonic::PSHUFB)
        .value("PSHUFD", Mnemonic::PSHUFD)
        .value("PSHUFHW", Mnemonic::PSHUFHW)
        .value("PSHUFLW", Mnemonic::PSHUFLW)
        .value("PSHUFW", Mnemonic::PSHUFW)
        .value("PSIGNB", Mnemonic::PSIGNB)
        .value("PSIGND", Mnemonic::PSIGND)
        .value("PSIGNW", Mnemonic::PSIGNW)
        .value("PSLLD", Mnemonic::PSLLD)
        .value("PSLLDQ", Mnemonic::PSLLDQ)
        .value("PSLLQ", Mnemonic::PSLLQ)
        .value("PSLLW", Mnemonic::PSLLW)
        .value("PSRAD", Mnemonic::PSRAD)
        .value("PSRAW", Mnemonic::PSRAW)
        .value("PSRLD", Mnemonic::PSRLD)
        .value("PSRLDQ", Mnemonic::PSRLDQ)
        .value("PSRLQ", Mnemonic::PSRLQ)
        .value("PSRLW", Mnemonic::PSRLW)
        .value("PSUBB", Mnemonic::PSUBB)
        .value("PSUBD", Mnemonic::PSUBD)
        .value("PSUBQ", Mnemonic::PSUBQ)
        .value("PSUBSB", Mnemonic::PSUBSB)
        .value("PSUBSW", Mnemonic::PSUBSW)
        .value("PSUBUSB", Mnemonic::PSUBUSB)
        .value("PSUBUSW", Mnemonic::PSUBUSW)
        .value("PSUBW", Mnemonic::PSUBW)
        .value("PTEST", Mnemonic::PTEST)
        .value("PUNPCKHBW", Mnemonic::PUNPCKHBW)
        .value("PUNPCKHDQ", Mnemonic::PUNPCKHDQ)
        .value("PUNPCKHQDQ", Mnemonic::PUNPCKHQDQ)
        .value("PUNPCKHWD", Mnemonic::PUNPCKHWD)
        .value("PUNPCKLBW", Mnemonic::PUNPCKLBW)
        .value("PUNPCKLDQ", Mnemonic::PUNPCKLDQ)
        .value("PUNPCKLQDQ", Mnemonic::PUNPCKLQDQ)
        .value("PUNPCKLWD", Mnemonic::PUNPCKLWD)
        .value("PUSH", Mnemonic::PUSH)
        .value("PUSHA", Mnemonic::PUSHA)
        .value("PUSHAD", Mnemonic::PUSHAD)
        .value("PUSHF", Mnemonic::PUSHF)
        .value("PUSHFD", Mnemonic::PUSHFD)
        .value("PUSHFQ", Mnemonic::PUSHFQ)
        .value("PXOR", Mnemonic::PXOR)
        .value("RCL", Mnemonic::RCL)
        .value("RCPPS", Mnemonic::RCPPS)
        .value("RCPSS", Mnemonic::RCPSS)
        .value("RCR", Mnemonic::RCR)
        .value("RDMSR", Mnemonic::RDMSR)
        .value("RDPMC", Mnemonic::RDPMC)
        .value("RDTSC", Mnemonic::RDTSC)
        .value("RDTSCP", Mnemonic::RDTSCP)
        .value("RETF", Mnemonic::RETF)
        .value("RETN", Mnemonic::RETN)
        .value("ROL", Mnemonic::ROL)
        .value("ROR", Mnemonic::ROR)
        .value("ROUNDPD", Mnemonic::ROUNDPD)
        .value("ROUNDPS", Mnemonic::ROUNDPS)
        .value("ROUNDSD", Mnemonic::ROUNDSD)
        .value("ROUNDSS", Mnemonic::ROUNDSS)
        .value("RSM", Mnemonic::RSM)
        .value("RSQRTPS", Mnemonic::RSQRTPS)
        .value("RSQRTSS", Mnemonic::RSQRTSS)
        .value("SAHF", Mnemonic::SAHF)
        .value("SAL", Mnemonic::SAL)
        .value("SALC", Mnemonic::SALC)
        .value("SAR", Mnemonic::SAR)
        .value("SBB", Mnemonic::SBB)
        .value("SCAS", Mnemonic::SCAS)
        .value("SCASB", Mnemonic::SCASB)
        .value("SCASD", Mnemonic::SCASD)
        .value("SCASQ", Mnemonic::SCASQ)
        .value("SCASW", Mnemonic::SCASW)
        .value("SETA", Mnemonic::SETA)
        .value("SETAE", Mnemonic::SETAE)
        .value("SETALC", Mnemonic::SETALC)
        .value("SETB", Mnemonic::SETB)
        .value("SETBE", Mnemonic::SETBE)
        .value("SETC", Mnemonic::SETC)
        .value("SETE", Mnemonic::SETE)
        .value("SETG", Mnemonic::SETG)
        .value("SETGE", Mnemonic::SETGE)
        .value("SETL", Mnemonic::SETL)
        .value("SETLE", Mnemonic::SETLE)
        .value("SETNA", Mnemonic::SETNA)
        .value("SETNAE", Mnemonic::SETNAE)
        .value("SETNB", Mnemonic::SETNB)
        .value("SETNBE", Mnemonic::SETNBE)
        .value("SETNC", Mnemonic::SETNC)
        .value("SETNE", Mnemonic::SETNE)
        .value("SETNG", Mnemonic::SETNG)
        .value("SETNGE", Mnemonic::SETNGE)
        .value("SETNL", Mnemonic::SETNL)
        .value("SETNLE", Mnemonic::SETNLE)
        .value("SETNO", Mnemonic::SETNO)
        .value("SETNP", Mnemonic::SETNP)
        .value("SETNS", Mnemonic::SETNS)
        .value("SETNZ", Mnemonic::SETNZ)
        .value("SETO", Mnemonic::SETO)
        .value("SETP", Mnemonic::SETP)
        .value("SETPE", Mnemonic::SETPE)
        .value("SETPO", Mnemonic::SETPO)
        .value("SETS", Mnemonic::SETS)
        .value("SETZ", Mnemonic::SETZ)
        .value("SFENCE", Mnemonic::SFENCE)
        .value("SGDT", Mnemonic::SGDT)
        .value("SHL", Mnemonic::SHL)
        .value("SHLD", Mnemonic::SHLD)
        .value("SHR", Mnemonic::SHR)
        .value("SHRD", Mnemonic::SHRD)
        .value("SHUFPD", Mnemonic::SHUFPD)
        .value("SHUFPS", Mnemonic::SHUFPS)
        .value("SIDT", Mnemonic::SIDT)
        .value("SLDT", Mnemonic::SLDT)
        .value("SMSW", Mnemonic::SMSW)
        .value("SQRTPD", Mnemonic::SQRTPD)
        .value("SQRTPS", Mnemonic::SQRTPS)
        .value("SQRTSD", Mnemonic::SQRTSD)
        .value("SQRTSS", Mnemonic::SQRTSS)
        .value("STC", Mnemonic::STC)
        .value("STD", Mnemonic::STD)
        .value("STI", Mnemonic::STI)
        .value("STMXCSR", Mnemonic::STMXCSR)
        .value("STOS", Mnemonic::STOS)
        .value("STOSB", Mnemonic::STOSB)
        .value("STOSD", Mnemonic::STOSD)
        .value("STOSQ", Mnemonic::STOSQ)
        .value("STOSW", Mnemonic::STOSW)
        .value("STR", Mnemonic::STR)
        .value("SUB", Mnemonic::SUB)
        .value("SUBPD", Mnemonic::SUBPD)
        .value("SUBPS", Mnemonic::SUBPS)
        .value("SUBSD", Mnemonic::SUBSD)
        .value("SUBSS", Mnemonic::SUBSS)
        .value("SWAPGS", Mnemonic::SWAPGS)
        .value("SYSCALL", Mnemonic::SYSCALL)
        .value("SYSENTER", Mnemonic::SYSENTER)
        .value("SYSEXIT", Mnemonic::SYSEXIT)
        .value("SYSRET", Mnemonic::SYSRET)
        .value("TEST", Mnemonic::TEST)
        .value("UCOMISD", Mnemonic::UCOMISD)
        .value("UCOMISS", Mnemonic::UCOMISS)
        .value("UD", Mnemonic::UD)
        .value("UD2", Mnemonic::UD2)
        .value("UNPCKHPD", Mnemonic::UNPCKHPD)
        .value("UNPCKHPS", Mnemonic::UNPCKHPS)
        .value("UNPCKLPD", Mnemonic::UNPCKLPD)
        .value("UNPCKLPS", Mnemonic::UNPCKLPS)
        .value("VERR", Mnemonic::VERR)
        .value("VERW", Mnemonic::VERW)
        .value("VMCALL", Mnemonic::VMCALL)
        .value("VMCLEAR", Mnemonic::VMCLEAR)
        .value("VMLAUNCH", Mnemonic::VMLAUNCH)
        .value("VMPTRLD", Mnemonic::VMPTRLD)
        .value("VMPTRST", Mnemonic::VMPTRST)
        .value("VMREAD", Mnemonic::VMREAD)
        .value("VMRESUME", Mnemonic::VMRESUME)
        .value("VMWRITE", Mnemonic::VMWRITE)
        .value("VMXOFF", Mnemonic::VMXOFF)
        .value("VMXON", Mnemonic::VMXON)
        .value("WAIT", Mnemonic::WAIT)
        .value("WBINVD", Mnemonic::WBINVD)
        .value("WRMSR", Mnemonic::WRMSR)
        .value("XADD", Mnemonic::XADD)
        .value("XCHG", Mnemonic::XCHG)
        .value("XGETBV", Mnemonic::XGETBV)
        .value("XLAT", Mnemonic::XLAT)
        .value("XLATB", Mnemonic::XLATB)
        .value("XOR", Mnemonic::XOR)
        .value("XORPD", Mnemonic::XORPD)
        .value("XORPS", Mnemonic::XORPS)
        .value("XRSTOR", Mnemonic::XRSTOR)
        .value("XSAVE", Mnemonic::XSAVE)
        .value("XSETBV", Mnemonic::XSETBV)
        .export_values();

    nb::enum_<OpSize>(m, "OpSize")
        .value("None", OpSize::None)
        .value("Byte", OpSize::Byte)
        .value("Word", OpSize::Word)
        .value("Dword", OpSize::Dword)
        .value("Qword", OpSize::Qword)
        .value("OpSz", OpSize::OpSz)
        .value("OpSzQ", OpSize::OpSzQ)
        .value("OpSzDS", OpSize::OpSzDS)
        .value("ByteSignExt", OpSize::ByteSignExt)
        .value("PackedSingle", OpSize::PackedSingle)
        .value("PackedDouble", OpSize::PackedDouble)
        .value("ScalarSingle", OpSize::ScalarSingle)
        .value("ScalarDouble", OpSize::ScalarDouble)
        .value("Xmm128", OpSize::Xmm128)
        .value("PseudoDesc", OpSize::PseudoDesc)
        .value("BoundPair", OpSize::BoundPair)
        .value("FarPtr", OpSize::FarPtr)
        .value("Mmx64", OpSize::Mmx64)
        .export_values();

    nb::enum_<AddrMode>(m, "AddrMode")
        .value("None", AddrMode::None)
        .value("Fixed", AddrMode::Fixed)
        .value("ModRM_RM", AddrMode::ModRM_RM)
        .value("ModRM_Reg", AddrMode::ModRM_Reg)
        .value("Immediate", AddrMode::Immediate)
        .value("RelOffset", AddrMode::RelOffset)
        .value("MemOnly", AddrMode::MemOnly)
        .value("RegOnly", AddrMode::RegOnly)
        .value("XmmReg", AddrMode::XmmReg)
        .value("XmmRM", AddrMode::XmmRM)
        .value("OpcodeReg", AddrMode::OpcodeReg)
        .value("Moffset", AddrMode::Moffset)
        .value("StringSrc", AddrMode::StringSrc)
        .value("StringDst", AddrMode::StringDst)
        .value("SegReg", AddrMode::SegReg)
        .value("CtrlReg", AddrMode::CtrlReg)
        .value("DbgReg", AddrMode::DbgReg)
        .value("Flags", AddrMode::Flags)
        .value("MmxReg", AddrMode::MmxReg)
        .value("MmxRM", AddrMode::MmxRM)
        .value("MmxRegOnly", AddrMode::MmxRegOnly)
        .value("XmmRegOnly", AddrMode::XmmRegOnly)
        .value("Stack", AddrMode::Stack)
        .export_values();

    nb::class_<DecodedInstr>(m, "DecodedInstr")
        .def(nb::init<>())
        .def_ro("num_prefixes", &DecodedInstr::num_prefixes)
        .def_ro("rex", &DecodedInstr::rex)
        .def_ro("modrm", &DecodedInstr::modrm)
        .def_ro("sib", &DecodedInstr::sib)
        .def_ro("opcode_reg", &DecodedInstr::opcode_reg)
        .def_ro("displacement", &DecodedInstr::displacement)
        .def_ro("immediate", &DecodedInstr::immediate)
        .def_ro("length", &DecodedInstr::length)
        .def_prop_ro("mnemonic", [](const DecodedInstr& di) -> Mnemonic {
            return di.desc ? di.desc->mnemonic : Mnemonic::INVALID;
        })
#ifdef VEDX64_STRINGS
        .def_prop_ro("mnemonic_name", [](const DecodedInstr& di) -> std::string {
            return di.desc ? mnemonic_name(di.desc->mnemonic) : "???";
        })
#endif // VEDX64_STRINGS
        .def_prop_ro("has_modrm", [](const DecodedInstr& di) -> bool {
            return di.desc ? di.desc->has_modrm : false;
        })
        .def_prop_ro("opcode", [](const DecodedInstr& di) -> uint16_t {
            return di.desc ? di.desc->opcode : 0;
        })
        .def_prop_ro("num_operands", [](const DecodedInstr& di) -> uint8_t {
            return di.desc ? di.desc->num_operands : 0;
        })
        .def_ro("has_vex", &DecodedInstr::has_vex)
        .def_ro("vex_vvvv", &DecodedInstr::vex_vvvv)
        .def_ro("vex_L", &DecodedInstr::vex_L)
        .def_ro("vex_W", &DecodedInstr::vex_W);

    m.def("decode", [](nb::bytes data) -> nb::object {
        const uint8_t* ptr = (const uint8_t*)data.c_str();
        size_t len = data.size();
        DecodedInstr* di = new DecodedInstr();
        size_t n = vedx64::decode(ptr, len, *di);
        if (n == 0) {
            delete di;
            return nb::none();
        }
        return nb::cast(di, nb::rv_policy::take_ownership);
    }, nb::arg("data"), "Decode the first instruction from a byte sequence");

    m.def("encode", [](const DecodedInstr& di) -> nb::bytes {
        uint8_t buf[16];
        size_t n = vedx64::encode(di, buf, sizeof(buf));
        return nb::bytes((const char*)buf, n);
    }, nb::arg("instr"), "Encode a decoded instruction back to bytes");

#ifdef VEDX64_STRINGS
    m.def("disassemble", [](nb::bytes data, uint64_t rip) -> nb::object {
        const uint8_t* ptr = (const uint8_t*)data.c_str();
        size_t len = data.size();
        char buf[512];
        size_t n = vedx64::disassemble(ptr, len, buf, sizeof(buf), rip);
        if (n == 0) return nb::none();
        return nb::make_tuple(std::string(buf), (int)n);
    }, nb::arg("data"), nb::arg("rip") = 0, "Disassemble the first instruction, returns (text, length) or None");

    m.def("disassemble_all", [](nb::bytes data, uint64_t rip) -> std::vector<nb::object> {
        const uint8_t* ptr = (const uint8_t*)data.c_str();
        size_t len = data.size();
        std::vector<nb::object> results;
        size_t offset = 0;
        while (offset < len) {
            char buf[512];
            size_t n = vedx64::disassemble(&ptr[offset], len - offset, buf, sizeof(buf), rip + offset);
            if (n == 0) break;
            results.push_back(nb::make_tuple(rip + offset, std::string(buf), (int)n));
            offset += n;
        }
        return results;
    }, nb::arg("data"), nb::arg("rip") = 0, "Disassemble all instructions, returns list of (address, text, length)");

#ifdef VEDX64_ASSEMBLER
    m.def("assemble", [](const std::string& text) -> nb::object {
        auto bytes = vedx64::assemble(text);
        if (!bytes) return nb::none();
        return nb::bytes((const char*)bytes->data(), bytes->size());
    }, nb::arg("text"), "Assemble a single x86-64 instruction, returns bytes or None");

    m.def("assemble_ex", [](const std::string& text) -> nb::bytes {
        std::string err;
        auto bytes = vedx64::assemble(text, err);
        if (!bytes) throw std::runtime_error(err);
        return nb::bytes((const char*)bytes->data(), bytes->size());
    }, nb::arg("text"), "Assemble instruction, raises RuntimeError on failure");

    m.def("assemble_block", [](const std::string& text) -> nb::object {
        auto bytes = vedx64::assemble_block(text);
        if (!bytes) return nb::none();
        return nb::bytes((const char*)bytes->data(), bytes->size());
    }, nb::arg("text"), "Assemble block with labels, returns bytes or None");

    m.def("assemble_block_ex", [](const std::string& text) -> nb::bytes {
        std::string err;
        auto bytes = vedx64::assemble_block(text, err);
        if (!bytes) throw std::runtime_error(err);
        return nb::bytes((const char*)bytes->data(), bytes->size());
    }, nb::arg("text"), "Assemble block, raises RuntimeError with line number on failure");
#endif // VEDX64_ASSEMBLER

    m.def("mnemonic_name", [](Mnemonic m) -> std::string {
        return vedx64::mnemonic_name(m);
    }, nb::arg("mnemonic"), "Get string name of a mnemonic");
#endif // VEDX64_STRINGS

    m.def("table_size", []() -> size_t {
        return g_instr_table_size;
    }, "Number of instruction descriptors in the table");

    nb::enum_<EncodingId>(m, "EncodingId")
        .export_values();

    // Operand types
    nb::enum_<OperandType>(m, "OperandType")
        .value("None", OperandType::None)
        .value("Register", OperandType::Register)
        .value("Immediate", OperandType::Immediate)
        .value("Memory", OperandType::Memory)
        .value("RelativeOffset", OperandType::RelativeOffset)
        .export_values();

    nb::enum_<RegisterClass>(m, "RegisterClass")
        .value("GPR8", RegisterClass::GPR8)
        .value("GPR8H", RegisterClass::GPR8H)
        .value("GPR16", RegisterClass::GPR16)
        .value("GPR32", RegisterClass::GPR32)
        .value("GPR64", RegisterClass::GPR64)
        .value("XMM", RegisterClass::XMM)
        .value("MMX", RegisterClass::MMX)
        .value("Segment", RegisterClass::Segment)
        .value("Control", RegisterClass::Control)
        .value("Debug", RegisterClass::Debug)
        .value("Flags", RegisterClass::Flags)
        .value("IP", RegisterClass::IP)
        .export_values();

    nb::class_<Operand>(m, "Operand")
        .def_ro("type", &Operand::type)
        .def("is_register", &Operand::is_register)
        .def("is_immediate", &Operand::is_immediate)
        .def("is_memory", &Operand::is_memory)
        .def("is_relative", &Operand::is_relative)
        .def_prop_ro("reg_class", [](const Operand& o) { return o.reg.reg_class; })
        .def_prop_ro("reg_id", [](const Operand& o) { return o.reg.reg_id; })
        .def_prop_ro("reg_size", [](const Operand& o) { return o.reg.size_bits; })
        .def_prop_ro("imm_value", [](const Operand& o) { return o.imm.value; })
        .def_prop_ro("mem_base", [](const Operand& o) { return o.mem.base_reg; })
        .def_prop_ro("mem_index", [](const Operand& o) { return o.mem.index_reg; })
        .def_prop_ro("mem_disp", [](const Operand& o) { return o.mem.disp; })
        .def_prop_ro("mem_size", [](const Operand& o) { return o.mem.size_bytes; })
        .def_prop_ro("rel_offset", [](const Operand& o) { return o.rel.offset; });

    // Instruction high-level API
    nb::class_<Instruction>(m, "Instruction")
        .def(nb::init<>())
        .def_static("decode_instruction", [](nb::bytes data, uint64_t rip) -> nb::object {
            const uint8_t* ptr = (const uint8_t*)data.c_str();
            size_t len = data.size();
            Instruction instr;
            size_t n = Instruction::decode_instruction(ptr, len, instr, rip);
            if (n == 0) return nb::none();
            return nb::cast(instr);
        }, nb::arg("data"), nb::arg("rip") = 0, "Decode bytes into an Instruction")
        .def_ro("mnemonic", &Instruction::mnemonic)
        .def_ro("length", &Instruction::length)
        .def_ro("address", &Instruction::address)
        .def_ro("operands", &Instruction::operands);

    // Relocation API
    m.def("can_relocate", [](nb::bytes data) -> bool {
        return vedx64::can_relocate((const uint8_t*)data.c_str(), data.size());
    }, nb::arg("data"), "Check if instruction bytes can be safely relocated");

    m.def("is_rip_relative", [](nb::bytes data) -> bool {
        return vedx64::is_rip_relative((const uint8_t*)data.c_str(), data.size());
    }, nb::arg("data"), "Check if instruction uses RIP-relative addressing");

    // Branch following API
    nb::enum_<BranchFlowType>(m, "FlowType")
        .value("Sequential", BranchFlowType::Sequential)
        .value("Branch", BranchFlowType::Branch)
        .value("Call", BranchFlowType::Call)
        .value("ConditionalBranch", BranchFlowType::ConditionalBranch)
        .value("Return", BranchFlowType::Return)
        .value("Exception", BranchFlowType::Exception)
        .value("Unknown", BranchFlowType::Unknown)
        .export_values();

    nb::class_<FlowInfo>(m, "FlowInfo")
        .def_ro("type", &FlowInfo::type)
        .def_ro("address", &FlowInfo::address)
        .def_ro("target", &FlowInfo::target)
        .def_ro("is_indirect", &FlowInfo::is_indirect)
        .def_ro("has_fallthrough", &FlowInfo::has_fallthrough)
        .def_ro("length", &FlowInfo::length);

    m.def("classify_flow", [](nb::bytes data, uint64_t rip) -> FlowInfo {
        const uint8_t* ptr = (const uint8_t*)data.c_str();
        size_t len = data.size();
        return vedx64::classify_flow(ptr, len, rip);
    }, nb::arg("data"), nb::arg("rip") = 0, "Classify the control flow of an instruction");

    // analysis submodule
    nb::module_ analysis_m = m.def_submodule("analysis",
        "Mnemonic-level queries (Jcc/CMOV/EFLAGS), patchers, condition codes.");

    nb::enum_<vedx64::analysis::CondCode>(analysis_m, "CondCode")
        .value("O", vedx64::analysis::CondCode::O)
        .value("NO", vedx64::analysis::CondCode::NO)
        .value("B", vedx64::analysis::CondCode::B)
        .value("NB", vedx64::analysis::CondCode::NB)
        .value("Z", vedx64::analysis::CondCode::Z)
        .value("NZ", vedx64::analysis::CondCode::NZ)
        .value("BE", vedx64::analysis::CondCode::BE)
        .value("NBE", vedx64::analysis::CondCode::NBE)
        .value("S", vedx64::analysis::CondCode::S)
        .value("NS", vedx64::analysis::CondCode::NS)
        .value("P", vedx64::analysis::CondCode::P)
        .value("NP", vedx64::analysis::CondCode::NP)
        .value("L", vedx64::analysis::CondCode::L)
        .value("NL", vedx64::analysis::CondCode::NL)
        .value("LE", vedx64::analysis::CondCode::LE)
        .value("NLE", vedx64::analysis::CondCode::NLE)
        .export_values();

    analysis_m.def("is_jcc", &vedx64::analysis::is_jcc, nb::arg("m"));
    analysis_m.def("is_cmov", &vedx64::analysis::is_cmov, nb::arg("m"));
    analysis_m.def("is_call", &vedx64::analysis::is_call, nb::arg("m"));
    analysis_m.def("is_ret", &vedx64::analysis::is_ret, nb::arg("m"));
    analysis_m.def("is_unconditional_branch", &vedx64::analysis::is_unconditional_branch, nb::arg("m"));
    analysis_m.def("is_relative_branch", &vedx64::analysis::is_relative_branch, nb::arg("m"));
    analysis_m.def("changes_rip", &vedx64::analysis::changes_rip, nb::arg("m"));
    analysis_m.def("is_arith", &vedx64::analysis::is_arith, nb::arg("m"));
    analysis_m.def("is_logical", &vedx64::analysis::is_logical, nb::arg("m"));
    analysis_m.def("is_shift", &vedx64::analysis::is_shift, nb::arg("m"));
    analysis_m.def("sets_eflags", &vedx64::analysis::sets_eflags, nb::arg("m"));
    analysis_m.def("reads_eflags", &vedx64::analysis::reads_eflags, nb::arg("m"));
    analysis_m.def("canonical_size", &vedx64::analysis::canonical_size, nb::arg("m"));
    analysis_m.def("jcc_condition", [](Mnemonic m) -> nb::object {
        if (!vedx64::analysis::is_jcc(m)) return nb::none();
        return nb::cast(vedx64::analysis::jcc_condition(m));
    }, nb::arg("m"), "4-bit condition code for a Jcc; None if not Jcc.");
    analysis_m.def("cmov_condition", [](Mnemonic m) -> nb::object {
        if (!vedx64::analysis::is_cmov(m)) return nb::none();
        return nb::cast(vedx64::analysis::cmov_condition(m));
    }, nb::arg("m"), "4-bit condition code for a CMOVcc; None if not CMOV.");
    analysis_m.def("jcc_for_condition", &vedx64::analysis::jcc_for_condition,
        nb::arg("cc"), "Canonical Jcc Mnemonic for a condition code.");

    analysis_m.attr("EF_CF") = (uint8_t)vedx64::analysis::EF_CF;
    analysis_m.attr("EF_PF") = (uint8_t)vedx64::analysis::EF_PF;
    analysis_m.attr("EF_AF") = (uint8_t)vedx64::analysis::EF_AF;
    analysis_m.attr("EF_ZF") = (uint8_t)vedx64::analysis::EF_ZF;
    analysis_m.attr("EF_SF") = (uint8_t)vedx64::analysis::EF_SF;
    analysis_m.attr("EF_OF") = (uint8_t)vedx64::analysis::EF_OF;
    analysis_m.attr("EF_DF") = (uint8_t)vedx64::analysis::EF_DF;
    analysis_m.attr("EF_IF") = (uint8_t)vedx64::analysis::EF_IF;

    analysis_m.def("build_jmp_rel32", [](int32_t disp) {
        uint8_t buf[5]{}; vedx64::analysis::patch_jmp_rel32(buf, disp);
        return nb::bytes((const char*)buf, sizeof(buf));
    }, nb::arg("disp"));
    analysis_m.def("build_jcc_rel32", [](uint8_t cc, int32_t disp) {
        uint8_t buf[6]{};
        vedx64::analysis::patch_jcc_rel32(buf, static_cast<vedx64::analysis::CondCode>(cc & 0xF), disp);
        return nb::bytes((const char*)buf, sizeof(buf));
    }, nb::arg("cc"), nb::arg("disp"));
    analysis_m.def("build_call_rel32", [](int32_t disp) {
        uint8_t buf[5]{}; vedx64::analysis::patch_call_rel32(buf, disp);
        return nb::bytes((const char*)buf, sizeof(buf));
    }, nb::arg("disp"));
    analysis_m.def("build_mov_imm64", [](uint8_t reg_id, uint64_t imm) {
        uint8_t buf[10]{}; vedx64::analysis::patch_mov_imm64(buf, reg_id, imm);
        return nb::bytes((const char*)buf, sizeof(buf));
    }, nb::arg("reg_id"), nb::arg("imm"));

#ifdef VEDX64_EMU
    // Emulator subsystem
    nb::enum_<StepResult>(m, "StepResult")
        .value("OK", StepResult::OK)
        .value("Halt", StepResult::Halt)
        .value("Unsupported", StepResult::Unsupported)
        .value("DecodeError", StepResult::DecodeError)
        .value("MemFault", StepResult::MemFault)
        .value("DivByZero", StepResult::DivByZero)
        .export_values();

    nb::class_<CpuState>(m, "CpuState")
        .def(nb::init<>())
        .def_rw("rip", &CpuState::rip)
        .def_rw("rflags", &CpuState::rflags)
        .def_prop_rw("rax",
            [](const CpuState& s) { return s.gpr[0]; },
            [](CpuState& s, uint64_t v) { s.gpr[0] = v; })
        .def_prop_rw("rcx",
            [](const CpuState& s) { return s.gpr[1]; },
            [](CpuState& s, uint64_t v) { s.gpr[1] = v; })
        .def_prop_rw("rdx",
            [](const CpuState& s) { return s.gpr[2]; },
            [](CpuState& s, uint64_t v) { s.gpr[2] = v; })
        .def_prop_rw("rbx",
            [](const CpuState& s) { return s.gpr[3]; },
            [](CpuState& s, uint64_t v) { s.gpr[3] = v; })
        .def_prop_rw("rsp",
            [](const CpuState& s) { return s.gpr[4]; },
            [](CpuState& s, uint64_t v) { s.gpr[4] = v; })
        .def_prop_rw("rbp",
            [](const CpuState& s) { return s.gpr[5]; },
            [](CpuState& s, uint64_t v) { s.gpr[5] = v; })
        .def_prop_rw("rsi",
            [](const CpuState& s) { return s.gpr[6]; },
            [](CpuState& s, uint64_t v) { s.gpr[6] = v; })
        .def_prop_rw("rdi",
            [](const CpuState& s) { return s.gpr[7]; },
            [](CpuState& s, uint64_t v) { s.gpr[7] = v; });

    m.def("emu_step", [](CpuState& cpu) -> StepResult {
        return emu_step(cpu);
    }, nb::arg("cpu"), "Execute one instruction");
    m.def("emu_run", [](CpuState& cpu, size_t max_steps) -> size_t {
        return emu_run(cpu, max_steps);
    }, nb::arg("cpu"), nb::arg("max_steps") = 1000000, "Run until halt/error or max_steps");
#endif // VEDX64_EMU

#ifdef VEDX64_IR
    // IR subsystem
    nb::enum_<ir::Opcode>(m, "IrOpcode")
        .value("COPY", ir::Opcode::COPY)
        .value("LOAD", ir::Opcode::LOAD)
        .value("STORE", ir::Opcode::STORE)
        .value("ADD", ir::Opcode::ADD)
        .value("SUB", ir::Opcode::SUB)
        .value("MUL", ir::Opcode::MUL)
        .value("IMUL", ir::Opcode::IMUL)
        .value("DIV", ir::Opcode::DIV)
        .value("IDIV", ir::Opcode::IDIV)
        .value("NEG", ir::Opcode::NEG)
        .value("AND", ir::Opcode::AND)
        .value("OR", ir::Opcode::OR)
        .value("XOR", ir::Opcode::XOR)
        .value("NOT", ir::Opcode::NOT)
        .value("SHL", ir::Opcode::SHL)
        .value("SHR", ir::Opcode::SHR)
        .value("SAR", ir::Opcode::SAR)
        .value("ROL", ir::Opcode::ROL)
        .value("ROR", ir::Opcode::ROR)
        .value("CMP_EQ", ir::Opcode::CMP_EQ)
        .value("CMP_NE", ir::Opcode::CMP_NE)
        .value("CMP_SLT", ir::Opcode::CMP_SLT)
        .value("CMP_ULT", ir::Opcode::CMP_ULT)
        .value("CMP_SLE", ir::Opcode::CMP_SLE)
        .value("CMP_ULE", ir::Opcode::CMP_ULE)
        .value("ZEXT", ir::Opcode::ZEXT)
        .value("SEXT", ir::Opcode::SEXT)
        .value("TRUNC", ir::Opcode::TRUNC)
        .value("ADD_FLAGS", ir::Opcode::ADD_FLAGS)
        .value("SUB_FLAGS", ir::Opcode::SUB_FLAGS)
        .value("AND_FLAGS", ir::Opcode::AND_FLAGS)
        .value("GET_CF", ir::Opcode::GET_CF)
        .value("GET_ZF", ir::Opcode::GET_ZF)
        .value("GET_SF", ir::Opcode::GET_SF)
        .value("GET_OF", ir::Opcode::GET_OF)
        .value("GET_PF", ir::Opcode::GET_PF)
        .value("SET_CF", ir::Opcode::SET_CF)
        .value("SET_ZF", ir::Opcode::SET_ZF)
        .value("SET_SF", ir::Opcode::SET_SF)
        .value("SET_OF", ir::Opcode::SET_OF)
        .value("SET_PF", ir::Opcode::SET_PF)
        .value("BRANCH", ir::Opcode::BRANCH)
        .value("CBRANCH", ir::Opcode::CBRANCH)
        .value("CALL", ir::Opcode::CALL)
        .value("RET", ir::Opcode::RET)
        .value("NOP", ir::Opcode::NOP)
        .value("UNDEF", ir::Opcode::UNDEF)
        .value("BARRIER", ir::Opcode::BARRIER)
        .export_values();

    nb::enum_<ir::Space>(m, "IrSpace")
        .value("Const", ir::Space::Const)
        .value("Temp", ir::Space::Temp)
        .value("GPR", ir::Space::GPR)
        .value("Flags", ir::Space::Flags)
        .value("XMM", ir::Space::XMM)
        .value("MMX", ir::Space::MMX)
        .value("Seg", ir::Space::Seg)
        .value("RAM", ir::Space::RAM)
        .export_values();

    nb::class_<ir::VarNode>(m, "IrVarNode")
        .def_ro("space", &ir::VarNode::space)
        .def_ro("offset", &ir::VarNode::offset)
        .def_ro("size", &ir::VarNode::size)
        .def_ro("value", &ir::VarNode::value);

    nb::class_<ir::Op>(m, "IrOp")
        .def_ro("opcode", &ir::Op::opcode)
        .def_ro("output", &ir::Op::output)
        .def_ro("num_inputs", &ir::Op::num_inputs)
        .def_prop_ro("inputs", [](const ir::Op& op) {
            std::vector<ir::VarNode> v;
            for (uint8_t i = 0; i < op.num_inputs && i < 3; ++i) v.push_back(op.inputs[i]);
            return v;
        });

    nb::class_<ir::Lifted>(m, "IrLifted")
        .def_ro("ops", &ir::Lifted::ops)
        .def_ro("address", &ir::Lifted::address)
        .def_ro("length", &ir::Lifted::length);

    m.def("ir_lift", [](nb::bytes data, uint64_t rip) -> nb::object {
        const uint8_t* ptr = (const uint8_t*)data.c_str();
        size_t len = data.size();
        auto result = ir::lift(ptr, len, rip);
        if (!result) return nb::none();
        return nb::cast(*result);
    }, nb::arg("data"), nb::arg("rip") = 0, "Lift instruction bytes to IR");
#endif // VEDX64_IR

}
