#pragma once

#include <rasi/vcode/vcode.hh>
#include <rasi/vcode/frame.hh>

namespace rasi::isel::x86_64
{
    void emit_prologue( VCode& vcode, FrameLayout& frame );
    void emit_epilogue( VCode& vcode, FrameLayout& frame );
}