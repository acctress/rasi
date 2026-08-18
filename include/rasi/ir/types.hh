#pragma once

#include <rasi/support/types.hh>

namespace rasi
{
    enum class Type : u8 { i8, i16, i32, i64, i128, f32, f64, ptr, void_ };

    struct ValueRef
    {
        u32 id;
    };

    struct InstRef
    {
        u32 id;
    };

    struct BlockRef
    {
        u32 id;
    };

    struct FunctionRef
    {
        u32 id;
    };
}