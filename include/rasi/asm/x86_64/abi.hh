#pragma once

#include "rasi/asm/registers.hh"
#include "rasi/ir/call_conv.hh"

#include <array>
#include <cassert>

namespace rasi::x86_64
{
    using namespace rasi::asm_;

    inline constexpr std::array win64_args {
        regs::rcx,
        regs::rdx,
        regs::r8,
        regs::r9,
    };

    inline constexpr std::array sysv_args { regs::rdi, regs::rsi, regs::rdx, regs::rcx, regs::r8, regs::r9 };

    constexpr CallConv resolve_cc( const CallConv cc )
    {
        if ( cc != CallConv::native ) return cc;

#ifdef _WIN32
        return CallConv::win_fastcall;
#else
        return CallConv::sys_v;
#endif
    }

    inline PhysReg arg_reg( CallConv cc, const u16 idx )
    {
        cc = resolve_cc( cc );
        if ( cc == CallConv::win_fastcall )
        {
            assert( idx < win64_args.size( ) );
            return win64_args[ idx ];
        }

        assert( idx < sysv_args.size( ) );
        return sysv_args[ idx ];
    }
} // namespace rasi::x86_64