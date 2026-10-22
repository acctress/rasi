//
// Created by griffith on 18/10/2026.
//

#pragma once

#include <xiao/ir/visitor.hh>

namespace xiao::ir {
    //! @brief Validates invariants structural of finalised IR.
    struct IRVerifier : IRVisitor<IRVerifier> {
        struct Err {
            std::string msg;
        };

        std::vector<Err> errs;

        [[nodiscard]] bool ok() const { return errs.empty(); }

        void on_block_end(IRFunction& fn, IRBlock& block) {
            if (block.insts_count == 0) {
                errs.push_back({ "block is empty, no terminator." });
                return;
            }

            const auto& l = fn.insts[block.insts_offset+block.insts_count-1];
            const bool is_term = l.op == IROp::ret
                              || l.op == IROp::br
                              || l.op == IROp::brif;

            if (!is_term) errs.push_back({ std::format("block does not end with a terminator: {}", l) });
        }
    };
}