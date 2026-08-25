#pragma once

#include "block.hh"
#include <rasi/support/slab.hh>
#include <rasi/ir/call_conv.hh>
#include <string>
#include <cassert>
#include <utility>

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
        Slab< i64 >      immediates;
        Type             return_type;
        BlockRef         entry_block{};
        CallConv         call_conv;

        explicit Function(
            const std::string_view name,
            const std::initializer_list< Type > params,
            const Type ret_type,
            Arena* arena,
            CallConv conv = CallConv::native
        ) :
            arena( arena ),
            name( name ),
            blocks( *arena, 64 ),
            instructions( *arena, 64 ),
            operands( *arena, 64 ),
            block_params( *arena, 64 ),
            param_types( *arena, 64 ),
            value_types( *arena, 64 ),
            immediates( *arena, 64 ),
            return_type( ret_type ),
            call_conv( conv )
        {
            for ( const auto type : params ) param_types.push( type );
            entry_block = new_block( params );
        }

        BlockRef new_block( const std::initializer_list< Type > params )
        {
            assert( std::in_range< u32 >( block_params.size( ) ) );
            assert( std::in_range< u16 >( params.size( ) ) );

            const BasicBlock bb{
                .params_offset = static_cast< u32 >( block_params.size( ) ),
                .params_count = static_cast< u16 >( params.size( ) ),
            };

            for ( const auto& type : params )
            {
                const auto id = static_cast< u32 >( value_types.push( type ) );
                block_params.push( ValueRef{ id } );
            }

            const auto idx = static_cast< u32 >( blocks.push( bb ) );
            const auto ref = BlockRef{ idx };

            blocks[idx].ref = ref;
            return ref;
        }
    };
}