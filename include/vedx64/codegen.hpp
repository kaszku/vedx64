// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <stdexcept>

namespace vedx64 {

struct Reg {
    uint8_t id;
    uint8_t bits; // 8, 16, 32, 64
    bool operator==(const Reg& o) const { return id == o.id && bits == o.bits; }
};

struct Xmm {
    uint8_t id;
};

struct Zmm {
    uint8_t id;
    operator Xmm() const { return {id}; }
};

struct Mmx {
    uint8_t id;
};

struct CL_Reg {
    constexpr operator Reg() const noexcept { return Reg{1, 8}; }
};
inline constexpr Reg al{0, 8};
inline constexpr CL_Reg cl{};
inline constexpr Reg dl{2, 8};
inline constexpr Reg bl{3, 8};
inline constexpr Reg spl{4, 8};
inline constexpr Reg bpl{5, 8};
inline constexpr Reg sil{6, 8};
inline constexpr Reg dil{7, 8};
inline constexpr Reg r8b{8, 8};
inline constexpr Reg r9b{9, 8};
inline constexpr Reg r10b{10, 8};
inline constexpr Reg r11b{11, 8};
inline constexpr Reg r12b{12, 8};
inline constexpr Reg r13b{13, 8};
inline constexpr Reg r14b{14, 8};
inline constexpr Reg r15b{15, 8};
inline constexpr Reg ax{0, 16};
inline constexpr Reg cx{1, 16};
inline constexpr Reg dx{2, 16};
inline constexpr Reg bx{3, 16};
inline constexpr Reg sp{4, 16};
inline constexpr Reg bp{5, 16};
inline constexpr Reg si{6, 16};
inline constexpr Reg di{7, 16};
inline constexpr Reg r8w{8, 16};
inline constexpr Reg r9w{9, 16};
inline constexpr Reg r10w{10, 16};
inline constexpr Reg r11w{11, 16};
inline constexpr Reg r12w{12, 16};
inline constexpr Reg r13w{13, 16};
inline constexpr Reg r14w{14, 16};
inline constexpr Reg r15w{15, 16};
inline constexpr Reg eax{0, 32};
inline constexpr Reg ecx{1, 32};
inline constexpr Reg edx{2, 32};
inline constexpr Reg ebx{3, 32};
inline constexpr Reg esp{4, 32};
inline constexpr Reg ebp{5, 32};
inline constexpr Reg esi{6, 32};
inline constexpr Reg edi{7, 32};
inline constexpr Reg r8d{8, 32};
inline constexpr Reg r9d{9, 32};
inline constexpr Reg r10d{10, 32};
inline constexpr Reg r11d{11, 32};
inline constexpr Reg r12d{12, 32};
inline constexpr Reg r13d{13, 32};
inline constexpr Reg r14d{14, 32};
inline constexpr Reg r15d{15, 32};
inline constexpr Reg rax{0, 64};
inline constexpr Reg rcx{1, 64};
inline constexpr Reg rdx{2, 64};
inline constexpr Reg rbx{3, 64};
inline constexpr Reg rsp{4, 64};
inline constexpr Reg rbp{5, 64};
inline constexpr Reg rsi{6, 64};
inline constexpr Reg rdi{7, 64};
inline constexpr Reg r8{8, 64};
inline constexpr Reg r9{9, 64};
inline constexpr Reg r10{10, 64};
inline constexpr Reg r11{11, 64};
inline constexpr Reg r12{12, 64};
inline constexpr Reg r13{13, 64};
inline constexpr Reg r14{14, 64};
inline constexpr Reg r15{15, 64};

inline constexpr Xmm xmm0{0};
inline constexpr Xmm xmm1{1};
inline constexpr Xmm xmm2{2};
inline constexpr Xmm xmm3{3};
inline constexpr Xmm xmm4{4};
inline constexpr Xmm xmm5{5};
inline constexpr Xmm xmm6{6};
inline constexpr Xmm xmm7{7};
inline constexpr Xmm xmm8{8};
inline constexpr Xmm xmm9{9};
inline constexpr Xmm xmm10{10};
inline constexpr Xmm xmm11{11};
inline constexpr Xmm xmm12{12};
inline constexpr Xmm xmm13{13};
inline constexpr Xmm xmm14{14};
inline constexpr Xmm xmm15{15};
inline constexpr Zmm zmm0{0};
inline constexpr Zmm zmm1{1};
inline constexpr Zmm zmm2{2};
inline constexpr Zmm zmm3{3};
inline constexpr Zmm zmm4{4};
inline constexpr Zmm zmm5{5};
inline constexpr Zmm zmm6{6};
inline constexpr Zmm zmm7{7};
inline constexpr Zmm zmm8{8};
inline constexpr Zmm zmm9{9};
inline constexpr Zmm zmm10{10};
inline constexpr Zmm zmm11{11};
inline constexpr Zmm zmm12{12};
inline constexpr Zmm zmm13{13};
inline constexpr Zmm zmm14{14};
inline constexpr Zmm zmm15{15};
inline constexpr Zmm zmm16{16};
inline constexpr Zmm zmm17{17};
inline constexpr Zmm zmm18{18};
inline constexpr Zmm zmm19{19};
inline constexpr Zmm zmm20{20};
inline constexpr Zmm zmm21{21};
inline constexpr Zmm zmm22{22};
inline constexpr Zmm zmm23{23};
inline constexpr Zmm zmm24{24};
inline constexpr Zmm zmm25{25};
inline constexpr Zmm zmm26{26};
inline constexpr Zmm zmm27{27};
inline constexpr Zmm zmm28{28};
inline constexpr Zmm zmm29{29};
inline constexpr Zmm zmm30{30};
inline constexpr Zmm zmm31{31};
inline constexpr Mmx mm0{0};
inline constexpr Mmx mm1{1};
inline constexpr Mmx mm2{2};
inline constexpr Mmx mm3{3};
inline constexpr Mmx mm4{4};
inline constexpr Mmx mm5{5};
inline constexpr Mmx mm6{6};
inline constexpr Mmx mm7{7};

/// x86-64 instruction prefix.
enum class Prefix : uint8_t {
    // Group 1: lock / repeat
    Lock   = 0xF0,
    RepNE  = 0xF2, RepNZ = 0xF2,
    Rep    = 0xF3, RepE = 0xF3, RepZ = 0xF3,
    // Group 2: segment override
    ES = 0x26, CS = 0x2E, SS = 0x36, DS = 0x3E,
    FS = 0x64, GS = 0x65,
    // Group 2: branch hints (same bytes as CS/DS)
    BranchNotTaken = 0x2E, BranchTaken = 0x3E,
    // Group 3: operand-size override
    Data16 = 0x66,
    // Group 4: address-size override
    Addr32 = 0x67
};

/// Segment override for memory operands (subset of Prefix).
enum class SegOverride : uint8_t {
    None = 0,
    ES = 0x26, CS = 0x2E, SS = 0x36, DS = 0x3E,
    FS = 0x64, GS = 0x65
};

struct Mem {
    Reg base;    bool has_base = false;
    Reg index;   bool has_index = false;
    uint8_t scale = 1;
    int32_t disp = 0;
    uint8_t size_hint = 0; // 0=auto, 1=byte, 2=word, 4=dword, 8=qword
    bool rip_relative = false;
    SegOverride seg = SegOverride::None; // segment override prefix
};

inline Mem ptr(Reg base) { Mem m; m.base = base; m.has_base = true; return m; }
inline Mem ptr(Reg base, int32_t disp) { Mem m; m.base = base; m.has_base = true; m.disp = disp; return m; }
inline Mem ptr(Reg base, Reg index, uint8_t scale) { Mem m; m.base = base; m.has_base = true; m.index = index; m.has_index = true; m.scale = scale; return m; }
inline Mem ptr(Reg base, Reg index, uint8_t scale, int32_t disp) { Mem m; m.base = base; m.has_base = true; m.index = index; m.has_index = true; m.scale = scale; m.disp = disp; return m; }
inline Mem rip_ptr(int32_t disp) { Mem m; m.disp = disp; m.rip_relative = true; return m; }
inline Mem byte_ptr(Reg b) { Mem m = ptr(b); m.size_hint = 1; return m; }
inline Mem byte_ptr(Reg b, int32_t d) { Mem m = ptr(b,d); m.size_hint = 1; return m; }
inline Mem dword_ptr(Reg b) { Mem m = ptr(b); m.size_hint = 4; return m; }
inline Mem dword_ptr(Reg b, int32_t d) { Mem m = ptr(b,d); m.size_hint = 4; return m; }
inline Mem qword_ptr(Reg b) { Mem m = ptr(b); m.size_hint = 8; return m; }
inline Mem qword_ptr(Reg b, int32_t d) { Mem m = ptr(b,d); m.size_hint = 8; return m; }
inline Mem word_ptr(Reg b) { Mem m = ptr(b); m.size_hint = 2; return m; }
inline Mem word_ptr(Reg b, int32_t d) { Mem m = ptr(b,d); m.size_hint = 2; return m; }

/// Apply a segment override to a memory operand.
inline Mem seg(SegOverride s, Mem m) { m.seg = s; return m; }
inline Mem fs(Mem m) { m.seg = SegOverride::FS; return m; }
inline Mem gs(Mem m) { m.seg = SegOverride::GS; return m; }
inline Mem cs(Mem m) { m.seg = SegOverride::CS; return m; }
inline Mem ss(Mem m) { m.seg = SegOverride::SS; return m; }
inline Mem ds(Mem m) { m.seg = SegOverride::DS; return m; }
inline Mem es(Mem m) { m.seg = SegOverride::ES; return m; }

inline Mem fs_ptr(Reg b) { return fs(ptr(b)); }
inline Mem fs_ptr(Reg b, int32_t d) { return fs(ptr(b, d)); }
inline Mem gs_ptr(Reg b) { return gs(ptr(b)); }
inline Mem gs_ptr(Reg b, int32_t d) { return gs(ptr(b, d)); }

/// Absolute segment-relative address: seg:[disp32].
inline Mem fs_abs(int32_t disp) { Mem m; m.disp = disp; m.seg = SegOverride::FS; return m; }
inline Mem gs_abs(int32_t disp) { Mem m; m.disp = disp; m.seg = SegOverride::GS; return m; }

struct Label {
    uint32_t id = 0xFFFFFFFF;
    int64_t offset = -1;
    bool bound = false;
};

class CodeGen {
    std::vector<uint8_t> buf_;
    uint32_t label_count_ = 0;
    struct Patch { size_t offset; uint32_t label_id; uint8_t size; };
    std::vector<Patch> patches_;
    std::vector<Label*> labels_;

    void emit8(uint8_t v) { buf_.push_back(v); }
    void emit16(uint16_t v) { emit8((uint8_t)v); emit8((uint8_t)(v>>8)); }
    void emit32(uint32_t v) { emit8((uint8_t)v); emit8((uint8_t)(v>>8)); emit8((uint8_t)(v>>16)); emit8((uint8_t)(v>>24)); }
    void emit64(uint64_t v) { emit32((uint32_t)v); emit32((uint32_t)(v>>32)); }

    bool needs_rex(Reg r) { return r.id >= 8; }
    uint8_t rex_byte(bool w, uint8_t r, uint8_t x, uint8_t b) {
        return 0x40 | (w?8:0) | ((r>>3)<<2) | ((x>>3)<<1) | (b>>3);
    }

    void emit_rex_if_needed(bool w, uint8_t reg, uint8_t index, uint8_t rm) {
        uint8_t r = rex_byte(w, reg, index, rm);
        if (r != 0x40 || w) emit8(r);
    }

    void emit_modrm(uint8_t mod_, uint8_t reg, uint8_t rm) {
        emit8((mod_<<6)|((reg&7)<<3)|(rm&7));
    }

    void emit_sib(uint8_t scale, uint8_t index, uint8_t base) {
        uint8_t s = 0;
        if (scale==2) s=1; else if (scale==4) s=2; else if (scale==8) s=3;
        emit8((s<<6)|((index&7)<<3)|(base&7));
    }

    void emit_mem(uint8_t reg_opcd, const Mem& m) {
        if (m.rip_relative) {
            emit_modrm(0, reg_opcd, 5);
            emit32((uint32_t)m.disp);
            return;
        }
        uint8_t base_id = m.has_base ? (m.base.id & 7) : 5;
        bool need_sib = m.has_index || (m.has_base && (m.base.id & 7) == 4);
        uint8_t mod_ = 0;
        if (m.disp == 0 && (base_id != 5 || !m.has_base)) mod_ = 0;
        else if (m.disp >= -128 && m.disp <= 127 && m.has_base) mod_ = 1;
        else mod_ = 2;
        if (!m.has_base) mod_ = 0; // disp32 only
        if (need_sib) {
            emit_modrm(mod_, reg_opcd, 4);
            uint8_t idx = m.has_index ? (m.index.id & 7) : 4;
            emit_sib(m.scale, idx, base_id);
        } else {
            emit_modrm(mod_, reg_opcd, base_id);
        }
        if (mod_ == 1) emit8((uint8_t)(int8_t)m.disp);
        else if (mod_ == 2 || !m.has_base) emit32((uint32_t)m.disp);
    }

    void emit_seg_prefix(const Mem& m) {
        if (m.seg != SegOverride::None) emit8(static_cast<uint8_t>(m.seg));
    }

    void emit_rex_mem(bool w, uint8_t reg, const Mem& m) {
        emit_seg_prefix(m);
        uint8_t idx = m.has_index ? m.index.id : 0;
        uint8_t base = m.has_base ? m.base.id : 0;
        uint8_t r = rex_byte(w, reg, idx, base);
        if (r != 0x40 || w) emit8(r);
    }

    void emit_imm(int64_t v, uint8_t sz) {
        if (sz==1) emit8((uint8_t)(int8_t)v);
        else if (sz==2) emit16((uint16_t)(int16_t)v);
        else if (sz==4) emit32((uint32_t)(int32_t)v);
        else if (sz==8) emit64((uint64_t)v);
    }

    void emit_label_rel32(Label& lab) {
        if (lab.bound) {
            int32_t rel = (int32_t)(lab.offset - (int64_t)(buf_.size() + 4));
            emit32((uint32_t)rel);
        } else {
            patches_.push_back({buf_.size(), lab.id, 4});
            emit32(0); // placeholder
        }
    }

    void emit_label_rel8(Label& lab) {
        if (lab.bound) {
            int8_t rel = (int8_t)(lab.offset - (int64_t)(buf_.size() + 1));
            emit8((uint8_t)rel);
        } else {
            patches_.push_back({buf_.size(), lab.id, 1});
            emit8(0); // placeholder
        }
    }

    void emit_vex2(uint8_t pp, uint8_t reg, uint8_t vvvv, uint8_t L) {
        // 2-byte VEX: C5 [R.vvvv.L.pp]
        uint8_t R = (reg > 7) ? 0 : 0x80;
        emit8(0xC5);
        emit8(R | ((~vvvv & 0xF) << 3) | (L << 2) | pp);
    }

    void emit_vex3(uint8_t map, uint8_t pp, uint8_t reg, uint8_t idx, uint8_t base, uint8_t vvvv, uint8_t L, uint8_t W) {
        // 3-byte VEX: C4 [RXB.mmmmm] [W.vvvv.L.pp]
        uint8_t R = (reg > 7) ? 0 : 0x80;
        uint8_t X = (idx > 7) ? 0 : 0x40;
        uint8_t B = (base > 7) ? 0 : 0x20;
        emit8(0xC4);
        emit8(R | X | B | (map & 0x1F));
        emit8((W << 7) | ((~vvvv & 0xF) << 3) | (L << 2) | pp);
    }

