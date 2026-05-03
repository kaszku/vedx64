// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <deque>
#include <memory>
#include <functional>
#include <optional>
#include "vedx64/core.hpp"
#include "vedx64/ir.hpp"
#include "vedx64/semantics.hpp"

namespace vedx64::symx {

// ============================================================
//  Expression layer — hash-consed bitvector AST
// ============================================================

enum class ExprOp : uint8_t {
    ConstU,    // bitvector constant — fits in u64 for widths ≤ 64
    Symbol,    // free variable: id + width
    Add, Sub, Mul, UDiv, SDiv, UMod, SMod,
    And, Or, Xor, Not, Neg,
    Shl, LShr, AShr,
    Zext, Sext, Trunc, Extract,    // Extract packs (hi, lo) into k
    Concat,
    Eq, Ne, Ult, Slt, Ule, Sle,    // → 1-bit
    Ite,
};

struct Expr {
    ExprOp   op;
    uint16_t width;   // result width in bits (1..64)
    uint64_t k;       // ConstU value, Symbol id, or Extract packed (hi<<8 | lo)
    const Expr* a;
    const Expr* b;
    const Expr* c;   // Ite uses all three; binops use a,b
};

class Builder {
public:
    Builder();

    const Expr* k(uint64_t v, uint16_t width);   // canonical ConstU
    const Expr* sym(uint16_t width);              // fresh Symbol
    const Expr* sym_named(const char* name, uint16_t width);

    // Arithmetic / logic — all assume operands have matching width.
    const Expr* add(const Expr* x, const Expr* y);
    const Expr* sub(const Expr* x, const Expr* y);
    const Expr* mul(const Expr* x, const Expr* y);
    const Expr* udiv(const Expr* x, const Expr* y);
    const Expr* sdiv(const Expr* x, const Expr* y);
    const Expr* umod(const Expr* x, const Expr* y);
    const Expr* smod(const Expr* x, const Expr* y);
    const Expr* band(const Expr* x, const Expr* y);
    const Expr* bor (const Expr* x, const Expr* y);
    const Expr* bxor(const Expr* x, const Expr* y);
    const Expr* bnot(const Expr* x);
    const Expr* neg (const Expr* x);
    const Expr* shl (const Expr* x, const Expr* n);
    const Expr* lshr(const Expr* x, const Expr* n);
    const Expr* ashr(const Expr* x, const Expr* n);

    // Width-changing ops.
    const Expr* zext   (const Expr* x, uint16_t to);
    const Expr* sext   (const Expr* x, uint16_t to);
    const Expr* trunc  (const Expr* x, uint16_t to);
    const Expr* extract(const Expr* x, uint16_t hi, uint16_t lo);
    const Expr* concat (const Expr* hi, const Expr* lo);

    // Comparisons return 1-bit.
    const Expr* eq (const Expr* x, const Expr* y);
    const Expr* ne (const Expr* x, const Expr* y);
    const Expr* ult(const Expr* x, const Expr* y);
    const Expr* slt(const Expr* x, const Expr* y);
    const Expr* ule(const Expr* x, const Expr* y);
    const Expr* sle(const Expr* x, const Expr* y);

    // Branchless select.
    const Expr* ite(const Expr* c, const Expr* t, const Expr* f);

    // Diagnostics.
    static std::string to_string(const Expr* e);
    size_t arena_size() const { return arena_.size(); }
    size_t symbol_count() const { return next_symbol_; }

private:
    struct Key {
        ExprOp op; uint16_t width; uint64_t k;
        const Expr* a; const Expr* b; const Expr* c;
        bool operator==(const Key& o) const noexcept {
            return op == o.op && width == o.width && k == o.k && a == o.a && b == o.b && c == o.c;
        }
    };
    struct KeyHash {
        size_t operator()(const Key& k) const noexcept {
            uint64_t h = (uint64_t)k.op | ((uint64_t)k.width << 8);
            h ^= k.k * 0x9E3779B97F4A7C15ULL;
            h ^= reinterpret_cast<uintptr_t>(k.a);
            h *= 0xBF58476D1CE4E5B9ULL;
            h ^= reinterpret_cast<uintptr_t>(k.b);
            h *= 0x94D049BB133111EBULL;
            h ^= reinterpret_cast<uintptr_t>(k.c);
            return static_cast<size_t>(h);
        }
    };

    const Expr* intern(ExprOp op, uint16_t w, uint64_t k,
                       const Expr* a, const Expr* b, const Expr* c = nullptr);

    static uint64_t mask_to(uint16_t width) {
        return (width >= 64) ? ~0ULL : ((1ULL << width) - 1);
    }
    static int64_t  sext_to_i64(uint64_t v, uint16_t width);

    std::vector<std::unique_ptr<Expr>>      arena_;
    std::unordered_map<Key, const Expr*, KeyHash> intern_;
    uint64_t next_symbol_ = 0;
};

// Helpers usable from anywhere.
inline bool is_const(const Expr* e) { return e && e->op == ExprOp::ConstU; }
inline uint64_t const_value(const Expr* e) { return e->k; }

// ============================================================
//  Memory — flat byte map with concrete fast-path
// ============================================================

class Memory {
public:
    explicit Memory(Builder* b) : b_(b) {}

    const Expr* load (const Expr* addr, uint8_t bytes);
    void        store(const Expr* addr, const Expr* val, uint8_t bytes);

    // Concrete-only seed (e.g. for code or initial data).
    void seed(uint64_t addr, const uint8_t* data, size_t n);

