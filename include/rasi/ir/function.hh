#pragma once

#include "block.hh"
#include "rasi/support/slab.hh"

#include <string>

namespace rasi
{
    struct Function
    {
        Arena*           arena;
        std::string      name;
        Slab<BasicBlock> blocks;
        Slab<Inst>       instructions;
        Slab<ValueRef>   operands;
        Slab<ValueRef>   block_params;
        Slab<Type>       param_types;
        Slab<Type>       value_types;
        Type             return_type;
        BlockRef         entry_block{};

        explicit Function( const std::string_view name, const Type ret_type, Arena* arena ) :
            arena( arena ),
            name( name ),
            blocks( *arena, 64 ),
            instructions( *arena, 64 ),
            operands( *arena, 64 ),
            block_params( *arena, 64 ),
            param_types( *arena, 64 ),
            value_types( *arena, 64 ),
            return_type( ret_type )
        { }
    };
}