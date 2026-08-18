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
        ValueRef emit( Inst inst ) const;

        /// @brief Helper function for retrieving a parameters ValueRef.
        ValueRef param( u32 idx ) const;

        ValueRef iadd( ValueRef a, ValueRef b );
    };
}