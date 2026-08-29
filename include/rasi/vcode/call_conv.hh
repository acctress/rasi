#pragma once

#include <rasi/ir/call_conv.hh>
#include <rasi/vcode/registers.hh>
#include <span>

namespace rasi
{
    struct RegSet
    {
        const u8 *data;
        u8        count;

        [[nodiscard]] constexpr std::span< const u8 > span( ) const noexcept { return { data, count }; }
    };

    struct CallConvRegs
    {
        RegSet allocatable;
        RegSet args_int;
        RegSet args_float;
        RegSet ret_int;
        RegSet ret_float;
        RegSet callee_saved;
    };

    [[nodiscard]] const CallConvRegs& conv_regs( CallConv conv ) noexcept;
} // namespace rasi