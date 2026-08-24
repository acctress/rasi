#include <rasi/isle/selector.hh>

#include <array>
#include <vector>

using namespace rasi::asm_;

#ifdef _WIN32
static constexpr std::array arg_regs = { regs::rcx, regs::rdx, regs::r8, regs::r9 };
#else
static constexpr std::array arg_regs = { regs::rdi, regs::rsi, regs::rdx, regs::rcx, regs::r8, regs::r9 };
#endif

using namespace rasi::isle;

void Selector::select( const Function &fn )
{
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
        const auto vreg
            = m_vcode.append( InstKind::copy, {}, OperandConstraint { .kind = ConstraintKind::fixed, .fixed_reg = arg_regs[ i ] } );

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
