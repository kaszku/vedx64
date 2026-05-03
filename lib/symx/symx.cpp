// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/symx.hpp"
#include "vedx64/analysis.hpp"
#include <cstdio>
#include <sstream>
#include <cinttypes>

namespace vedx64::symx {

// ============================================================
//  Builder
// ============================================================

Builder::Builder() {
    arena_.reserve(1024);
    intern_.reserve(1024);
}

const Expr* Builder::intern(ExprOp op, uint16_t w, uint64_t k,
                            const Expr* a, const Expr* b, const Expr* c) {
    Key key{op, w, k, a, b, c};
    auto it = intern_.find(key);
    if (it != intern_.end()) return it->second;
    auto e = std::make_unique<Expr>();
    e->op = op; e->width = w; e->k = k; e->a = a; e->b = b; e->c = c;
    const Expr* p = e.get();
    arena_.push_back(std::move(e));
    intern_.emplace(key, p);
    return p;
}

int64_t Builder::sext_to_i64(uint64_t v, uint16_t width) {
    if (width >= 64) return static_cast<int64_t>(v);
    uint64_t sign = 1ULL << (width - 1);
    uint64_t m = mask_to(width);
    v &= m;
    return static_cast<int64_t>((v ^ sign) - sign);
}

const Expr* Builder::k(uint64_t v, uint16_t width) {
    return intern(ExprOp::ConstU, width, v & mask_to(width), nullptr, nullptr);
}

const Expr* Builder::sym(uint16_t width) {
    return intern(ExprOp::Symbol, width, next_symbol_++, nullptr, nullptr);
}

const Expr* Builder::sym_named(const char* /*name*/, uint16_t width) {
    // For now names are only used for to_string formatting; we identify by id.
    return sym(width);
}

// Constant folding helpers.
#define FOLD2(op_macro, expr)                                                         \
    if (is_const(x) && is_const(y)) {                                                  \
        return k((expr) & mask_to(x->width), x->width);                                \
    }

const Expr* Builder::add(const Expr* x, const Expr* y) {
    FOLD2(Add, x->k + y->k)
    if (is_const(y) && y->k == 0) return x;
    if (is_const(x) && x->k == 0) return y;
    return intern(ExprOp::Add, x->width, 0, x, y);
}
const Expr* Builder::sub(const Expr* x, const Expr* y) {
    FOLD2(Sub, x->k - y->k)
    if (is_const(y) && y->k == 0) return x;
    if (x == y) return k(0, x->width);
    return intern(ExprOp::Sub, x->width, 0, x, y);
}
const Expr* Builder::mul(const Expr* x, const Expr* y) {
    FOLD2(Mul, x->k * y->k)
    if (is_const(y) && y->k == 0) return k(0, x->width);
    if (is_const(x) && x->k == 0) return k(0, x->width);
    if (is_const(y) && y->k == 1) return x;
    if (is_const(x) && x->k == 1) return y;
    return intern(ExprOp::Mul, x->width, 0, x, y);
}
const Expr* Builder::udiv(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y) && y->k != 0) return k(x->k / y->k, x->width);
    return intern(ExprOp::UDiv, x->width, 0, x, y);
}
const Expr* Builder::sdiv(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y) && y->k != 0) {
        int64_t a = sext_to_i64(x->k, x->width);
        int64_t b = sext_to_i64(y->k, x->width);
        return k(static_cast<uint64_t>(a / b) & mask_to(x->width), x->width);
    }
    return intern(ExprOp::SDiv, x->width, 0, x, y);
}
const Expr* Builder::umod(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y) && y->k != 0) return k(x->k % y->k, x->width);
    return intern(ExprOp::UMod, x->width, 0, x, y);
}
const Expr* Builder::smod(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y) && y->k != 0) {
        int64_t a = sext_to_i64(x->k, x->width);
        int64_t b = sext_to_i64(y->k, x->width);
        return k(static_cast<uint64_t>(a % b) & mask_to(x->width), x->width);
    }
    return intern(ExprOp::SMod, x->width, 0, x, y);
}
const Expr* Builder::band(const Expr* x, const Expr* y) {
    FOLD2(And, x->k & y->k)
    if (is_const(y) && y->k == 0) return k(0, x->width);
    if (is_const(x) && x->k == 0) return k(0, x->width);
    if (is_const(y) && (y->k & mask_to(x->width)) == mask_to(x->width)) return x;
    if (is_const(x) && (x->k & mask_to(y->width)) == mask_to(y->width)) return y;
    if (x == y) return x;
    return intern(ExprOp::And, x->width, 0, x, y);
}
const Expr* Builder::bor(const Expr* x, const Expr* y) {
    FOLD2(Or, x->k | y->k)
    if (is_const(y) && y->k == 0) return x;
    if (is_const(x) && x->k == 0) return y;
    if (x == y) return x;
    return intern(ExprOp::Or, x->width, 0, x, y);
}
const Expr* Builder::bxor(const Expr* x, const Expr* y) {
    FOLD2(Xor, x->k ^ y->k)
    if (is_const(y) && y->k == 0) return x;
    if (is_const(x) && x->k == 0) return y;
    if (x == y) return k(0, x->width);
    return intern(ExprOp::Xor, x->width, 0, x, y);
}
const Expr* Builder::bnot(const Expr* x) {
    if (is_const(x)) return k((~x->k) & mask_to(x->width), x->width);
    if (x->op == ExprOp::Not) return x->a;   // ~~x → x
    return intern(ExprOp::Not, x->width, 0, x, nullptr);
}
const Expr* Builder::neg(const Expr* x) {
    if (is_const(x)) return k((~x->k + 1) & mask_to(x->width), x->width);
    return intern(ExprOp::Neg, x->width, 0, x, nullptr);
}
const Expr* Builder::shl(const Expr* x, const Expr* n) {
    if (is_const(n) && n->k == 0) return x;
    if (is_const(x) && is_const(n)) return k((x->k << (n->k & 63)) & mask_to(x->width), x->width);
    return intern(ExprOp::Shl, x->width, 0, x, n);
}
const Expr* Builder::lshr(const Expr* x, const Expr* n) {
    if (is_const(n) && n->k == 0) return x;
    if (is_const(x) && is_const(n)) return k((x->k & mask_to(x->width)) >> (n->k & 63), x->width);
    return intern(ExprOp::LShr, x->width, 0, x, n);
}
const Expr* Builder::ashr(const Expr* x, const Expr* n) {
    if (is_const(n) && n->k == 0) return x;
    if (is_const(x) && is_const(n)) {
        int64_t s = sext_to_i64(x->k, x->width);
        return k(static_cast<uint64_t>(s >> (n->k & 63)) & mask_to(x->width), x->width);
    }
    return intern(ExprOp::AShr, x->width, 0, x, n);
}

