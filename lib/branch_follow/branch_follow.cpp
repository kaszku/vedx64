// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Kevin Szkudlapski

#include "vedx64/branch_follow.hpp"
#include <algorithm>
#include <unordered_set>

namespace vedx64 {

FlowInfo classify_flow(const uint8_t* code, size_t len, uint64_t address) {
    FlowInfo fi;
    fi.address = address;

    DecodedInstr di;
    size_t n = decode(code, len, di);
    if (n == 0 || !di.desc) {
        fi.type = BranchFlowType::Unknown;
        fi.has_fallthrough = false;
        fi.length = 1;
        return fi;
    }
    fi.length = static_cast<uint8_t>(n);

    Mnemonic m = di.desc->mnemonic;

    if (m == Mnemonic::RETN || m == Mnemonic::RETF ||
        m == Mnemonic::IRET || m == Mnemonic::IRETD || m == Mnemonic::IRETQ) {
        fi.type = BranchFlowType::Return;
        fi.has_fallthrough = false;
        return fi;
    }

    if (m == Mnemonic::JMP) {
        fi.type = BranchFlowType::Branch;
        fi.has_fallthrough = false;
        // Check if relative
        bool is_rel = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                fi.target = address + n + di.immediate;
                is_rel = true;
                break;
            }
        }
        if (!is_rel) fi.is_indirect = true;
        return fi;
    }

    if (m == Mnemonic::CALL) {
        fi.type = BranchFlowType::Call;
        fi.has_fallthrough = true;
        bool is_rel = false;
        for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
            if (di.desc->operands[i].addr == AddrMode::RelOffset) {
                fi.target = address + n + di.immediate;
                is_rel = true;
                break;
            }
        }
        if (!is_rel) fi.is_indirect = true;
        return fi;
    }

    // Conditional branches: any mnemonic starting with J and having a RelOffset operand (except JMP/JCXZ group)
    for (uint8_t i = 0; i < di.desc->num_operands; ++i) {
        if (di.desc->operands[i].addr == AddrMode::RelOffset && m != Mnemonic::JMP && m != Mnemonic::CALL) {
            fi.type = BranchFlowType::ConditionalBranch;
            fi.has_fallthrough = true;
            fi.target = address + n + di.immediate;
            return fi;
        }
    }

    if (m == Mnemonic::UD2) {
        fi.type = BranchFlowType::Exception;
        fi.has_fallthrough = false;
        return fi;
    }
    if (m == Mnemonic::INT || m == Mnemonic::INT1 || m == Mnemonic::INTO ||
        m == Mnemonic::SYSCALL || m == Mnemonic::SYSENTER) {
        fi.type = BranchFlowType::Exception;
        fi.has_fallthrough = true;
        return fi;
    }
    if (m == Mnemonic::HLT) {
        fi.type = BranchFlowType::Exception;
        fi.has_fallthrough = false;
        return fi;
    }

    fi.type = BranchFlowType::Sequential;
    fi.has_fallthrough = true;
    return fi;
}

BasicBlock walk_basic_block(
    uint64_t start,
    std::function<bool(uint64_t, uint8_t*)> read_byte,
    size_t max_insns) {
    BasicBlock bb;
    bb.start = start;
    uint64_t addr = start;

    for (size_t i = 0; i < max_insns; ++i) {
        uint8_t buf[15];
        size_t avail = 0;
        for (size_t j = 0; j < 15; ++j) {
            if (!read_byte(addr + j, &buf[j])) break;
            avail = j + 1;
        }
        if (avail == 0) break;

        FlowInfo fi = classify_flow(buf, avail, addr);
        bb.instructions.push_back(fi);
        addr += fi.length;

        if (fi.type != BranchFlowType::Sequential) {
            if (fi.target != 0 && !fi.is_indirect)
                bb.successors.push_back(fi.target);
            if (fi.has_fallthrough)
                bb.successors.push_back(addr);
            break;
        }
    }
    bb.end = addr;
    return bb;
}

std::vector<BasicBlock> walk_cfg(
    uint64_t entry,
    std::function<bool(uint64_t, uint8_t*)> read_byte,
    size_t max_blocks,
    size_t max_insns_per_block) {
    std::vector<BasicBlock> cfg;
    std::vector<uint64_t> worklist;
    std::unordered_set<uint64_t> visited;

    worklist.push_back(entry);
    while (!worklist.empty() && cfg.size() < max_blocks) {
        uint64_t addr = worklist.back();
        worklist.pop_back();
        if (visited.count(addr)) continue;
        visited.insert(addr);

        BasicBlock bb = walk_basic_block(addr, read_byte, max_insns_per_block);
        for (auto succ : bb.successors)
            if (!visited.count(succ))
                worklist.push_back(succ);
        cfg.push_back(std::move(bb));
    }
    return cfg;
}

} // namespace vedx64
