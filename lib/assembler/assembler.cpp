// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#ifdef VEDX64_ASSEMBLER
#include "vedx64/assembler.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unordered_map>

namespace vedx64 {

namespace {

std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

std::string to_lower(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return r;
}

struct RegInfo { const char* name; uint8_t id; uint8_t bits; };
const RegInfo g_gpr_table[] = {
    {"rax", 0, 64},
    {"rcx", 1, 64},
    {"rdx", 2, 64},
    {"rbx", 3, 64},
    {"rsp", 4, 64},
    {"rbp", 5, 64},
    {"rsi", 6, 64},
    {"rdi", 7, 64},
    {"r8", 8, 64},
    {"r9", 9, 64},
    {"r10", 10, 64},
    {"r11", 11, 64},
    {"r12", 12, 64},
    {"r13", 13, 64},
    {"r14", 14, 64},
    {"r15", 15, 64},
    {"eax", 0, 32},
    {"ecx", 1, 32},
    {"edx", 2, 32},
    {"ebx", 3, 32},
    {"esp", 4, 32},
    {"ebp", 5, 32},
    {"esi", 6, 32},
    {"edi", 7, 32},
    {"r8d", 8, 32},
    {"r9d", 9, 32},
    {"r10d", 10, 32},
    {"r11d", 11, 32},
    {"r12d", 12, 32},
    {"r13d", 13, 32},
    {"r14d", 14, 32},
    {"r15d", 15, 32},
    {"ax", 0, 16},
    {"cx", 1, 16},
    {"dx", 2, 16},
    {"bx", 3, 16},
    {"sp", 4, 16},
    {"bp", 5, 16},
    {"si", 6, 16},
    {"di", 7, 16},
    {"r8w", 8, 16},
    {"r9w", 9, 16},
    {"r10w", 10, 16},
    {"r11w", 11, 16},
    {"r12w", 12, 16},
    {"r13w", 13, 16},
    {"r14w", 14, 16},
    {"r15w", 15, 16},
    {"al", 0, 8},
    {"cl", 1, 8},
    {"dl", 2, 8},
    {"bl", 3, 8},
    {"spl", 4, 8},
    {"bpl", 5, 8},
    {"sil", 6, 8},
    {"dil", 7, 8},
    {"r8b", 8, 8},
    {"r9b", 9, 8},
    {"r10b", 10, 8},
    {"r11b", 11, 8},
    {"r12b", 12, 8},
    {"r13b", 13, 8},
    {"r14b", 14, 8},
    {"r15b", 15, 8},
    {"ah", 4, 8},
    {"ch", 5, 8},
    {"dh", 6, 8},
    {"bh", 7, 8},
};

struct XmmInfo { const char* name; uint8_t id; };
const XmmInfo g_xmm_table[] = {
    {"xmm0", 0},
    {"xmm1", 1},
    {"xmm2", 2},
    {"xmm3", 3},
    {"xmm4", 4},
    {"xmm5", 5},
    {"xmm6", 6},
    {"xmm7", 7},
    {"xmm8", 8},
    {"xmm9", 9},
    {"xmm10", 10},
    {"xmm11", 11},
    {"xmm12", 12},
    {"xmm13", 13},
    {"xmm14", 14},
    {"xmm15", 15},
};

const XmmInfo g_mmx_table[] = {
    {"mm0", 0},
    {"mm1", 1},
    {"mm2", 2},
    {"mm3", 3},
    {"mm4", 4},
    {"mm5", 5},
    {"mm6", 6},
    {"mm7", 7},
};

const XmmInfo g_zmm_table[] = {
    {"zmm0", 0},
    {"zmm1", 1},
    {"zmm2", 2},
    {"zmm3", 3},
    {"zmm4", 4},
    {"zmm5", 5},
    {"zmm6", 6},
    {"zmm7", 7},
    {"zmm8", 8},
    {"zmm9", 9},
    {"zmm10", 10},
    {"zmm11", 11},
    {"zmm12", 12},
    {"zmm13", 13},
    {"zmm14", 14},
    {"zmm15", 15},
    {"zmm16", 16},
    {"zmm17", 17},
    {"zmm18", 18},
    {"zmm19", 19},
    {"zmm20", 20},
    {"zmm21", 21},
    {"zmm22", 22},
    {"zmm23", 23},
    {"zmm24", 24},
    {"zmm25", 25},
    {"zmm26", 26},
    {"zmm27", 27},
    {"zmm28", 28},
    {"zmm29", 29},
    {"zmm30", 30},
    {"zmm31", 31},
};

enum class OpKind { Reg, Xmm, Zmm, Mmx, Mem, Imm };
struct ParsedOp {
    OpKind kind;
    Reg reg;
    Xmm xmm;
    Zmm zmm;
    Mmx mmx;
    Mem mem;
    int64_t imm;
    uint8_t mask_k;   // 0 = no mask, 1-7 = k1-k7
    bool zeroing;     // {z} decorator
};

bool parse_gpr(const std::string& s, Reg& out) {
    for (auto& r : g_gpr_table) {
        if (s == r.name) { out = Reg{r.id, r.bits}; return true; }
    }
    return false;
}

bool parse_xmm(const std::string& s, Xmm& out) {
    for (auto& x : g_xmm_table) {
        if (s == x.name) { out = Xmm{x.id}; return true; }
    }
    return false;
}

bool parse_zmm(const std::string& s, Zmm& out) {
    for (auto& x : g_zmm_table) {
        if (s == x.name) { out = Zmm{x.id}; return true; }
    }
    return false;
}

bool parse_mmx(const std::string& s, Mmx& out) {
    for (auto& x : g_mmx_table) {
        if (s == x.name) { out = Mmx{x.id}; return true; }
    }
    return false;
}

bool parse_imm(const std::string& s, int64_t& out) {
    if (s.empty()) return false;
    const char* p = s.c_str();
    bool neg = false;
    if (*p == '-') { neg = true; p++; }
    else if (*p == '+') p++;
    if (*p == '\0') return false;
    char* end = nullptr;
    uint64_t val;
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
        val = strtoull(p, &end, 16);
    else
        val = strtoull(p, &end, 10);
    if (end == p || *end != '\0') return false;
    out = neg ? -(int64_t)val : (int64_t)val;
    return true;
}

bool parse_mem(const std::string& s, Mem& out) {
    std::string t = s;
    out = Mem{};

    // Size prefix
    if (t.substr(0, 5) == "byte ")  { out.size_hint = 1; t = trim(t.substr(5)); }
    else if (t.substr(0, 5) == "word ")  { out.size_hint = 2; t = trim(t.substr(5)); }
    else if (t.substr(0, 6) == "dword ") { out.size_hint = 4; t = trim(t.substr(6)); }
    else if (t.substr(0, 6) == "qword ") { out.size_hint = 8; t = trim(t.substr(6)); }
    if (t.substr(0, 4) == "ptr ") t = trim(t.substr(4));

    if (t.size() >= 3 && t[2] == ':') {
        std::string seg = t.substr(0, 2);
        if (seg == "fs") { out.seg = SegOverride::FS; t = trim(t.substr(3)); }
        else if (seg == "gs") { out.seg = SegOverride::GS; t = trim(t.substr(3)); }
        else if (seg == "cs") { out.seg = SegOverride::CS; t = trim(t.substr(3)); }
        else if (seg == "ss") { out.seg = SegOverride::SS; t = trim(t.substr(3)); }
        else if (seg == "ds") { out.seg = SegOverride::DS; t = trim(t.substr(3)); }
        else if (seg == "es") { out.seg = SegOverride::ES; t = trim(t.substr(3)); }
    }

    if (t.empty() || t.front() != '[' || t.back() != ']') return false;
    t = t.substr(1, t.size() - 2); // strip brackets
    t = trim(t);
    if (t.empty()) return false;

    // Tokenize: split on +/-, preserving sign for immediates
    std::vector<std::string> tokens;
    size_t pos = 0;
    while (pos < t.size()) {
        while (pos < t.size() && t[pos] == ' ') pos++;
        if (pos >= t.size()) break;
        bool is_neg = (t[pos] == '-');
        if (t[pos] == '+' || t[pos] == '-') pos++;
        while (pos < t.size() && t[pos] == ' ') pos++;
        size_t tstart = pos;
        while (pos < t.size() && t[pos] != '+' && t[pos] != '-') pos++;
        std::string tok = trim(t.substr(tstart, pos - tstart));
        if (is_neg) tok = "-" + tok;
        if (!tok.empty()) tokens.push_back(tok);
    }

    bool has_base = false, has_index = false;
    for (auto& tok : tokens) {
        // Check for reg*scale
        size_t star = tok.find('*');
        if (star != std::string::npos) {
            std::string rname = trim(tok.substr(0, star));
            std::string sstr = trim(tok.substr(star + 1));
            Reg idx;
            if (!parse_gpr(rname, idx)) return false;
            int64_t sc; if (!parse_imm(sstr, sc)) return false;
            out.index = idx; out.has_index = true; out.scale = (uint8_t)sc; has_index = true;
            continue;
        }
        // Check for rip
        if (tok == "rip") {
            out.rip_relative = true;
            continue;
        }
        // Check for register
        Reg r;
        std::string clean = tok;
        if (!clean.empty() && clean[0] == '-') clean = clean.substr(1);
        if (parse_gpr(clean, r)) {
            if (!has_base) {
                out.base = r; out.has_base = true; has_base = true;
            } else if (!has_index) {
                out.index = r; out.has_index = true; out.scale = 1; has_index = true;
            } else return false;
            continue;
        }
        // Must be immediate displacement
        int64_t d;
        if (parse_imm(tok, d)) {
            out.disp = (int32_t)d;
            continue;
        }
        return false; // unrecognized token
    }

    if (out.rip_relative && !has_base) {
        // rip_ptr(disp) — base stays zero
    }
    return true;
}

bool parse_operand(const std::string& s, ParsedOp& out) {
    out.mask_k = 0; out.zeroing = false;
    std::string clean = s;
    for (;;) {
        size_t brace = clean.find('{');
        if (brace == std::string::npos) break;
        size_t end = clean.find('}', brace);
        if (end == std::string::npos) break;
        std::string dec = trim(clean.substr(brace + 1, end - brace - 1));
        if (dec == "z") out.zeroing = true;
        else if (dec.size() == 2 && dec[0] == 'k' && dec[1] >= '1' && dec[1] <= '7') out.mask_k = dec[1] - '0';
        clean = trim(clean.substr(0, brace) + clean.substr(end + 1));
    }
    clean = trim(clean);

    Reg r; Xmm x; Zmm z; Mmx mm; Mem m; int64_t imm;
    if (parse_gpr(clean, r)) { out.kind = OpKind::Reg; out.reg = r; return true; }
    if (parse_zmm(clean, z)) { out.kind = OpKind::Zmm; out.zmm = z; return true; }
    if (parse_xmm(clean, x)) { out.kind = OpKind::Xmm; out.xmm = x; return true; }
    if (parse_mmx(clean, mm)) { out.kind = OpKind::Mmx; out.mmx = mm; return true; }
    if (clean.find('[') != std::string::npos || clean.substr(0,4) == "byte" ||
        clean.substr(0,4) == "word" || clean.substr(0,5) == "dword" || clean.substr(0,5) == "qword") {
        if (parse_mem(clean, m)) { out.kind = OpKind::Mem; out.mem = m; return true; }
    }
    if (parse_imm(clean, imm)) { out.kind = OpKind::Imm; out.imm = imm; return true; }
    return false;
}

std::vector<std::string> split_operands(const std::string& s) {
    std::vector<std::string> result;
    int depth = 0;
    size_t start = 0;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '[') depth++;
        else if (s[i] == ']') depth--;
        else if (s[i] == ',' && depth == 0) {
            result.push_back(trim(s.substr(start, i - start)));
            start = i + 1;
        }
    }
    result.push_back(trim(s.substr(start)));
    return result;
}

} // anonymous namespace