const Expr* Builder::zext(const Expr* x, uint16_t to) {
    if (to == x->width) return x;
    if (is_const(x)) return k(x->k & mask_to(to), to);
    if (to < x->width) return trunc(x, to);            // narrowing → trunc
    if (x->op == ExprOp::Zext) return zext(x->a, to);  // zext stacking
    return intern(ExprOp::Zext, to, 0, x, nullptr);
}
const Expr* Builder::sext(const Expr* x, uint16_t to) {
    if (to == x->width) return x;
    if (is_const(x)) {
        int64_t s = sext_to_i64(x->k, x->width);
        return k(static_cast<uint64_t>(s) & mask_to(to), to);
    }
    return intern(ExprOp::Sext, to, 0, x, nullptr);
}
const Expr* Builder::trunc(const Expr* x, uint16_t to) {
    if (to == x->width) return x;
    if (is_const(x)) return k(x->k & mask_to(to), to);
    if (to > x->width) return zext(x, to);                                 // widening → zext
    // trunc(zext(y)) — three cases:
    //   to == y.width   → y
    //   to >  y.width   → zext(y, to)
    //   to <  y.width   → trunc(y, to)
    if (x->op == ExprOp::Zext) {
        uint16_t yw = x->a->width;
        if (to == yw) return x->a;
        if (to >  yw) return zext (x->a, to);
        if (to <  yw) return trunc(x->a, to);
    }
    // trunc(concat(h, l)) — entirely in the low part?
    if (x->op == ExprOp::Concat && to <= x->b->width) return trunc(x->b, to);
    // trunc(extract(hi, lo, y)) — re-express as a tighter extract.
    if (x->op == ExprOp::Extract) {
        uint16_t inner_lo = static_cast<uint16_t>(x->k & 0xFF);
        return extract(x->a, static_cast<uint16_t>(inner_lo + to - 1), inner_lo);
    }
    return intern(ExprOp::Trunc, to, 0, x, nullptr);
}
const Expr* Builder::extract(const Expr* x, uint16_t hi, uint16_t lo) {
    uint16_t w = static_cast<uint16_t>(hi - lo + 1);
    if (lo == 0 && hi + 1 == x->width) return x;
    if (is_const(x)) return k((x->k >> lo) & mask_to(w), w);
    // Nested extract: extract(hi2,lo2, extract(hi1,lo1, y)) → extract(hi1+hi2, hi1+lo2... wait
    //   inner extract returns the slice [hi1..lo1] of y as a new fresh-width value;
    //   the outer extract picks bits [hi2..lo2] of THAT — i.e. bits [lo1+hi2..lo1+lo2] of y.
    if (x->op == ExprOp::Extract) {
        uint16_t inner_lo = static_cast<uint16_t>(x->k & 0xFF);
        return extract(x->a, static_cast<uint16_t>(inner_lo + hi),
                              static_cast<uint16_t>(inner_lo + lo));
    }
    // extract(hi, lo, concat(h, l)) — split based on which side the range falls.
    if (x->op == ExprOp::Concat) {
        uint16_t lo_w = x->b->width;
        if (hi < lo_w) return extract(x->b, hi, lo);                          // entirely in low part
        if (lo >= lo_w) return extract(x->a, static_cast<uint16_t>(hi - lo_w),
                                                static_cast<uint16_t>(lo - lo_w)); // entirely in high
        // Spans both — split into two extracts then re-concat.
        const Expr* hi_part = extract(x->a, static_cast<uint16_t>(hi - lo_w), 0);
        const Expr* lo_part = extract(x->b, static_cast<uint16_t>(lo_w - 1), lo);
        return concat(hi_part, lo_part);
    }
    // extract(hi, lo, zext(to, y)):
    //   bits below y->width come from y, bits above are zero.
    if (x->op == ExprOp::Zext) {
        uint16_t inner_w = x->a->width;
        if (hi < inner_w) return extract(x->a, hi, lo);                       // entirely in y
        if (lo >= inner_w) return k(0, w);                                    // entirely in zero pad
        // Spans the boundary — pad zeros on top of an inner extract.
        const Expr* low_slice = extract(x->a, static_cast<uint16_t>(inner_w - 1), lo);
        return zext(low_slice, w);
    }
    // extract(hi, lo, sext(to, y)) when slice lies entirely below the sign extension.
    if (x->op == ExprOp::Sext && hi < x->a->width) return extract(x->a, hi, lo);
    return intern(ExprOp::Extract, w, ((uint64_t)hi << 8) | lo, x, nullptr);
}

