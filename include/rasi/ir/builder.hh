#pragma once

#include <rasi/ir/function.hh>

namespace rasi
{
    struct IRBuilder
    {
        explicit IRBuilder( Function &func ) : current_func( &func ), current_block( func.entry_block ) { }

        Function *current_func { nullptr };
        BlockRef  current_block {};

        [[nodiscard]] ValueRef param( const u32 idx ) const
        {
            const auto &block = current_func->blocks[ current_block.id ];
            assert( idx < block.params_count );
            const auto id = current_func->block_params[ block.params_offset + idx ];
            assert( current_func->value_types[ id.id ] == current_func->param_types[ idx ] );
            return id;
        }

        template < typename First, typename... Rest > u32 push_operands( First first, Rest... rest )
        {
            const auto offset = static_cast< u32 >( current_func->operands.push( first ) );
            ( current_func->operands.push( rest ), ... );
            return offset;
        }

    private:
        ValueRef emit_raw( Inst inst, const bool has_result ) const
        {
            auto      &block = current_func->blocks[ current_block.id ];
            const auto id    = static_cast< u32 >( current_func->instructions.size( ) );

            if ( block.instructions_count == 0 ) block.instructions_offset = id;
            else
                assert( block.instructions_offset + block.instructions_count == id && "block instructions must be contiguous" );

            if ( has_result )
            {
                const auto value_id = static_cast< u32 >( current_func->value_types.push( inst.result_type ) );
                inst.result         = ValueRef { value_id };
            }

            current_func->instructions.push( inst );
            ++block.instructions_count;
            return inst.result;
        }

        template < typename First, typename... Rest >
        [[nodiscard]] ValueRef build_arith_inst( const InstKind kind, First first_operand, Rest... other_operands )
        {
            const auto ret_type = current_func->value_types[ first_operand.id ];
            const auto offset   = push_operands( first_operand, other_operands... );
            return emit_raw(
                Inst {
                    .kind           = kind,
                    .result_type    = ret_type,
                    .operand_offset = offset,
                    .operand_count  = 1 + sizeof...( Rest ),
                },
                true );
        }

        template < typename Predicate, typename First, typename... Rest >
        [[nodiscard]] ValueRef build_comp_inst( InstKind kind, Predicate predicate, First first_operand, Rest... other_operands )
        {
            const auto ret_type = current_func->value_types[ first_operand.id ];
            const auto offset   = push_operands( first_operand, other_operands... );

            Inst inst {
                .kind           = kind,
                .result_type    = ret_type,
                .operand_offset = offset,
                .operand_count  = 1 + sizeof...( Rest ),
            };

            if constexpr ( std::is_same_v< Predicate, IntCC > ) inst.predicate = { .icc = predicate };
            else
                inst.predicate = { .fcc = predicate };

            return emit_raw( inst, true );
        }

    public:
        ValueRef iconst( const i64 val )
        {
            const auto idx = static_cast< u32 >( current_func->immediates.push( val ) );
            return emit_raw( Inst { .kind = InstKind::iconst, .result_type = Type::i64, .imm_idx = idx }, true );
        }

        ValueRef iadd( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::iadd, a, b ); }

        ValueRef isub( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::isub, a, b ); }

        ValueRef imul( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::imul, a, b ); }

        ValueRef sdiv( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::sdiv, a, b ); }

        ValueRef udiv( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::udiv, a, b ); }

        ValueRef srem( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::srem, a, b ); }

        ValueRef urem( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::urem, a, b ); }

        ValueRef fadd( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::fadd, a, b ); }

        ValueRef fsub( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::fsub, a, b ); }

        ValueRef fmul( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::fmul, a, b ); }

        ValueRef fdiv( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::fdiv, a, b ); }

        ValueRef band( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::and_, a, b ); }

        ValueRef bor( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::or_, a, b ); }

        ValueRef bxor( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::xor_, a, b ); }

        ValueRef shl( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::shl, a, b ); }

        ValueRef shr( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::shr, a, b ); }

        ValueRef sar( const ValueRef a, const ValueRef b ) { return build_arith_inst( InstKind::sar, a, b ); }

        ValueRef icmp( const IntCC predicate, const ValueRef a, const ValueRef b )
        {
            return build_comp_inst( InstKind::icmp, predicate, a, b );
        }

        ValueRef fcmp( const FloatCC predicate, const ValueRef a, const ValueRef b )
        {
            return build_comp_inst( InstKind::fcmp, predicate, a, b );
        }

        ValueRef load( const Type T, const ValueRef ptr )
        {
            const auto offset = push_operands( ptr );
            return emit_raw( Inst { .kind = InstKind::load, .result_type = T, .operand_offset = offset, .operand_count = 1 }, true );
        }

        void store( const Type T, const ValueRef value, const ValueRef addr )
        {
            const auto offset = push_operands( value, addr );
            emit_raw( Inst { .kind = InstKind::store, .t_type = T, .operand_offset = offset, .operand_count = 2 }, false );
        }

        ValueRef alloca_( const Type T )
        {
            return emit_raw( Inst { .kind = InstKind::alloca_, .result_type = Type::ptr, .t_type = T }, true );
        }

        void ret( const ValueRef a )
        {
            const auto offset = push_operands( a );
            emit_raw( Inst { .kind = InstKind::ret, .result_type = Type::void_, .operand_offset = offset, .operand_count = 1 }, false );
        }

        void ret( ) const { emit_raw( Inst { .kind = InstKind::ret, .result_type = Type::void_ }, false ); }

        template < typename First, typename... Rest > void br( const BlockRef block, First first_arg, Rest... rest )
        {
            const auto offset = push_operands( first_arg, rest... );
            emit_raw( Inst { .kind           = InstKind::br,
                             .result_type    = Type::void_,
                             .target_block   = block,
                             .operand_offset = offset,
                             .operand_count  = 1 + sizeof...( rest ) },
                      false );
        }

        void br( const BlockRef block ) const
        {
            emit_raw( Inst { .kind = InstKind::br, .result_type = Type::void_, .target_block = block }, false );
        }

        void cbr( ValueRef const cond, const BlockRef a, const BlockRef b )
        {
            const auto offset = push_operands( cond );
            emit_raw( Inst { .kind           = InstKind::cbr,
                             .result_type    = Type::void_,
                             .operand_offset = offset,
                             .operand_count  = 1,
                             .target_block   = a,
                             .false_block    = b },
                      false );
        }
    };
} // namespace rasi