#pragma once

#include "ir/function.hh"

namespace rasi
{
    struct IRBuilder
    {
        explicit IRBuilder( Function& func ) : current_func( &func ), current_block( func.entry_block ) { }

        Function* current_func  { nullptr };
        BlockRef  current_block { };

        /// @brief Helper function for pushing a variadic number of operands into the pool.
        /// @returns Operand offset.
        template<typename First, typename... Rest>
        u32 push_operands( First first, Rest... rest );

        /// @brief Helper function for creating and pushing instructions into the pool.
        [[nodiscard]] ValueRef emit( Inst inst ) const;

        /// @brief Helper function for creating and pushing void instructions.
        void emit_void( const Inst &inst ) const;

        /// @brief Helper function for creating and pushing terminator instructions.
        void emit_term( const Inst &inst ) const;

        /// @brief Helper function for retrieving a parameters ValueRef.
        [[nodiscard]] ValueRef param( u32 idx ) const;

        /// @brief Helper for emitting and building an arithmetic instruction.
        template <typename First, typename... Rest>
        [[nodiscard]] ValueRef build_arith_inst( InstKind kind, First first_operand, Rest... other_operands );

        /// @brief Helper for emitting and building a comparison instruction.
        template <typename Predicate, typename First, typename... Rest>
        [[nodiscard]] ValueRef build_comp_inst( InstKind kind, Predicate predicate, First first_operand, Rest... other_operands );

        ValueRef iconst( const i64 val );
        ValueRef iadd( ValueRef a, ValueRef b );
        ValueRef isub( ValueRef a, ValueRef b );
        ValueRef imul( ValueRef a, ValueRef b );
        ValueRef sdiv( ValueRef a, ValueRef b );
        ValueRef udiv( ValueRef a, ValueRef b );
        ValueRef srem( ValueRef a, ValueRef b );
        ValueRef urem( ValueRef a, ValueRef b );
        ValueRef fadd( ValueRef a, ValueRef b );
        ValueRef fsub( ValueRef a, ValueRef b );
        ValueRef fmul( ValueRef a, ValueRef b );
        ValueRef fdiv( ValueRef a, ValueRef b );
        ValueRef band( ValueRef a, ValueRef b );
        ValueRef bor( ValueRef a, ValueRef b );
        ValueRef bxor( ValueRef a, ValueRef b );
        ValueRef shl( ValueRef a, ValueRef b );
        ValueRef shr( ValueRef a, ValueRef b );
        ValueRef sar( ValueRef a, ValueRef b );

        ValueRef icmp( IntCC predicate, ValueRef a, ValueRef b );
        ValueRef fcmp( FloatCC predicate, ValueRef a, ValueRef b );

        ValueRef load( Type T, ValueRef ptr );
        void store( Type T, ValueRef value, ValueRef addr );
        ValueRef alloca_( Type T );

        void ret( ValueRef a );
        void ret( ) const;

        template <typename First, typename... Rest>
        void br( BlockRef block, First first_arg, Rest... rest );
        void br( BlockRef block ) const;

        void cbr( ValueRef cond, BlockRef a, BlockRef b );
    };
}