    void emit_vex_rr(uint8_t map, uint8_t pp, uint8_t opcode, uint8_t dst, uint8_t src, uint8_t vvvv, uint8_t L, uint8_t W) {
        if (map == 1 && src <= 7 && !W) emit_vex2(pp, dst, vvvv, L);
        else emit_vex3(map, pp, dst, 0, src, vvvv, L, W);
        emit8(opcode);
        emit_modrm(3, dst & 7, src & 7);
    }

    void emit_vex_rm(uint8_t map, uint8_t pp, uint8_t opcode, uint8_t reg, const Mem& m, uint8_t vvvv, uint8_t L, uint8_t W) {
        uint8_t base = m.has_base ? m.base.id : 0;
        uint8_t idx = m.has_index ? m.index.id : 0;
        emit_vex3(map, pp, reg, idx, base, vvvv, L, W);
        emit8(opcode);
        emit_mem(reg & 7, m);
    }

    void emit_evex(uint8_t map, uint8_t pp, uint8_t reg, uint8_t idx, uint8_t base, uint8_t vvvv, uint8_t L, uint8_t W, uint8_t aaa = 0, bool z = false) {
        uint8_t p0 = ((reg > 7) ? 0 : 0x80) | ((idx > 7) ? 0 : 0x40) | ((base > 7) ? 0 : 0x20) | 0x10 | (map & 7);
        uint8_t p1 = (W << 7) | ((~vvvv & 0xF) << 3) | 0x04 | pp;
        uint8_t p2 = (z ? 0x80 : 0) | ((L & 3) << 5) | 0x08 | (aaa & 7);
        emit8(0x62); emit8(p0); emit8(p1); emit8(p2);
    }

    void emit_evex_rr(uint8_t map, uint8_t pp, uint8_t opcode, uint8_t dst, uint8_t src, uint8_t vvvv, uint8_t L, uint8_t W, uint8_t aaa = 0, bool z = false) {
        emit_evex(map, pp, dst, 0, src, vvvv, L, W, aaa, z);
        emit8(opcode);
        emit_modrm(3, dst & 7, src & 7);
    }

    void emit_evex_rr_ib(uint8_t map, uint8_t pp, uint8_t opcode, uint8_t dst, uint8_t src, uint8_t vvvv, uint8_t L, uint8_t W, uint8_t imm, uint8_t aaa = 0, bool z = false) {
        emit_evex_rr(map, pp, opcode, dst, src, vvvv, L, W, aaa, z);
        emit8(imm);
    }

public:
    CodeGen() = default;
    const uint8_t* data() const { return buf_.data(); }
    size_t size() const { return buf_.size(); }
    void clear() { buf_.clear(); patches_.clear(); label_count_ = 0; }

    Label label() { Label l; l.id = label_count_++; return l; }
    void bind(Label& lab) { lab.offset = (int64_t)buf_.size(); lab.bound = true;
        for (auto& p : patches_) {
            if (p.label_id != lab.id) continue;
            if (p.size == 4) {
                int32_t rel = (int32_t)(lab.offset - (int64_t)(p.offset + 4));
                memcpy(&buf_[p.offset], &rel, 4);
            } else {
                int8_t rel = (int8_t)(lab.offset - (int64_t)(p.offset + 1));
                buf_[p.offset] = (uint8_t)rel;
            }
        }
    }

    CodeGen& db(uint8_t v) { emit8(v); return *this; }
    CodeGen& dw(uint16_t v) { emit16(v); return *this; }
    CodeGen& dd(uint32_t v) { emit32(v); return *this; }
    CodeGen& dq(uint64_t v) { emit64(v); return *this; }

    CodeGen& aaa() {
        emit8(0x37);
        return *this;
    }

    CodeGen& aad() {
        emit8(0xD5);
        emit8(0x0A);
        return *this;
    }

    CodeGen& aam() {
        emit8(0xD4);
        emit8(0x0A);
        return *this;
    }

    CodeGen& aas() {
        emit8(0x3F);
        return *this;
    }