const Expr* Builder::concat(const Expr* hi, const Expr* lo) {
    uint16_t w = static_cast<uint16_t>(hi->width + lo->width);
    if (is_const(hi) && is_const(lo)) return k((hi->k << lo->width) | (lo->k & mask_to(lo->width)), w);
    // concat(#0, x) where the high part is constant zero of any width → zext(x, w).
    if (is_const(hi) && hi->k == 0) return zext(lo, w);
    // concat(extract(a, h1, m1), extract(b, m1-1, l1)) where a == b and the
    // ranges abut → a single extract(a, h1, l1).
    if (hi->op == ExprOp::Extract && lo->op == ExprOp::Extract && hi->a == lo->a) {
        uint16_t hi_lo = static_cast<uint16_t>(hi->k & 0xFF);
        uint16_t lo_hi = static_cast<uint16_t>((lo->k >> 8) & 0xFF);
        uint16_t lo_lo = static_cast<uint16_t>(lo->k & 0xFF);
        uint16_t hi_hi = static_cast<uint16_t>((hi->k >> 8) & 0xFF);
        if (hi_lo == lo_hi + 1) return extract(hi->a, hi_hi, lo_lo);
    }
    return intern(ExprOp::Concat, w, 0, hi, lo);
}

const Expr* Builder::eq(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y)) return k(x->k == y->k ? 1 : 0, 1);
    if (x == y) return k(1, 1);
    return intern(ExprOp::Eq, 1, 0, x, y);
}
const Expr* Builder::ne(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y)) return k(x->k != y->k ? 1 : 0, 1);
    if (x == y) return k(0, 1);
    return intern(ExprOp::Ne, 1, 0, x, y);
}
const Expr* Builder::ult(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y)) return k(x->k < y->k ? 1 : 0, 1);
    return intern(ExprOp::Ult, 1, 0, x, y);
}
const Expr* Builder::slt(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y))
        return k(sext_to_i64(x->k, x->width) < sext_to_i64(y->k, y->width) ? 1 : 0, 1);
    return intern(ExprOp::Slt, 1, 0, x, y);
}
const Expr* Builder::ule(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y)) return k(x->k <= y->k ? 1 : 0, 1);
    return intern(ExprOp::Ule, 1, 0, x, y);
}
const Expr* Builder::sle(const Expr* x, const Expr* y) {
    if (is_const(x) && is_const(y))
        return k(sext_to_i64(x->k, x->width) <= sext_to_i64(y->k, y->width) ? 1 : 0, 1);
    return intern(ExprOp::Sle, 1, 0, x, y);
}

const Expr* Builder::ite(const Expr* c, const Expr* t, const Expr* f) {
    if (is_const(c)) return c->k ? t : f;
    if (t == f) return t;
    return intern(ExprOp::Ite, t->width, 0, c, t, f);
}

// Pretty-printer — kept compact; for full S-expression dumps see to_string.
static void format_expr(std::ostringstream& os, const Expr* e) {
    if (!e) { os << "<null>"; return; }
    switch (e->op) {
    case ExprOp::ConstU: os << "#" << e->k << ":" << e->width; return;
    case ExprOp::Symbol: os << "sym" << e->k << ":" << e->width; return;
    case ExprOp::Add:    os << "(+ ";  format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Sub:    os << "(- ";  format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Mul:    os << "(* ";  format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::And:    os << "(& ";  format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Or:     os << "(| ";  format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Xor:    os << "(^ ";  format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Not:    os << "(~ ";  format_expr(os, e->a); os << ")"; return;
    case ExprOp::Neg:    os << "(neg "; format_expr(os, e->a); os << ")"; return;
    case ExprOp::Shl:    os << "(<< "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::LShr:   os << "(>> "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::AShr:   os << "(>>> "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Zext:   os << "(zext " << e->width << " "; format_expr(os, e->a); os << ")"; return;
    case ExprOp::Sext:   os << "(sext " << e->width << " "; format_expr(os, e->a); os << ")"; return;
    case ExprOp::Trunc:  os << "(trunc " << e->width << " "; format_expr(os, e->a); os << ")"; return;
    case ExprOp::Extract:os << "(extract " << ((e->k >> 8) & 0xFF) << " " << (e->k & 0xFF) << " "; format_expr(os, e->a); os << ")"; return;
    case ExprOp::Concat: os << "(concat "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Eq:     os << "(= ";  format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Ne:     os << "(!= "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Ult:    os << "(u< "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Slt:    os << "(s< "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Ule:    os << "(u<= "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Sle:    os << "(s<= "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << ")"; return;
    case ExprOp::Ite:    os << "(ite "; format_expr(os, e->a); os << " "; format_expr(os, e->b); os << " "; format_expr(os, e->c); os << ")"; return;
    default: os << "<op " << (int)e->op << ">"; return;
    }
}

std::string Builder::to_string(const Expr* e) {
    std::ostringstream os;
    format_expr(os, e);
    return os.str();
}

// ============================================================
//  Memory
// ============================================================

void Memory::clear() { concrete_.clear(); symbolic_.clear(); }

void Memory::seed(uint64_t addr, const uint8_t* data, size_t n) {
    for (size_t i = 0; i < n; ++i) concrete_[addr + i] = b_->k(data[i], 8);
}

