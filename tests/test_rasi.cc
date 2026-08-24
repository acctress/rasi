#include <Catch2/catch_test_macros.hpp>
#include <rasi/ir/module.hh>
#include <rasi/builder.hh>
#include <rasi/isle/selector.hh>
#include <rasi/regalloc/linear_scan.hh>
#include <rasi/codegen/x86_64_assembler.hpp>
#include <windows.h>

using namespace rasi;

TEST_CASE( "add(a, b) -> a + b", "[e2e" )
{
    Module mod;

    const auto ref = mod.new_function( "add", { Type::i64, Type::i64 }, Type::i64 );
    auto fn = mod.get_function( ref );

    IRBuilder builder { fn };
    auto a = builder.param( 0 );
    auto b = builder.param( 1 );
    auto r = builder.iadd( a, b );
    builder.ret( r );

    VCode vcode { &mod.arena };

    isle::Selector sel { fn, vcode };
    sel.lower_args( fn );
    sel.select( fn );

    regalloc::LinearScan regalloc;
    auto alloc = regalloc.run( vcode );

    x86_64::Buffer buf;
    CodeGen cg;


}