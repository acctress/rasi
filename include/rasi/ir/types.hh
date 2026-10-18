//
// Created by griffith on 17/10/2026.
//

#pragma once

#include <format>
#include <cstdint>

namespace rasi::ir {
    //! @brief All of rasi's data types which are representable in the IR
    enum class Type : uint8_t {
        i8,
        i16,
        i32,
        i64,
        f32,
        f64,
        ptr,
        unit
    };
}

//! @brief std::formatter specialisation for ir::Type for formatting.
template <>
struct std::formatter<rasi::ir::Type> {
    constexpr auto parse(auto& ctx) { return ctx.begin(); }
    auto format(rasi::ir::Type t, auto& ctx) const {
        switch (t) {
            case rasi::ir::Type::i8: return std::format_to(ctx.out(), "i8");
            case rasi::ir::Type::i16: return std::format_to(ctx.out(), "i16");
            case rasi::ir::Type::i32: return std::format_to(ctx.out(), "i32");
            case rasi::ir::Type::i64: return std::format_to(ctx.out(), "i64");
            case rasi::ir::Type::f32: return std::format_to(ctx.out(), "f32");
            case rasi::ir::Type::f64: return std::format_to(ctx.out(), "f64");
            case rasi::ir::Type::ptr: return std::format_to(ctx.out(), "ptr");
            case rasi::ir::Type::unit: return std::format_to(ctx.out(), "unit");
        }
    }
};
