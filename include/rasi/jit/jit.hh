#pragma once

#include <rasi/asm/assembler.hh>
#include <rasi/asm/buffer.hh>
#include <rasi/asm/vcode.hh>
#include <rasi/ir/function.hh>
#include <rasi/isle/selector.hh>
#include <rasi/regalloc/linear_scan.hh>

namespace rasi::jit
{
    class JIT
    {
    public:
        explicit JIT( Arena *arena ) : m_arena( arena ) { }

        template < typename Fn > Fn *compile( Function &fn ) { return reinterpret_cast< Fn * >( compile_impl( fn ) ); }

    private:
        Arena *m_arena;

        void *compile( Function &fn );
    };
} // namespace rasi::jit