    CodeGen& adc(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x11);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& adc(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x13);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& adc(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x11);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& adc(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0x80);
            emit_modrm(3, 2, dst.id);
            emit_imm(imm, 1);
        } else if (imm >= -128 && imm <= 127) {
            emit8(0x83);
            emit_modrm(3, 2, dst.id);
            emit_imm(imm, 1);
        } else {
            emit8(0x81);
            emit_modrm(3, 2, dst.id);
            emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& adc(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x80);
        emit_mem(2, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& add(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x01);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& add(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x03);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& add(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x01);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& add(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0x80);
            emit_modrm(3, 0, dst.id);
            emit_imm(imm, 1);
        } else if (imm >= -128 && imm <= 127) {
            emit8(0x83);
            emit_modrm(3, 0, dst.id);
            emit_imm(imm, 1);
        } else {
            emit8(0x81);
            emit_modrm(3, 0, dst.id);
            emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& add(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x80);
        emit_mem(0, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& addpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x58);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& addpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x58);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& addps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x58);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& addps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x58);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& addsd(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x58);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& addsd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x58);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& addss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x58);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& addss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x58);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& addsubpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xD0);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& addsubpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD0);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& addsubps(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xD0);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& addsubps(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD0);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& adx() {
        emit8(0xD5);
        return *this;
    }

    CodeGen& amx() {
        emit8(0xD4);
        return *this;
    }

    CodeGen& and_(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x21);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& and_(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x23);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& and_(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x21);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& and_(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0x80);
            emit_modrm(3, 4, dst.id);
            emit_imm(imm, 1);
        } else if (imm >= -128 && imm <= 127) {
            emit8(0x83);
            emit_modrm(3, 4, dst.id);
            emit_imm(imm, 1);
        } else {
            emit8(0x81);
            emit_modrm(3, 4, dst.id);
            emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& and_(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x80);
        emit_mem(4, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& andnpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x55);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& andnpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x55);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& andnps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x55);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& andnps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x55);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& andpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x54);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& andpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x54);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& andps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x54);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& andps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x54);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& blendpd(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0D);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& blendpd(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0D);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& blendps(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0C);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& blendps(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0C);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& blendvpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x15);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& blendvpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x15);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& blendvps(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x14);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& blendvps(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x14);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& bound(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x62);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& bsf(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xBC);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& bsf(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xBC);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& bsr(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xBD);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& bsr(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xBD);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& bswap(Reg r) {
        // Default-64 ops (push/pop) don't need REX.W
        emit_rex_if_needed(false, 0, 0, r.id);
        emit8(0x0F); emit8(0xC8 | (r.id & 7));
        return *this;
    }

    CodeGen& bt(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x0F); emit8(0xA3);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& bt(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0xA3);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& bt(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        emit8(0x0F); emit8(0xBA);
        emit_modrm(3, 4, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& bt(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x0F); emit8(0xBA);
        emit_mem(4, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& btc(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x0F); emit8(0xBB);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& btc(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0xBA);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& btc(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        emit8(0x0F); emit8(0xBA);
        emit_modrm(3, 7, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& btc(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x0F); emit8(0xBA);
        emit_mem(7, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& btr(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x0F); emit8(0xB3);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& btr(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0xB3);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& btr(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        emit8(0x0F); emit8(0xBA);
        emit_modrm(3, 6, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& btr(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x0F); emit8(0xBA);
        emit_mem(6, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& bts(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x0F); emit8(0xAB);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& bts(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0xAB);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& bts(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        emit8(0x0F); emit8(0xBA);
        emit_modrm(3, 5, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& bts(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x0F); emit8(0xBA);
        emit_mem(5, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& call(Label& lab) {
        emit8(0xE8);
        emit_label_rel32(lab);
        return *this;
    }
    CodeGen& call(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xFF);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& call(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xFF);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& callf(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xFF);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& callf(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xFF);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& cbw() {
        emit8(0x98);
        return *this;
    }

    CodeGen& cdq() {
        emit8(0x99);
        return *this;
    }

    CodeGen& clc() {
        emit8(0xF8);
        return *this;
    }

    CodeGen& cld() {
        emit8(0xFC);
        return *this;
    }

    CodeGen& clflush(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& clflush(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xAE);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& cli() {
        emit8(0xFA);
        return *this;
    }

    CodeGen& clts() {
        emit8(0x0F); emit8(0x06);
        return *this;
    }

    CodeGen& cmc() {
        emit8(0xF5);
        return *this;
    }

    CodeGen& cmovb(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x42);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovb(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x42);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovbe(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x46);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovbe(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x46);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovl(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x4C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovl(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x4C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovle(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x4E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovle(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x4E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovnb(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x43);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovnb(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x43);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovnbe(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x47);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovnbe(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x47);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovnl(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x4D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovnl(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x4D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovnle(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x4F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovnle(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x4F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovno(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x41);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovno(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x41);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovnp(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x4B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovnp(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x4B);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovns(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x49);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovns(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x49);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovnz(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x45);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovnz(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x45);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovo(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x40);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovo(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x40);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovp(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x4A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovp(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x4A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovs(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x48);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovs(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x48);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmovz(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x44);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cmovz(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x44);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cmp(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x39);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& cmp(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x3B);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& cmp(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x39);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& cmp(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0x80);
            emit_modrm(3, 7, dst.id);
            emit_imm(imm, 1);
        } else if (imm >= -128 && imm <= 127) {
            emit8(0x83);
            emit_modrm(3, 7, dst.id);
            emit_imm(imm, 1);
        } else {
            emit8(0x81);
            emit_modrm(3, 7, dst.id);
            emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& cmp(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x80);
        emit_mem(7, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& cmppd(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xC2);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& cmppd(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xC2);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& cmpps(Xmm dst, Xmm src, uint8_t imm) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xC2);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& cmpps(Xmm dst, Mem src, uint8_t imm) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xC2);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& cmps() {
        emit8(0xA6);
        return *this;
    }

    CodeGen& cmpsd(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xC2);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& cmpsd(Xmm dst, Mem src, uint8_t imm) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xC2);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& cmpss(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xC2);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& cmpss(Xmm dst, Mem src, uint8_t imm) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xC2);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& cmpxchg(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x0F); emit8(0xB1);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& cmpxchg(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0xB1);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& cmpxchg8b(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xC7);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& cmpxchg8b(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xC7);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& comisd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& comisd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& comiss(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& comiss(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cpuid() {
        emit8(0x0F); emit8(0xA2);
        return *this;
    }

    CodeGen& crc32(Reg dst, Reg src) {
        if (dst.bits == 16) emit8(0x66); else emit8(0xF2);
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x38);
        emit8(0xF1);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& crc32(Reg dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x38);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtdq2pd(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE6);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtdq2pd(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE6);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtdq2ps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtdq2ps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5B);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtpd2dq(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE6);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtpd2dq(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE6);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtpd2pi(Mmx dst, Xmm src) {
        emit8(0x66);
        emit8(0x0F);
        emit8(0x2D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtpd2pi(Mmx dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtpd2ps(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtpd2ps(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtpi2pd(Xmm dst, Mmx src) {
        emit8(0x66);
        emit8(0x0F);
        emit8(0x2A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtpi2pd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtpi2ps(Xmm dst, Mmx src) {
        emit8(0x0F);
        emit8(0x2A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtpi2ps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtps2dq(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtps2dq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5B);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtps2pd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtps2pd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtps2pi(Mmx dst, Xmm src) {
        emit8(0x0F);
        emit8(0x2D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtps2pi(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtsd2si(Reg dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtsd2si(Reg dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F);
        emit8(0x2D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtsd2ss(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtsd2ss(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtsi2sd(Xmm dst, Reg src) {
        emit8(0xF2);
        emit_rex_if_needed(src.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtsi2sd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtsi2ss(Xmm dst, Reg src) {
        emit8(0xF3);
        emit_rex_if_needed(src.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtsi2ss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtss2sd(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtss2sd(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvtss2si(Reg dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvtss2si(Reg dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F);
        emit8(0x2D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvttpd2dq(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE6);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvttpd2dq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE6);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvttpd2pi(Mmx dst, Xmm src) {
        emit8(0x66);
        emit8(0x0F);
        emit8(0x2C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvttpd2pi(Mmx dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvttps2dq(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvttps2dq(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5B);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvttps2pi(Mmx dst, Xmm src) {
        emit8(0x0F);
        emit8(0x2C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvttps2pi(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvttsd2si(Reg dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvttsd2si(Reg dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F);
        emit8(0x2C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cvttss2si(Reg dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& cvttss2si(Reg dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F);
        emit8(0x2C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& cwd() {
        emit8(0x99);
        return *this;
    }

    CodeGen& cwde() {
        emit8(0x98);
        return *this;
    }

    CodeGen& daa() {
        emit8(0x27);
        return *this;
    }

    CodeGen& das() {
        emit8(0x2F);
        return *this;
    }

    CodeGen& dec(Reg r) {
        if (r.bits == 16) emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(r.bits == 8 ? 0xFE : 0xFF);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& dec(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        if (m.size_hint == 2) emit8(0x66);
        emit8(m.size_hint == 1 ? 0xFE : 0xFF);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& div(Reg r) {
        if (r.bits == 16) emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(r.bits == 8 ? 0xF6 : 0xF7);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& div(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        if (m.size_hint == 2) emit8(0x66);
        emit8(m.size_hint == 1 ? 0xF6 : 0xF7);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& divpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& divpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& divps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& divps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& divsd(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& divsd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& divss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& divss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& dppd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x41);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& dppd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x41);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& dpps(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x40);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& dpps(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x40);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& emms() {
        emit8(0x0F); emit8(0x77);
        return *this;
    }

    CodeGen& enter() {
        emit8(0xC8);
        return *this;
    }

    CodeGen& extractps(Reg dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, src.id, 0, dst.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x17);
        emit_modrm(3, src.id, dst.id);
        emit8(imm);
        return *this;
    }
    CodeGen& extractps(Mem dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x17);
        emit_mem(src.id, dst);
        emit8(imm);
        return *this;
    }

    CodeGen& f2xm1() {
        emit8(0xD9);
        emit8(0xF0);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& fabs() {
        emit8(0xD9);
        emit8(0xE1);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fadd(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD8);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& fadd(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD8);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& faddp() {
        emit8(0xDE);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& fbld(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDF);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& fbld(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDF);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& fbstp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDF);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& fbstp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDF);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& fchs() {
        emit8(0xD9);
        emit8(0xE0);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fclex() {
        emit8(0x9B);
        emit8(0xDB);
        emit8(0xE2);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fcmovb() {
        emit8(0xDA);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& fcmovbe() {
        emit8(0xDA);
        emit_modrm(3, 2, 0);
        return *this;
    }

    CodeGen& fcmove() {
        emit8(0xDA);
        emit_modrm(3, 1, 0);
        return *this;
    }

    CodeGen& fcmovnb() {
        emit8(0xDB);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& fcmovnbe() {
        emit8(0xDB);
        emit_modrm(3, 2, 0);
        return *this;
    }

    CodeGen& fcmovne() {
        emit8(0xDB);
        emit_modrm(3, 1, 0);
        return *this;
    }

    CodeGen& fcmovnu() {
        emit8(0xDB);
        emit_modrm(3, 3, 0);
        return *this;
    }

    CodeGen& fcmovu() {
        emit8(0xDA);
        emit_modrm(3, 3, 0);
        return *this;
    }

    CodeGen& fcom() {
        emit8(0xD8);
        emit_modrm(3, 2, 0);
        return *this;
    }
    CodeGen& fcom(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD8);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& fcom(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD8);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& fcom2() {
        emit8(0xDC);
        emit_modrm(3, 2, 0);
        return *this;
    }

    CodeGen& fcomi() {
        emit8(0xDB);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& fcomip() {
        emit8(0xDF);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& fcomp() {
        emit8(0xD8);
        emit_modrm(3, 3, 0);
        return *this;
    }
    CodeGen& fcomp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD8);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& fcomp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD8);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& fcomp3() {
        emit8(0xDC);
        emit_modrm(3, 3, 0);
        return *this;
    }

    CodeGen& fcomp5() {
        emit8(0xDE);
        emit_modrm(3, 2, 0);
        return *this;
    }

    CodeGen& fcompp() {
        emit8(0xDE);
        emit8(0xD9);
        emit_modrm(3, 3, 0);
        return *this;
    }

    CodeGen& fcos() {
        emit8(0xD9);
        emit8(0xFF);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& fdecstp() {
        emit8(0xD9);
        emit8(0xF6);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& fdisi() {
        emit8(0x9B);
        emit8(0xDB);
        emit8(0xE1);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fdiv(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD8);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& fdiv(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD8);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& fdivp() {
        emit8(0xDE);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& fdivr(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD8);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& fdivr(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD8);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& fdivrp() {
        emit8(0xDE);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& feni() {
        emit8(0x9B);
        emit8(0xDB);
        emit8(0xE0);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& ffree() {
        emit8(0xDD);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& ffreep() {
        emit8(0xDF);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& fiadd(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDA);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& fiadd(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDA);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& ficom(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDA);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& ficom(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDA);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& ficomp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDA);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& ficomp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDA);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& fidiv(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDA);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& fidiv(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDA);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& fidivr(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDA);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& fidivr(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDA);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& fild(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDB);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& fild(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDB);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& fimul(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDA);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& fimul(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDA);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& fincstp() {
        emit8(0xD9);
        emit8(0xF7);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& finit() {
        emit8(0x9B);
        emit8(0xDB);
        emit8(0xE3);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fist(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDB);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& fist(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDB);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& fistp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDB);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& fistp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDB);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& fisttp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDB);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& fisttp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDB);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& fisub(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDA);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& fisub(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDA);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& fisubr(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDA);
        emit_modrm(3, 5, r.id);
        return *this;
    }
    CodeGen& fisubr(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDA);
        emit_mem(5, m);
        return *this;
    }

    CodeGen& fld(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& fld(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& fld1() {
        emit8(0xD9);
        emit8(0xE8);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fldcw(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 5, r.id);
        return *this;
    }
    CodeGen& fldcw(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(5, m);
        return *this;
    }

    CodeGen& fldenv(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& fldenv(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& fldl2e() {
        emit8(0xD9);
        emit8(0xEA);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fldl2t() {
        emit8(0xD9);
        emit8(0xE9);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fldlg2() {
        emit8(0xD9);
        emit8(0xEC);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fldln2() {
        emit8(0xD9);
        emit8(0xED);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fldpi() {
        emit8(0xD9);
        emit8(0xEB);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fldz() {
        emit8(0xD9);
        emit8(0xEE);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fmul(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD8);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& fmul(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD8);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& fmulp() {
        emit8(0xDE);
        emit_modrm(3, 1, 0);
        return *this;
    }

    CodeGen& fnclex() {
        emit8(0xDB);
        emit8(0xE2);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fndisi() {
        emit8(0xDB);
        emit8(0xE1);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fneni() {
        emit8(0xDB);
        emit8(0xE0);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fninit() {
        emit8(0xDB);
        emit8(0xE3);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fnop() {
        emit8(0xD9);
        emit8(0xD0);
        emit_modrm(3, 2, 0);
        return *this;
    }

    CodeGen& fnsave(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDD);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& fnsave(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDD);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& fnsetpm() {
        emit8(0xDB);
        emit8(0xE4);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fnstcw(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& fnstcw(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& fnstenv(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& fnstenv(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& fnstsw(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDD);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& fnstsw(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDD);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& fpatan() {
        emit8(0xD9);
        emit8(0xF3);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& fprem() {
        emit8(0xD9);
        emit8(0xF8);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& fprem1() {
        emit8(0xD9);
        emit8(0xF5);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& fptan() {
        emit8(0xD9);
        emit8(0xF2);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& frndint() {
        emit8(0xD9);
        emit8(0xFC);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& frstor(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDD);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& frstor(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDD);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& fsave(Reg r) {
        emit8(0x9B);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDD);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& fsave(Mem m) {
        emit8(0x9B);
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDD);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& fscale() {
        emit8(0xD9);
        emit8(0xFD);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& fsetpm() {
        emit8(0x9B);
        emit8(0xDB);
        emit8(0xE4);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fsin() {
        emit8(0xD9);
        emit8(0xFE);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& fsincos() {
        emit8(0xD9);
        emit8(0xFB);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& fsqrt() {
        emit8(0xD9);
        emit8(0xFA);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& fst(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& fst(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& fstcw(Reg r) {
        emit8(0x9B);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& fstcw(Mem m) {
        emit8(0x9B);
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& fstenv(Reg r) {
        emit8(0x9B);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& fstenv(Mem m) {
        emit8(0x9B);
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& fstp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD9);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& fstp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD9);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& fstp1() {
        emit8(0xD9);
        emit_modrm(3, 3, 0);
        return *this;
    }

    CodeGen& fstp8() {
        emit8(0xDF);
        emit_modrm(3, 2, 0);
        return *this;
    }

    CodeGen& fstp9() {
        emit8(0xDF);
        emit_modrm(3, 3, 0);
        return *this;
    }

    CodeGen& fstsw(Reg r) {
        emit8(0x9B);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xDD);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& fstsw(Mem m) {
        emit8(0x9B);
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xDD);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& fsub(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD8);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& fsub(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD8);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& fsubp() {
        emit8(0xDE);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fsubr(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xD8);
        emit_modrm(3, 5, r.id);
        return *this;
    }
    CodeGen& fsubr(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xD8);
        emit_mem(5, m);
        return *this;
    }

    CodeGen& fsubrp() {
        emit8(0xDE);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& ftst() {
        emit8(0xD9);
        emit8(0xE4);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fucom() {
        emit8(0xDD);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fucomi() {
        emit8(0xDB);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fucomip() {
        emit8(0xDF);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fucomp() {
        emit8(0xDD);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fucompp() {
        emit8(0xDA);
        emit8(0xE9);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& fxam() {
        emit8(0xD9);
        emit8(0xE5);
        emit_modrm(3, 4, 0);
        return *this;
    }

    CodeGen& fxch() {
        emit8(0xD9);
        emit_modrm(3, 1, 0);
        return *this;
    }

    CodeGen& fxch4() {
        emit8(0xDD);
        emit_modrm(3, 1, 0);
        return *this;
    }

    CodeGen& fxch7() {
        emit8(0xDF);
        emit_modrm(3, 1, 0);
        return *this;
    }

    CodeGen& fxrstor(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& fxrstor(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xAE);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& fxsave(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& fxsave(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xAE);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& fxtract() {
        emit8(0xD9);
        emit8(0xF4);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& fyl2x() {
        emit8(0xD9);
        emit8(0xF1);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& fyl2xp1() {
        emit8(0xD9);
        emit8(0xF9);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& getsec() {
        emit8(0x0F); emit8(0x37);
        return *this;
    }

    CodeGen& haddpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x7C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& haddpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x7C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& haddps(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x7C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& haddps(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x7C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& hint_nop(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x18);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& hint_nop(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x18);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& hlt() {
        emit8(0xF4);
        return *this;
    }

    CodeGen& hsubpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x7D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& hsubpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x7D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& hsubps(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x7D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& hsubps(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x7D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& idiv(Reg r) {
        if (r.bits == 16) emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(r.bits == 8 ? 0xF6 : 0xF7);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& idiv(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        if (m.size_hint == 2) emit8(0x66);
        emit8(m.size_hint == 1 ? 0xF6 : 0xF7);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& imul(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xAF);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& imul(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xAF);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& imul(Reg dst, Reg src, int64_t imm) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x69);
        emit_modrm(3, dst.id, src.id);
        emit_imm(imm, dst.bits/8);
        return *this;
    }
    CodeGen& imul(Reg r) {
        if (r.bits == 16) emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(r.bits == 8 ? 0xF6 : 0x69);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& imul(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        if (m.size_hint == 2) emit8(0x66);
        emit8(m.size_hint == 1 ? 0xF6 : 0x69);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& in() {
        emit8(0xE4);
        return *this;
    }

    CodeGen& inc(Reg r) {
        if (r.bits == 16) emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(r.bits == 8 ? 0xFE : 0xFF);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& inc(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        if (m.size_hint == 2) emit8(0x66);
        emit8(m.size_hint == 1 ? 0xFE : 0xFF);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& ins() {
        emit8(0x6C);
        return *this;
    }

    CodeGen& insertps(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x21);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }

    CodeGen& int_() {
        emit8(0xCC);
        return *this;
    }

    CodeGen& int1() {
        emit8(0xF1);
        return *this;
    }

    CodeGen& into() {
        emit8(0xCE);
        return *this;
    }

    CodeGen& invd() {
        emit8(0x0F); emit8(0x08);
        return *this;
    }

    CodeGen& invept(Reg dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x38);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& invlpg(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x01);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& invlpg(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x01);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& invvpid(Reg dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x38);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& iret() {
        emit8(0xCF);
        return *this;
    }

    CodeGen& iretd() {
        emit8(0xCF);
        return *this;
    }

    CodeGen& jb(Label& lab) {
        emit8(0x0F); emit8(0x82);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jbe(Label& lab) {
        emit8(0x0F); emit8(0x86);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jcxz(Label& lab) {
        emit8(0xE3);
        emit_label_rel8(lab);
        return *this;
    }

    CodeGen& jecxz(Label& lab) {
        emit8(0xE3);
        emit_label_rel8(lab);
        return *this;
    }

    CodeGen& jl(Label& lab) {
        emit8(0x0F); emit8(0x8C);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jle(Label& lab) {
        emit8(0x0F); emit8(0x8E);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jmp(Label& lab) {
        emit8(0xE9);
        emit_label_rel32(lab);
        return *this;
    }
    CodeGen& jmp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xFF);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& jmp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xFF);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& jmpe() {
        emit8(0x0F); emit8(0x00);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& jmpf(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0xFF);
        emit_modrm(3, 5, r.id);
        return *this;
    }
    CodeGen& jmpf(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0xFF);
        emit_mem(5, m);
        return *this;
    }

    CodeGen& jnb(Label& lab) {
        emit8(0x0F); emit8(0x83);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jnbe(Label& lab) {
        emit8(0x0F); emit8(0x87);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jnl(Label& lab) {
        emit8(0x0F); emit8(0x8D);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jnle(Label& lab) {
        emit8(0x0F); emit8(0x8F);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jno(Label& lab) {
        emit8(0x0F); emit8(0x81);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jnp(Label& lab) {
        emit8(0x0F); emit8(0x8B);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jns(Label& lab) {
        emit8(0x0F); emit8(0x89);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jnz(Label& lab) {
        emit8(0x0F); emit8(0x85);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jo(Label& lab) {
        emit8(0x0F); emit8(0x80);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jp(Label& lab) {
        emit8(0x0F); emit8(0x8A);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& js(Label& lab) {
        emit8(0x0F); emit8(0x88);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& jz(Label& lab) {
        emit8(0x0F); emit8(0x84);
        emit_label_rel32(lab);
        return *this;
    }

    CodeGen& lahf() {
        emit8(0x9F);
        return *this;
    }

    CodeGen& lar(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x02);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& lddqu(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF0);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& ldmxcsr(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& ldmxcsr(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xAE);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& lds(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0xC5);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& lea(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x8D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& leave() {
        emit8(0xC9);
        return *this;
    }

    CodeGen& les(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0xC4);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& lfence() {
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 5, 0);
        return *this;
    }

    CodeGen& lfs(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xB4);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& lgdt(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x01);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& lgdt(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x01);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& lgs(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xB5);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& lidt(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x01);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& lidt(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x01);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& lldt(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x00);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& lldt(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x00);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& lmsw(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x01);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& lmsw(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x01);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& lods() {
        emit8(0xAC);
        return *this;
    }

    CodeGen& loop(Label& lab) {
        emit8(0xE2);
        emit_label_rel8(lab);
        return *this;
    }

    CodeGen& loopnz(Label& lab) {
        emit8(0xE0);
        emit_label_rel8(lab);
        return *this;
    }

    CodeGen& loopz(Label& lab) {
        emit8(0xE1);
        emit_label_rel8(lab);
        return *this;
    }

    CodeGen& lsl(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x03);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& lss(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xB2);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& ltr(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x00);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& ltr(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x00);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& maskmovdqu(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xF7);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }

    CodeGen& maskmovq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xF7);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }

    CodeGen& maxpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& maxpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& maxps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& maxps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& maxsd(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& maxsd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& maxss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& maxss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& mfence() {
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 6, 0);
        return *this;
    }

    CodeGen& minpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& minpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& minps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& minps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& minsd(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& minsd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& minss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& minss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& monitor() {
        emit8(0x0F); emit8(0x01);
        emit8(0xC8);
        emit_modrm(3, 1, 0);
        return *this;
    }

    CodeGen& mov(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x89);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& mov(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x8B);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& mov(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x89);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& mov(Reg dst, int64_t imm) {
        if (dst.bits == 8) {
            emit_rex_if_needed(false, 0, 0, dst.id);
            emit8(0xB0 | (dst.id & 7));
            emit_imm(imm, 1);
        } else {
            if (dst.bits == 16) emit8(0x66);
            emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
            emit8(0xB8 | (dst.id & 7));
            emit_imm(imm, dst.bits == 64 ? 8 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& mov(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC6);
        emit_mem(0, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& movapd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x28);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movapd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x28);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movapd(Mem dst, Xmm src) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x28);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movaps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x28);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movaps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x28);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movaps(Mem dst, Xmm src) {
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x28);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movbe(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x38);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movbe(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0x38);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movd(Mem dst, Xmm src) {
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x6E);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& movd(Xmm dst, Reg src) {
        emit_rex_if_needed(src.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x6E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movddup(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x12);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movddup(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x12);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movdq2q(Mmx dst, Xmm src) {
        emit8(0xF2);
        emit8(0x0F);
        emit8(0xD6);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movdq2q(Mmx dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD6);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movdqa(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x6F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movdqa(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6F);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movdqa(Mem dst, Xmm src) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x6F);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movdqu(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x6F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movdqu(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6F);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movdqu(Mem dst, Xmm src) {
        emit8(0xF3);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x6F);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movhlps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x12);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }

    CodeGen& movhpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x16);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movhpd(Mem dst, Xmm src) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x16);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movhps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x16);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movhps(Mem dst, Xmm src) {
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x16);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movlhps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x16);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }

    CodeGen& movlpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x12);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movlpd(Mem dst, Xmm src) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x12);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movlps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x12);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movlps(Mem dst, Xmm src) {
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x12);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movmskpd(Reg dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x50);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }

    CodeGen& movmskps(Reg dst, Xmm src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x50);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }

    CodeGen& movntdq(Mem dst, Xmm src) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0xE7);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movntdqa(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x2A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movnti(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0xC3);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movntpd(Mem dst, Xmm src) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x2B);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movntps(Mem dst, Xmm src) {
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x2B);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movntq(Mem dst, Xmm src) {
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0xE7);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movq(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x6F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movq(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6F);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movq(Mem dst, Xmm src) {
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x6F);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& movq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x6F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movq(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movq2dq(Xmm dst, Mmx src) {
        emit8(0xF3);
        emit8(0x0F);
        emit8(0xD6);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movq2dq(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD6);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movs() {
        emit8(0xA4);
        return *this;
    }

    CodeGen& movsd(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x10);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movsd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x10);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movsd(Mem dst, Xmm src) {
        emit8(0xF2);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x10);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movshdup(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x16);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movshdup(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x16);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movsldup(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x12);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movsldup(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x12);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x10);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x10);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movss(Mem dst, Xmm src) {
        emit8(0xF3);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x10);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movsx(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xBE);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movsx(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xBE);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movsxd(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x63);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movsxd(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x63);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& movupd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x10);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movupd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x10);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movupd(Mem dst, Xmm src) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x10);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movups(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x10);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movups(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x10);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& movups(Mem dst, Xmm src) {
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x10);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& movzx(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xB6);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& movzx(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xB6);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& mpsadbw(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x42);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& mpsadbw(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x42);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& mul(Reg r) {
        if (r.bits == 16) emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(r.bits == 8 ? 0xF6 : 0xF7);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& mul(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        if (m.size_hint == 2) emit8(0x66);
        emit8(m.size_hint == 1 ? 0xF6 : 0xF7);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& mulpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x59);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& mulpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x59);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& mulps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x59);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& mulps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x59);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& mulsd(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x59);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& mulsd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x59);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& mulss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x59);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& mulss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x59);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& mwait() {
        emit8(0x0F); emit8(0x01);
        emit8(0xC9);
        emit_modrm(3, 1, 0);
        return *this;
    }

    CodeGen& neg(Reg r) {
        if (r.bits == 16) emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(r.bits == 8 ? 0xF6 : 0xF7);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& neg(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        if (m.size_hint == 2) emit8(0x66);
        emit8(m.size_hint == 1 ? 0xF6 : 0xF7);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& nop() {
        emit8(0x90);
        return *this;
    }
    CodeGen& nop(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x0D);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& nop(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x0D);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& not_(Reg r) {
        if (r.bits == 16) emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(r.bits == 8 ? 0xF6 : 0xF7);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& not_(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        if (m.size_hint == 2) emit8(0x66);
        emit8(m.size_hint == 1 ? 0xF6 : 0xF7);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& or_(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x09);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& or_(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0B);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& or_(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x09);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& or_(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0x80);
            emit_modrm(3, 1, dst.id);
            emit_imm(imm, 1);
        } else if (imm >= -128 && imm <= 127) {
            emit8(0x83);
            emit_modrm(3, 1, dst.id);
            emit_imm(imm, 1);
        } else {
            emit8(0x81);
            emit_modrm(3, 1, dst.id);
            emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& or_(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x80);
        emit_mem(1, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& orpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x56);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& orpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x56);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& orps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x56);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& orps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x56);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& out() {
        emit8(0xE6);
        return *this;
    }

    CodeGen& outs() {
        emit8(0x6E);
        return *this;
    }

    CodeGen& pabsb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pabsb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1C);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pabsb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pabsb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pabsd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pabsd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1E);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pabsd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pabsd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pabsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pabsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1D);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pabsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pabsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x1D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& packssdw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x6B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& packssdw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6B);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& packssdw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x6B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& packssdw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6B);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& packsswb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x63);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& packsswb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x63);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& packsswb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x63);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& packsswb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x63);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& packusdw(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x2B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& packusdw(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x2B);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& packuswb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x67);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& packuswb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x67);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& packuswb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x67);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& packuswb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x67);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& paddb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xFC);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFC);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& paddb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xFC);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFC);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& paddd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xFE);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFE);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& paddd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xFE);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFE);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& paddq(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xD4);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddq(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD4);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& paddq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xD4);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddq(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD4);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& paddsb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xEC);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddsb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEC);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& paddsb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xEC);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddsb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEC);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& paddsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xED);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xED);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& paddsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xED);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xED);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& paddusb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xDC);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddusb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDC);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& paddusb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xDC);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddusb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDC);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& paddusw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xDD);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddusw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDD);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& paddusw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xDD);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddusw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDD);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& paddw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xFD);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFD);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& paddw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xFD);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& paddw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFD);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& palignr(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& palignr(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0F);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& palignr(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& palignr(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pand(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xDB);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pand(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDB);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pand(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xDB);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pand(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDB);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pandn(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xDF);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pandn(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDF);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pandn(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xDF);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pandn(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDF);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pause() {
        emit8(0xF3);
        emit8(0x90);
        return *this;
    }

    CodeGen& pavgb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE0);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pavgb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE0);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pavgb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xE0);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pavgb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE0);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pavgw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE3);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pavgw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE3);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pavgw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xE3);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pavgw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE3);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pblendvb(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x10);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pblendvb(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x10);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pblendw(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0E);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pblendw(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0E);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pcmpeqb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x74);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpeqb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x74);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pcmpeqb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x74);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpeqb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x74);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pcmpeqd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x76);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpeqd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x76);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pcmpeqd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x76);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpeqd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x76);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pcmpeqq(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x29);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpeqq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x29);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pcmpeqw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x75);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpeqw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x75);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pcmpeqw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x75);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpeqw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x75);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pcmpestri(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x61);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pcmpestri(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x61);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pcmpestrm(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x60);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pcmpestrm(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x60);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pcmpgtb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x64);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpgtb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x64);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pcmpgtb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x64);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpgtb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x64);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pcmpgtd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x66);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpgtd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x66);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pcmpgtd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x66);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpgtd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x66);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pcmpgtq(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x37);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpgtq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x37);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pcmpgtw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x65);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpgtw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x65);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pcmpgtw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x65);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pcmpgtw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x65);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pcmpistri(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x63);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pcmpistri(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x63);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pcmpistrm(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x62);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pcmpistrm(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x62);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pextrb(Reg dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, src.id, 0, dst.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x14);
        emit_modrm(3, src.id, dst.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pextrb(Mem dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x14);
        emit_mem(src.id, dst);
        emit8(imm);
        return *this;
    }

    CodeGen& pextrd(Reg dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, src.id, 0, dst.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x16);
        emit_modrm(3, src.id, dst.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pextrd(Mem dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x16);
        emit_mem(src.id, dst);
        emit8(imm);
        return *this;
    }

    CodeGen& pextrw(Reg dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, src.id, 0, dst.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x15);
        emit_modrm(3, src.id, dst.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pextrw(Mem dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, src.id, dst);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x15);
        emit_mem(src.id, dst);
        emit8(imm);
        return *this;
    }

    CodeGen& phaddd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x02);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phaddd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x02);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& phaddd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x02);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phaddd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x02);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& phaddsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x03);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phaddsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x03);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& phaddsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x03);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phaddsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x03);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& phaddw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x01);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phaddw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x01);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& phaddw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x01);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phaddw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x01);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& phminposuw(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x41);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phminposuw(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x41);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& phsubd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x06);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phsubd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x06);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& phsubd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x06);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phsubd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x06);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& phsubsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x07);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phsubsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x07);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& phsubsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x07);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phsubsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x07);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& phsubw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x05);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phsubw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x05);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& phsubw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x05);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& phsubw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x05);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pinsrb(Xmm dst, Reg src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(src.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x20);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pinsrb(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x20);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pinsrd(Xmm dst, Reg src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(src.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x22);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pinsrd(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x22);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pinsrw(Xmm dst, Reg src, uint8_t imm) {
        emit_rex_if_needed(src.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xC4);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pinsrw(Xmm dst, Mem src, uint8_t imm) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xC4);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pmaddubsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x04);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaddubsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x04);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmaddubsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x04);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaddubsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x04);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmaddwd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xF5);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaddwd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF5);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmaddwd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xF5);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaddwd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF5);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmaxsb(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaxsb(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmaxsd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaxsd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmaxsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xEE);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaxsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEE);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmaxsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xEE);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaxsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEE);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmaxub(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xDE);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaxub(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDE);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmaxub(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xDE);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaxub(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDE);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmaxud(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3F);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaxud(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3F);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmaxuw(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmaxuw(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pminsb(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x38);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pminsb(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x38);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pminsd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x39);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pminsd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x39);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pminsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xEA);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pminsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEA);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pminsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xEA);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pminsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEA);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pminub(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xDA);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pminub(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDA);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pminub(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xDA);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pminub(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xDA);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pminud(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pminud(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3B);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pminuw(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pminuw(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x3A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovmskb(Reg dst, Xmm src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xD7);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }

    CodeGen& pmovsxbd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x21);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovsxbq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x22);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovsxbw(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x20);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovsxdq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x25);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovsxwd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x23);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovsxwq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x24);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovzxbd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x31);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovzxbq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x32);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovzxbw(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x30);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovzxdq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x35);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovzxwd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x33);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmovzxwq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x34);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmuldq(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x28);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmuldq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x28);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmulhrsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x0B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmulhrsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x0B);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmulhrsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x0B);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmulhrsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x0B);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmulhuw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE4);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmulhuw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE4);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmulhuw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xE4);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmulhuw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE4);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmulhw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE5);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmulhw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE5);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmulhw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xE5);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmulhw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE5);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmulld(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x40);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmulld(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x40);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmullw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xD5);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmullw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD5);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmullw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xD5);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmullw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD5);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pmuludq(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xF4);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmuludq(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF4);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pmuludq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xF4);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pmuludq(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF4);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pop(Reg r) {
        // Default-64 ops (push/pop) don't need REX.W
        emit_rex_if_needed(false, 0, 0, r.id);
        emit8(0x58 | (r.id & 7));
        return *this;
    }

    CodeGen& popa() {
        emit8(0x61);
        return *this;
    }

    CodeGen& popad() {
        emit8(0x61);
        return *this;
    }

    CodeGen& popcnt(Reg dst, Reg src) {
        if (dst.bits == 16) emit8(0x66); else emit8(0xF3);
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xB8);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& popcnt(Reg dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xB8);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& popf() {
        emit8(0x9D);
        return *this;
    }

    CodeGen& popfd() {
        emit8(0x9D);
        return *this;
    }

    CodeGen& por(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xEB);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& por(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEB);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& por(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xEB);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& por(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEB);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& prefetchnta(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x18);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& prefetchnta(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x18);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& prefetcht0(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x18);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& prefetcht0(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x18);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& prefetcht1(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x18);
        emit_modrm(3, 2, r.id);
        return *this;
    }
    CodeGen& prefetcht1(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x18);
        emit_mem(2, m);
        return *this;
    }

    CodeGen& prefetcht2(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x18);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& prefetcht2(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x18);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& psadbw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xF6);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psadbw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF6);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psadbw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xF6);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psadbw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF6);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pshufb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x00);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pshufb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x00);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pshufb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x00);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pshufb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x00);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pshufd(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x70);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pshufd(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x70);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pshufhw(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x70);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pshufhw(Xmm dst, Mem src, uint8_t imm) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x70);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pshuflw(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x70);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& pshuflw(Xmm dst, Mem src, uint8_t imm) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x70);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& pshufw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x70);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }

    CodeGen& psignb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x08);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psignb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x08);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psignb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x08);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psignb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x08);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psignd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x0A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psignd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x0A);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psignd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x0A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psignd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x0A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psignw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x09);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psignw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x09);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psignw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x38);
        emit8(0x09);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psignw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x09);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& pslld(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pslld(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x72);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pslld(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pslld(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x72);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pslld(Xmm dst, uint8_t imm) {
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, 6, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& pslldq(Xmm dst, uint8_t imm) {
        emit8(0x66);
        emit8(0x0F);
        emit8(0x73);
        emit_modrm(3, 7, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psllq(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x73);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psllq(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x73);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psllq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x73);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psllq(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x73);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psllq(Xmm dst, uint8_t imm) {
        emit8(0x0F);
        emit8(0x73);
        emit_modrm(3, 6, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psllw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psllw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x71);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psllw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psllw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x71);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psllw(Xmm dst, uint8_t imm) {
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, 6, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psrad(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psrad(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x72);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psrad(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psrad(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x72);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psrad(Xmm dst, uint8_t imm) {
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, 4, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psraw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psraw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x71);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psraw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psraw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x71);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psraw(Xmm dst, uint8_t imm) {
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, 4, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psrld(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psrld(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x72);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psrld(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psrld(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x72);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psrld(Xmm dst, uint8_t imm) {
        emit8(0x0F);
        emit8(0x72);
        emit_modrm(3, 2, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psrldq(Xmm dst, uint8_t imm) {
        emit8(0x66);
        emit8(0x0F);
        emit8(0x73);
        emit_modrm(3, 3, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psrlq(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x73);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psrlq(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x73);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psrlq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x73);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psrlq(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x73);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psrlq(Xmm dst, uint8_t imm) {
        emit8(0x0F);
        emit8(0x73);
        emit_modrm(3, 2, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psrlw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psrlw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x71);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psrlw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psrlw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x71);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psrlw(Xmm dst, uint8_t imm) {
        emit8(0x0F);
        emit8(0x71);
        emit_modrm(3, 2, dst.id);
        emit8(imm);
        return *this;
    }

    CodeGen& psubb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xF8);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF8);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psubb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xF8);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF8);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psubd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xFA);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFA);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psubd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xFA);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFA);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psubq(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xFB);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubq(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFB);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psubq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xFB);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubq(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xFB);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psubsb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE8);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubsb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE8);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psubsb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xE8);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubsb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE8);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psubsw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xE9);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubsw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE9);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psubsw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xE9);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubsw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xE9);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psubusb(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xD8);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubusb(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD8);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psubusb(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xD8);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubusb(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD8);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psubusw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xD9);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubusw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD9);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psubusw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xD9);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubusw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xD9);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& psubw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xF9);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF9);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& psubw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xF9);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& psubw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xF9);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& ptest(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x17);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& ptest(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x38);
        emit8(0x17);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& punpckhbw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x68);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckhbw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x68);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& punpckhbw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x68);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckhbw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x68);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& punpckhdq(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x6A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckhdq(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6A);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& punpckhdq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x6A);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckhdq(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6A);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& punpckhqdq(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x6D);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckhqdq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6D);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& punpckhwd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x69);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckhwd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x69);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& punpckhwd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x69);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckhwd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x69);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& punpcklbw(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x60);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpcklbw(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x60);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& punpcklbw(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x60);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpcklbw(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x60);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& punpckldq(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x62);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckldq(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x62);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& punpckldq(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x62);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpckldq(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x62);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& punpcklqdq(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x6C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpcklqdq(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x6C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& punpcklwd(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x61);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpcklwd(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x61);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& punpcklwd(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0x61);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& punpcklwd(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x61);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& push(Reg r) {
        // Default-64 ops (push/pop) don't need REX.W
        emit_rex_if_needed(false, 0, 0, r.id);
        emit8(0x50 | (r.id & 7));
        return *this;
    }

    CodeGen& pusha() {
        emit8(0x60);
        return *this;
    }

    CodeGen& pushad() {
        emit8(0x60);
        return *this;
    }

    CodeGen& pushf() {
        emit8(0x9C);
        return *this;
    }

    CodeGen& pushfd() {
        emit8(0x9C);
        return *this;
    }

    CodeGen& pxor(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xEF);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pxor(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEF);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& pxor(Mmx dst, Mmx src) {
        emit8(0x0F);
        emit8(0xEF);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& pxor(Mmx dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xEF);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& rcl(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xC0);
        } else {
            emit8(0xC1);
        }
        emit_modrm(3, 2, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& rcl(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC0);
        emit_mem(2, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& rcpps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x53);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& rcpps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x53);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& rcpss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x53);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& rcpss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x53);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& rcr(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xC0);
        } else {
            emit8(0xC1);
        }
        emit_modrm(3, 3, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& rcr(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC0);
        emit_mem(3, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& rdmsr() {
        emit8(0x0F); emit8(0x32);
        return *this;
    }

    CodeGen& rdpmc() {
        emit8(0x0F); emit8(0x33);
        return *this;
    }

    CodeGen& rdtsc() {
        emit8(0x0F); emit8(0x31);
        return *this;
    }

    CodeGen& rdtscp() {
        emit8(0x0F); emit8(0x01);
        emit8(0xF9);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& retf() {
        emit8(0xCA);
        return *this;
    }

    CodeGen& retn() {
        emit8(0xC2);
        return *this;
    }

    CodeGen& rol(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xC0);
        } else {
            emit8(0xC1);
        }
        emit_modrm(3, 0, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& rol(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC0);
        emit_mem(0, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& ror(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xC0);
        } else {
            emit8(0xC1);
        }
        emit_modrm(3, 1, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& ror(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC0);
        emit_mem(1, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& roundpd(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x09);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& roundpd(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x09);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& roundps(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x08);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& roundps(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x08);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& roundsd(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0B);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& roundsd(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0B);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& roundss(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0A);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& roundss(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x3A);
        emit8(0x0A);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& rsm() {
        emit8(0x0F); emit8(0xAA);
        return *this;
    }

    CodeGen& rsqrtps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x52);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& rsqrtps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x52);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& rsqrtss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x52);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& rsqrtss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x52);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& sahf() {
        emit8(0x9E);
        return *this;
    }

    CodeGen& sal(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xC0);
        } else {
            emit8(0xC1);
        }
        emit_modrm(3, 6, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& sal(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC0);
        emit_mem(6, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& salc() {
        emit8(0xD6);
        return *this;
    }

    CodeGen& sar(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xC0);
        } else {
            emit8(0xC1);
        }
        emit_modrm(3, 7, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& sar(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC0);
        emit_mem(7, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& sbb(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x19);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& sbb(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x1B);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& sbb(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x19);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& sbb(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0x80);
            emit_modrm(3, 3, dst.id);
            emit_imm(imm, 1);
        } else if (imm >= -128 && imm <= 127) {
            emit8(0x83);
            emit_modrm(3, 3, dst.id);
            emit_imm(imm, 1);
        } else {
            emit8(0x81);
            emit_modrm(3, 3, dst.id);
            emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& sbb(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x80);
        emit_mem(3, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& scas() {
        emit8(0xAE);
        return *this;
    }

    CodeGen& setb(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x92);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setb(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x92);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setbe(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x96);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setbe(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x96);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setl(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x9C);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setl(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x9C);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setle(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x9E);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setle(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x9E);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setnb(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x93);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setnb(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x93);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setnbe(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x97);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setnbe(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x97);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setnl(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x9D);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setnl(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x9D);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setnle(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x9F);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setnle(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x9F);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setno(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x91);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setno(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x91);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setnp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x9B);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setnp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x9B);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setns(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x99);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setns(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x99);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setnz(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x95);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setnz(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x95);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& seto(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x90);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& seto(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x90);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setp(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x9A);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setp(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x9A);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& sets(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x98);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& sets(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x98);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& setz(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x94);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& setz(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x94);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& sfence() {
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& sgdt(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x01);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& sgdt(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x01);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& shl(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xC0);
        } else {
            emit8(0xC1);
        }
        emit_modrm(3, 4, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& shl(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC0);
        emit_mem(4, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& shld(Reg dst, Reg src, int64_t imm) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xA4);
        emit_modrm(3, dst.id, src.id);
        emit_imm(imm, dst.bits/8);
        return *this;
    }

    CodeGen& shr(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xC0);
        } else {
            emit8(0xC1);
        }
        emit_modrm(3, 5, dst.id);
        emit_imm(imm, 1);
        return *this;
    }
    CodeGen& shr(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xC0);
        emit_mem(5, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& shrd(Reg dst, Reg src, int64_t imm) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xAC);
        emit_modrm(3, dst.id, src.id);
        emit_imm(imm, dst.bits/8);
        return *this;
    }

    CodeGen& shufpd(Xmm dst, Xmm src, uint8_t imm) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xC6);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& shufpd(Xmm dst, Mem src, uint8_t imm) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xC6);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& shufps(Xmm dst, Xmm src, uint8_t imm) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0xC6);
        emit_modrm(3, dst.id, src.id);
        emit8(imm);
        return *this;
    }
    CodeGen& shufps(Xmm dst, Mem src, uint8_t imm) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0xC6);
        emit_mem(dst.id, src);
        emit8(imm);
        return *this;
    }

    CodeGen& sidt(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x01);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& sidt(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x01);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& sldt(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x00);
        emit_modrm(3, 0, r.id);
        return *this;
    }
    CodeGen& sldt(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x00);
        emit_mem(0, m);
        return *this;
    }

    CodeGen& smsw(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x01);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& smsw(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x01);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& sqrtpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x51);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& sqrtpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x51);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& sqrtps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x51);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& sqrtps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x51);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& sqrtsd(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x51);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& sqrtsd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x51);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& sqrtss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x51);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& sqrtss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x51);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& stc() {
        emit8(0xF9);
        return *this;
    }

    CodeGen& std() {
        emit8(0xFD);
        return *this;
    }

    CodeGen& sti() {
        emit8(0xFB);
        return *this;
    }

    CodeGen& stmxcsr(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 3, r.id);
        return *this;
    }
    CodeGen& stmxcsr(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xAE);
        emit_mem(3, m);
        return *this;
    }

    CodeGen& stos() {
        emit8(0xAA);
        return *this;
    }

    CodeGen& str(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x00);
        emit_modrm(3, 1, r.id);
        return *this;
    }
    CodeGen& str(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x00);
        emit_mem(1, m);
        return *this;
    }

    CodeGen& sub(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x29);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& sub(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x2B);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& sub(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x29);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& sub(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0x80);
            emit_modrm(3, 5, dst.id);
            emit_imm(imm, 1);
        } else if (imm >= -128 && imm <= 127) {
            emit8(0x83);
            emit_modrm(3, 5, dst.id);
            emit_imm(imm, 1);
        } else {
            emit8(0x81);
            emit_modrm(3, 5, dst.id);
            emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& sub(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x80);
        emit_mem(5, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& subpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& subpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& subps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& subps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& subsd(Xmm dst, Xmm src) {
        emit8(0xF2);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& subsd(Xmm dst, Mem src) {
        emit8(0xF2);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& subss(Xmm dst, Xmm src) {
        emit8(0xF3);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x5C);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& subss(Xmm dst, Mem src) {
        emit8(0xF3);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x5C);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& swapgs() {
        emit8(0x0F); emit8(0x01);
        emit8(0xF8);
        emit_modrm(3, 7, 0);
        return *this;
    }

    CodeGen& syscall() {
        emit8(0x0F); emit8(0x05);
        return *this;
    }

    CodeGen& sysenter() {
        emit8(0x0F); emit8(0x34);
        return *this;
    }

    CodeGen& sysexit() {
        emit8(0x0F); emit8(0x35);
        return *this;
    }

    CodeGen& sysret() {
        emit8(0x0F); emit8(0x07);
        return *this;
    }

    CodeGen& test(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x85);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& test(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x85);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& test(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0xF6);
        } else {
            emit8(0xF7);
        }
        emit_modrm(3, 0, dst.id);
        if (dst.bits == 8) emit_imm(imm, 1);
        else emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        return *this;
    }
    CodeGen& test(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0xF6);
        emit_mem(0, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& ucomisd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& ucomisd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& ucomiss(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x2E);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& ucomiss(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x2E);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& ud(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0xB9);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& ud(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0xB9);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& ud2() {
        emit8(0x0F); emit8(0x0B);
        return *this;
    }

    CodeGen& unpckhpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x15);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& unpckhpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x15);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& unpckhps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x15);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& unpckhps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x15);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& unpcklpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x14);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& unpcklpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x14);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& unpcklps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x14);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& unpcklps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x14);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& verr(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x00);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& verr(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x00);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& verw(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0x00);
        emit_modrm(3, 5, r.id);
        return *this;
    }
    CodeGen& verw(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0x00);
        emit_mem(5, m);
        return *this;
    }

    CodeGen& vmcall() {
        emit8(0x0F); emit8(0x01);
        emit8(0xC1);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& vmclear(Reg r) {
        emit8(0x66);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xC7);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& vmclear(Mem m) {
        emit8(0x66);
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xC7);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& vmlaunch() {
        emit8(0x0F); emit8(0x01);
        emit8(0xC2);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& vmptrld(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xC7);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& vmptrld(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xC7);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& vmptrst(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xC7);
        emit_modrm(3, 7, r.id);
        return *this;
    }
    CodeGen& vmptrst(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xC7);
        emit_mem(7, m);
        return *this;
    }

    CodeGen& vmread(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x0F); emit8(0x78);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& vmread(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0x78);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& vmresume() {
        emit8(0x0F); emit8(0x01);
        emit8(0xC3);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& vmwrite(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x0F); emit8(0x79);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& vmwrite(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x0F); emit8(0x79);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& vmxoff() {
        emit8(0x0F); emit8(0x01);
        emit8(0xC4);
        emit_modrm(3, 0, 0);
        return *this;
    }

    CodeGen& vmxon(Reg r) {
        emit8(0xF3);
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xC7);
        emit_modrm(3, 6, r.id);
        return *this;
    }
    CodeGen& vmxon(Mem m) {
        emit8(0xF3);
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xC7);
        emit_mem(6, m);
        return *this;
    }

    CodeGen& wbinvd() {
        emit8(0x0F); emit8(0x09);
        return *this;
    }

    CodeGen& wrmsr() {
        emit8(0x0F); emit8(0x30);
        return *this;
    }

    CodeGen& xadd(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x0F); emit8(0xC1);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& xadd(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x0F); emit8(0xC1);
        emit_mem(src.id, dst);
        return *this;
    }

    CodeGen& xchg(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id);
        emit8(0x87);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& xchg(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x87);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& xchg(Reg r) {
        // Default-64 ops (push/pop) don't need REX.W
        emit_rex_if_needed(false, 0, 0, r.id);
        emit8(0x90 | (r.id & 7));
        return *this;
    }

    CodeGen& xgetbv() {
        emit8(0x0F); emit8(0x01);
        emit8(0xD0);
        emit_modrm(3, 2, 0);
        return *this;
    }

    CodeGen& xlat() {
        emit8(0xD7);
        return *this;
    }

    CodeGen& xor_(Reg dst, Reg src) {
        emit_rex_if_needed(dst.bits==64, src.id, 0, dst.id);
        emit8(0x31);
        emit_modrm(3, src.id, dst.id);
        return *this;
    }
    CodeGen& xor_(Reg dst, Mem src) {
        emit_rex_mem(dst.bits==64, dst.id, src);
        emit8(0x33);
        emit_mem(dst.id, src);
        return *this;
    }
    CodeGen& xor_(Mem dst, Reg src) {
        emit_rex_mem(src.bits==64, src.id, dst);
        emit8(0x31);
        emit_mem(src.id, dst);
        return *this;
    }
    CodeGen& xor_(Reg dst, int64_t imm) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        if (dst.bits == 8) {
            emit8(0x80);
            emit_modrm(3, 6, dst.id);
            emit_imm(imm, 1);
        } else if (imm >= -128 && imm <= 127) {
            emit8(0x83);
            emit_modrm(3, 6, dst.id);
            emit_imm(imm, 1);
        } else {
            emit8(0x81);
            emit_modrm(3, 6, dst.id);
            emit_imm(imm, dst.bits == 64 ? 4 : dst.bits/8);
        }
        return *this;
    }
    CodeGen& xor_(Mem dst, int64_t imm) {
        emit_rex_mem(false, 0, dst);
        emit8(0x80);
        emit_mem(6, dst);
        emit_imm(imm, dst.size_hint ? dst.size_hint : 4);
        return *this;
    }

    CodeGen& xorpd(Xmm dst, Xmm src) {
        emit8(0x66);
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x57);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& xorpd(Xmm dst, Mem src) {
        emit8(0x66);
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x57);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& xorps(Xmm dst, Xmm src) {
        emit_rex_if_needed(false, dst.id, 0, src.id);
        emit8(0x0F);
        emit8(0x57);
        emit_modrm(3, dst.id, src.id);
        return *this;
    }
    CodeGen& xorps(Xmm dst, Mem src) {
        emit_rex_mem(false, dst.id, src);
        emit8(0x0F);
        emit8(0x57);
        emit_mem(dst.id, src);
        return *this;
    }

    CodeGen& xrstor(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 5, r.id);
        return *this;
    }
    CodeGen& xrstor(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xAE);
        emit_mem(5, m);
        return *this;
    }

    CodeGen& xsave(Reg r) {
        emit_rex_if_needed(r.bits==64, 0, 0, r.id);
        emit8(0x0F); emit8(0xAE);
        emit_modrm(3, 4, r.id);
        return *this;
    }
    CodeGen& xsave(Mem m) {
        emit_rex_mem(m.size_hint==8, 0, m);
        emit8(0x0F); emit8(0xAE);
        emit_mem(4, m);
        return *this;
    }

    CodeGen& xsetbv() {
        emit8(0x0F); emit8(0x01);
        emit8(0xD1);
        emit_modrm(3, 2, 0);
        return *this;
    }

    CodeGen& ret() { emit8(0xC3); return *this; }
    CodeGen& int3() { emit8(0xCC); return *this; }
    CodeGen& nops(int n) { for (int i=0; i<n; ++i) emit8(0x90); return *this; }

    CodeGen& je(Label& l) { jz(l); return *this; }
    CodeGen& ja(Label& l) { jnbe(l); return *this; }
    CodeGen& jae(Label& l) { jnb(l); return *this; }
    CodeGen& jc(Label& l) { jb(l); return *this; }
    CodeGen& jg(Label& l) { jnle(l); return *this; }
    CodeGen& jge(Label& l) { jnl(l); return *this; }
    CodeGen& jna(Label& l) { jbe(l); return *this; }
    CodeGen& jnae(Label& l) { jb(l); return *this; }
    CodeGen& jnc(Label& l) { jnb(l); return *this; }
    CodeGen& jne(Label& l) { jnz(l); return *this; }
    CodeGen& jng(Label& l) { jle(l); return *this; }
    CodeGen& jnge(Label& l) { jl(l); return *this; }
    CodeGen& jpe(Label& l) { jp(l); return *this; }
    CodeGen& jpo(Label& l) { jnp(l); return *this; }
    CodeGen& jrcxz(Label& l) { jecxz(l); return *this; }

    CodeGen& cmova(Reg d, Reg s) { cmovnbe(d, s); return *this; }
    CodeGen& cmova(Reg d, Mem s) { cmovnbe(d, s); return *this; }
    CodeGen& cmovae(Reg d, Reg s) { cmovnb(d, s); return *this; }
    CodeGen& cmovae(Reg d, Mem s) { cmovnb(d, s); return *this; }
    CodeGen& cmovc(Reg d, Reg s) { cmovb(d, s); return *this; }
    CodeGen& cmovc(Reg d, Mem s) { cmovb(d, s); return *this; }
    CodeGen& cmove(Reg d, Reg s) { cmovz(d, s); return *this; }
    CodeGen& cmove(Reg d, Mem s) { cmovz(d, s); return *this; }
    CodeGen& cmovg(Reg d, Reg s) { cmovnle(d, s); return *this; }
    CodeGen& cmovg(Reg d, Mem s) { cmovnle(d, s); return *this; }
    CodeGen& cmovge(Reg d, Reg s) { cmovnl(d, s); return *this; }
    CodeGen& cmovge(Reg d, Mem s) { cmovnl(d, s); return *this; }
    CodeGen& cmovna(Reg d, Reg s) { cmovbe(d, s); return *this; }
    CodeGen& cmovna(Reg d, Mem s) { cmovbe(d, s); return *this; }
    CodeGen& cmovnae(Reg d, Reg s) { cmovb(d, s); return *this; }
    CodeGen& cmovnae(Reg d, Mem s) { cmovb(d, s); return *this; }
    CodeGen& cmovnc(Reg d, Reg s) { cmovnb(d, s); return *this; }
    CodeGen& cmovnc(Reg d, Mem s) { cmovnb(d, s); return *this; }
    CodeGen& cmovne(Reg d, Reg s) { cmovnz(d, s); return *this; }
    CodeGen& cmovne(Reg d, Mem s) { cmovnz(d, s); return *this; }
    CodeGen& cmovng(Reg d, Reg s) { cmovle(d, s); return *this; }
    CodeGen& cmovng(Reg d, Mem s) { cmovle(d, s); return *this; }
    CodeGen& cmovnge(Reg d, Reg s) { cmovl(d, s); return *this; }
    CodeGen& cmovnge(Reg d, Mem s) { cmovl(d, s); return *this; }
    CodeGen& cmovpe(Reg d, Reg s) { cmovp(d, s); return *this; }
    CodeGen& cmovpe(Reg d, Mem s) { cmovp(d, s); return *this; }
    CodeGen& cmovpo(Reg d, Reg s) { cmovnp(d, s); return *this; }
    CodeGen& cmovpo(Reg d, Mem s) { cmovnp(d, s); return *this; }

    CodeGen& seta(Reg r) { setnbe(r); return *this; }
    CodeGen& seta(Mem m) { setnbe(m); return *this; }
    CodeGen& setae(Reg r) { setnb(r); return *this; }
    CodeGen& setae(Mem m) { setnb(m); return *this; }
    CodeGen& setc(Reg r) { setb(r); return *this; }
    CodeGen& setc(Mem m) { setb(m); return *this; }
    CodeGen& sete(Reg r) { setz(r); return *this; }
    CodeGen& sete(Mem m) { setz(m); return *this; }
    CodeGen& setg(Reg r) { setnle(r); return *this; }
    CodeGen& setg(Mem m) { setnle(m); return *this; }
    CodeGen& setge(Reg r) { setnl(r); return *this; }
    CodeGen& setge(Mem m) { setnl(m); return *this; }
    CodeGen& setna(Reg r) { setbe(r); return *this; }
    CodeGen& setna(Mem m) { setbe(m); return *this; }
    CodeGen& setnae(Reg r) { setb(r); return *this; }
    CodeGen& setnae(Mem m) { setb(m); return *this; }
    CodeGen& setnc(Reg r) { setnb(r); return *this; }
    CodeGen& setnc(Mem m) { setnb(m); return *this; }
    CodeGen& setne(Reg r) { setnz(r); return *this; }
    CodeGen& setne(Mem m) { setnz(m); return *this; }
    CodeGen& setng(Reg r) { setle(r); return *this; }
    CodeGen& setng(Mem m) { setle(m); return *this; }
    CodeGen& setnge(Reg r) { setl(r); return *this; }
    CodeGen& setnge(Mem m) { setl(m); return *this; }
    CodeGen& setpe(Reg r) { setp(r); return *this; }
    CodeGen& setpe(Mem m) { setp(m); return *this; }
    CodeGen& setpo(Reg r) { setnp(r); return *this; }
    CodeGen& setpo(Mem m) { setnp(m); return *this; }

    CodeGen& movsb() { emit8(0xA4); return *this; }
    CodeGen& movsw() { emit8(0x66); emit8(0xA5); return *this; }
    CodeGen& movsd_str() { emit8(0xA5); return *this; } // string MOVSD (not SSE)
    CodeGen& movsq() { emit8(0x48); emit8(0xA5); return *this; }
    CodeGen& stosb() { emit8(0xAA); return *this; }
    CodeGen& stosw() { emit8(0x66); emit8(0xAB); return *this; }
    CodeGen& stosd() { emit8(0xAB); return *this; }
    CodeGen& stosq() { emit8(0x48); emit8(0xAB); return *this; }
    CodeGen& lodsb() { emit8(0xAC); return *this; }
    CodeGen& lodsw() { emit8(0x66); emit8(0xAD); return *this; }
    CodeGen& lodsd() { emit8(0xAD); return *this; }
    CodeGen& lodsq() { emit8(0x48); emit8(0xAD); return *this; }
    CodeGen& cmpsb() { emit8(0xA6); return *this; }
    CodeGen& cmpsw() { emit8(0x66); emit8(0xA7); return *this; }
    CodeGen& cmpsd_str() { emit8(0xA7); return *this; } // string CMPSD (not SSE)
    CodeGen& cmpsq() { emit8(0x48); emit8(0xA7); return *this; }
    CodeGen& scasb() { emit8(0xAE); return *this; }
    CodeGen& scasw() { emit8(0x66); emit8(0xAF); return *this; }
    CodeGen& scasd() { emit8(0xAF); return *this; }
    CodeGen& scasq() { emit8(0x48); emit8(0xAF); return *this; }
    CodeGen& insb() { emit8(0x6C); return *this; }
    CodeGen& insw() { emit8(0x66); emit8(0x6D); return *this; }
    CodeGen& insd() { emit8(0x6D); return *this; }
    CodeGen& outsb() { emit8(0x6E); return *this; }
    CodeGen& outsw() { emit8(0x66); emit8(0x6F); return *this; }
    CodeGen& outsd() { emit8(0x6F); return *this; }

    /// Emit a typed instruction prefix.
    CodeGen& prefix(Prefix p) { emit8(static_cast<uint8_t>(p)); return *this; }

    CodeGen& lock()   { return prefix(Prefix::Lock); }
    CodeGen& rep()    { return prefix(Prefix::Rep); }
    CodeGen& repe()   { return prefix(Prefix::RepE); }
    CodeGen& repz()   { return prefix(Prefix::RepZ); }
    CodeGen& repne()  { return prefix(Prefix::RepNE); }
    CodeGen& repnz()  { return prefix(Prefix::RepNZ); }
    CodeGen& data16() { return prefix(Prefix::Data16); }
    CodeGen& addr32() { return prefix(Prefix::Addr32); }

    CodeGen& rep_movsb() { emit8(0xF3); movsb(); return *this; }
    CodeGen& rep_movsw() { emit8(0xF3); movsw(); return *this; }
    CodeGen& rep_movsd() { emit8(0xF3); movsd_str(); return *this; }
    CodeGen& rep_movsq() { emit8(0xF3); movsq(); return *this; }
    CodeGen& rep_stosb() { emit8(0xF3); stosb(); return *this; }
    CodeGen& rep_stosw() { emit8(0xF3); stosw(); return *this; }
    CodeGen& rep_stosd() { emit8(0xF3); stosd(); return *this; }
    CodeGen& rep_stosq() { emit8(0xF3); stosq(); return *this; }
    CodeGen& repe_cmpsb() { emit8(0xF3); cmpsb(); return *this; }
    CodeGen& repe_scasb() { emit8(0xF3); scasb(); return *this; }
    CodeGen& repne_cmpsb() { emit8(0xF2); cmpsb(); return *this; }
    CodeGen& repne_scasb() { emit8(0xF2); scasb(); return *this; }

    CodeGen& icebp() { int1(); return *this; }
    CodeGen& fwait_() { emit8(0x9B); return *this; }
    CodeGen& wait_() { emit8(0x9B); return *this; }
    CodeGen& cdqe() { emit8(0x48); emit8(0x98); return *this; }
    CodeGen& cqo() { emit8(0x48); emit8(0x99); return *this; }
    CodeGen& iretq() { emit8(0x48); emit8(0xCF); return *this; }
    CodeGen& pushfq() { emit8(0x9C); return *this; }
    CodeGen& push(int64_t imm) { emit8(0x68); emit_imm(imm, 4); return *this; }  // push imm32
    CodeGen& popfq() { emit8(0x9D); return *this; }
    CodeGen& xlatb() { emit8(0xD7); return *this; }
    CodeGen& setalc() { emit8(0xD6); return *this; }
    CodeGen& loope(Label& l) { emit8(0xE1); emit_label_rel8(l); return *this; }
    CodeGen& loopne(Label& l) { emit8(0xE0); emit_label_rel8(l); return *this; }
    CodeGen& in_(Reg dst, uint8_t port) { if (dst.bits==8) emit8(0xE4); else emit8(0xE5); emit8(port); return *this; }
    CodeGen& in_(Reg dst, Reg /*dx*/) { if (dst.bits==8) emit8(0xEC); else emit8(0xED); return *this; }
    CodeGen& cmpxchg16b(Mem m) { emit8(0x48); emit8(0x0F); emit8(0xC7); emit_mem(1, m); return *this; }
    CodeGen& pextrq(Reg dst, Xmm src, uint8_t imm) { emit8(0x66); emit8(0x48); emit_rex_if_needed(true, src.id, 0, dst.id); emit8(0x0F); emit8(0x3A); emit8(0x16); emit_modrm(3, src.id, dst.id); emit8(imm); return *this; }
    CodeGen& pinsrq(Xmm dst, Reg src, uint8_t imm) { emit8(0x66); emit8(0x48); emit_rex_if_needed(true, dst.id, 0, src.id); emit8(0x0F); emit8(0x3A); emit8(0x22); emit_modrm(3, dst.id, src.id); emit8(imm); return *this; }
    CodeGen& pshufw(Mmx dst, Mmx src, uint8_t imm) { emit8(0x0F); emit8(0x70); emit_modrm(3, dst.id, src.id); emit8(imm); return *this; }

    // Shift/rotate by CL — D2 /ext (byte) | D3 /ext (16/32/64-bit).
    CodeGen& shl(Reg dst, CL_Reg) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        emit8(dst.bits == 8 ? 0xD2 : 0xD3);
        emit_modrm(3, 4, dst.id);
        return *this;
    }
    CodeGen& shl(Mem dst, CL_Reg) {
        if (dst.size_hint == 2) emit8(0x66);
        emit_rex_mem(dst.size_hint==8, 0, dst);
        emit8(dst.size_hint == 1 ? 0xD2 : 0xD3);
        emit_mem(4, dst);
        return *this;
    }
    CodeGen& shr(Reg dst, CL_Reg) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        emit8(dst.bits == 8 ? 0xD2 : 0xD3);
        emit_modrm(3, 5, dst.id);
        return *this;
    }
    CodeGen& shr(Mem dst, CL_Reg) {
        if (dst.size_hint == 2) emit8(0x66);
        emit_rex_mem(dst.size_hint==8, 0, dst);
        emit8(dst.size_hint == 1 ? 0xD2 : 0xD3);
        emit_mem(5, dst);
        return *this;
    }
    CodeGen& sar(Reg dst, CL_Reg) {
        if (dst.bits == 16) emit8(0x66);
        emit_rex_if_needed(dst.bits==64, 0, 0, dst.id);
        emit8(dst.bits == 8 ? 0xD2 : 0xD3);
        emit_modrm(3, 7, dst.id);
        return *this;
    }
    CodeGen& sar(Mem dst, CL_Reg) {
        if (dst.size_hint == 2) emit8(0x66);
        emit_rex_mem(dst.size_hint==8, 0, dst);
        emit8(dst.size_hint == 1 ? 0xD2 : 0xD3);
        emit_mem(7, dst);
        return *this;
    }

    // Raw-displacement JMP: EB cb | 66 E9 cw | E9 cd.
    CodeGen& jmp(int8_t rel)  { emit8(0xEB); emit_imm(rel, 1); return *this; }
    CodeGen& jmp(int16_t rel) { emit8(0x66); emit8(0xE9); emit_imm(rel, 2); return *this; }
    CodeGen& jmp(int32_t rel) { emit8(0xE9); emit_imm(rel, 4); return *this; }
    CodeGen& jmp_rel8(int8_t rel)   { return jmp(rel); }
    CodeGen& jmp_rel16(int16_t rel) { return jmp(rel); }
    CodeGen& jmp_rel32(int32_t rel) { return jmp(rel); }
    // Raw-displacement CALL: E8 cd (only rel32 exists).
    CodeGen& call(int32_t rel) { emit8(0xE8); emit_imm(rel, 4); return *this; }
    CodeGen& call_rel32(int32_t rel) { return call(rel); }

    // BMI1
    CodeGen& andn(Reg dst, Reg src1, Reg src2) { emit_vex_rr(2, 0, 0xF2, dst.id, src2.id, src1.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& andn(Reg dst, Reg src1, Mem src2) { emit_vex_rm(2, 0, 0xF2, dst.id, src2, src1.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& bextr(Reg dst, Reg src, Reg ctl) { emit_vex_rr(2, 0, 0xF7, dst.id, src.id, ctl.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& bextr(Reg dst, Mem src, Reg ctl) { emit_vex_rm(2, 0, 0xF7, dst.id, src, ctl.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& blsi(Reg dst, Reg src) { emit_vex_rr(2, 0, 0xF3, 3, src.id, dst.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& blsmsk(Reg dst, Reg src) { emit_vex_rr(2, 0, 0xF3, 2, src.id, dst.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& blsr(Reg dst, Reg src) { emit_vex_rr(2, 0, 0xF3, 1, src.id, dst.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& tzcnt(Reg dst, Reg src) { emit8(0xF3); emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id); emit8(0x0F); emit8(0xBC); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& lzcnt(Reg dst, Reg src) { emit8(0xF3); emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id); emit8(0x0F); emit8(0xBD); emit_modrm(3, dst.id&7, src.id&7); return *this; }

    // BMI2
    CodeGen& bzhi(Reg dst, Reg src, Reg ctl) { emit_vex_rr(2, 0, 0xF5, dst.id, src.id, ctl.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& mulx(Reg hi, Reg lo, Reg src) { emit_vex_rr(2, 3, 0xF6, hi.id, src.id, lo.id, 0, hi.bits==64?1:0); return *this; }
    CodeGen& pdep(Reg dst, Reg src1, Reg src2) { emit_vex_rr(2, 3, 0xF5, dst.id, src2.id, src1.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& pext(Reg dst, Reg src1, Reg src2) { emit_vex_rr(2, 2, 0xF5, dst.id, src2.id, src1.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& rorx(Reg dst, Reg src, uint8_t imm) { emit_vex_rr(3, 3, 0xF0, dst.id, src.id, 0, 0, dst.bits==64?1:0); emit8(imm); return *this; }
    CodeGen& sarx(Reg dst, Reg src, Reg ctl) { emit_vex_rr(2, 2, 0xF7, dst.id, src.id, ctl.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& shlx(Reg dst, Reg src, Reg ctl) { emit_vex_rr(2, 1, 0xF7, dst.id, src.id, ctl.id, 0, dst.bits==64?1:0); return *this; }
    CodeGen& shrx(Reg dst, Reg src, Reg ctl) { emit_vex_rr(2, 3, 0xF7, dst.id, src.id, ctl.id, 0, dst.bits==64?1:0); return *this; }

    // AES-NI
    CodeGen& aesenc(Xmm dst, Xmm src) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xDC); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& aesenclast(Xmm dst, Xmm src) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xDD); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& aesdec(Xmm dst, Xmm src) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xDE); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& aesdeclast(Xmm dst, Xmm src) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xDF); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& aesimc(Xmm dst, Xmm src) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xDB); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& aeskeygenassist(Xmm dst, Xmm src, uint8_t imm) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x3A); emit8(0xDF); emit_modrm(3, dst.id&7, src.id&7); emit8(imm); return *this; }
    CodeGen& pclmulqdq(Xmm dst, Xmm src, uint8_t imm) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x3A); emit8(0x44); emit_modrm(3, dst.id&7, src.id&7); emit8(imm); return *this; }

    // SHA
    CodeGen& sha1rnds4(Xmm dst, Xmm src, uint8_t imm) { emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x3A); emit8(0xCC); emit_modrm(3, dst.id&7, src.id&7); emit8(imm); return *this; }
    CodeGen& sha1nexte(Xmm dst, Xmm src) { emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xC8); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& sha1msg1(Xmm dst, Xmm src) { emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xC9); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& sha1msg2(Xmm dst, Xmm src) { emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xCA); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& sha256rnds2(Xmm dst, Xmm src) { emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xCB); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& sha256msg1(Xmm dst, Xmm src) { emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xCC); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& sha256msg2(Xmm dst, Xmm src) { emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xCD); emit_modrm(3, dst.id&7, src.id&7); return *this; }

    // ADX
    CodeGen& adcx(Reg dst, Reg src) { emit8(0x66); emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xF6); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& adox(Reg dst, Reg src) { emit8(0xF3); emit_rex_if_needed(dst.bits==64, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xF6); emit_modrm(3, dst.id&7, src.id&7); return *this; }

    // RDRAND / RDSEED
    CodeGen& rdrand(Reg dst) { emit_rex_if_needed(dst.bits==64, 0, 0, dst.id); emit8(0x0F); emit8(0xC7); emit_modrm(3, 6, dst.id&7); return *this; }
    CodeGen& rdseed(Reg dst) { emit_rex_if_needed(dst.bits==64, 0, 0, dst.id); emit8(0x0F); emit8(0xC7); emit_modrm(3, 7, dst.id&7); return *this; }

    // AVX management
    CodeGen& vzeroall() { emit_vex2(0, 0, 0, 1); emit8(0x77); return *this; }
    CodeGen& vzeroupper() { emit_vex2(0, 0, 0, 0); emit8(0x77); return *this; }

    // CET
    CodeGen& endbr64() { emit8(0xF3); emit8(0x0F); emit8(0x1E); emit8(0xFA); return *this; }
    CodeGen& endbr32() { emit8(0xF3); emit8(0x0F); emit8(0x1E); emit8(0xFB); return *this; }

    // FSGSBASE
    CodeGen& rdfsbase(Reg dst) { emit8(0xF3); emit_rex_if_needed(dst.bits==64, 0, 0, dst.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 0, dst.id&7); return *this; }
    CodeGen& rdgsbase(Reg dst) { emit8(0xF3); emit_rex_if_needed(dst.bits==64, 0, 0, dst.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 1, dst.id&7); return *this; }
    CodeGen& wrfsbase(Reg src) { emit8(0xF3); emit_rex_if_needed(src.bits==64, 0, 0, src.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 2, src.id&7); return *this; }
    CodeGen& wrgsbase(Reg src) { emit8(0xF3); emit_rex_if_needed(src.bits==64, 0, 0, src.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 3, src.id&7); return *this; }

    // FMA3
    CodeGen& vfmadd132ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x98, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmadd132pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x98, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmadd132ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x99, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmadd132sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x99, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmadd213ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xA8, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmadd213pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xA8, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmadd213ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xA9, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmadd213sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xA9, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmadd231ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xB8, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmadd231pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xB8, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmadd231ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xB9, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmadd231sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xB9, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsub132ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9A, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsub132pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9A, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsub132ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9B, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsub132sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9B, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsub213ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAA, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsub213pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAA, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsub213ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAB, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsub213sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAB, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsub231ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBA, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsub231pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBA, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsub231ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBB, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsub231sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBB, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmadd132ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9C, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmadd132pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9C, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmadd132ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9D, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmadd132sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9D, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmadd213ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAC, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmadd213pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAC, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmadd213ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAD, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmadd213sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAD, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmadd231ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBC, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmadd231pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBC, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmadd231ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBD, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmadd231sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBD, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmsub132ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9E, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmsub132pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9E, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmsub132ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9F, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmsub132sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x9F, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmsub213ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAE, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmsub213pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAE, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmsub213ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAF, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmsub213sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xAF, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmsub231ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBE, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmsub231pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBE, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfnmsub231ss(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBF, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfnmsub231sd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xBF, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmaddsub132ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x96, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmaddsub132pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x96, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmaddsub213ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xA6, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmaddsub213pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xA6, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmaddsub231ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xB6, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmaddsub231pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xB6, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsubadd132ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x97, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsubadd132pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x97, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsubadd213ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xA7, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsubadd213pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xA7, dst.id, src2.id, src1.id, 0, 1); return *this; }
    CodeGen& vfmsubadd231ps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xB7, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vfmsubadd231pd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0xB7, dst.id, src2.id, src1.id, 0, 1); return *this; }

    // AVX broadcast/permute
    CodeGen& vbroadcastss(Xmm dst, Xmm src) { emit_vex_rr(2, 1, 0x18, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& vbroadcastsd(Xmm dst, Xmm src) { emit_vex_rr(2, 1, 0x19, dst.id, src.id, 0, 1, 0); return *this; }
    CodeGen& vbroadcastf128(Xmm dst, Mem src) { emit_vex_rm(2, 1, 0x1A, dst.id, src, 0, 1, 0); return *this; }
    CodeGen& vpbroadcastb(Xmm dst, Xmm src) { emit_vex_rr(2, 1, 0x78, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& vpbroadcastw(Xmm dst, Xmm src) { emit_vex_rr(2, 1, 0x79, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& vpbroadcastd(Xmm dst, Xmm src) { emit_vex_rr(2, 1, 0x58, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& vpbroadcastq(Xmm dst, Xmm src) { emit_vex_rr(2, 1, 0x59, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& vperm2f128(Xmm dst, Xmm src1, Xmm src2, uint8_t imm) { emit_vex_rr(3, 1, 0x06, dst.id, src2.id, src1.id, 1, 0); emit8(imm); return *this; }
    CodeGen& vperm2i128(Xmm dst, Xmm src1, Xmm src2, uint8_t imm) { emit_vex_rr(3, 1, 0x46, dst.id, src2.id, src1.id, 1, 0); emit8(imm); return *this; }
    CodeGen& vpermd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x36, dst.id, src2.id, src1.id, 1, 0); return *this; }
    CodeGen& vpermq(Xmm dst, Xmm src, uint8_t imm) { emit_vex_rr(3, 1, 0x00, dst.id, src.id, 0, 1, 1); emit8(imm); return *this; }
    CodeGen& vpermpd(Xmm dst, Xmm src, uint8_t imm) { emit_vex_rr(3, 1, 0x01, dst.id, src.id, 0, 1, 1); emit8(imm); return *this; }
    CodeGen& vpermps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x16, dst.id, src2.id, src1.id, 1, 0); return *this; }
    CodeGen& vpermilps(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x0C, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vpermilps(Xmm dst, Xmm src, uint8_t imm) { emit_vex_rr(3, 1, 0x04, dst.id, src.id, 0, 0, 0); emit8(imm); return *this; }
    CodeGen& vpermilpd(Xmm dst, Xmm src1, Xmm src2) { emit_vex_rr(2, 1, 0x0D, dst.id, src2.id, src1.id, 0, 0); return *this; }
    CodeGen& vpermilpd(Xmm dst, Xmm src, uint8_t imm) { emit_vex_rr(3, 1, 0x05, dst.id, src.id, 0, 0, 0); emit8(imm); return *this; }
    CodeGen& vinsertf128(Xmm dst, Xmm src1, Xmm src2, uint8_t imm) { emit_vex_rr(3, 1, 0x18, dst.id, src2.id, src1.id, 1, 0); emit8(imm); return *this; }
    CodeGen& vinserti128(Xmm dst, Xmm src1, Xmm src2, uint8_t imm) { emit_vex_rr(3, 1, 0x38, dst.id, src2.id, src1.id, 1, 0); emit8(imm); return *this; }
    CodeGen& vextractf128(Xmm dst, Xmm src, uint8_t imm) { emit_vex_rr(3, 1, 0x19, src.id, dst.id, 0, 1, 0); emit8(imm); return *this; }
    CodeGen& vextracti128(Xmm dst, Xmm src, uint8_t imm) { emit_vex_rr(3, 1, 0x39, src.id, dst.id, 0, 1, 0); emit8(imm); return *this; }
    CodeGen& vgatherdps(Xmm dst, Mem m, Xmm mask) { emit_vex_rm(2, 1, 0x92, dst.id, m, mask.id, 0, 0); return *this; }
    CodeGen& vgatherdpd(Xmm dst, Mem m, Xmm mask) { emit_vex_rm(2, 1, 0x92, dst.id, m, mask.id, 0, 1); return *this; }
    CodeGen& vgatherqps(Xmm dst, Mem m, Xmm mask) { emit_vex_rm(2, 1, 0x93, dst.id, m, mask.id, 0, 0); return *this; }
    CodeGen& vgatherqpd(Xmm dst, Mem m, Xmm mask) { emit_vex_rm(2, 1, 0x93, dst.id, m, mask.id, 0, 1); return *this; }
    CodeGen& vpgatherdd(Xmm dst, Mem m, Xmm mask) { emit_vex_rm(2, 1, 0x90, dst.id, m, mask.id, 0, 0); return *this; }
    CodeGen& vpgatherdq(Xmm dst, Mem m, Xmm mask) { emit_vex_rm(2, 1, 0x90, dst.id, m, mask.id, 0, 1); return *this; }
    CodeGen& vpgatherqd(Xmm dst, Mem m, Xmm mask) { emit_vex_rm(2, 1, 0x91, dst.id, m, mask.id, 0, 0); return *this; }
    CodeGen& vpgatherqq(Xmm dst, Mem m, Xmm mask) { emit_vex_rm(2, 1, 0x91, dst.id, m, mask.id, 0, 1); return *this; }

    // Opmask (AVX-512)
    CodeGen& kmovw(Reg dst, Reg src) { emit_vex_rr(1, 0, 0x90, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& kmovw_store(Mem dst, Reg src) { emit_vex_rm(1, 0, 0x91, src.id, dst, 0, 0, 0); return *this; }
    CodeGen& kmovw_load(Reg dst, Mem src) { emit_vex_rm(1, 0, 0x90, dst.id, src, 0, 0, 0); return *this; }
    CodeGen& kmovw_from_gpr(Reg dst, Reg src) { emit_vex_rr(1, 0, 0x92, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& kmovw_to_gpr(Reg dst, Reg src) { emit_vex_rr(1, 0, 0x93, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& kandw(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x41, dst.id, src2.id, src1.id, 1, 0); return *this; }
    CodeGen& kandd(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 1, 0x41, dst.id, src2.id, src1.id, 1, 1); return *this; }
    CodeGen& kandq(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x41, dst.id, src2.id, src1.id, 1, 1); return *this; }
    CodeGen& kandnw(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x42, dst.id, src2.id, src1.id, 1, 0); return *this; }
    CodeGen& korw(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x45, dst.id, src2.id, src1.id, 1, 0); return *this; }
    CodeGen& kord(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 1, 0x45, dst.id, src2.id, src1.id, 1, 1); return *this; }
    CodeGen& korq(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x45, dst.id, src2.id, src1.id, 1, 1); return *this; }
    CodeGen& kxorw(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x47, dst.id, src2.id, src1.id, 1, 0); return *this; }
    CodeGen& kxord(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 1, 0x47, dst.id, src2.id, src1.id, 1, 1); return *this; }
    CodeGen& kxorq(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x47, dst.id, src2.id, src1.id, 1, 1); return *this; }
    CodeGen& knotw(Reg dst, Reg src) { emit_vex_rr(1, 0, 0x44, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& knotd(Reg dst, Reg src) { emit_vex_rr(1, 1, 0x44, dst.id, src.id, 0, 0, 1); return *this; }
    CodeGen& knotq(Reg dst, Reg src) { emit_vex_rr(1, 0, 0x44, dst.id, src.id, 0, 0, 1); return *this; }
    CodeGen& kortestw(Reg dst, Reg src) { emit_vex_rr(1, 0, 0x98, dst.id, src.id, 0, 0, 0); return *this; }
    CodeGen& kortestd(Reg dst, Reg src) { emit_vex_rr(1, 1, 0x98, dst.id, src.id, 0, 0, 1); return *this; }
    CodeGen& kortestq(Reg dst, Reg src) { emit_vex_rr(1, 0, 0x98, dst.id, src.id, 0, 0, 1); return *this; }
    CodeGen& kunpckbw(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 1, 0x4B, dst.id, src2.id, src1.id, 1, 0); return *this; }
    CodeGen& kunpckwd(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x4B, dst.id, src2.id, src1.id, 1, 0); return *this; }
    CodeGen& kunpckdq(Reg dst, Reg src1, Reg src2) { emit_vex_rr(1, 0, 0x4B, dst.id, src2.id, src1.id, 1, 1); return *this; }
    CodeGen& kshiftrw(Reg dst, Reg src, uint8_t imm) { emit_vex_rr(3, 1, 0x30, dst.id, src.id, 0, 0, 1); emit8(imm); return *this; }
    CodeGen& kshiftlw(Reg dst, Reg src, uint8_t imm) { emit_vex_rr(3, 1, 0x32, dst.id, src.id, 0, 0, 1); emit8(imm); return *this; }

    // Misc modern instructions
    CodeGen& clac() { emit8(0x0F); emit8(0x01); emit8(0xCA); return *this; }
    CodeGen& stac() { emit8(0x0F); emit8(0x01); emit8(0xCB); return *this; }
    CodeGen& clflushopt(Mem m) { emit8(0x66); emit_rex_mem(false, 7, m); emit8(0x0F); emit8(0xAE); emit_mem(7, m); return *this; }
    CodeGen& clwb(Mem m) { emit8(0x66); emit_rex_mem(false, 6, m); emit8(0x0F); emit8(0xAE); emit_mem(6, m); return *this; }
    CodeGen& serialize() { emit8(0x0F); emit8(0x01); emit8(0xE8); return *this; }
    CodeGen& wbnoinvd() { emit8(0xF3); emit8(0x0F); emit8(0x09); return *this; }
    CodeGen& rdpid(Reg dst) { emit8(0xF3); emit_rex_if_needed(false, 7, 0, dst.id); emit8(0x0F); emit8(0xC7); emit_modrm(3, 7, dst.id&7); return *this; }
    CodeGen& rdpkru() { emit8(0x0F); emit8(0x01); emit8(0xEE); return *this; }
    CodeGen& wrpkru() { emit8(0x0F); emit8(0x01); emit8(0xEF); return *this; }
    CodeGen& movdiri(Mem dst, Reg src) { emit_rex_mem(src.bits==64, src.id, dst); emit8(0x0F); emit8(0x38); emit8(0xF9); emit_mem(src.id&7, dst); return *this; }
    CodeGen& movdir64b(Reg dst, Mem src) { emit8(0x66); emit_rex_mem(false, dst.id, src); emit8(0x0F); emit8(0x38); emit8(0xF8); emit_mem(dst.id&7, src); return *this; }
    CodeGen& prefetchw(Mem m) { emit_rex_mem(false, 1, m); emit8(0x0F); emit8(0x0D); emit_mem(1, m); return *this; }
    CodeGen& invpcid(Reg r, Mem m) { emit8(0x66); emit_rex_mem(false, r.id, m); emit8(0x0F); emit8(0x38); emit8(0x82); emit_mem(r.id&7, m); return *this; }
    CodeGen& ptwrite(Reg src) { emit8(0xF3); emit_rex_if_needed(src.bits==64, 4, 0, src.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 4, src.id&7); return *this; }
    CodeGen& fwait() { emit8(0x9B); return *this; }
    CodeGen& wait() { emit8(0x9B); return *this; }
    CodeGen& int_n(uint8_t n) { emit8(0xCD); emit8(n); return *this; }
    CodeGen& tpause(Reg r) { emit8(0x66); emit_rex_if_needed(false, 6, 0, r.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 6, r.id&7); return *this; }
    CodeGen& umonitor(Reg r) { emit8(0xF3); emit_rex_if_needed(false, 6, 0, r.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 6, r.id&7); return *this; }
    CodeGen& umwait(Reg r) { emit8(0xF2); emit_rex_if_needed(false, 6, 0, r.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 6, r.id&7); return *this; }
    CodeGen& enqcmd(Reg dst, Mem src) { emit8(0xF2); emit_rex_mem(false, dst.id, src); emit8(0x0F); emit8(0x38); emit8(0xF8); emit_mem(dst.id&7, src); return *this; }
    CodeGen& enqcmds(Reg dst, Mem src) { emit8(0xF3); emit_rex_mem(false, dst.id, src); emit8(0x0F); emit8(0x38); emit8(0xF8); emit_mem(dst.id&7, src); return *this; }
    CodeGen& hreset(uint8_t imm) { emit8(0xF3); emit8(0x0F); emit8(0x3A); emit8(0xF0); emit8(0xC0); emit8(imm); return *this; }

    // TSX/XSAVE
    CodeGen& xbegin(Label& lab) { emit8(0xC7); emit8(0xF8); emit_label_rel32(lab); return *this; }
    CodeGen& xend() { emit8(0x0F); emit8(0x01); emit8(0xD5); return *this; }
    CodeGen& xabort(uint8_t imm) { emit8(0xC6); emit8(0xF8); emit8(imm); return *this; }
    CodeGen& xtest() { emit8(0x0F); emit8(0x01); emit8(0xD6); return *this; }
    CodeGen& xsavec(Mem m) { emit_rex_mem(false, 4, m); emit8(0x0F); emit8(0xC7); emit_mem(4, m); return *this; }
    CodeGen& xsaveopt(Mem m) { emit_rex_mem(false, 6, m); emit8(0x0F); emit8(0xAE); emit_mem(6, m); return *this; }
    CodeGen& xsaves(Mem m) { emit_rex_mem(false, 5, m); emit8(0x0F); emit8(0xC7); emit_mem(5, m); return *this; }
    CodeGen& xrstors(Mem m) { emit_rex_mem(false, 3, m); emit8(0x0F); emit8(0xC7); emit_mem(3, m); return *this; }

    // CET shadow stack
    CodeGen& incsspd(Reg r) { emit8(0xF3); emit_rex_if_needed(false, 5, 0, r.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 5, r.id&7); return *this; }
    CodeGen& incsspq(Reg r) { emit8(0xF3); emit_rex_if_needed(true, 5, 0, r.id); emit8(0x0F); emit8(0xAE); emit_modrm(3, 5, r.id&7); return *this; }
    CodeGen& rdsspd(Reg r) { emit8(0xF3); emit_rex_if_needed(false, 1, 0, r.id); emit8(0x0F); emit8(0x1E); emit_modrm(3, 1, r.id&7); return *this; }
    CodeGen& rdsspq(Reg r) { emit8(0xF3); emit_rex_if_needed(true, 1, 0, r.id); emit8(0x0F); emit8(0x1E); emit_modrm(3, 1, r.id&7); return *this; }
    CodeGen& clrssbsy(Mem m) { emit8(0xF3); emit_rex_mem(false, 6, m); emit8(0x0F); emit8(0xAE); emit_mem(6, m); return *this; }
    CodeGen& rstorssp(Mem m) { emit8(0xF3); emit_rex_mem(false, 5, m); emit8(0x0F); emit8(0x01); emit_mem(5, m); return *this; }
    CodeGen& saveprevssp() { emit8(0xF3); emit8(0x0F); emit8(0x01); emit8(0xEA); return *this; }
    CodeGen& setssbsy() { emit8(0xF3); emit8(0x0F); emit8(0x01); emit8(0xE8); return *this; }
    CodeGen& wrssd(Mem dst, Reg src) { emit_rex_mem(false, src.id, dst); emit8(0x0F); emit8(0x38); emit8(0xF6); emit_mem(src.id&7, dst); return *this; }
    CodeGen& wrssq(Mem dst, Reg src) { emit_rex_mem(true, src.id, dst); emit8(0x0F); emit8(0x38); emit8(0xF6); emit_mem(src.id&7, dst); return *this; }
    CodeGen& wrussd(Mem dst, Reg src) { emit8(0x66); emit_rex_mem(false, src.id, dst); emit8(0x0F); emit8(0x38); emit8(0xF5); emit_mem(src.id&7, dst); return *this; }
    CodeGen& wrussq(Mem dst, Reg src) { emit8(0x66); emit_rex_mem(true, src.id, dst); emit8(0x0F); emit8(0x38); emit8(0xF5); emit_mem(src.id&7, dst); return *this; }

    // User interrupts
    CodeGen& clui() { emit8(0xF3); emit8(0x0F); emit8(0x01); emit8(0xEE); return *this; }
    CodeGen& stui() { emit8(0xF3); emit8(0x0F); emit8(0x01); emit8(0xEF); return *this; }
    CodeGen& testui() { emit8(0xF3); emit8(0x0F); emit8(0x01); emit8(0xED); return *this; }
    CodeGen& uiret() { emit8(0xF3); emit8(0x0F); emit8(0x01); emit8(0xEC); return *this; }
    CodeGen& senduipi(Reg r) { emit8(0xF3); emit_rex_if_needed(false, 6, 0, r.id); emit8(0x0F); emit8(0xC7); emit_modrm(3, 6, r.id&7); return *this; }

    // MPX (raw byte stubs)
    CodeGen& bndmk(Reg bnd, Mem m) { emit8(0xF3); emit_rex_mem(false, bnd.id, m); emit8(0x0F); emit8(0x1B); emit_mem(bnd.id&7, m); return *this; }
    CodeGen& bndcl(Reg bnd, Reg src) { emit8(0xF3); emit_rex_if_needed(false, bnd.id, 0, src.id); emit8(0x0F); emit8(0x1A); emit_modrm(3, bnd.id&7, src.id&7); return *this; }
    CodeGen& bndcu(Reg bnd, Reg src) { emit8(0xF2); emit_rex_if_needed(false, bnd.id, 0, src.id); emit8(0x0F); emit8(0x1A); emit_modrm(3, bnd.id&7, src.id&7); return *this; }
    CodeGen& bndcn(Reg bnd, Reg src) { emit8(0xF2); emit_rex_if_needed(false, bnd.id, 0, src.id); emit8(0x0F); emit8(0x1B); emit_modrm(3, bnd.id&7, src.id&7); return *this; }
    CodeGen& bndldx(Reg bnd, Mem m) { emit_rex_mem(false, bnd.id, m); emit8(0x0F); emit8(0x1A); emit_mem(bnd.id&7, m); return *this; }
    CodeGen& bndstx(Mem m, Reg bnd) { emit_rex_mem(false, bnd.id, m); emit8(0x0F); emit8(0x1B); emit_mem(bnd.id&7, m); return *this; }
    CodeGen& bndmov(Reg dst, Reg src) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x1A); emit_modrm(3, dst.id&7, src.id&7); return *this; }

    // Key Locker
    CodeGen& aesenc128kl(Xmm dst, Mem m) { emit8(0xF3); emit_rex_mem(false, dst.id, m); emit8(0x0F); emit8(0x38); emit8(0xDC); emit_mem(dst.id&7, m); return *this; }
    CodeGen& aesdec128kl(Xmm dst, Mem m) { emit8(0xF3); emit_rex_mem(false, dst.id, m); emit8(0x0F); emit8(0x38); emit8(0xDD); emit_mem(dst.id&7, m); return *this; }
    CodeGen& aesenc256kl(Xmm dst, Mem m) { emit8(0xF3); emit_rex_mem(false, dst.id, m); emit8(0x0F); emit8(0x38); emit8(0xDE); emit_mem(dst.id&7, m); return *this; }
    CodeGen& aesdec256kl(Xmm dst, Mem m) { emit8(0xF3); emit_rex_mem(false, dst.id, m); emit8(0x0F); emit8(0x38); emit8(0xDF); emit_mem(dst.id&7, m); return *this; }
    CodeGen& aesencwide128kl(Mem m) { emit8(0xF3); emit_rex_mem(false, 0, m); emit8(0x0F); emit8(0x38); emit8(0xD8); emit_mem(0, m); return *this; }
    CodeGen& aesdecwide128kl(Mem m) { emit8(0xF3); emit_rex_mem(false, 1, m); emit8(0x0F); emit8(0x38); emit8(0xD8); emit_mem(1, m); return *this; }
    CodeGen& aesencwide256kl(Mem m) { emit8(0xF3); emit_rex_mem(false, 2, m); emit8(0x0F); emit8(0x38); emit8(0xD8); emit_mem(2, m); return *this; }
    CodeGen& aesdecwide256kl(Mem m) { emit8(0xF3); emit_rex_mem(false, 3, m); emit8(0x0F); emit8(0x38); emit8(0xD8); emit_mem(3, m); return *this; }
    CodeGen& encodekey128(Reg dst, Reg src) { emit8(0xF3); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xFA); emit_modrm(3, dst.id&7, src.id&7); return *this; }
    CodeGen& encodekey256(Reg dst, Reg src) { emit8(0xF3); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xFB); emit_modrm(3, dst.id&7, src.id&7); return *this; }

    // GFNI
    CodeGen& gf2p8affineinvqb(Xmm dst, Xmm src, uint8_t imm) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x3A); emit8(0xCF); emit_modrm(3, dst.id&7, src.id&7); emit8(imm); return *this; }
    CodeGen& gf2p8affineqb(Xmm dst, Xmm src, uint8_t imm) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x3A); emit8(0xCE); emit_modrm(3, dst.id&7, src.id&7); emit8(imm); return *this; }
    CodeGen& gf2p8mulb(Xmm dst, Xmm src) { emit8(0x66); emit_rex_if_needed(false, dst.id, 0, src.id); emit8(0x0F); emit8(0x38); emit8(0xCF); emit_modrm(3, dst.id&7, src.id&7); return *this; }

    // === AVX-512 (EVEX) instructions ===

    CodeGen& vpxord(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0xEF, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpxord(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(1, 1, 0xEF, dst.id, src2.id, src1.id, 0, 0); return *this;
    }
    CodeGen& vpxorq(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0xEF, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vpxorq(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(1, 1, 0xEF, dst.id, src2.id, src1.id, 0, 1); return *this;
    }
    CodeGen& vpandd(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0xDB, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpandd(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(1, 1, 0xDB, dst.id, src2.id, src1.id, 0, 0); return *this;
    }
    CodeGen& vpandq(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0xDB, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vpandq(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(1, 1, 0xDB, dst.id, src2.id, src1.id, 0, 1); return *this;
    }
    CodeGen& vpord(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0xEB, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpord(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(1, 1, 0xEB, dst.id, src2.id, src1.id, 0, 0); return *this;
    }
    CodeGen& vporq(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0xEB, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vporq(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(1, 1, 0xEB, dst.id, src2.id, src1.id, 0, 1); return *this;
    }
    CodeGen& vpandnd(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0xDF, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpandnd(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(1, 1, 0xDF, dst.id, src2.id, src1.id, 0, 0); return *this;
    }
    CodeGen& vpandnq(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0xDF, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vpandnq(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(1, 1, 0xDF, dst.id, src2.id, src1.id, 0, 1); return *this;
    }

    CodeGen& vpternlogd(Zmm dst, Zmm src1, Zmm src2, uint8_t imm) {
        emit_evex_rr_ib(3, 1, 0x25, dst.id, src2.id, src1.id, 2, 0, imm); return *this;
    }
    CodeGen& vpternlogq(Zmm dst, Zmm src1, Zmm src2, uint8_t imm) {
        emit_evex_rr_ib(3, 1, 0x25, dst.id, src2.id, src1.id, 2, 1, imm); return *this;
    }
    CodeGen& vpternlogd(Xmm dst, Xmm src1, Xmm src2, uint8_t imm) {
        emit_evex_rr_ib(3, 1, 0x25, dst.id, src2.id, src1.id, 0, 0, imm); return *this;
    }

    CodeGen& vaddps_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 0, 0x58, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vaddpd_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0x58, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vsubps_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 0, 0x5C, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vsubpd_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0x5C, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vmulps_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 0, 0x59, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vmulpd_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0x59, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vdivps_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 0, 0x5E, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vdivpd_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0x5E, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vminps_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 0, 0x5D, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vminpd_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0x5D, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vmaxps_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 0, 0x5F, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vmaxpd_z(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(1, 1, 0x5F, dst.id, src2.id, src1.id, 2, 1); return *this;
    }

    CodeGen& vpdpbusd(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(2, 1, 0x50, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpdpbusd(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(2, 1, 0x50, dst.id, src2.id, src1.id, 0, 0); return *this;
    }
    CodeGen& vpdpbusds(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(2, 1, 0x51, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpdpbusds(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(2, 1, 0x51, dst.id, src2.id, src1.id, 0, 0); return *this;
    }
    CodeGen& vpdpwssd(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(2, 1, 0x52, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpdpwssd(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(2, 1, 0x52, dst.id, src2.id, src1.id, 0, 0); return *this;
    }
    CodeGen& vpdpwssds(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(2, 1, 0x53, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpdpwssds(Xmm dst, Xmm src1, Xmm src2) {
        emit_evex_rr(2, 1, 0x53, dst.id, src2.id, src1.id, 0, 0); return *this;
    }

    CodeGen& vblendmps(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(2, 1, 0x65, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vblendmpd(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(2, 1, 0x65, dst.id, src2.id, src1.id, 2, 1); return *this;
    }
    CodeGen& vpblendmd(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(2, 1, 0x64, dst.id, src2.id, src1.id, 2, 0); return *this;
    }
    CodeGen& vpblendmq(Zmm dst, Zmm src1, Zmm src2) {
        emit_evex_rr(2, 1, 0x64, dst.id, src2.id, src1.id, 2, 1); return *this;
    }

    // === AVX-512 masked variants ===

    CodeGen& vpxord_k(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 1, 0xEF, dst.id, src2.id, src1.id, 2, 0, k, z); return *this;
    }
    CodeGen& vpxorq_k(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 1, 0xEF, dst.id, src2.id, src1.id, 2, 1, k, z); return *this;
    }
    CodeGen& vpandd_k(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 1, 0xDB, dst.id, src2.id, src1.id, 2, 0, k, z); return *this;
    }
    CodeGen& vpandq_k(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 1, 0xDB, dst.id, src2.id, src1.id, 2, 1, k, z); return *this;
    }
    CodeGen& vpord_k(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 1, 0xEB, dst.id, src2.id, src1.id, 2, 0, k, z); return *this;
    }
    CodeGen& vporq_k(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 1, 0xEB, dst.id, src2.id, src1.id, 2, 1, k, z); return *this;
    }

    CodeGen& vpternlogd_k(Zmm dst, Zmm src1, Zmm src2, uint8_t imm, uint8_t k, bool z = false) {
        emit_evex_rr_ib(3, 1, 0x25, dst.id, src2.id, src1.id, 2, 0, imm, k, z); return *this;
    }

    CodeGen& vaddps_kz(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 0, 0x58, dst.id, src2.id, src1.id, 2, 0, k, z); return *this;
    }
    CodeGen& vaddpd_kz(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 1, 0x58, dst.id, src2.id, src1.id, 2, 1, k, z); return *this;
    }
    CodeGen& vmulps_kz(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 0, 0x59, dst.id, src2.id, src1.id, 2, 0, k, z); return *this;
    }
    CodeGen& vmulpd_kz(Zmm dst, Zmm src1, Zmm src2, uint8_t k, bool z = false) {
        emit_evex_rr(1, 1, 0x59, dst.id, src2.id, src1.id, 2, 1, k, z); return *this;
    }

    CodeGen& vcmpps_k(Xmm kdst, Xmm src1, Xmm src2, uint8_t pred) {
        emit_evex_rr_ib(1, 0, 0xC2, kdst.id, src2.id, src1.id, 0, 0, pred); return *this;
    }
    CodeGen& vcmppd_k(Xmm kdst, Xmm src1, Xmm src2, uint8_t pred) {
        emit_evex_rr_ib(1, 1, 0xC2, kdst.id, src2.id, src1.id, 0, 1, pred); return *this;
    }

    CodeGen& vshufi32x4(Zmm dst, Zmm src1, Zmm src2, uint8_t imm) {
        emit_evex_rr_ib(3, 1, 0x43, dst.id, src2.id, src1.id, 2, 0, imm); return *this;
    }
    CodeGen& vshufi64x2(Zmm dst, Zmm src1, Zmm src2, uint8_t imm) {
        emit_evex_rr_ib(3, 1, 0x43, dst.id, src2.id, src1.id, 2, 1, imm); return *this;
    }

};

} // namespace vedx64
