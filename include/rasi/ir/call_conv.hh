#pragma once

#include <rasi/support/types.hh>

namespace rasi
{
    enum class CallConv : u8
    {
        native,
        sys_v,
        win_fastcall,
    };
}