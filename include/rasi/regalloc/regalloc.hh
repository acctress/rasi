#pragma once

#include <queue>
#include <rasi/support/types.hh>
#include <rasi/vcode/operand.hh>
#include <rasi/vcode/vcode.hh>
#include <rasi/vcode/call_conv.hh>
#include <vector>

namespace rasi::regalloc
{
    struct LiveRange
    {
        VReg vreg {};
        u32  start {};
        u32  end {};
        u32  uses {};

        [[nodiscard]] float spill_weight( ) const noexcept
        {
            if ( end <= start ) return 0.f;
            return static_cast< float >( uses ) / static_cast< float >( end - start );
        }

        [[nodiscard]] bool overlaps( const LiveRange &ot ) const noexcept { return start < ot.end && ot.start < end; }
    };

    struct Assignment
    {
        VReg vreg {};
        bool spilled { false };
        bool pre_coloured { false };

        union
        {
            PhysReg   reg {};
            SpillSlot slot;
        };
    };

    struct AllocContext
    {
        std::vector< LiveRange >  live_ranges;
        std::vector< Assignment > assignments;
        u32                       nx_spill_slot {};
        u32                       param_count {};
        u8                        num_regs {};

        using WeightQueue = std::priority_queue< u32, std::vector< u32 >,
                                                 decltype( []( u32, u32 )
        {
            return true;
        } ) >;
    };

    void compute_liveness( AllocContext &ctx, const VCode &vcode );
    void alloc( AllocContext &ctx, VCode &vcode );
    void apply( AllocContext &ctx, VCode &vcode );
} // namespace rasi::regalloc