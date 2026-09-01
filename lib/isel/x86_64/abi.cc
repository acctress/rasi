#include <rasi/isel/x86_64/abi.hh>

#include "rasi/isel/x86_64/prelude.hh"

namespace rasi::isel::x86_64
{
    using namespace registers;

    void emit_prologue( VCode &vcode, FrameLayout &frame )
    {
        /// 16 =
        ///      8 for saved rbp
        ///      +
        ///      8 for return address
        frame.setup_area_size = 16;

        const auto rbp_push = vcode.new_vreg( );
        vcode.append_void( MachInstKind::push_r64, { Operand::use_fixed( rbp_push, PhysReg { RBP } ) } );

        const auto dst = vcode.new_vreg( );
        const auto src = vcode.new_vreg( );
        vcode.append( MachInstKind::mov_rr64, dst,
                      {
                          Operand::def_fixed( dst, PhysReg { RBP } ),
                          Operand::use_fixed( src, PhysReg { RSP } ),
                      } );

        const u32 min_outgoing = ( vcode.call_conv == CallConv::win_fastcall ) ? 32u : 0u;
        const u32 outgoing     = std::max( frame.outgoing_args_size, min_outgoing );
        const u32 stack_size   = frame.fixed_frame_stor_size + frame.clobber_size + outgoing;
        const u32 pre_sub_disp = 16 + frame.clobber_size;

        if ( const u32 sub_sz = ( ( stack_size + pre_sub_disp + 15u ) & ~15u ) - pre_sub_disp; sub_sz > 0 )
        {
            const auto rsp = vcode.new_vreg( );
            vcode.append_void( sub_sz <= 127 ? MachInstKind::sub_ri8 : MachInstKind::sub_ri32,
                               { Operand::def_fixed( rsp, PhysReg { RSP } ), Operand::use_fixed( rsp, PhysReg { RSP } ) }, sub_sz );
        }

        const u32 clobber_base = outgoing + frame.fixed_frame_stor_size;
        u32       cur_offset   = 0;
        for ( const auto preg : frame.clobbered_callee_saves )
        {
            const auto rsp = vcode.new_vreg( );
            const auto reg = vcode.new_vreg( );
            vcode.append_void( MachInstKind::store_mr64,
                               {
                                   Operand::use_fixed( reg, preg ),
                                   Operand::use_fixed( rsp, PhysReg { RSP } ),
                               },
                               0, MemRef { rsp, static_cast< i32 >( clobber_base + cur_offset ) } );
            cur_offset += 8;
        }
    }

    void emit_epilogue( VCode &vcode, FrameLayout &frame )
    {
        const u32 min_outgoing = ( vcode.call_conv == CallConv::win_fastcall ) ? 32u : 0u;
        const u32 outgoing     = std::max( frame.outgoing_args_size, min_outgoing );
        const u32 stack_size   = frame.fixed_frame_stor_size + frame.clobber_size + outgoing;
        const u32 pre_sub_disp = 16 + frame.clobber_size;
        const u32 sub_sz       = ( ( stack_size + pre_sub_disp + 15u ) & ~15u ) - pre_sub_disp;

        const u32 clobber_base = outgoing + frame.fixed_frame_stor_size;
        for ( auto i { frame.clobbered_callee_saves.size( ) }; i > 0; --i )
        {
            const auto preg   = frame.clobbered_callee_saves[ i ];
            const auto rsp    = vcode.new_vreg( );
            const auto reg    = vcode.new_vreg( );
            const u32  offset = clobber_base + static_cast< u32 >( ( i - 1 ) * 8 );

            vcode.append( MachInstKind::load_rm64, reg,
                          {
                              Operand::def_fixed( reg, preg ),
                              Operand::use_fixed( rsp, PhysReg { RSP } ),
                          },
                          0, MemRef { rsp, static_cast< i32 >( offset ) } );
        }

        if ( sub_sz > 0 )
        {
            const auto rsp = vcode.new_vreg( );
            vcode.append_void( sub_sz <= 127 ? MachInstKind::add_ri8 : MachInstKind::add_ri32,
                               { Operand::def_fixed( rsp, PhysReg { RSP } ), Operand::use_fixed( rsp, PhysReg { RSP } ) }, sub_sz );
        }

        const auto rsp_dst = vcode.new_vreg( );
        const auto rbp_src = vcode.new_vreg( );
        vcode.append(MachInstKind::mov_rr64, rsp_dst, {
            Operand::def_fixed( rsp_dst, PhysReg { RSP } ),
            Operand::use_fixed( rbp_src, PhysReg { RBP } ),
        });

        const auto rbp = vcode.new_vreg( );
        vcode.append_void( MachInstKind::pop_r64, { Operand::def_fixed( rbp, PhysReg { RBP } ) } );
    }
} // namespace rasi::isel::x86_64