    void clear();
    size_t concrete_byte_count() const { return concrete_.size(); }
    size_t symbolic_write_count() const { return symbolic_.size(); }

private:
    struct SymWrite { const Expr* addr; const Expr* val; }; // val is 1 byte wide
    Builder*                          b_;
    std::map<uint64_t, const Expr*>   concrete_;   // byte-addressable
    std::vector<SymWrite>             symbolic_;
    const Expr* load_byte(const Expr* addr_byte);
    void        store_byte(const Expr* addr_byte, const Expr* val_byte);
};

// ============================================================
//  PathCondition — conjunction of 1-bit Expr predicates
// ============================================================

struct PathCondition {
    std::vector<const Expr*> terms;   // each width=1
    bool empty() const { return terms.empty(); }
    void push(const Expr* p) { if (p) terms.push_back(p); }
    void pop_back() { if (!terms.empty()) terms.pop_back(); }
};

// ============================================================
//  State — registers, flags, memory, RIP, PC
// ============================================================

enum FlagBit { CF = 0, PF, AF, ZF, SF, OF, DF }; // matches vedx64::ir flag layout

struct State {
    Builder*       b;
    const Expr*    gpr[16]   = {};       // width=64 each
    const Expr*    flags[7]  = {};       // width=1 each
    Memory*        mem       = nullptr;  // shared across forks of the same engine
    uint64_t       rip       = 0;
    PathCondition  pc;
    uint32_t       fork_depth = 0;
    bool           dead       = false;

    // Per-instruction scratch — temps live only during one Lifted apply.
    std::unordered_map<uint16_t, const Expr*> temps;
    void clear_temps() { temps.clear(); }

    // Convenience accessors.
    void set_gpr(uint8_t r, const Expr* e);
    void set_gpr_low(uint8_t r, uint8_t bytes, const Expr* e);
    const Expr* get_gpr(uint8_t r) const;
    const Expr* get_gpr_slice(uint8_t r, uint8_t bytes) const;

    State fork() const;   // deep copy except for `mem` (shared by reference)
};

// ============================================================
//  Solver — concrete-only stub. Returns Unknown for symbolic
//  questions; the engine treats Unknown as "both sides feasible".
// ============================================================

enum class SolveResult { Sat, Unsat, Unknown };

class Solver {
public:
    Solver();
    ~Solver();
    Solver(const Solver&) = delete;
    Solver& operator=(const Solver&) = delete;

    // Is `cond` satisfiable under the path condition? `cond` is width=1.
    SolveResult sat(const PathCondition& pc, const Expr* cond);

    // Try to resolve `e` to a single concrete u64 under `pc`. Returns
    // nullopt if the value isn't pinned down (or the solver can't tell).
    std::optional<uint64_t> get_value(const PathCondition& pc, const Expr* e);

    // Enumerate up to `max` distinct values of `e` consistent with `pc`.
    std::vector<uint64_t>   enumerate(const PathCondition& pc, const Expr* e, size_t max = 8);

    // True if the solver actually does any reasoning beyond constant folding.
    // False for the stub backend (when vedx64_core was built without VEDX64_Z3).
    static bool is_smt_backed();

private:
    // PIMPL hides z3.h from the public header. When vedx64_core is built
    // without VEDX64_Z3 the impl_ pointer stays null and the methods take
    // the constant-only fast paths.
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// ============================================================
//  Engine — main loop
// ============================================================

struct Config {
    uint32_t max_fork_depth     = 64;
    uint32_t max_steps_per_path = 4096;
    bool     stop_on_undef      = false;  // if true, kill paths on UNDEF; else havoc
    bool     verbose            = false;
};

class Engine {
public:
    using ReadCode = std::function<bool(uint64_t addr, uint8_t* out, size_t n)>;

    Engine(Config cfg, ReadCode read_code);

    // Mutate the seed state before run() — caller can fix concrete inputs.
    State& seed_state() { return seed_; }

    // Run until each active path either terminates or hits stop().
    // `stop` may return true to mark a path "finished and worth keeping".
    std::vector<State> run(uint64_t entry, std::function<bool(const State&)> stop);

    // Single-instruction step. Decodes whatever is at `s.rip`, lifts, and
    // applies all IR ops to `s` in-place. Symbolic CBRANCH still forks —
    // the alternate path goes onto the engine's queue if there's room.
    // Returns false on decode failure or when the path is already dead.
    bool step(State& s);

    // Straight-line run of a single state with no fork enqueueing. At a
    // symbolic CBRANCH the engine picks the *taken* side and pushes the
    // negated condition into `s.pc`, leaving the fall-through unexplored.
    // Useful for "summarize this basic block" callers that don't want
    // path enumeration. Returns the number of instructions executed.
    size_t run_block(State& s, size_t max_instructions);

    Builder& builder() { return builder_; }
    Solver&  solver()  { return solver_;  }
    const std::vector<std::string>& diagnostics() const { return diagnostics_; }

private:
    void apply_op(State& s, const ir::Op& op, const ir::Lifted& l, const DecodedInstr& di);
    void havoc_state(State& s, const DecodedInstr& di);
    void seed_initial_state(State& s);

    Config                   cfg_;
    ReadCode                 read_code_;
    Builder                  builder_;
    Memory                   memory_;
    Solver                   solver_;
    State                    seed_;
    std::deque<State>        queue_;
    std::vector<std::string> diagnostics_;
};

// Emit a human-readable representation of an Op after symbolic translation
// (mostly for debugging / tracing).
std::string dump_op_with_state(const ir::Op& op, const State& s);

} // namespace vedx64::symx
