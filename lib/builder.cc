#include <rasi/builder.hh>

using namespace rasi;

template < typename First, typename... Rest >
u32 IRBuilder::push_operands( First first, Rest... rest )
{
    const auto offset = static_cast< u32 >( current_func->operands.push( first ) );
    ( current_func->operands.push( rest ), ... );
    return offset;
}

ValueRef IRBuilder::emit( Inst inst ) const
{
    auto      &block = current_func->blocks[ current_block.id ];
    const auto id    = static_cast< u32 >( current_func->instructions.size( ) );

    if ( block.instructions_count == 0 ) block.instructions_offset = id;
    else
        assert( block.instructions_offset + block.instructions_count == id && "block instructions must be contiguous" );

    const auto     value_id = static_cast< u32 >( current_func->value_types.push( inst.result_type ) );
    const ValueRef result { value_id };

    inst.result = result;
    current_func->instructions.push( inst );
    ++block.instructions_count;

    return result;
}

void IRBuilder::emit_term( const Inst &inst ) const
{
    auto      &block = current_func->blocks[ current_block.id ];
    const auto id    = static_cast< u32 >( current_func->instructions.size( ) );

    if ( block.instructions_count == 0 ) block.instructions_offset = id;
    else
        assert( block.instructions_offset + block.instructions_count == id && "block instructions must be contiguous" );

    current_func->instructions.push( inst );
    ++block.instructions_count;
}

ValueRef IRBuilder::param( const u32 idx ) const
{
    const auto &block = current_func->blocks[ current_block.id ];
    assert( idx < block.params_count );

    const auto id = current_func->block_params[ block.params_offset + idx ];

    /// check if the id matches what was pushed in new_block
    assert( current_func->value_types[ id.id ] == current_func->param_types[ idx ] );

    return id;
}

template < typename First, typename... Rest >
ValueRef IRBuilder::build_arith_inst( const InstKind kind, First first_operand, Rest... other_operands )
{
    const auto ret_type = current_func->value_types[ first_operand.id ];
    const auto offset   = push_operands( first_operand, other_operands... );
    return emit( Inst {
        .kind           = kind,
        .result_type    = ret_type,
        .operand_offset = offset,
        .operand_count  = 1 + sizeof...(Rest),
    } );
}

template < typename Predicate, typename First, typename... Rest >
ValueRef IRBuilder::build_comp_inst( const InstKind kind, const Predicate predicate, First first_operand, Rest... other_operands )
{
    const auto ret_type = current_func->value_types[ first_operand.id ];
    const auto offset   = push_operands( first_operand, other_operands... );

    if constexpr ( std::is_same_v< Predicate, IntCC> )
        return emit( Inst {
            .kind           = kind,
            .result_type    = ret_type,
            .operand_offset = offset,
            .operand_count  = 1 + sizeof...(Rest),
            .predicate      = { .icc = predicate },
        } );
    else
        return emit( Inst {
            .kind           = kind,
            .result_type    = ret_type,
            .operand_offset = offset,
            .operand_count  = 1 + sizeof...(Rest),
            .predicate      = { .fcc = predicate },
        } );
}

ValueRef IRBuilder::iadd( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::iadd, a, b );
}

ValueRef IRBuilder::isub( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::isub, a, b );
}

ValueRef IRBuilder::imul( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::imul, a, b );
}

ValueRef IRBuilder::sdiv( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::sdiv, a, b );
}

ValueRef IRBuilder::udiv( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::udiv, a, b );
}

ValueRef IRBuilder::srem( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::srem, a, b );
}

ValueRef IRBuilder::urem( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::urem, a, b );
}

ValueRef IRBuilder::fadd( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::fadd, a, b );
}

ValueRef IRBuilder::fsub( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::fsub, a, b );
}

ValueRef IRBuilder::fmul( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::fmul, a, b );
}

ValueRef IRBuilder::fdiv( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::fdiv, a, b );
}

ValueRef IRBuilder::band( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::and_, a, b );
}

ValueRef IRBuilder::bor( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::or_, a, b );
}

ValueRef IRBuilder::bxor( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::xor_, a, b );
}

ValueRef IRBuilder::shl( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::shl, a, b );
}

ValueRef IRBuilder::shr( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::shr, a, b );
}

ValueRef IRBuilder::sar( const ValueRef a, const ValueRef b )
{
    return build_arith_inst( InstKind::sar, a, b );
}

ValueRef IRBuilder::icmp( const IntCC predicate, const ValueRef a, const ValueRef b )
{
    return build_comp_inst( InstKind::icmp, predicate, a, b );
}

ValueRef IRBuilder::fcmp( const FloatCC predicate, const ValueRef a, const ValueRef b )
{
    return build_comp_inst( InstKind::fcmp, predicate, a, b );
}

void IRBuilder::ret( const ValueRef a )
{
    const auto offset = push_operands( a );
    emit_term( Inst {
        .kind           = InstKind::ret,
        .result_type    = Type::void_,
        .operand_offset = offset,
        .operand_count  = 1,
    } );
}

void IRBuilder::ret( ) const
{
    emit_term( Inst {
        .kind           = InstKind::ret,
        .result_type    = Type::void_,
    } );
}

template < typename First, typename... Rest > void
IRBuilder::br( const BlockRef block, First first_arg, Rest... rest )
{
    const auto offset   = push_operands( first_arg, rest... );
    emit_term( Inst {
        .kind           = InstKind::br,
        .result_type    = Type::void_,
        .target_block   = block,
        .operand_offset = offset,
        .operand_count  = 1 + sizeof...( rest ),
    } );
}

void IRBuilder::br( const BlockRef block ) const
{
    emit_term( Inst {
        .kind           = InstKind::br,
        .result_type    = Type::void_,
        .target_block   = block,
    } );
}

void IRBuilder::cbr( const ValueRef cond, const BlockRef a, const BlockRef b )
{
    const auto offset = push_operands( cond );
    emit_term( Inst {
        .kind           = InstKind::cbr,
        .result_type    = Type::void_,
        .operand_offset = offset,
        .operand_count  = 1,
        .target_block   = a,
        .false_block    = b,
    } );
}
