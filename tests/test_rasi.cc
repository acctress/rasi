#include "rasi/codegen/codegen.hh"
#include <Catch2/catch_test_macros.hpp>
#include <print>
#include <rasi/builder.hh>
#include <rasi/ir/module.hh>
#include <rasi/isle/selector.hh>
#include <rasi/regalloc/linear_scan.hh>
#include <rasi/support/vcode_printer.hh>
#include <windows.h>

using namespace rasi;

TEST_CASE( "add(a, b) -> a + b", "[e2e]" )
{
    Module mod;

    const auto ref = mod.new_function( "add", { Type::i64, Type::i64 }, Type::i64 );
    auto       fn  = mod.get_function( ref );

    IRBuilder builder { fn };
    auto      a = builder.param( 0 );
    auto      b = builder.param( 1 );
    auto      r = builder.iadd( a, b );
    builder.ret( r );

    VCode vcode { &mod.arena };

    isle::Selector sel { fn, vcode };
    sel.lower_args( fn );
    sel.select( fn );

    regalloc::LinearScan regalloc;
    auto                 alloc = regalloc.run( vcode );

    Buffer           buf { 1024 };
    codegen::CodeGen cg;
    cg.emit( vcode, alloc, buf );

    // std::println("{}", rasi::fmt_vcode( vcode ));
    std::println( "{}", fmt_vcode( vcode, alloc ) );

    REQUIRE( buf.make_exec( ) );

    using AddFn = i64 ( * )( i64, i64 );
    auto add    = reinterpret_cast< AddFn >( buf.data( ) );

    REQUIRE( add( 3, 5 ) == 8 );
}