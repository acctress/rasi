#include <rasi/builder.hh>

using namespace rasi;

template < typename First, typename... Rest > u32 IRBuilder::push_operands( First first, Rest... rest )
{
    const auto offset = static_cast< u32 >( current_func->operands.push( first ) );
    (current_func->operands.push(rest), ...);
    return offset;
}

ValueRef IRBuilder::emit( const Inst &inst ) const
{
    const u32 id = static_cast< u32 >( current_func->instructions.push( inst ) );
    current_func->value_types.push( inst.result_type );
    return ValueRef{ id };
}

ValueRef IRBuilder::param( const u32 idx ) const
{
    const auto& block = current_func->blocks[ current_block.id ];
    assert( idx < block.params_count );
    return current_func->block_params[block.params_offset + idx];
}

ValueRef IRBuilder::iadd( const ValueRef a, const ValueRef b )
{
    const auto ret_type = current_func->value_types[a.id];
    const auto offset = push_operands( a, b );
    return emit( Inst{
        .kind           = InstKind::iadd,
        .result_type    = ret_type,
        .operand_offset = offset,
        .operand_count = 2,
    } );
}
