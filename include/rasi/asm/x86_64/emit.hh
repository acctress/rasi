#pragma once

#include <rasi/support/buffer.hh>
#include <rasi/regalloc/regalloc.hh>
#include <rasi/vcode/frame.hh>
#include <rasi/vcode/vcode.hh>

namespace rasi::azm::x86_64
{
    [[nodiscard]] Buffer emit( const VCode& vcode, const regalloc::AllocContext& ctx, FrameLayout& frame );
}