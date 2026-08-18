#include <rasi/builder.hh>

using namespace rasi;

template < typename First, typename... Rest > u32 IRBuilder::push_operands( First first, Rest... rest )
{
    const auto offset = static_cast< u32 >( current_func->operands.push( first ) );
    (current_func->operands.push(rest), ...);
    return offset;
}

ValueRef IRBuilder::emit( Inst inst ) const
{
    auto& block = current_func->blocks[current_block.id];
    const auto id = static_cast< u32 >( current_func->instructions.size( ) );

    if ( block.instructions_count == 0 ) block.instructions_offset = id;
    else assert( block.instructions_offset + block.instructions_count == id && "block instructions must be contiguous");

    const auto value_id = static_cast< u32 >( current_func->value_types.push( inst.result_type ) );
    const ValueRef result { value_id };

    inst.result = result;
    current_func->instructions.push( inst );
    ++block.instructions_count;

    return result;
}

ValueRef IRBuilder::param( const u32 idx ) const
{
    const auto& block = current_func->blocks[ current_block.id ];
    assert( idx < block.params_count );

    const auto id = current_func->block_params[block.params_offset + idx];

    /// check if the id matches what was pushed in new_block
    assert( current_func->value_types[id.id] == current_func->param_types[idx] );

    return id;
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