const Expr* Memory::load_byte(const Expr* addr_byte) {
    if (is_const(addr_byte)) {
        uint64_t a = addr_byte->k;
        // Walk symbolic writes in reverse — last write wins on alias.
        for (auto it = symbolic_.rbegin(); it != symbolic_.rend(); ++it) {
            if (is_const(it->addr) && it->addr->k == a) return it->val;
        }
        auto cit = concrete_.find(a);
        if (cit != concrete_.end()) return cit->second;
        // Unmapped byte → fresh symbol (treats memory as initially symbolic).
        const Expr* fresh = b_->sym(8);
        concrete_[a] = fresh;
        return fresh;
    }
    // Symbolic address — fold over the symbolic write list as an Ite chain.
    const Expr* cur = b_->sym(8);  // unknown background
    for (auto& w : symbolic_) {
        const Expr* hit = b_->eq(addr_byte, w.addr);
        cur = b_->ite(hit, w.val, cur);
    }
    // Also consider concrete writes — but only the recent ones; arbitrary cap.
    size_t taken = 0;
    for (auto it = concrete_.rbegin(); it != concrete_.rend() && taken < 32; ++it, ++taken) {
        const Expr* a = b_->k(it->first, addr_byte->width);
        const Expr* hit = b_->eq(addr_byte, a);
        cur = b_->ite(hit, it->second, cur);
    }
    return cur;
}

void Memory::store_byte(const Expr* addr_byte, const Expr* val_byte) {
    if (is_const(addr_byte)) {
        concrete_[addr_byte->k] = val_byte;
        return;
    }
    symbolic_.push_back({addr_byte, val_byte});
}

const Expr* Memory::load(const Expr* addr, uint8_t bytes) {
    // Little-endian: byte 0 at lowest address.
    const Expr* result = nullptr;
    for (uint8_t i = 0; i < bytes; ++i) {
        const Expr* off = b_->k(i, addr->width);
        const Expr* b   = load_byte(b_->add(addr, off));
        const Expr* widened = b_->zext(b, bytes * 8);
        const Expr* shifted = b_->shl(widened, b_->k(i * 8, bytes * 8));
        result = result ? b_->bor(result, shifted) : shifted;
    }
    return result;
}

void Memory::store(const Expr* addr, const Expr* val, uint8_t bytes) {
    for (uint8_t i = 0; i < bytes; ++i) {
        const Expr* off = b_->k(i, addr->width);
        const Expr* a   = b_->add(addr, off);
        const Expr* shifted = b_->lshr(val, b_->k(i * 8, val->width));
        const Expr* byte    = b_->trunc(shifted, 8);
        store_byte(a, byte);
    }
}

// ============================================================
//  State
// ============================================================

void State::set_gpr(uint8_t r, const Expr* e) {
    if (r >= 16) return;
    gpr[r] = e->width == 64 ? e : b->zext(e, 64);
}

void State::set_gpr_low(uint8_t r, uint8_t bytes, const Expr* e) {
    if (r >= 16) return;
    if (bytes == 8) { gpr[r] = e; return; }
    if (bytes == 4) { gpr[r] = b->zext(b->trunc(e, 32), 64); return; }
    // 16-bit / 8-bit: preserve upper bits of RAX, write low.
    uint16_t bits = bytes * 8;
    const Expr* low  = b->trunc(e, bits);
    const Expr* high = b->extract(gpr[r], 63, bits);
    gpr[r] = b->concat(high, low);
}

const Expr* State::get_gpr(uint8_t r) const {
    return gpr[r];
}

const Expr* State::get_gpr_slice(uint8_t r, uint8_t bytes) const {
    if (bytes >= 8) return gpr[r];
    return b->extract(gpr[r], bytes * 8 - 1, 0);
}

State State::fork() const {
    State copy;
    copy.b = b;
    for (int i = 0; i < 16; ++i) copy.gpr[i]   = gpr[i];
    for (int i = 0; i < 7;  ++i) copy.flags[i] = flags[i];
    copy.mem        = mem;          // shared — caller should snapshot if needed
    copy.rip        = rip;
    copy.pc         = pc;
    copy.fork_depth = fork_depth + 1;
    return copy;
}

// ============================================================
//  Solver — concrete fast-path + (optional) Z3-backed reasoning
// ============================================================

#ifdef VEDX64_Z3
#include <z3++.h>
#include <cstdio>
#endif

// Always provide a complete Solver::Impl definition so unique_ptr<Impl>'s
// destructor can be instantiated. The Z3 build adds real fields; the stub
// build keeps it empty.
#ifdef VEDX64_Z3
struct Solver::Impl {
    z3::context ctx;
    z3::solver  core;
    // Cache of Expr* → translated z3::expr. We store the index into `terms`
    // because z3::expr's lifetime is tied to the context; copying is cheap
    // (refcounted handle) but the unordered_map<expr> dance is annoying.
    std::unordered_map<const Expr*, unsigned> cache;
    std::vector<z3::expr>                     terms;
    Impl() : core(ctx) {}
};
#else
struct Solver::Impl {};
#endif

