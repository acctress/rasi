#include <rasi/isle/isle_context.hh>

using namespace rasi::isle;

VReg IsleContext::arg( const Inst &inst, const std::size_t idx ) const
{
    assert( idx < inst.operand_count );
    const auto [ id ] = m_fn.operands[ inst.operand_offset + idx ];
    return VReg {  id };
}

VReg IsleContext::emit_iadd( const VReg lhs, const VReg rhs )
{
    return m_vcode.append(
        InstKind::iadd,
        { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
        OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 }
    );
}

VReg IsleContext::emit_isub( const VReg lhs, const VReg rhs )
{
    return m_vcode.append(
        InstKind::isub,
        { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
        OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 }
    );
}

VReg IsleContext::emit_imul( const VReg lhs, const VReg rhs )
{
    return m_vcode.append(
        InstKind::imul,
        { Operand::from_reg( lhs ), Operand::from_reg( rhs ) },
        OperandConstraint { .kind = ConstraintKind::reuse, .reuse_index = 0 }
    );
}

void IsleContext::emit_ret( const VReg val ) {
    m_vcode.append(
        InstKind::ret,
        { Operand::from_reg( val ) }
    );
}
