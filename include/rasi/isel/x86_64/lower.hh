#pragma once

#include <rasi/isel/context.hh>
#include <rasi/ir/instruction.hh>
#include <rasi/vcode/vcode.hh>

namespace rasi::isel::x86_64
{
    VReg lower_inst( ISELCtx &ctx, const Inst &inst ) noexcept;
    void lower_function( ISELCtx& ctx ) noexcept;
}