#ifdef VEDX64_Z3
static z3::expr translate(Solver::Impl& s, const Expr* e) {
    auto it = s.cache.find(e);
    if (it != s.cache.end()) return s.terms[it->second];
    z3::expr r(s.ctx);
    auto bv1_true  = [&] { return s.ctx.bv_val(1u, 1u); };
    auto bv1_false = [&] { return s.ctx.bv_val(0u, 1u); };
    auto pred_to_bv = [&](z3::expr p) { return z3::ite(p, bv1_true(), bv1_false()); };

    switch (e->op) {
    case ExprOp::ConstU: r = s.ctx.bv_val(e->k, e->width); break;
    case ExprOp::Symbol: {
        char name[32];
        std::snprintf(name, sizeof name, "sym%llu", (unsigned long long)e->k);
        r = s.ctx.bv_const(name, e->width);
        break;
    }
    case ExprOp::Add:  r = translate(s, e->a) + translate(s, e->b); break;
    case ExprOp::Sub:  r = translate(s, e->a) - translate(s, e->b); break;
    case ExprOp::Mul:  r = translate(s, e->a) * translate(s, e->b); break;
    case ExprOp::UDiv: r = z3::udiv(translate(s, e->a), translate(s, e->b)); break;
    case ExprOp::SDiv: r = translate(s, e->a) / translate(s, e->b); break;
    case ExprOp::UMod: r = z3::urem(translate(s, e->a), translate(s, e->b)); break;
    case ExprOp::SMod: r = z3::srem(translate(s, e->a), translate(s, e->b)); break;
    case ExprOp::And:  r = translate(s, e->a) & translate(s, e->b); break;
    case ExprOp::Or:   r = translate(s, e->a) | translate(s, e->b); break;
    case ExprOp::Xor:  r = translate(s, e->a) ^ translate(s, e->b); break;
    case ExprOp::Not:  r = ~translate(s, e->a); break;
    case ExprOp::Neg:  r = -translate(s, e->a); break;
    case ExprOp::Shl:  r = z3::shl (translate(s, e->a), translate(s, e->b)); break;
    case ExprOp::LShr: r = z3::lshr(translate(s, e->a), translate(s, e->b)); break;
    case ExprOp::AShr: r = z3::ashr(translate(s, e->a), translate(s, e->b)); break;
    case ExprOp::Zext: r = z3::zext(translate(s, e->a), e->width - e->a->width); break;
    case ExprOp::Sext: r = z3::sext(translate(s, e->a), e->width - e->a->width); break;
    case ExprOp::Trunc: r = translate(s, e->a).extract(e->width - 1, 0); break;
    case ExprOp::Extract: {
        unsigned hi = static_cast<unsigned>((e->k >> 8) & 0xFF);
        unsigned lo = static_cast<unsigned>(e->k & 0xFF);
        r = translate(s, e->a).extract(hi, lo);
        break;
    }
    case ExprOp::Concat: r = z3::concat(translate(s, e->a), translate(s, e->b)); break;
    case ExprOp::Eq:  r = pred_to_bv(translate(s, e->a) == translate(s, e->b)); break;
    case ExprOp::Ne:  r = pred_to_bv(translate(s, e->a) != translate(s, e->b)); break;
    case ExprOp::Ult: r = pred_to_bv(z3::ult(translate(s, e->a), translate(s, e->b))); break;
    case ExprOp::Slt: r = pred_to_bv(translate(s, e->a) <  translate(s, e->b)); break;
    case ExprOp::Ule: r = pred_to_bv(z3::ule(translate(s, e->a), translate(s, e->b))); break;
    case ExprOp::Sle: r = pred_to_bv(translate(s, e->a) <= translate(s, e->b)); break;
    case ExprOp::Ite:
        r = z3::ite(translate(s, e->a) == bv1_true(),
                    translate(s, e->b),
                    translate(s, e->c));
        break;
    default:
        // Unknown op — model as a fresh free variable so the path stays sound.
        r = s.ctx.bv_const("opaque", e->width ? e->width : 1u);
        break;
    }
    unsigned idx = static_cast<unsigned>(s.terms.size());
    s.terms.push_back(r);
    s.cache.emplace(e, idx);
    return r;
}

static void apply_pc(Solver::Impl& s, const PathCondition& pc) {
    z3::expr one = s.ctx.bv_val(1u, 1u);
    for (auto* t : pc.terms) s.core.add(translate(s, t) == one);
}
#endif // VEDX64_Z3

Solver::Solver()
#ifdef VEDX64_Z3
    : impl_(std::make_unique<Impl>())
#endif
{}
Solver::~Solver() = default;

bool Solver::is_smt_backed() {
#ifdef VEDX64_Z3
    return true;
#else
    return false;
#endif
}

SolveResult Solver::sat(const PathCondition& pc, const Expr* cond) {
    if (is_const(cond)) return cond->k ? SolveResult::Sat : SolveResult::Unsat;
#ifdef VEDX64_Z3
    auto& s = *impl_;
    s.core.push();
    apply_pc(s, pc);
    z3::expr c = translate(s, cond);
    if (cond->width != 1) c = c != s.ctx.bv_val(0u, cond->width);
    else                  c = c == s.ctx.bv_val(1u, 1u);
    s.core.add(c);
    z3::check_result r = s.core.check();
    s.core.pop();
    if (r == z3::sat)   return SolveResult::Sat;
    if (r == z3::unsat) return SolveResult::Unsat;
    return SolveResult::Unknown;
#else
    (void)pc;
    return SolveResult::Unknown;
#endif
}

std::optional<uint64_t> Solver::get_value(const PathCondition& pc, const Expr* e) {
    if (is_const(e)) return e->k;
#ifdef VEDX64_Z3
    auto& s = *impl_;
    s.core.push();
    apply_pc(s, pc);
    z3::expr ze = translate(s, e);
    if (s.core.check() != z3::sat) { s.core.pop(); return std::nullopt; }
    z3::model m = s.core.get_model();
    z3::expr v = m.eval(ze, /*model_completion=*/true);
    uint64_t value = 0;
    bool ok = false;
    if (v.is_numeral_u64(value)) ok = true;
    if (!ok) { s.core.pop(); return std::nullopt; }
    // Confirm uniqueness — if `e` could take any other value under `pc`,
    // refuse to concretize. This matches the spirit of get_value.
    s.core.add(ze != s.ctx.bv_val(value, e->width));
    z3::check_result other = s.core.check();
    s.core.pop();
    if (other == z3::sat) return std::nullopt;
    return value;
#else
    (void)pc;
    return std::nullopt;
#endif
}

