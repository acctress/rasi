#pragma once

#include <rasi/ir/function.hh>
#include <rasi/support/buffer.hh>

namespace rasi
{
    Buffer jit_compile_fn( Function &fn );

    template < typename Fn >
    [[nodiscard]] Fn jit_fn_ptr( Buffer &buf )
    {
        buf.make_exec( );
        return reinterpret_cast< Fn >( buf.data( ) );
    }
} // namespace rasi