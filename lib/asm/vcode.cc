#include <rasi/asm/vcode.hh>

using namespace rasi::asm_;

VReg VCode::append( const InstKind kind, const std::initializer_list< Operand > ops, const OperandConstraint constraint )
{
    const auto offset = static_cast< u32 >( operands.size( ) );
    for ( const auto &op : ops )
        operands.push( op );

    const auto result = new_vreg( );
    insts.push( MachInst { .kind              = kind,
                           .operand_offset    = offset,
                           .operand_count     = static_cast< u16 >( ops.size( ) ),
                           .output_constraint = constraint,
                           .result            = result } );

    return result;
}

void VCode::append( const InstKind kind, const std::initializer_list< Operand > ops )
{
    const auto offset = static_cast< u32 >( operands.size( ) );
    for ( const auto &op : ops )
        operands.push( op );

    insts.push(
        MachInst { .kind = kind, .operand_offset = offset, .operand_count = static_cast< u16 >( ops.size( ) ), .result = VReg { ~0u } } );
}