std::vector<uint64_t> Solver::enumerate(const PathCondition& pc, const Expr* e, size_t max) {
    std::vector<uint64_t> out;
    if (is_const(e)) { out.push_back(e->k); return out; }
#ifdef VEDX64_Z3
    auto& s = *impl_;
    s.core.push();
    apply_pc(s, pc);
    z3::expr ze = translate(s, e);
    while (out.size() < max) {
        if (s.core.check() != z3::sat) break;
        z3::model m = s.core.get_model();
        z3::expr v = m.eval(ze, true);
        uint64_t value = 0;
        if (!v.is_numeral_u64(value)) break;
        out.push_back(value);
        s.core.add(ze != s.ctx.bv_val(value, e->width));
    }
    s.core.pop();
#else
    (void)pc; (void)max;
#endif
    return out;
}

// ============================================================
//  Engine
// ============================================================

Engine::Engine(Config cfg, ReadCode read_code)
    : cfg_(cfg), read_code_(std::move(read_code)), memory_(&builder_) {
    seed_.b = &builder_;
    seed_.mem = &memory_;
    seed_initial_state(seed_);
}

void Engine::seed_initial_state(State& s) {
    for (int i = 0; i < 16; ++i) s.gpr[i] = builder_.sym(64);
    for (int i = 0; i < 7;  ++i) s.flags[i] = builder_.sym(1);
}

static const Expr* read_var(State& s, const ir::VarNode& v) {
    Builder* b = s.b;
    switch (v.space) {
    case ir::Space::Const: return b->k(static_cast<uint64_t>(v.value), v.size * 8);
    case ir::Space::GPR:   return s.get_gpr_slice(static_cast<uint8_t>(v.offset), v.size);
    case ir::Space::Flags: return s.flags[CF];   // generic flags read — caller usually uses GET_*
    case ir::Space::Temp: {
        auto it = s.temps.find(v.offset);
        if (it != s.temps.end()) return it->second;
        const Expr* fresh = b->sym(v.size * 8);
        s.temps[v.offset] = fresh;
        return fresh;
    }
    case ir::Space::RAM:   return b->k(0, v.size * 8);  // RAM in inputs is a tag; the address comes elsewhere
    case ir::Space::XMM:   return b->sym(v.size * 8);   // SIMD havoc — not modeled
    case ir::Space::OpMask:return b->sym(v.size * 8);
    default:               return b->k(0, v.size * 8);
    }
}

static void write_var(State& s, const ir::VarNode& v, const Expr* e) {
    if (!e) return;
    if (e->width != v.size * 8 && v.size > 0) {
        // Width mismatch — coerce by zext or trunc.
        e = (e->width < v.size * 8) ? s.b->zext(e, v.size * 8)
                                    : s.b->trunc(e, v.size * 8);
    }
    switch (v.space) {
    case ir::Space::GPR:   s.set_gpr_low(static_cast<uint8_t>(v.offset), v.size, e); break;
    case ir::Space::Flags: s.flags[CF] = e; break;
    case ir::Space::Temp:  s.temps[v.offset] = e; break;
    default: break;
    }
}

static FlagBit flag_of(ir::Opcode oc) {
    using OC = ir::Opcode;
    switch (oc) {
    case OC::GET_CF: case OC::SET_CF: return CF;
    case OC::GET_PF: case OC::SET_PF: return PF;
    case OC::GET_ZF: case OC::SET_ZF: return ZF;
    case OC::GET_SF: case OC::SET_SF: return SF;
    case OC::GET_OF: case OC::SET_OF: return OF;
    case OC::GET_DF: case OC::SET_DF: return DF;
    default:                          return CF;
    }
}

void Engine::havoc_state(State& s, const DecodedInstr& di) {
    auto* sem = vedx64::get_semantics(di);
    if (sem) {
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (!vedx64::is_operand_written(*sem, i)) continue;
            const auto& od = di.desc->operands[i];
            if (od.addr == AddrMode::ModRM_Reg || od.addr == AddrMode::ModRM_RM ||
                od.addr == AddrMode::OpcodeReg || od.addr == AddrMode::Fixed) {
                uint8_t reg = (od.addr == AddrMode::Fixed) ? od.fixed_reg
                            : (od.addr == AddrMode::OpcodeReg) ? di.opcode_reg
                            : (od.addr == AddrMode::ModRM_Reg) ? (((di.modrm >> 3) & 7) | (((di.rex >> 2) & 1) << 3))
                                                               : ((di.modrm & 7) | ((di.rex & 1) << 3));
                if (reg < 16) s.gpr[reg] = builder_.sym(64);
            }
        }
        uint16_t implicit = vedx64::implicit_gpr_writes(di);
        for (int r = 0; r < 16; ++r) if (implicit & (1u << r)) s.gpr[r] = builder_.sym(64);
        if (sem->flags_written) for (int f = 0; f < 6; ++f) s.flags[f] = builder_.sym(1);
    } else {
        for (int r = 0; r < 16; ++r) s.gpr[r] = builder_.sym(64);
        for (int f = 0; f < 6; ++f) s.flags[f] = builder_.sym(1);
    }
}