static std::optional<std::vector<uint8_t>> asm_a(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "aaa") {
        if (n == 0) { gen.aaa(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "aad") {
        if (n == 0) { gen.aad(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "aam") {
        if (n == 0) { gen.aam(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "aas") {
        if (n == 0) { gen.aas(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "adc") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.adc(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.adc(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.adc(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.adc(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.adc(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "add") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.add(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.add(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.add(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.add(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.add(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "addpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.addpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.addpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "addps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.addps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.addps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "addsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.addsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.addsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "addss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.addss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.addss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "addsubpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.addsubpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.addsubpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "addsubps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.addsubps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.addsubps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "adx") {
        if (n == 0) { gen.adx(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "amx") {
        if (n == 0) { gen.amx(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "and") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.and_(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.and_(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.and_(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.and_(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.and_(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "andnpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.andnpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.andnpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "andnps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.andnps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.andnps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "andpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.andpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.andpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "andps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.andps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.andps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_b(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "blendpd") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.blendpd(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.blendpd(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "blendps") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.blendps(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.blendps(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "blendvpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.blendvpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.blendvpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "blendvps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.blendvps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.blendvps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "bound") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.bound(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "bsf") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.bsf(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.bsf(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "bsr") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.bsr(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.bsr(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "bswap") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.bswap(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "bt") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.bt(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.bt(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.bt(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.bt(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "btc") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.btc(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.btc(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.btc(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.btc(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "btr") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.btr(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.btr(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.btr(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.btr(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "bts") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.bts(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.bts(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.bts(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.bts(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_ca(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "call") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.call(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.call(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "callf") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.callf(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.callf(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_cb(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "cbw") {
        if (n == 0) { gen.cbw(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_cd(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "cdq") {
        if (n == 0) { gen.cdq(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_cl(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "clc") {
        if (n == 0) { gen.clc(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cld") {
        if (n == 0) { gen.cld(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "clflush") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.clflush(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.clflush(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cli") {
        if (n == 0) { gen.cli(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "clts") {
        if (n == 0) { gen.clts(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_cm(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "cmc") {
        if (n == 0) { gen.cmc(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovb") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovb(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovb(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovbe") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovbe(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovbe(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovl") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovl(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovl(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovle") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovle(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovle(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovnb") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovnb(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovnb(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovnbe") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovnbe(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovnbe(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovnl") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovnl(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovnl(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovnle") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovnle(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovnle(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovno") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovno(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovno(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovnp") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovnp(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovnp(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovns") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovns(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovns(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovnz") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovnz(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovnz(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovo") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovo(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovo(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovp") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovp(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovp(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovs") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovs(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovs(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmovz") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmovz(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmovz(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmp") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmp(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cmp(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.cmp(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.cmp(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.cmp(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmppd") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.cmppd(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.cmppd(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmpps") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.cmpps(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.cmpps(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmps") {
        if (n == 0) { gen.cmps(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmpsd") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.cmpsd(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.cmpsd(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmpss") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.cmpss(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.cmpss(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmpxchg") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.cmpxchg(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.cmpxchg(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cmpxchg8b") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.cmpxchg8b(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.cmpxchg8b(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_co(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "comisd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.comisd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.comisd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "comiss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.comiss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.comiss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_cp(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "cpuid") {
        if (n == 0) { gen.cpuid(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_cr(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "crc32") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.crc32(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.crc32(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_cv(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "cvtdq2pd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvtdq2pd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtdq2pd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtdq2ps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvtdq2ps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtdq2ps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtpd2dq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvtpd2dq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtpd2dq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtpd2pi") {
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Xmm) { gen.cvtpd2pi(ops[0].mmx, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.cvtpd2pi(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtpd2ps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvtpd2ps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtpd2ps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtpi2pd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mmx) { gen.cvtpi2pd(ops[0].xmm, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtpi2pd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtpi2ps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mmx) { gen.cvtpi2ps(ops[0].xmm, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtpi2ps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtps2dq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvtps2dq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtps2dq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtps2pd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvtps2pd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtps2pd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtps2pi") {
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Xmm) { gen.cvtps2pi(ops[0].mmx, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.cvtps2pi(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtsd2si") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm) { gen.cvtsd2si(ops[0].reg, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cvtsd2si(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtsd2ss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvtsd2ss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtsd2ss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtsi2sd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Reg) { gen.cvtsi2sd(ops[0].xmm, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtsi2sd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtsi2ss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Reg) { gen.cvtsi2ss(ops[0].xmm, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtsi2ss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtss2sd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvtss2sd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvtss2sd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvtss2si") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm) { gen.cvtss2si(ops[0].reg, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cvtss2si(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvttpd2dq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvttpd2dq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvttpd2dq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvttpd2pi") {
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Xmm) { gen.cvttpd2pi(ops[0].mmx, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.cvttpd2pi(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvttps2dq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.cvttps2dq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.cvttps2dq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvttps2pi") {
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Xmm) { gen.cvttps2pi(ops[0].mmx, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.cvttps2pi(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvttsd2si") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm) { gen.cvttsd2si(ops[0].reg, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cvttsd2si(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cvttss2si") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm) { gen.cvttss2si(ops[0].reg, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.cvttss2si(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_cw(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "cwd") {
        if (n == 0) { gen.cwd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "cwde") {
        if (n == 0) { gen.cwde(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_d(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "daa") {
        if (n == 0) { gen.daa(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "das") {
        if (n == 0) { gen.das(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "dec") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.dec(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.dec(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "div") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.div(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.div(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "divpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.divpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.divpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "divps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.divps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.divps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "divsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.divsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.divsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "divss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.divss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.divss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "dppd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.dppd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.dppd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "dpps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.dpps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.dpps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_e(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "emms") {
        if (n == 0) { gen.emms(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "enter") {
        if (n == 0) { gen.enter(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "extractps") {
        if (n == 3 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.extractps(ops[0].reg, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_f2(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "f2xm1") {
        if (n == 0) { gen.f2xm1(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fa(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fabs") {
        if (n == 0) { gen.fabs(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fadd") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fadd(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fadd(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "faddp") {
        if (n == 0) { gen.faddp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fb(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fbld") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fbld(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fbld(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fbstp") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fbstp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fbstp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fc(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fchs") {
        if (n == 0) { gen.fchs(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fclex") {
        if (n == 0) { gen.fclex(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcmovb") {
        if (n == 0) { gen.fcmovb(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcmovbe") {
        if (n == 0) { gen.fcmovbe(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcmove") {
        if (n == 0) { gen.fcmove(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcmovnb") {
        if (n == 0) { gen.fcmovnb(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcmovnbe") {
        if (n == 0) { gen.fcmovnbe(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcmovne") {
        if (n == 0) { gen.fcmovne(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcmovnu") {
        if (n == 0) { gen.fcmovnu(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcmovu") {
        if (n == 0) { gen.fcmovu(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcom") {
        if (n == 0) { gen.fcom(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fcom(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fcom(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcom2") {
        if (n == 0) { gen.fcom2(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcomi") {
        if (n == 0) { gen.fcomi(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcomip") {
        if (n == 0) { gen.fcomip(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcomp") {
        if (n == 0) { gen.fcomp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fcomp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fcomp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcomp3") {
        if (n == 0) { gen.fcomp3(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcomp5") {
        if (n == 0) { gen.fcomp5(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcompp") {
        if (n == 0) { gen.fcompp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fcos") {
        if (n == 0) { gen.fcos(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fd(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fdecstp") {
        if (n == 0) { gen.fdecstp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fdisi") {
        if (n == 0) { gen.fdisi(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fdiv") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fdiv(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fdiv(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fdivp") {
        if (n == 0) { gen.fdivp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fdivr") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fdivr(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fdivr(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fdivrp") {
        if (n == 0) { gen.fdivrp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fe(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "feni") {
        if (n == 0) { gen.feni(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_ff(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "ffree") {
        if (n == 0) { gen.ffree(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ffreep") {
        if (n == 0) { gen.ffreep(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fi(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fiadd") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fiadd(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fiadd(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ficom") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.ficom(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.ficom(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ficomp") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.ficomp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.ficomp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fidiv") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fidiv(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fidiv(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fidivr") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fidivr(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fidivr(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fild") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fild(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fild(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fimul") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fimul(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fimul(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fincstp") {
        if (n == 0) { gen.fincstp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "finit") {
        if (n == 0) { gen.finit(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fist") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fist(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fist(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fistp") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fistp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fistp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fisttp") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fisttp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fisttp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fisub") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fisub(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fisub(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fisubr") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fisubr(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fisubr(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fl(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fld") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fld(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fld(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fld1") {
        if (n == 0) { gen.fld1(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fldcw") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fldcw(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fldcw(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fldenv") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fldenv(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fldenv(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fldl2e") {
        if (n == 0) { gen.fldl2e(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fldl2t") {
        if (n == 0) { gen.fldl2t(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fldlg2") {
        if (n == 0) { gen.fldlg2(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fldln2") {
        if (n == 0) { gen.fldln2(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fldpi") {
        if (n == 0) { gen.fldpi(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fldz") {
        if (n == 0) { gen.fldz(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fm(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fmul") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fmul(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fmul(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fmulp") {
        if (n == 0) { gen.fmulp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fn(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fnclex") {
        if (n == 0) { gen.fnclex(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fndisi") {
        if (n == 0) { gen.fndisi(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fneni") {
        if (n == 0) { gen.fneni(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fninit") {
        if (n == 0) { gen.fninit(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fnop") {
        if (n == 0) { gen.fnop(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fnsave") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fnsave(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fnsave(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fnsetpm") {
        if (n == 0) { gen.fnsetpm(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fnstcw") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fnstcw(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fnstcw(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fnstenv") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fnstenv(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fnstenv(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fnstsw") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fnstsw(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fnstsw(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fp(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fpatan") {
        if (n == 0) { gen.fpatan(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fprem") {
        if (n == 0) { gen.fprem(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fprem1") {
        if (n == 0) { gen.fprem1(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fptan") {
        if (n == 0) { gen.fptan(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fr(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "frndint") {
        if (n == 0) { gen.frndint(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "frstor") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.frstor(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.frstor(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fs(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fsave") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fsave(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fsave(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fscale") {
        if (n == 0) { gen.fscale(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fsetpm") {
        if (n == 0) { gen.fsetpm(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fsin") {
        if (n == 0) { gen.fsin(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fsincos") {
        if (n == 0) { gen.fsincos(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fsqrt") {
        if (n == 0) { gen.fsqrt(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fst") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fst(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fst(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fstcw") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fstcw(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fstcw(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fstenv") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fstenv(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fstenv(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fstp") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fstp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fstp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fstp1") {
        if (n == 0) { gen.fstp1(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fstp8") {
        if (n == 0) { gen.fstp8(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fstp9") {
        if (n == 0) { gen.fstp9(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fstsw") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fstsw(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fstsw(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fsub") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fsub(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fsub(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fsubp") {
        if (n == 0) { gen.fsubp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fsubr") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fsubr(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fsubr(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fsubrp") {
        if (n == 0) { gen.fsubrp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_ft(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "ftst") {
        if (n == 0) { gen.ftst(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fu(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fucom") {
        if (n == 0) { gen.fucom(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fucomi") {
        if (n == 0) { gen.fucomi(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fucomip") {
        if (n == 0) { gen.fucomip(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fucomp") {
        if (n == 0) { gen.fucomp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fucompp") {
        if (n == 0) { gen.fucompp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fx(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fxam") {
        if (n == 0) { gen.fxam(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fxch") {
        if (n == 0) { gen.fxch(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fxch4") {
        if (n == 0) { gen.fxch4(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fxch7") {
        if (n == 0) { gen.fxch7(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fxrstor") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fxrstor(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fxrstor(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fxsave") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.fxsave(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.fxsave(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fxtract") {
        if (n == 0) { gen.fxtract(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_fy(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "fyl2x") {
        if (n == 0) { gen.fyl2x(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "fyl2xp1") {
        if (n == 0) { gen.fyl2xp1(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_g(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "getsec") {
        if (n == 0) { gen.getsec(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_h(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "haddpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.haddpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.haddpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "haddps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.haddps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.haddps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "hint_nop") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.hint_nop(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.hint_nop(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "hlt") {
        if (n == 0) { gen.hlt(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "hsubpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.hsubpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.hsubpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "hsubps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.hsubps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.hsubps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_i(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "idiv") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.idiv(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.idiv(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "imul") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.imul(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.imul(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg && ops[2].kind == OpKind::Imm) { gen.imul(ops[0].reg, ops[1].reg, (int32_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.imul(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.imul(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "in") {
        if (n == 0) { gen.in(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "inc") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.inc(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.inc(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ins") {
        if (n == 0) { gen.ins(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "insertps") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.insertps(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "int") {
        if (n == 0) { gen.int_(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "int1") {
        if (n == 0) { gen.int1(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "into") {
        if (n == 0) { gen.into(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "invd") {
        if (n == 0) { gen.invd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "invept") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.invept(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "invlpg") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.invlpg(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.invlpg(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "invvpid") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.invvpid(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "iret") {
        if (n == 0) { gen.iret(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "iretd") {
        if (n == 0) { gen.iretd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_j(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "jb") {
    }
    else if (mnem == "jbe") {
    }
    else if (mnem == "jcxz") {
    }
    else if (mnem == "jecxz") {
    }
    else if (mnem == "jl") {
    }
    else if (mnem == "jle") {
    }
    else if (mnem == "jmp") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.jmp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.jmp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "jmpe") {
        if (n == 0) { gen.jmpe(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "jmpf") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.jmpf(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.jmpf(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "jnb") {
    }
    else if (mnem == "jnbe") {
    }
    else if (mnem == "jnl") {
    }
    else if (mnem == "jnle") {
    }
    else if (mnem == "jno") {
    }
    else if (mnem == "jnp") {
    }
    else if (mnem == "jns") {
    }
    else if (mnem == "jnz") {
    }
    else if (mnem == "jo") {
    }
    else if (mnem == "jp") {
    }
    else if (mnem == "js") {
    }
    else if (mnem == "jz") {
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_l(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "lahf") {
        if (n == 0) { gen.lahf(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lar") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.lar(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lddqu") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.lddqu(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ldmxcsr") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.ldmxcsr(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.ldmxcsr(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lds") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.lds(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lea") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.lea(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "leave") {
        if (n == 0) { gen.leave(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "les") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.les(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lfence") {
        if (n == 0) { gen.lfence(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lfs") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.lfs(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lgdt") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.lgdt(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.lgdt(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lgs") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.lgs(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lidt") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.lidt(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.lidt(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lldt") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.lldt(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.lldt(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lmsw") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.lmsw(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.lmsw(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lods") {
        if (n == 0) { gen.lods(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "loop") {
    }
    else if (mnem == "loopnz") {
    }
    else if (mnem == "loopz") {
    }
    else if (mnem == "lsl") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.lsl(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "lss") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.lss(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ltr") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.ltr(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.ltr(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_ma(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "maskmovdqu") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.maskmovdqu(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "maskmovq") {
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.maskmovq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "maxpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.maxpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.maxpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "maxps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.maxps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.maxps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "maxsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.maxsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.maxsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "maxss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.maxss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.maxss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_mf(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "mfence") {
        if (n == 0) { gen.mfence(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_mi(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "minpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.minpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.minpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "minps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.minps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.minps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "minsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.minsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.minsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "minss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.minss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.minss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_mo(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "monitor") {
        if (n == 0) { gen.monitor(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "mov") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.mov(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.mov(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.mov(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) {
            if (ops[0].reg.bits == 64) gen.mov(ops[0].reg, ops[1].imm);
            else gen.mov(ops[0].reg, (int32_t)ops[1].imm);
            return std::vector<uint8_t>(gen.data(), gen.data() + gen.size());
        }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.mov(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.mov(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movapd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movapd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movapd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movapd(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movaps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movaps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movaps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movaps(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movbe") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.movbe(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.movbe(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movd") {
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movd(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Reg) { gen.movd(ops[0].xmm, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movddup") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movddup(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movddup(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movdq2q") {
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Xmm) { gen.movdq2q(ops[0].mmx, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.movdq2q(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movdqa") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movdqa(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movdqa(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movdqa(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movdqu") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movdqu(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movdqu(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movdqu(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movhlps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movhlps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movhpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movhpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movhpd(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movhps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movhps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movhps(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movlhps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movlhps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movlpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movlpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movlpd(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movlps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movlps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movlps(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movmskpd") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm) { gen.movmskpd(ops[0].reg, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movmskps") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm) { gen.movmskps(ops[0].reg, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movntdq") {
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movntdq(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movntdqa") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movntdqa(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movnti") {
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.movnti(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movntpd") {
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movntpd(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movntps") {
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movntps(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movntq") {
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movntq(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movq(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.movq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.movq(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movq2dq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mmx) { gen.movq2dq(ops[0].xmm, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movq2dq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movs") {
        if (n == 0) { gen.movs(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movsd(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movshdup") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movshdup(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movshdup(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movsldup") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movsldup(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movsldup(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movss(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movsx") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.movsx(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.movsx(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movsxd") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.movsxd(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.movsxd(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movupd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movupd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movupd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movupd(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movups") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.movups(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.movups(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Xmm) { gen.movups(ops[0].mem, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "movzx") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.movzx(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.movzx(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_mp(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "mpsadbw") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.mpsadbw(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.mpsadbw(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_mu(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "mul") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.mul(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.mul(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "mulpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.mulpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.mulpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "mulps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.mulps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.mulps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "mulsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.mulsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.mulsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "mulss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.mulss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.mulss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_mw(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "mwait") {
        if (n == 0) { gen.mwait(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_n(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "neg") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.neg(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.neg(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "nop") {
        if (n == 0) { gen.nop(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.nop(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.nop(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "not") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.not_(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.not_(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_o(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "or") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.or_(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.or_(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.or_(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.or_(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.or_(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "orpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.orpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.orpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "orps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.orps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.orps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "out") {
        if (n == 0) { gen.out(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "outs") {
        if (n == 0) { gen.outs(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pa(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "pabsb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pabsb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pabsb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pabsb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pabsb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pabsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pabsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pabsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pabsd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pabsd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pabsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pabsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pabsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pabsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pabsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "packssdw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.packssdw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.packssdw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.packssdw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.packssdw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "packsswb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.packsswb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.packsswb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.packsswb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.packsswb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "packusdw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.packusdw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.packusdw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "packuswb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.packuswb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.packuswb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.packuswb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.packuswb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "paddb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.paddb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.paddb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.paddb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.paddb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "paddd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.paddd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.paddd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.paddd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.paddd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "paddq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.paddq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.paddq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.paddq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.paddq(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "paddsb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.paddsb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.paddsb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.paddsb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.paddsb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "paddsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.paddsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.paddsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.paddsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.paddsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "paddusb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.paddusb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.paddusb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.paddusb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.paddusb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "paddusw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.paddusw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.paddusw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.paddusw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.paddusw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "paddw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.paddw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.paddw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.paddw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.paddw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "palignr") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.palignr(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.palignr(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.palignr(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.palignr(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pand") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pand(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pand(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pand(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pand(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pandn") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pandn(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pandn(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pandn(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pandn(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pause") {
        if (n == 0) { gen.pause(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pavgb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pavgb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pavgb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pavgb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pavgb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pavgw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pavgw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pavgw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pavgw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pavgw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pb(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "pblendvb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pblendvb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pblendvb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pblendw") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pblendw(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.pblendw(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pc(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "pcmpeqb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pcmpeqb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pcmpeqb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pcmpeqb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pcmpeqb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpeqd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pcmpeqd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pcmpeqd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pcmpeqd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pcmpeqd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpeqq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pcmpeqq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pcmpeqq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpeqw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pcmpeqw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pcmpeqw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pcmpeqw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pcmpeqw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpestri") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pcmpestri(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.pcmpestri(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpestrm") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pcmpestrm(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.pcmpestrm(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpgtb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pcmpgtb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pcmpgtb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pcmpgtb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pcmpgtb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpgtd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pcmpgtd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pcmpgtd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pcmpgtd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pcmpgtd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpgtq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pcmpgtq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pcmpgtq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpgtw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pcmpgtw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pcmpgtw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pcmpgtw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pcmpgtw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpistri") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pcmpistri(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.pcmpistri(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pcmpistrm") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pcmpistrm(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.pcmpistrm(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pe(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "pextrb") {
        if (n == 3 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pextrb(ops[0].reg, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pextrd") {
        if (n == 3 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pextrd(ops[0].reg, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pextrw") {
        if (n == 3 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pextrw(ops[0].reg, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pextrw(ops[0].reg, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_ph(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "phaddd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.phaddd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.phaddd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.phaddd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.phaddd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "phaddsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.phaddsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.phaddsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.phaddsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.phaddsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "phaddw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.phaddw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.phaddw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.phaddw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.phaddw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "phminposuw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.phminposuw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.phminposuw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "phsubd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.phsubd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.phsubd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.phsubd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.phsubd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "phsubsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.phsubsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.phsubsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.phsubsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.phsubsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "phsubw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.phsubw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.phsubw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.phsubw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.phsubw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pi(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "pinsrb") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Reg && ops[2].kind == OpKind::Imm) { gen.pinsrb(ops[0].xmm, ops[1].reg, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pinsrd") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Reg && ops[2].kind == OpKind::Imm) { gen.pinsrd(ops[0].xmm, ops[1].reg, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pinsrw") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Reg && ops[2].kind == OpKind::Imm) { gen.pinsrw(ops[0].xmm, ops[1].reg, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pm(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "pmaddubsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmaddubsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmaddubsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmaddubsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmaddubsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmaddwd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmaddwd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmaddwd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmaddwd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmaddwd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmaxsb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmaxsb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmaxsb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmaxsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmaxsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmaxsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmaxsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmaxsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmaxsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmaxsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmaxsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmaxub") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmaxub(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmaxub(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmaxub(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmaxub(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmaxud") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmaxud(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmaxud(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmaxuw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmaxuw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmaxuw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pminsb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pminsb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pminsb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pminsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pminsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pminsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pminsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pminsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pminsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pminsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pminsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pminub") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pminub(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pminub(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pminub(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pminub(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pminud") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pminud(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pminud(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pminuw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pminuw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pminuw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovmskb") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Xmm) { gen.pmovmskb(ops[0].reg, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovsxbd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovsxbd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovsxbq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovsxbq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovsxbw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovsxbw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovsxdq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovsxdq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovsxwd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovsxwd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovsxwq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovsxwq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovzxbd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovzxbd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovzxbq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovzxbq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovzxbw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovzxbw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovzxdq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovzxdq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovzxwd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovzxwd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmovzxwq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmovzxwq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmuldq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmuldq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmuldq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmulhrsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmulhrsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmulhrsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmulhrsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmulhrsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmulhuw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmulhuw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmulhuw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmulhuw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmulhuw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmulhw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmulhw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmulhw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmulhw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmulhw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmulld") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmulld(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmulld(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmullw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmullw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmullw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmullw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmullw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pmuludq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pmuludq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pmuludq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pmuludq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pmuludq(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_po(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "pop") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.pop(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.pop(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "popa") {
        if (n == 0) { gen.popa(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "popad") {
        if (n == 0) { gen.popad(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "popcnt") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.popcnt(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.popcnt(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "popf") {
        if (n == 0) { gen.popf(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "popfd") {
        if (n == 0) { gen.popfd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "por") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.por(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.por(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.por(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.por(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pr(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "prefetchnta") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.prefetchnta(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.prefetchnta(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "prefetcht0") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.prefetcht0(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.prefetcht0(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "prefetcht1") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.prefetcht1(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.prefetcht1(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "prefetcht2") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.prefetcht2(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.prefetcht2(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_ps(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "psadbw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psadbw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psadbw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psadbw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psadbw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pshufb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pshufb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pshufb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pshufb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pshufb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pshufd") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pshufd(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.pshufd(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pshufhw") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pshufhw(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.pshufhw(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pshuflw") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.pshuflw(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.pshuflw(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pshufw") {
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pshufw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psignb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psignb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psignb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psignb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psignb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psignd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psignd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psignd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psignd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psignd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psignw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psignw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psignw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psignw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psignw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pslld") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pslld(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pslld(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pslld(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pslld(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.pslld(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pslldq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.pslldq(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psllq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psllq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psllq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psllq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psllq(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.psllq(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psllw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psllw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psllw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psllw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psllw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.psllw(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psrad") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psrad(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psrad(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psrad(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psrad(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.psrad(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psraw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psraw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psraw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psraw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psraw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.psraw(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psrld") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psrld(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psrld(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psrld(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psrld(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.psrld(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psrldq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.psrldq(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psrlq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psrlq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psrlq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psrlq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psrlq(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.psrlq(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psrlw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psrlw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psrlw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psrlw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psrlw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Imm) { gen.psrlw(ops[0].xmm, (uint8_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psubb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psubb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psubb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psubb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psubb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psubd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psubd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psubd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psubd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psubd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psubq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psubq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psubq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psubq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psubq(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psubsb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psubsb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psubsb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psubsb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psubsb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psubsw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psubsw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psubsw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psubsw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psubsw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psubusb") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psubusb(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psubusb(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psubusb(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psubusb(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psubusw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psubusw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psubusw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psubusw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psubusw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "psubw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.psubw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.psubw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.psubw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.psubw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pt(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "ptest") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.ptest(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.ptest(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_pu(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "punpckhbw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.punpckhbw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.punpckhbw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.punpckhbw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.punpckhbw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "punpckhdq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.punpckhdq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.punpckhdq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.punpckhdq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.punpckhdq(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "punpckhqdq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.punpckhqdq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.punpckhqdq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "punpckhwd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.punpckhwd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.punpckhwd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.punpckhwd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.punpckhwd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "punpcklbw") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.punpcklbw(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.punpcklbw(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.punpcklbw(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.punpcklbw(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "punpckldq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.punpckldq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.punpckldq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.punpckldq(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.punpckldq(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "punpcklqdq") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.punpcklqdq(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.punpcklqdq(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "punpcklwd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.punpcklwd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.punpcklwd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.punpcklwd(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.punpcklwd(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "push") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.push(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.push(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pusha") {
        if (n == 0) { gen.pusha(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pushad") {
        if (n == 0) { gen.pushad(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pushf") {
        if (n == 0) { gen.pushf(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "pushfd") {
        if (n == 0) { gen.pushfd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_px(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "pxor") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.pxor(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.pxor(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mmx) { gen.pxor(ops[0].mmx, ops[1].mmx); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mmx && ops[1].kind == OpKind::Mem) { gen.pxor(ops[0].mmx, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_r(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "rcl") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.rcl(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.rcl(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rcpps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.rcpps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.rcpps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rcpss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.rcpss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.rcpss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rcr") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.rcr(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.rcr(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rdmsr") {
        if (n == 0) { gen.rdmsr(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rdpmc") {
        if (n == 0) { gen.rdpmc(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rdtsc") {
        if (n == 0) { gen.rdtsc(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rdtscp") {
        if (n == 0) { gen.rdtscp(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "retf") {
        if (n == 0) { gen.retf(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "retn") {
        if (n == 0) { gen.retn(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rol") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.rol(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.rol(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ror") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.ror(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.ror(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "roundpd") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.roundpd(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.roundpd(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "roundps") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.roundps(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.roundps(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "roundsd") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.roundsd(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.roundsd(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "roundss") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.roundss(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.roundss(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rsm") {
        if (n == 0) { gen.rsm(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rsqrtps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.rsqrtps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.rsqrtps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "rsqrtss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.rsqrtss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.rsqrtss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sa(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "sahf") {
        if (n == 0) { gen.sahf(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sal") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.sal(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.sal(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "salc") {
        if (n == 0) { gen.salc(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sar") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.sar(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.sar(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sb(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "sbb") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.sbb(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.sbb(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.sbb(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.sbb(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.sbb(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sc(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "scas") {
        if (n == 0) { gen.scas(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_se(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "setb") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setb(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setb(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setbe") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setbe(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setbe(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setl") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setl(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setl(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setle") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setle(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setle(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setnb") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setnb(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setnb(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setnbe") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setnbe(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setnbe(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setnl") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setnl(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setnl(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setnle") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setnle(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setnle(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setno") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setno(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setno(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setnp") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setnp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setnp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setns") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setns(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setns(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setnz") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setnz(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setnz(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "seto") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.seto(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.seto(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setp") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setp(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sets") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.sets(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.sets(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "setz") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.setz(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.setz(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sf(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "sfence") {
        if (n == 0) { gen.sfence(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sg(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "sgdt") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.sgdt(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.sgdt(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sh(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "shl") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.shl(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.shl(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "shld") {
        if (n == 3 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg && ops[2].kind == OpKind::Imm) { gen.shld(ops[0].reg, ops[1].reg, (int32_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "shr") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.shr(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.shr(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "shrd") {
        if (n == 3 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg && ops[2].kind == OpKind::Imm) { gen.shrd(ops[0].reg, ops[1].reg, (int32_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "shufpd") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.shufpd(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.shufpd(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "shufps") {
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm && ops[2].kind == OpKind::Imm) { gen.shufps(ops[0].xmm, ops[1].xmm, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 3 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem && ops[2].kind == OpKind::Imm) { gen.shufps(ops[0].xmm, ops[1].mem, (uint8_t)ops[2].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_si(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "sidt") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.sidt(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.sidt(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sl(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "sldt") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.sldt(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.sldt(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sm(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "smsw") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.smsw(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.smsw(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sq(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "sqrtpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.sqrtpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.sqrtpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sqrtps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.sqrtps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.sqrtps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sqrtsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.sqrtsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.sqrtsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sqrtss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.sqrtss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.sqrtss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_st(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "stc") {
        if (n == 0) { gen.stc(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "std") {
        if (n == 0) { gen.std(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sti") {
        if (n == 0) { gen.sti(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "stmxcsr") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.stmxcsr(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.stmxcsr(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "stos") {
        if (n == 0) { gen.stos(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "str") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.str(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.str(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_su(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "sub") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.sub(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.sub(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.sub(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.sub(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.sub(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "subpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.subpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.subpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "subps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.subps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.subps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "subsd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.subsd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.subsd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "subss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.subss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.subss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sw(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "swapgs") {
        if (n == 0) { gen.swapgs(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_sy(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "syscall") {
        if (n == 0) { gen.syscall(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sysenter") {
        if (n == 0) { gen.sysenter(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sysexit") {
        if (n == 0) { gen.sysexit(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "sysret") {
        if (n == 0) { gen.sysret(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_t(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "test") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.test(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.test(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.test(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.test(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_u(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "ucomisd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.ucomisd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.ucomisd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ucomiss") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.ucomiss(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.ucomiss(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ud") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.ud(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.ud(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "ud2") {
        if (n == 0) { gen.ud2(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "unpckhpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.unpckhpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.unpckhpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "unpckhps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.unpckhps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.unpckhps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "unpcklpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.unpcklpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.unpcklpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "unpcklps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.unpcklps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.unpcklps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_v(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "verr") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.verr(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.verr(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "verw") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.verw(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.verw(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmcall") {
        if (n == 0) { gen.vmcall(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmclear") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.vmclear(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.vmclear(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmlaunch") {
        if (n == 0) { gen.vmlaunch(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmptrld") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.vmptrld(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.vmptrld(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmptrst") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.vmptrst(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.vmptrst(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmread") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.vmread(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.vmread(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmresume") {
        if (n == 0) { gen.vmresume(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmwrite") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.vmwrite(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.vmwrite(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmxoff") {
        if (n == 0) { gen.vmxoff(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "vmxon") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.vmxon(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.vmxon(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_w(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "wbinvd") {
        if (n == 0) { gen.wbinvd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "wrmsr") {
        if (n == 0) { gen.wrmsr(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

static std::optional<std::vector<uint8_t>> asm_x(const std::string& mnem, const std::vector<ParsedOp>& ops, size_t n, CodeGen& gen) {
    (void)ops; (void)n; (void)gen;
    if (mnem == "xadd") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.xadd(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.xadd(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xchg") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.xchg(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.xchg(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.xchg(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xgetbv") {
        if (n == 0) { gen.xgetbv(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xlat") {
        if (n == 0) { gen.xlat(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xor") {
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Reg) { gen.xor_(ops[0].reg, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Mem) { gen.xor_(ops[0].reg, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Reg) { gen.xor_(ops[0].mem, ops[1].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Reg && ops[1].kind == OpKind::Imm) { gen.xor_(ops[0].reg, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Mem && ops[1].kind == OpKind::Imm) { gen.xor_(ops[0].mem, (int32_t)ops[1].imm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xorpd") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.xorpd(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.xorpd(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xorps") {
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Xmm) { gen.xorps(ops[0].xmm, ops[1].xmm); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 2 && ops[0].kind == OpKind::Xmm && ops[1].kind == OpKind::Mem) { gen.xorps(ops[0].xmm, ops[1].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xrstor") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.xrstor(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.xrstor(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xsave") {
        if (n == 1 && ops[0].kind == OpKind::Reg) { gen.xsave(ops[0].reg); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (n == 1 && ops[0].kind == OpKind::Mem) { gen.xsave(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    else if (mnem == "xsetbv") {
        if (n == 0) { gen.xsetbv(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }
    return std::nullopt;
}

std::optional<std::vector<uint8_t>> assemble(const std::string& text) {
    std::string s = trim(text);
    { bool in_quote = false; for (auto& c : s) { if (c == '"') in_quote = !in_quote; else if (!in_quote) c = (char)std::tolower((unsigned char)c); } }
    if (s.empty()) return std::nullopt;

    size_t space = s.find_first_of(" \t");
    std::string mnem = (space == std::string::npos) ? s : s.substr(0, space);
    std::string rest = (space == std::string::npos) ? "" : trim(s.substr(space));
    if (mnem == "db" || mnem == "dw" || mnem == "dd" || mnem == "dq") {
        CodeGen gen;
        auto vals = split_operands(rest);
        for (auto& v : vals) {
            std::string vt = trim(v);
            if (vt.empty()) continue;
            if (vt.size() >= 2 && vt.front() == '"' && vt.back() == '"') {
                for (size_t ci = 1; ci < vt.size() - 1; ++ci) gen.db((uint8_t)vt[ci]);
                continue;
            }
            int64_t imm; if (!parse_imm(vt, imm)) return std::nullopt;
            if (mnem == "db") gen.db((uint8_t)imm);
            else if (mnem == "dw") gen.dw((uint16_t)imm);
            else if (mnem == "dd") gen.dd((uint32_t)imm);
            else gen.dq((uint64_t)imm);
        }
        if (gen.size() == 0) return std::nullopt;
        return std::vector<uint8_t>(gen.data(), gen.data() + gen.size());
    }

    if (mnem == "times") {
        size_t sp2 = rest.find_first_of(" \t");
        if (sp2 == std::string::npos) return std::nullopt;
        int64_t count; std::string cnt_s = trim(rest.substr(0, sp2));
        if (!parse_imm(cnt_s, count) || count < 0) return std::nullopt;
        std::string inner = trim(rest.substr(sp2));
        auto one = assemble(inner);
        if (!one) return std::nullopt;
        std::vector<uint8_t> result;
        for (int64_t i = 0; i < count; ++i) result.insert(result.end(), one->begin(), one->end());
        return result;
    }

    bool has_lock = false, has_rep = false, has_repe = false, has_repne = false;
    if (mnem == "lock" || mnem == "rep" || mnem == "repe" || mnem == "repz" || mnem == "repne" || mnem == "repnz") {
        if (mnem == "lock") has_lock = true;
        else if (mnem == "rep") has_rep = true;
        else if (mnem == "repe" || mnem == "repz") has_repe = true;
        else has_repne = true;
        if (rest.empty()) return std::nullopt;
        size_t sp2 = rest.find_first_of(" \t");
        mnem = (sp2 == std::string::npos) ? rest : rest.substr(0, sp2);
        rest = (sp2 == std::string::npos) ? "" : trim(rest.substr(sp2));
    }

    auto raw_ops = split_operands(rest);
    if (raw_ops.size() == 1 && raw_ops[0].empty()) raw_ops.clear();

    std::vector<ParsedOp> ops;
    for (auto& r : raw_ops) {
        ParsedOp p{};
        if (!parse_operand(r, p)) return std::nullopt;
        ops.push_back(p);
    }
    size_t n = ops.size();

    CodeGen gen;

    if (has_lock) gen.db(0xF0);
    if (has_rep) gen.db(0xF3);
    if (has_repe) gen.db(0xF3);
    if (has_repne) gen.db(0xF2);

    if (n == 0) {
        if (mnem == "ret") { gen.ret(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "int3") { gen.int3(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "nop") { gen.nop(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "cqo") { gen.cqo(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "cdqe") { gen.cdqe(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "pushfq") { gen.pushfq(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "popfq") { gen.popfq(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "movsb") { gen.movsb(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "movsw") { gen.movsw(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "movsq") { gen.movsq(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "stosb") { gen.stosb(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "stosw") { gen.stosw(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "stosd") { gen.stosd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "stosq") { gen.stosq(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "lodsb") { gen.lodsb(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "lodsw") { gen.lodsw(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "lodsd") { gen.lodsd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "lodsq") { gen.lodsq(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "cmpsb") { gen.cmpsb(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "cmpsw") { gen.cmpsw(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "cmpsq") { gen.cmpsq(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "scasb") { gen.scasb(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "scasw") { gen.scasw(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "scasd") { gen.scasd(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "scasq") { gen.scasq(); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }

    std::string resolved = mnem;
    if (mnem == "cmova") resolved = "cmovnbe";
    if (mnem == "cmovae") resolved = "cmovnb";
    if (mnem == "cmove") resolved = "cmovz";
    if (mnem == "cmovne") resolved = "cmovnz";
    if (mnem == "cmovg") resolved = "cmovnle";
    if (mnem == "cmovge") resolved = "cmovnl";
    if (mnem == "cmovna") resolved = "cmovbe";
    if (mnem == "cmovnae") resolved = "cmovb";
    if (mnem == "cmovng") resolved = "cmovle";
    if (mnem == "cmovnge") resolved = "cmovl";
    if (mnem == "cmovc") resolved = "cmovb";
    if (mnem == "cmovnc") resolved = "cmovnb";
    if (mnem == "ja") resolved = "jnbe";
    if (mnem == "jae") resolved = "jnb";
    if (mnem == "je") resolved = "jz";
    if (mnem == "jne") resolved = "jnz";
    if (mnem == "jg") resolved = "jnle";
    if (mnem == "jge") resolved = "jnl";
    if (mnem == "jna") resolved = "jbe";
    if (mnem == "jnae") resolved = "jb";
    if (mnem == "jng") resolved = "jle";
    if (mnem == "jnge") resolved = "jl";
    if (mnem == "jc") resolved = "jb";
    if (mnem == "jnc") resolved = "jnb";
    if (mnem == "seta") resolved = "setnbe";
    if (mnem == "setae") resolved = "setnb";
    if (mnem == "sete") resolved = "setz";
    if (mnem == "setne") resolved = "setnz";
    if (mnem == "setg") resolved = "setnle";
    if (mnem == "setge") resolved = "setnl";
    if (mnem == "setna") resolved = "setbe";
    if (mnem == "setnae") resolved = "setb";
    if (mnem == "setng") resolved = "setle";
    if (mnem == "setnge") resolved = "setl";
    if (mnem == "setc") resolved = "setb";
    if (mnem == "setnc") resolved = "setnb";
    mnem = resolved;

    if (n == 1 && ops[0].kind == OpKind::Imm) {
        int32_t rel = (int32_t)ops[0].imm;
        if (mnem == "jmp") { gen.db(0xE9); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "call") { gen.db(0xE8); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jo") { gen.db(0x0F); gen.db(0x80); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jno") { gen.db(0x0F); gen.db(0x81); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jb") { gen.db(0x0F); gen.db(0x82); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jnb") { gen.db(0x0F); gen.db(0x83); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jz") { gen.db(0x0F); gen.db(0x84); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jnz") { gen.db(0x0F); gen.db(0x85); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jbe") { gen.db(0x0F); gen.db(0x86); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jnbe") { gen.db(0x0F); gen.db(0x87); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "js") { gen.db(0x0F); gen.db(0x88); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jns") { gen.db(0x0F); gen.db(0x89); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jp") { gen.db(0x0F); gen.db(0x8A); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jnp") { gen.db(0x0F); gen.db(0x8B); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jl") { gen.db(0x0F); gen.db(0x8C); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jnl") { gen.db(0x0F); gen.db(0x8D); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jle") { gen.db(0x0F); gen.db(0x8E); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jnle") { gen.db(0x0F); gen.db(0x8F); gen.dd((uint32_t)rel); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }

    if (n == 1 && ops[0].kind == OpKind::Mem) {
        if (mnem == "call") { gen.call(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
        if (mnem == "jmp") { gen.jmp(ops[0].mem); return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); }
    }

    uint8_t mask_k = 0; bool zeroing = false;
    for (auto& op : ops) { if (op.mask_k) mask_k = op.mask_k; if (op.zeroing) zeroing = true; }
    if (mask_k > 0) {
        auto ok = [&]() { return std::vector<uint8_t>(gen.data(), gen.data() + gen.size()); };
        if (mnem == "vpxord" && n == 3 && ops[0].kind == OpKind::Zmm && ops[1].kind == OpKind::Zmm && ops[2].kind == OpKind::Zmm) {
            gen.vpxord_k(ops[0].zmm, ops[1].zmm, ops[2].zmm, mask_k, zeroing); return ok();
        }
        if (mnem == "vpxorq" && n == 3 && ops[0].kind == OpKind::Zmm && ops[1].kind == OpKind::Zmm && ops[2].kind == OpKind::Zmm) {
            gen.vpxorq_k(ops[0].zmm, ops[1].zmm, ops[2].zmm, mask_k, zeroing); return ok();
        }
        if (mnem == "vpandd" && n == 3 && ops[0].kind == OpKind::Zmm && ops[1].kind == OpKind::Zmm && ops[2].kind == OpKind::Zmm) {
            gen.vpandd_k(ops[0].zmm, ops[1].zmm, ops[2].zmm, mask_k, zeroing); return ok();
        }
        if (mnem == "vpandq" && n == 3 && ops[0].kind == OpKind::Zmm && ops[1].kind == OpKind::Zmm && ops[2].kind == OpKind::Zmm) {
            gen.vpandq_k(ops[0].zmm, ops[1].zmm, ops[2].zmm, mask_k, zeroing); return ok();
        }
        if (mnem == "vpord" && n == 3 && ops[0].kind == OpKind::Zmm && ops[1].kind == OpKind::Zmm && ops[2].kind == OpKind::Zmm) {
            gen.vpord_k(ops[0].zmm, ops[1].zmm, ops[2].zmm, mask_k, zeroing); return ok();
        }
        if (mnem == "vporq" && n == 3 && ops[0].kind == OpKind::Zmm && ops[1].kind == OpKind::Zmm && ops[2].kind == OpKind::Zmm) {
            gen.vporq_k(ops[0].zmm, ops[1].zmm, ops[2].zmm, mask_k, zeroing); return ok();
        }
        if (mnem == "vpternlogd" && n == 4 && ops[0].kind == OpKind::Zmm && ops[1].kind == OpKind::Zmm && ops[2].kind == OpKind::Zmm && ops[3].kind == OpKind::Imm) {
            gen.vpternlogd_k(ops[0].zmm, ops[1].zmm, ops[2].zmm, (uint8_t)ops[3].imm, mask_k, zeroing); return ok();
        }
    }

    if (mnem.empty()) return std::nullopt;
    std::optional<std::vector<uint8_t>> result;

    switch (mnem[0]) {
    case 'a': {
        result = asm_a(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'b': {
        result = asm_b(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'c': {
        if (mnem.size() >= 2 && mnem[1] == 'a') {
            result = asm_ca(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'b') {
            result = asm_cb(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'd') {
            result = asm_cd(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'l') {
            result = asm_cl(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'm') {
            result = asm_cm(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'o') {
            result = asm_co(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'p') {
            result = asm_cp(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'r') {
            result = asm_cr(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'v') {
            result = asm_cv(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'w') {
            result = asm_cw(mnem, ops, n, gen);
            if (result) return result;
        }
        break;
    }
    case 'd': {
        result = asm_d(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'e': {
        result = asm_e(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'f': {
        if (mnem.size() >= 2 && mnem[1] == '2') {
            result = asm_f2(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'a') {
            result = asm_fa(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'b') {
            result = asm_fb(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'c') {
            result = asm_fc(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'd') {
            result = asm_fd(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'e') {
            result = asm_fe(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'f') {
            result = asm_ff(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'i') {
            result = asm_fi(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'l') {
            result = asm_fl(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'm') {
            result = asm_fm(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'n') {
            result = asm_fn(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'p') {
            result = asm_fp(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'r') {
            result = asm_fr(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 's') {
            result = asm_fs(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 't') {
            result = asm_ft(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'u') {
            result = asm_fu(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'x') {
            result = asm_fx(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'y') {
            result = asm_fy(mnem, ops, n, gen);
            if (result) return result;
        }
        break;
    }
    case 'g': {
        result = asm_g(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'h': {
        result = asm_h(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'i': {
        result = asm_i(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'j': {
        result = asm_j(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'l': {
        result = asm_l(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'm': {
        if (mnem.size() >= 2 && mnem[1] == 'a') {
            result = asm_ma(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'f') {
            result = asm_mf(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'i') {
            result = asm_mi(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'o') {
            result = asm_mo(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'p') {
            result = asm_mp(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'u') {
            result = asm_mu(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'w') {
            result = asm_mw(mnem, ops, n, gen);
            if (result) return result;
        }
        break;
    }
    case 'n': {
        result = asm_n(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'o': {
        result = asm_o(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'p': {
        if (mnem.size() >= 2 && mnem[1] == 'a') {
            result = asm_pa(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'b') {
            result = asm_pb(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'c') {
            result = asm_pc(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'e') {
            result = asm_pe(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'h') {
            result = asm_ph(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'i') {
            result = asm_pi(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'm') {
            result = asm_pm(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'o') {
            result = asm_po(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'r') {
            result = asm_pr(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 's') {
            result = asm_ps(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 't') {
            result = asm_pt(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'u') {
            result = asm_pu(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'x') {
            result = asm_px(mnem, ops, n, gen);
            if (result) return result;
        }
        break;
    }
    case 'r': {
        result = asm_r(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 's': {
        if (mnem.size() >= 2 && mnem[1] == 'a') {
            result = asm_sa(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'b') {
            result = asm_sb(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'c') {
            result = asm_sc(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'e') {
            result = asm_se(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'f') {
            result = asm_sf(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'g') {
            result = asm_sg(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'h') {
            result = asm_sh(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'i') {
            result = asm_si(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'l') {
            result = asm_sl(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'm') {
            result = asm_sm(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'q') {
            result = asm_sq(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 't') {
            result = asm_st(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'u') {
            result = asm_su(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'w') {
            result = asm_sw(mnem, ops, n, gen);
            if (result) return result;
        }
        if (mnem.size() >= 2 && mnem[1] == 'y') {
            result = asm_sy(mnem, ops, n, gen);
            if (result) return result;
        }
        break;
    }
    case 't': {
        result = asm_t(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'u': {
        result = asm_u(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'v': {
        result = asm_v(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'w': {
        result = asm_w(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    case 'x': {
        result = asm_x(mnem, ops, n, gen);
        if (result) return result;
        break;
    }
    }
    return std::nullopt;
}

namespace {
struct Stmt { std::string text; bool is_label; std::string label_name; int align; };
std::vector<Stmt> parse_stmts(const std::string& text) {
    std::vector<Stmt> stmts;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        std::istringstream semi(line);
        std::string part;
        while (std::getline(semi, part, ';')) {
            std::string t = trim(part);
            if (t.empty()) continue;
            size_t c = t.find('#');
            if (c == std::string::npos) c = t.find("//");
            if (c != std::string::npos) t = trim(t.substr(0, c));
            if (t.empty()) continue;
            if (t.back() == ':') {
                std::string name = trim(t.substr(0, t.size() - 1));
                if (!name.empty()) { stmts.push_back({t, true, to_lower(name), 0}); continue; }
            }
            size_t colon = t.find(':');
            if (colon != std::string::npos && colon > 0 && t[colon-1] != ']') {
                std::string before = trim(t.substr(0, colon));
                bool valid_label = !before.empty() && before.find(' ') == std::string::npos && before.find('[') == std::string::npos;
                if (valid_label) {
                    stmts.push_back({t, true, to_lower(before), 0});
                    std::string after = trim(t.substr(colon + 1));
                    if (!after.empty()) stmts.push_back({after, false, "", 0});
                    continue;
                }
            }
            if (t.substr(0, 6) == "align " || t.substr(0, 6) == "ALIGN ") {
                int a = atoi(t.substr(6).c_str());
                if (a > 0) { stmts.push_back({t, false, "", a}); continue; }
            }
            stmts.push_back({t, false, "", 0});
        }
    }
    return stmts;
}
} // anon ns

std::optional<std::vector<uint8_t>> assemble(const std::string& text, std::string& error) {
    if (text.empty() || trim(text).empty()) { error = "empty input"; return std::nullopt; }
    std::string s = trim(text);
    { bool in_quote = false; for (auto& c : s) { if (c == '"') in_quote = !in_quote; else if (!in_quote) c = (char)std::tolower((unsigned char)c); } }
    size_t space = s.find_first_of(" \t");
    std::string mnem = (space == std::string::npos) ? s : s.substr(0, space);
    std::string rest = (space == std::string::npos) ? "" : trim(s.substr(space));
    auto result = assemble(text);
    if (result) return result;
    // Diagnose failure
    if (rest.empty()) {
        error = "unknown instruction: " + mnem;
        return std::nullopt;
    }
    auto raw_ops = split_operands(rest);
    if (raw_ops.size() == 1 && raw_ops[0].empty()) raw_ops.clear();
    for (size_t i = 0; i < raw_ops.size(); ++i) {
        ParsedOp p{};
        if (!parse_operand(raw_ops[i], p)) {
            error = "invalid operand " + std::to_string(i + 1) + ": " + raw_ops[i];
            return std::nullopt;
        }
    }
    error = "unsupported operand combination for: " + mnem;
    return std::nullopt;
}

std::optional<std::vector<uint8_t>> assemble_block(const std::string& text) {
    auto stmts = parse_stmts(text);
    if (stmts.empty()) return std::vector<uint8_t>{};

    std::unordered_map<std::string, std::string> equates;
    std::vector<Stmt> filtered;
    for (auto& s : stmts) {
        if (!s.is_label) {
            std::string low = s.text;
            size_t equ_pos = low.find(" equ ");
            if (equ_pos == std::string::npos) equ_pos = low.find(" EQU ");
            if (equ_pos != std::string::npos) {
                std::string name = trim(low.substr(0, equ_pos));
                std::string value = trim(low.substr(equ_pos + 5));
                if (!name.empty() && !value.empty()) { equates[name] = value; continue; }
            }
        }
        filtered.push_back(s);
    }
    if (!equates.empty()) {
        for (auto& s : filtered) {
            if (s.is_label) continue;
            for (auto& [name, value] : equates) {
                size_t pos = 0;
                while ((pos = s.text.find(name, pos)) != std::string::npos) {
                    bool before_ok = (pos == 0 || !isalnum(s.text[pos-1]) && s.text[pos-1] != '_');
                    bool after_ok = (pos + name.size() >= s.text.size() || !isalnum(s.text[pos+name.size()]) && s.text[pos+name.size()] != '_');
                    if (before_ok && after_ok) { s.text.replace(pos, name.size(), value); pos += value.size(); }
                    else pos += name.size();
                }
            }
        }
    }
    stmts = std::move(filtered);

    bool has_labels = false;
    for (auto& s : stmts) if (s.is_label) { has_labels = true; break; }

    if (!has_labels) {
        std::vector<uint8_t> result;
        for (auto& s : stmts) {
            if (s.align > 0) {
                while (result.size() % s.align != 0) result.push_back(0x90);
                continue;
            }
            auto bytes = assemble(s.text);
            if (!bytes) return std::nullopt;
            result.insert(result.end(), bytes->begin(), bytes->end());
        }
        return result;
    }

    std::unordered_map<std::string, int32_t> labels;
    std::vector<std::vector<uint8_t>> insn_bytes;
    std::vector<int> insn_idx;
    std::vector<bool> needs_resolve;

    int32_t offset = 0;
    for (size_t i = 0; i < stmts.size(); ++i) {
        if (stmts[i].is_label) {
            labels[stmts[i].label_name] = offset;
            insn_idx.push_back(-1);
            needs_resolve.push_back(false);
            continue;
        }
        if (stmts[i].align > 0) {
            std::vector<uint8_t> pad;
            while ((offset + (int32_t)pad.size()) % stmts[i].align != 0) pad.push_back(0x90);
            insn_idx.push_back((int)insn_bytes.size());
            needs_resolve.push_back(false);
            offset += (int32_t)pad.size();
            insn_bytes.push_back(std::move(pad));
            continue;
        }
        auto bytes = assemble(stmts[i].text);
        if (bytes) {
            insn_idx.push_back((int)insn_bytes.size());
            needs_resolve.push_back(false);
            offset += (int32_t)bytes->size();
            insn_bytes.push_back(std::move(*bytes));
        } else {
            std::string low = to_lower(stmts[i].text);
            size_t spc = low.find_first_of(" \t");
            if (spc == std::string::npos) return std::nullopt;
            std::string mnem_s = trim(low.substr(0, spc));
            std::string placeholder = mnem_s + " 0";
            auto pbytes = assemble(placeholder);
            if (!pbytes) return std::nullopt;
            insn_idx.push_back((int)insn_bytes.size());
            needs_resolve.push_back(true);
            offset += (int32_t)pbytes->size();
            insn_bytes.push_back(std::move(*pbytes));
        }
    }

    offset = 0;
    for (size_t i = 0; i < stmts.size(); ++i) {
        if (stmts[i].is_label) continue;
        int idx = insn_idx[i];
        if (needs_resolve[i]) {
            std::string low = to_lower(stmts[i].text);
            size_t spc = low.find_first_of(" \t");
            std::string operand = trim(low.substr(spc));
            auto it = labels.find(operand);
            if (it != labels.end()) {
                int32_t insn_end = offset + (int32_t)insn_bytes[idx].size();
                int32_t rel = it->second - insn_end;
                std::string mnem_s = trim(low.substr(0, spc));
                std::string resolved = mnem_s + " " + std::to_string(rel);
                auto bytes = assemble(resolved);
                if (bytes) insn_bytes[idx] = std::move(*bytes);
            }
        }
        offset += (int32_t)insn_bytes[idx].size();
    }

    std::vector<uint8_t> result;
    for (auto& b : insn_bytes) result.insert(result.end(), b.begin(), b.end());
    return result;
}

std::optional<std::vector<uint8_t>> assemble_block(const std::string& text, std::string& error) {
    auto stmts = parse_stmts(text);
    if (stmts.empty()) return std::vector<uint8_t>{};
    int line = 0;
    for (auto& s : stmts) {
        line++;
        if (s.is_label) continue;
        std::string err;
        auto bytes = assemble(s.text, err);
        if (!bytes) {
            // Check if it might be a label reference (will be resolved in 2-pass)
            std::string low = to_lower(s.text);
            size_t spc = low.find_first_of(" \t");
            if (spc != std::string::npos) {
                std::string m = trim(low.substr(0, spc));
                std::string placeholder = m + " 0";
                if (assemble(placeholder)) continue; // likely a label ref
            }
            error = "line " + std::to_string(line) + ": " + err;
            return std::nullopt;
        }
    }
    // If validation passed, do the actual assembly
    return assemble_block(text);
}

} // namespace vedx64
#endif // VEDX64_ASSEMBLER
