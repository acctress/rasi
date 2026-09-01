#include "rasi/ir/builder.hh"
#include "rasi/ir/module.hh"
#include "rasi/module/module.hh"

#include <catch2/catch_test_macros.hpp>

using namespace rasi;

TEST_CASE( "codegen: adder execs", "[codegen]" )
{
    Module     m;
    const auto ref = m.new_function( "add", { Type::i64, Type::i64 }, Type::i64 );
    auto      &fn  = m.get_function( ref );
    IRBuilder  builder { fn };

    const auto a = builder.param( 0 );
    const auto b = builder.param( 1 );
    const auto r = builder.iadd( a, b );
    builder.ret( r );

    auto       buf    = jit_compile_fn( fn );
    const auto fn_ptr = jit_fn_ptr< i64 ( * )( i64, i64 ) >( buf );

    REQUIRE( fn_ptr( 3, 4 ) == 7 );
}