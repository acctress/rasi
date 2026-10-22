//
// Created by griffith on 18/10/2026.
//

#pragma once

#include <xiao/ir/block.hh>
#include <xiao/ir/inst.hh>
#include <xiao/ir/types.hh>
#include <xiao/ir/refs.hh>
#include <span>

#include "xiao/support/arena.hh"

namespace xiao::ir {
    //! @brief Function signature
    //! @returns The return type and a flat parameter type list.
    struct IRSig {
        Type        ret             {};
        uint32_t    params_offset   {};
        uint16_t    params_count    {};
    };

    //! @brief Owns flat pools for one IR function,
    //! @note All are views into the owning IRModule's arena
    struct IRFunction {
        support::Arena*     arena       {};
        StringRef           name        {};
        IRSig               sig         {};
        FunctionRef         self        {};
        std::span<IRBlock>  blocks      {};
        std::span<IRInst>   insts       {};
        std::span<ValueRef> operands    {};
        std::span<BlockRef> succs       {};
        std::span<Value>    values      {};
        std::span<ValueRef> block_params{};
    };
}

template <>
struct std::formatter<xiao::ir::IRFunction> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(const xiao::ir::IRFunction& fn, auto& ctx) const {
        auto out = ctx.out();
        out = std::format_to(out, "fn#{} sig=({} -> {}) blocks={} insts={} values={}\n",
            fn.self.id, fn.sig.params_count, fn.sig.ret,
            fn.blocks.size(), fn.insts.size(), fn.values.size());

        for (std::size_t bi = 0; bi < fn.blocks.size(); ++bi) {
            const auto& b = fn.blocks[bi];
            out = std::format_to(out, "  block{}(", bi);

            for (uint16_t pi = 0; pi < b.params_count; ++pi) {
                const auto vref = fn.block_params[b.params_offset + pi];
                if (pi) out = std::format_to(out, ", ");
                out = std::format_to(out, "v{}", vref.id);
            }
            out = std::format_to(out, "):\n");

            for (uint16_t ii = 0; ii < b.insts_count; ++ii) {
                const auto& inst = fn.insts[b.insts_offset + ii];
                out = std::format_to(out, "    v{} = {}", inst.result.id, inst.op);

                if (inst.op == xiao::ir::IROp::iconst) {
                    out = std::format_to(out, " #{}", inst.imm);
                } else if (inst.op == xiao::ir::IROp::icmp) {
                    out = std::format_to(out, " {}", inst.cc);
                }

                for (uint16_t oi = 0; oi < inst.ops_count; ++oi) {
                    out = std::format_to(out, " v{}", fn.operands[inst.ops_offset + oi].id);
                }
                out = std::format_to(out, "\n");
            }

            for (uint8_t si = 0; si < b.succs_count; ++si)
                out = std::format_to(out, "    -> block{}\n", fn.succs[b.succs_offset + si].id);
        }
        return out;
    }
};