void Engine::apply_op(State& s, const ir::Op& op, const ir::Lifted& l, const DecodedInstr& di) {
    using OC = ir::Opcode;
    Builder* b = &builder_;

    auto in = [&](size_t i) -> const Expr* {
        return read_var(s, op.inputs[i]);
    };

    switch (op.opcode) {
    case OC::COPY:  write_var(s, op.output, in(0)); break;
    case OC::ADD:   write_var(s, op.output, b->add (in(0), in(1))); break;
    case OC::SUB:   write_var(s, op.output, b->sub (in(0), in(1))); break;
    case OC::MUL:   write_var(s, op.output, b->mul (in(0), in(1))); break;
    case OC::IMUL:  write_var(s, op.output, b->mul (in(0), in(1))); break;  // signed/unsigned mul match in low bits
    case OC::DIV:   write_var(s, op.output, b->udiv(in(0), in(1))); break;
    case OC::IDIV:  write_var(s, op.output, b->sdiv(in(0), in(1))); break;
    case OC::MOD:   write_var(s, op.output, b->umod(in(0), in(1))); break;
    case OC::SMOD:  write_var(s, op.output, b->smod(in(0), in(1))); break;
    case OC::AND:   write_var(s, op.output, b->band(in(0), in(1))); break;
    case OC::OR:    write_var(s, op.output, b->bor (in(0), in(1))); break;
    case OC::XOR:   write_var(s, op.output, b->bxor(in(0), in(1))); break;
    case OC::NOT:   write_var(s, op.output, b->bnot(in(0))); break;
    case OC::NEG:   write_var(s, op.output, b->neg (in(0))); break;
    case OC::SHL:   write_var(s, op.output, b->shl (in(0), in(1))); break;
    case OC::SHR:   write_var(s, op.output, b->lshr(in(0), in(1))); break;
    case OC::SAR:   write_var(s, op.output, b->ashr(in(0), in(1))); break;
    case OC::ZEXT:  write_var(s, op.output, b->zext (in(0), op.output.size * 8)); break;
    case OC::SEXT:  write_var(s, op.output, b->sext (in(0), op.output.size * 8)); break;
    case OC::TRUNC: write_var(s, op.output, b->trunc(in(0), op.output.size * 8)); break;
    case OC::CMP_EQ:  write_var(s, op.output, b->eq (in(0), in(1))); break;
    case OC::CMP_NE:  write_var(s, op.output, b->ne (in(0), in(1))); break;
    case OC::CMP_ULT: write_var(s, op.output, b->ult(in(0), in(1))); break;
    case OC::CMP_SLT: write_var(s, op.output, b->slt(in(0), in(1))); break;
    case OC::CMP_ULE: write_var(s, op.output, b->ule(in(0), in(1))); break;
    case OC::CMP_SLE: write_var(s, op.output, b->sle(in(0), in(1))); break;
    case OC::SELECT:  write_var(s, op.output, b->ite(in(0), in(1), in(2))); break;
    case OC::BITSEL:  write_var(s, op.output, b->ite(b->eq(in(0), b->k(0, in(0)->width)), in(2), in(1))); break;

    case OC::GET_CF: case OC::GET_PF: case OC::GET_ZF: case OC::GET_SF:
    case OC::GET_OF: case OC::GET_DF:
        write_var(s, op.output, b->zext(s.flags[flag_of(op.opcode)], op.output.size * 8));
        break;
    case OC::SET_CF: case OC::SET_PF: case OC::SET_ZF: case OC::SET_SF:
    case OC::SET_OF: case OC::SET_DF: {
        const Expr* v = in(0);
        s.flags[flag_of(op.opcode)] = (v->width == 1) ? v : b->trunc(v, 1);
        break;
    }

    case OC::ADD_FLAGS: case OC::SUB_FLAGS: case OC::AND_FLAGS:
        // Caller should run ir::expand_flag_bundles before stepping, but if a
        // bundle slips through we conservatively havoc the status flags so the
        // analyzer doesn't silently pretend they're concrete.
        for (int f = 0; f < 6; ++f) s.flags[f] = b->sym(1);
        break;

    case OC::LOAD: {
        const Expr* addr = in(0);
        s.temps[op.output.offset] = s.mem->load(addr, op.output.size);
        break;
    }
    case OC::STORE: {
        const Expr* addr = in(0);
        const Expr* val  = in(1);
        s.mem->store(addr, val, op.inputs[1].size);
        break;
    }

    case OC::BRANCH:
        // Unconditional — input[0] is the constant target VA.
        if (is_const(in(0))) s.rip = in(0)->k;
        else {
            // Symbolic target — try to concretize.
            auto v = solver_.get_value(s.pc, in(0));
            if (v) s.rip = *v;
            else { s.dead = true; diagnostics_.push_back("BRANCH: symbolic target abandoned"); }
        }
        break;

    case OC::CBRANCH: {
        const Expr* cond = in(0);
        // The IR uses size=1 (1 byte = 8 bits) for boolean conds. Reduce
        // to a 1-bit predicate "is the condition non-zero?" so subsequent
        // path-condition arithmetic stays clean.
        if (cond->width != 1) cond = b->ne(cond, b->k(0, cond->width));
        uint64_t target  = is_const(in(1)) ? in(1)->k : 0;
        uint64_t fallthr = s.rip + l.length;
        if (is_const(cond)) {
            s.rip = cond->k ? target : fallthr;
            break;
        }
        SolveResult t = solver_.sat(s.pc, cond);
        SolveResult f = solver_.sat(s.pc, b->eq(cond, b->k(0, 1)));
        // Treat Unknown as feasible.
        bool t_feas = (t != SolveResult::Unsat);
        bool f_feas = (f != SolveResult::Unsat);
        if (t_feas && f_feas && s.fork_depth < cfg_.max_fork_depth) {
            State alt = s.fork();
            alt.pc.push(b->eq(cond, b->k(0, 1)));
            alt.rip = fallthr;
            queue_.push_back(std::move(alt));
            s.pc.push(cond);
            s.rip = target;
        } else if (t_feas) {
            s.pc.push(cond);
            s.rip = target;
        } else if (f_feas) {
            s.pc.push(b->eq(cond, b->k(0, 1)));
            s.rip = fallthr;
        } else {
            s.dead = true;
        }
        break;
    }

    case OC::INDIRECT_JMP: {
        auto v = solver_.get_value(s.pc, in(0));
        if (v) s.rip = *v;
        else   { s.dead = true; diagnostics_.push_back("INDIRECT_JMP: symbolic target abandoned"); }
        break;
    }

    case OC::CALL: {
        // Treat as straight-line to the next instruction; symbolic-aware
        // engines that want to descend into callees should inject their own
        // stack model and recurse.
        s.rip += l.length;
        break;
    }
    case OC::VCALL: s.rip += l.length; break;
    case OC::RET: {
        // Read the saved retaddr from [RSP] (it was stored by the lifter).
        const Expr* rsp = s.gpr[4];
        const Expr* ret = s.mem->load(rsp, 8);
        auto v = solver_.get_value(s.pc, ret);
        if (v) s.rip = *v;
        else   { s.dead = true; diagnostics_.push_back("RET: symbolic return target abandoned"); }
        break;
    }

    case OC::NOP: break;
    case OC::BARRIER:
    case OC::UNDEF:
        if (cfg_.stop_on_undef) { s.dead = true; }
        else { havoc_state(s, di); }
        break;

    case OC::POPCNT: case OC::CTZ: case OC::CLZ:
        // Approximate: result is symbolic of output width.
        s.temps[op.output.offset] = b->sym(op.output.size * 8);
        break;

    case OC::RDTSC: case OC::SYSCALL: case OC::UD2:
        havoc_state(s, di);
        s.rip += l.length;
        break;

    default:
        // Other opcodes (FP / SIMD / IR-only constructs) — havoc just the
        // explicit destination so we don't corrupt unrelated state.
        if (op.output.space == ir::Space::Temp) {
            s.temps[op.output.offset] = b->sym(op.output.size * 8 ? op.output.size * 8 : 64);
        } else if (op.output.space == ir::Space::GPR && op.output.offset < 16) {
            s.gpr[op.output.offset] = b->sym(64);
        }
        break;
    }
}

