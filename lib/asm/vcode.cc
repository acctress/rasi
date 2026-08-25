#include <rasi/asm/vcode.hh>
#include <cassert>
#include <limits>

using namespace rasi::asm_;

VReg VCode::append( const InstKind kind, const std::initializer_list< Operand > ops, const OperandConstraint constraint )
{
    assert( ops.size( ) <= std::numeric_limits< u16 >::max( ) );

    const auto operand_offset = static_cast< u32 >( operands.size( ) );
    for ( const auto &op : ops )
        operands.push( op );

    const auto res = new_vreg( );
    insts.push( MachInst {
        .kind              = kind,
        .operand_offset    = operand_offset,
        .operand_count     = static_cast< u16 >( ops.size( ) ),
        .output_constraint = constraint,
        .result            = res,
    } );

    return res;
}

void VCode::append( const InstKind kind, const std::initializer_list< Operand > ops )
{
    assert( ops.size( ) <= std::numeric_limits< u16 >::max( ) );

    const auto operand_offset = static_cast< u32 >( operands.size( ) );

    for ( const auto &op : ops )
        operands.push( op );

    insts.push( MachInst {
        .kind           = kind,
        .operand_offset = operand_offset,
        .operand_count  = static_cast< u16 >( ops.size( ) ),
    } );
}
