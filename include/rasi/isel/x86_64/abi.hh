#pragma once

#include <rasi/support/buffer.hh>
#include <rasi/vcode/frame.hh>

namespace rasi::isel::x86_64
{
    void emit_prologue( Buffer &buffer, const FrameLayout &frame, CallConv call_conv );

    void emit_epilogue( Buffer &buffer, const FrameLayout &frame, CallConv call_conv );
} // namespace rasi::isel::x86_64