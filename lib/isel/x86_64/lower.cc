#include <rasi/isel/x86_64/lower.hh>
#include <rasi/isel/x86_64/prelude.hh>
#include <rasi/vcode/call_conv.hh>

using namespace rasi;
using namespace rasi::isel;
using namespace rasi::isel::x86_64;

VReg x86_64::lower_inst( ISELCtx &ctx, const Inst &inst ) noexcept
{
    if ( const auto imm_idx = extract_iconst( inst ) ) return construct_mov_ri( ctx.vcode, ctx.fn.immediates[ *imm_idx ] );

    if ( const auto ops = extract_iadd( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        if ( const auto *rhs_prod = ctx.producer( rhs ); rhs_prod && ctx.legal_sink( rhs ) )
            if ( const auto imm_idx = extract_iconst( *rhs_prod ) )
                return construct_add_ri( ctx.vcode, ctx.lower_value( lhs ), ctx.fn.immediates[ *imm_idx ] );
        return construct_add_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_isub( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        if ( const auto *rhs_prod = ctx.producer( rhs ); rhs_prod && ctx.legal_sink( rhs ) )
            if ( const auto imm_idx = extract_iconst( *rhs_prod ) )
                return construct_sub_ri( ctx.vcode, ctx.lower_value( lhs ), ctx.fn.immediates[ *imm_idx ] );
        return construct_sub_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_imul( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_imul_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_sdiv( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_idiv_r( ctx.vcode, ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_udiv( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_udiv_r( ctx.vcode, ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_and( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_and_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_or( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_or_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_xor( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_xor_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_shl( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_shl_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_shr( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_shr_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ops = extract_sar( ctx.fn, inst ) )
    {
        const auto [ lhs, rhs ] = *ops;
        return construct_sar_rr( ctx.vcode, ctx.lower_value( lhs ), ctx.lower_value( rhs ) );
    }

    if ( const auto ptr = extract_load( ctx.fn, inst ) ) return construct_load( ctx.vcode, ctx.lower_value( *ptr ), 0 );

    if ( const auto ops = extract_store( ctx.fn, inst ) )
    {
        const auto [ value, addr ] = *ops;
        construct_store( ctx.vcode, ctx.lower_value( value ), ctx.lower_value( addr ), 0 );
        return VReg {};
    }

    if ( const auto val = extract_ret( ctx.fn, inst ) )
    {
        construct_ret( ctx.vcode, ctx.lower_value( *val ) );
        return VReg {};
    }

    if ( extract_ret_void( inst ) )
    {
        construct_ret_void( ctx.vcode );
        return VReg {};
    }

    return VReg {};
}

void x86_64::lower_function( ISELCtx &ctx ) noexcept
{
    const auto &cc      = conv_regs( ctx.fn.call_conv );
    u8          arg_idx = 0;

    for ( const auto &block : ctx.fn.blocks.as_span( ) )
        for ( u32 i { block.params_offset }; i < block.params_offset + block.params_count; ++i )
        {
            const auto param_ref = ctx.fn.block_params[ i ];
            const auto reg       = ctx.vcode.new_vreg( );
            ctx.cache( param_ref, reg );

            if ( arg_idx < cc.args_int.count )
            {
                const PhysReg preg { cc.args_int.data[ arg_idx++ ] };
                ctx.vcode.append_void( MachInstKind::nop, { Operand::def_fixed( reg, preg ) } );
            }
        }

    for ( const auto &block : ctx.fn.blocks.as_span( ) )
        for ( u32 i { block.instructions_offset }; i < block.instructions_offset + block.instructions_count; ++i )
        {
            if ( const auto &inst = ctx.fn.instructions[ i ];
                 inst.kind == InstKind::ret || inst.kind == InstKind::br || inst.kind == InstKind::cbr || inst.kind == InstKind::store )
                lower_inst( ctx, inst );
        }
}
