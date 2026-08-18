#include <Catch2/catch_test_macros.hpp>
#include <array>
#include <rasi/builder.hh>
#include <rasi/ir/module.hh>

using namespace rasi;

TEST_CASE( "basic module test", "[ir]" )
{
    Module module;

    const auto fn_ref = module.new_function( "foo", { Type::i64 }, Type::i64 );
    auto& fn          = module.get_function( fn_ref );

    IRBuilder builder { fn };

    auto a   = builder.param( 0 );
    auto res = builder.iadd( a, a );

    REQUIRE( res.id == 1 );
    REQUIRE( fn.instructions[0].result.id == 1 );
    REQUIRE( fn.blocks[0].instructions_count == 1 );
}
