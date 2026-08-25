#include "rasi/asm/x86_64/abi.hh"
#include <rasi/isle/selector.hh>

using namespace rasi::asm_;
using namespace rasi::isle;

void Selector::select( const Function &fn )
{
    m_vcode.call_conv = x86_64::resolve_cc( fn.call_conv );
    lower_args( fn );

    for ( const auto &blk : fn.blocks )
    {
        for ( u32 i = blk.instructions_offset; i < blk.instructions_offset + blk.instructions_count; ++i )
        {
            const auto &inst = fn.instructions[ i ];

            auto use = [ & ]( const u16 idx ) -> VReg
            {
                const auto [ id ] = fn.operands[ inst.operand_offset + idx ];
                const auto iter   = m_value_map.find( id );
                assert( iter != m_value_map.end( ) && "use before def" );
                return iter->second;
            };

            switch ( inst.kind )
            {
                case InstKind::iconst :
                {
                    const auto r                  = lower_iconst( m_fn.immediates[ inst.imm_idx ] );
                    m_value_map[ inst.result.id ] = r;
                    break;
                }
                case InstKind::iadd :
                {
                    const auto r                  = lower_iadd( use( 0 ), use( 1 ) );
                    m_value_map[ inst.result.id ] = r;
                    break;
                }
                case InstKind::isub :
                {
                    const auto r                  = lower_isub( use( 0 ), use( 1 ) );
                    m_value_map[ inst.result.id ] = r;
                    break;
                }
                case InstKind::imul :
                {
                    const auto r                  = lower_imul( use( 0 ), use( 1 ) );
                    m_value_map[ inst.result.id ] = r;
                    break;
                }
                case InstKind::ret : lower_ret( use( 0 ) ); break;
                default : break;
            }
        }
    }
}

void Selector::lower_args( const Function &fn )
{
    const auto &e = fn.blocks[ fn.entry_block.id ];
    for ( u16 i = 0; i < e.params_count; ++i )
    {
        const auto [ id ] = fn.block_params[ e.params_offset + i ];
        const auto vreg   = m_vcode.append(
            InstKind::copy, {}, OperandConstraint { .kind = ConstraintKind::fixed, .fixed_reg = x86_64::arg_reg( fn.call_conv, i ) } );

        m_value_map[ id ] = vreg;
    }
}

VReg Selector::arg( const Inst &inst, const std::size_t idx ) const
{
    assert( idx < inst.operand_count );
    const auto [ id ] = m_fn.operands[ inst.operand_offset + idx ];
    const auto iter   = m_value_map.find( id );
    assert( iter != m_value_map.end( ) && "use before def" );
    return iter->second;
}

i64 Selector::imm( const Inst &inst ) const { return m_fn.immediates[ inst.imm_idx ]; }

VReg Selector::lower_iconst( const i64 imm ) const
{
    return m_vcode.append( InstKind::iconst, { Operand::from_imm( imm ) }, OperandConstraint { .kind = ConstraintKind::any } );
}

VReg Selector::lower_iadd( const VReg lhs, const VReg rhs ) const
{
    return m_vcode.append( InstKind::iadd, { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
                           OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 } );
}

VReg Selector::lower_isub( const VReg lhs, const VReg rhs ) const
{
    return m_vcode.append( InstKind::isub, { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
                           OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 } );
}

VReg Selector::lower_imul( const VReg lhs, const VReg rhs ) const
{
    return m_vcode.append( InstKind::imul, { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
                           OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 } );
}

void Selector::lower_ret( const VReg val ) const { m_vcode.append( InstKind::ret, { Operand::from_reg( val ) } ); }
