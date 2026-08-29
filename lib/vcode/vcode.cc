#include <limits>
#include <rasi/vcode/vcode.hh>

using namespace rasi;

VReg VCode::append( const MachInstKind kind, const VReg result, const std::initializer_list< Operand > ops, const i64 imm, const MemRef mem )
{
    assert( ops.size( ) <= std::numeric_limits< u16 >::max( ) );

    const auto offset = static_cast< u32 >( operands.size( ) );
    for ( const auto &op : ops )
        operands.push( op );

    instructions.push( MachInst { .kind           = kind,
                                  .operand_offset = offset,
                                  .operand_count  = static_cast< u16 >( ops.size( ) ),
                                  .result         = result,
                                  .imm            = imm,
                                  .mem            = mem } );

    return result;
}

VReg VCode::append( MachInstKind kind, const VReg result, std::span< const Operand > ops, i64 imm, MemRef mem )
{
    assert( ops.size( ) <= std::numeric_limits< u16 >::max( ) );

    const auto offset = static_cast< u32 >( operands.size( ) );
    for ( const auto &op : ops )
        operands.push( op );

    instructions.push( MachInst { .kind           = kind,
                                  .operand_offset = offset,
                                  .operand_count  = static_cast< u16 >( ops.size( ) ),
                                  .result         = result,
                                  .imm            = imm,
                                  .mem            = mem } );

    return result;
}

void VCode::append_void( const MachInstKind kind, const std::initializer_list< Operand > ops, const i64 imm, const MemRef mem )
{
    assert( ops.size( ) <= std::numeric_limits< u16 >::max( ) );

    const auto offset = static_cast< u32 >( operands.size( ) );
    for ( const auto &op : ops )
        operands.push( op );

    instructions.push(
        MachInst { .kind = kind, .operand_offset = offset, .operand_count = static_cast< u16 >( ops.size( ) ), .imm = imm, .mem = mem } );
}

void VCode::append_void( MachInstKind kind, std::span< const Operand > ops, i64 imm, MemRef mem )
{
    assert( ops.size( ) <= std::numeric_limits< u16 >::max( ) );

    const auto offset = static_cast< u32 >( operands.size( ) );
    for ( const auto &op : ops )
        operands.push( op );

    instructions.push(
        MachInst { .kind = kind, .operand_offset = offset, .operand_count = static_cast< u16 >( ops.size( ) ), .imm = imm, .mem = mem } );
}
