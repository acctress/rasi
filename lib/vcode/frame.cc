#include <rasi/vcode/frame.hh>

using namespace rasi;

FrameLayout rasi::compute_frame_layout( const regalloc::AllocContext &ctx, const CallConvRegs &cc, u32 param_count )
{
    const auto fixed_frame_stor_size = ctx.nx_spill_slot * 8;

    std::vector< PhysReg > clobbered_callee_saves {};
    for ( auto i { 0uz }; i < ctx.assignments.size( ); ++i )
    {
        if ( const auto reg = ctx.assignments[ i ]; !reg.spilled && !reg.pre_coloured )
        {
            for ( const auto &id : cc.callee_saved.span( ) )
            {
                if ( reg.reg.id == id && !std::ranges::contains( clobbered_callee_saves, reg.reg ) )
                {
                    clobbered_callee_saves.push_back( reg.reg );
                }
            }
        }
    }

    u32 used_reg_count {};
    for ( const auto &ass : ctx.assignments )
    {
        if ( !ass.pre_coloured || ass.spilled ) continue;

        if ( const auto [ id ] = ass.reg;
             std::ranges::contains( cc.args_int.span( ),  id )
            || std::ranges::contains( cc.args_float.span( ),  id ) )
        {
            used_reg_count++;
        }
    }

    const u32 incoming_arg_count { param_count - std::min( param_count, used_reg_count ) };

    return FrameLayout { .incoming_args_size     = incoming_arg_count * 8,
                         .setup_area_size        = 0,
                         .clobber_size           = static_cast< u32 >( clobbered_callee_saves.size( ) * 8 ),
                         .fixed_frame_stor_size  = fixed_frame_stor_size,
                         .outgoing_args_size     = 0,
                         .clobbered_callee_saves = clobbered_callee_saves };
}