std::vector<State> Engine::run(uint64_t entry, std::function<bool(const State&)> stop) {
    std::vector<State> finished;
    State init = seed_.fork();
    init.fork_depth = 0;
    init.rip = entry;
    queue_.push_back(std::move(init));

    while (!queue_.empty()) {
        State s = std::move(queue_.front());
        queue_.pop_front();

        uint32_t steps = 0;
        while (!s.dead && steps < cfg_.max_steps_per_path) {
            if (stop && stop(s)) { finished.push_back(std::move(s)); goto next_path; }
            uint8_t code[15] = {};
            // Try the full 15-byte read first; if the host can't satisfy it
            // (end-of-buffer), shrink the window so trailing instructions
            // still decode. The decoder consumes whatever it needs.
            size_t got = 0;
            for (size_t want = 15; want >= 1; --want) {
                if (read_code_(s.rip, code, want)) { got = want; break; }
            }
            if (got == 0) {
                s.dead = true; diagnostics_.push_back("read_code failed"); break;
            }
            DecodedInstr di;
            size_t consumed = decode(code, got, di);
            if (consumed == 0 || !di.desc) { s.dead = true; diagnostics_.push_back("decode failed"); break; }

            auto lifted = ir::lift(code, consumed, s.rip);
            if (!lifted) { s.dead = true; diagnostics_.push_back("lift failed"); break; }
            auto expanded = ir::expand_flag_bundles(*lifted);

            // Snapshot rip — only auto-advance if no branch op fired.
            uint64_t pre_rip = s.rip;
            s.clear_temps();
            for (const auto& op : expanded.ops) {
                apply_op(s, op, expanded, di);
                if (s.dead) break;
            }
            if (!s.dead && s.rip == pre_rip) s.rip += expanded.length;
            ++steps;
        }
        if (!s.dead) finished.push_back(std::move(s));
        next_path:;
    }
    return finished;
}

std::string dump_op_with_state(const ir::Op& op, const State& s) {
    std::ostringstream os;
    os << "[op=" << (int)op.opcode << " out=" << (int)op.output.space
       << ":" << op.output.offset << ":" << (int)op.output.size << "]";
    if (op.opcode == ir::Opcode::COPY || op.opcode == ir::Opcode::ADD || op.opcode == ir::Opcode::SUB) {
        if (op.num_inputs > 0) {
            const Expr* e = nullptr;
            if (op.inputs[0].space == ir::Space::Temp) {
                auto it = s.temps.find(op.inputs[0].offset);
                if (it != s.temps.end()) e = it->second;
            } else if (op.inputs[0].space == ir::Space::GPR && op.inputs[0].offset < 16) {
                e = s.gpr[op.inputs[0].offset];
            }
            if (e) os << " in0=" << Builder::to_string(e);
        }
    }
    return os.str();
}

} // namespace vedx64::symx
