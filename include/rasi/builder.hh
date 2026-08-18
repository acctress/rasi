#pragma once

#include "ir/function.hh"

namespace rasi
{
    struct IRBuilder
    {
        explicit IRBuilder( Function& func, const BlockRef block ) : current_func( &func ), current_block( block ) { }

        Function* current_func  { nullptr };
        BlockRef  current_block { };

        /// @brief Helper function for pushing a variadic number of operands into the pool.
        /// @returns Operand offset.
        template<typename First, typename... Rest>
        u32 push_operands( First first, Rest... rest );

        /// @brief Helper function for creating and pushing instructions into the pool.
        ValueRef emit( const Inst &inst ) const;

        ValueRef iadd( ValueRef a, ValueRef b );
    };
}