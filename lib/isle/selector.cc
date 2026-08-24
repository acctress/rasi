#include <rasi/isle/selector.hh>

#include <vector>

using namespace rasi::isle;

void Selector::select( const Function &fn )
{
    const auto  val_cnt = fn.value_types.size( );
    std::vector values( fn.value_types.size( ), VReg { ~0u } );

    for ( const auto &blk : fn.blocks )
    {
        for ( u16 i = 0; i < blk.params_count; ++i )
        {
            const auto [ id ] = fn.block_params[ blk.params_offset + 1 ];
            values[ id ]      = m_vcode.new_vreg( );
        }
    }

    for ( const auto &blk : fn.blocks )
    {
        for ( u32 i = blk.instructions_offset; i < blk.instructions_offset + blk.instructions_count; ++i )
        {
            const auto &inst = fn.instructions[ i ];

            auto use = [ & ]( const u16 idx ) -> VReg
            {
                const auto [ id ] = fn.operands[ inst.operand_offset + idx ];
                assert( values[ ref.id ].id != ~0u && "use before def" );
                return values[ id ];
            };

            switch ( inst.kind )
            {
                case InstKind::iadd : values[ inst.result.id ] = lower_iadd( use( 0 ), use( 1 ) ); break;
                case InstKind::isub : values[ inst.result.id ] = lower_isub( use( 0 ), use( 1 ) ); break;
                case InstKind::imul : values[ inst.result.id ] = lower_imul( use( 0 ), use( 1 ) ); break;
                case InstKind::ret : lower_ret( use( 0 ) ); break;
                default : break;
            }
        }
    }
}

VReg Selector::arg( const Inst &inst, const std::size_t idx ) const
{
    assert( idx < inst.operand_count );
    const auto [ id ] = m_fn.operands[ inst.operand_offset + idx ];
    return VReg { id };
}

VReg Selector::lower_iadd( const VReg lhs, const VReg rhs )
{
    return m_vcode.append( InstKind::iadd, { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
                           OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 } );
}

VReg Selector::lower_isub( const VReg lhs, const VReg rhs )
{
    return m_vcode.append( InstKind::isub, { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
                           OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 } );
}

VReg Selector::lower_imul( const VReg lhs, const VReg rhs )
{
    return m_vcode.append( InstKind::imul, { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
                           OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 } );
}

void Selector::lower_ret( const VReg val ) { m_vcode.append( InstKind::ret, { Operand::from_reg( val ) } ); }
