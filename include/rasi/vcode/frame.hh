#pragma once

#include <rasi/regalloc/regalloc.hh>
#include <rasi/support/types.hh>
#include <rasi/vcode/call_conv.hh>
#include <rasi/vcode/operand.hh>
#include <vector>

namespace rasi
{
    struct FrameLayout
    {
        u32                    incoming_args_size {};     /// stack space for args passed to this function
        u32                    setup_area_size {};        /// bytes consumed by push rbp + return addr
        u32                    clobber_size {};           /// bytes consumed by callee saved reg pushes
        u32                    fixed_frame_stor_size {};  /// spill area = spill_slot_count * 8
        u32                    outgoing_args_size {};     /// stack space reserved for args this fn passes to callees
        std::vector< PhysReg > clobbered_callee_saves {}; /// regs that need saving
    };

    FrameLayout compute_frame_layout( const regalloc::AllocContext &ctx, const CallConvRegs &cc, u32 param_count );
} // namespace rasi