//
// Created by griffith on 17/10/2026.
//

#pragma once

#include <rasi/ir/types.hh>
#include <rasi/ir/refs.hh>

namespace rasi::ir {
    //! @brief A single SSA value, holds its type and
    //! instruction ref which owns this value.
    struct Value {
        Type    type;
        InstRef inst;
    };
}


template <>
struct std::formatter<rasi::ir::Value> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(const rasi::ir::Value& v, auto& ctx) const {
        return std::format_to(ctx.out(), "{}:v{}", v.type, v.inst.id);
    }
};