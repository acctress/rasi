#include "rasi/vcode/printer.hh"

#include <iostream>
#include <rasi/asm/x86_64/emit.hh>
#include <rasi/isel/context.hh>
#include <rasi/isel/x86_64/lower.hh>
#include <rasi/module/module.hh>
#include <rasi/regalloc/regalloc.hh>

using namespace rasi;
using namespace rasi::isel;

Buffer rasi::jit_compile_fn( Function &fn )
{
    Arena arena { 1024 * 1024 };
    VCode vcode { &arena };
    vcode.call_conv = fn.call_conv;

    ISELCtx ctx { fn, vcode, x86_64::lower_inst };
    x86_64::lower_function( ctx );

    regalloc::AllocContext alloc_ctx;
    alloc_ctx.num_regs = 16;

    regalloc::compute_liveness( alloc_ctx, vcode );
    regalloc::alloc( alloc_ctx, vcode );
    regalloc::apply( alloc_ctx, vcode );

    print_vcode( std::cout, vcode, alloc_ctx );

    Buffer buff { 1024 * 1024 };
    buff.write( azm::x86_64::emit( vcode, alloc_ctx ).bytes );

    return buff;
}