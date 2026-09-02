#include <catch2/catch_test_macros.hpp>
#include <rasi/ir/builder.hh>
#include <rasi/ir/module.hh>
#include <rasi/support/printer.hh>
#include <sstream>

using namespace rasi;

static std::string print_module( const Module &m )
{
    std::ostringstream os;
    IRPrinter { os }.print( m );
    return os.str( );
}

TEST_CASE( "module: new func added", "[ir]" )
{
    Module     m;
    const auto ref = m.new_function( "foo", {}, Type::void_ );
    REQUIRE( m.get_function( ref ).name == "foo" );
}

TEST_CASE( "module: lookup func by name", "[ir]" )
{
    Module m;
    m.new_function( "foo", {}, Type::void_ );
    const auto result = m.get_function( "foo" );
    REQUIRE( result.has_value( ) );
    REQUIRE( result->get( ).name == "foo" );
}

TEST_CASE( "module: force lookup no real function", "[ir]" )
{
    Module m;
    REQUIRE_FALSE( m.get_function( "foo" ).has_value( ) );
}

TEST_CASE( "function: entry block exists after construction", "[ir]" )
{
    Module      m;
    const auto  ref = m.new_function( "foo", { Type::i64 }, Type::void_ );
    const auto &fn  = m.get_function( ref );
    REQUIRE( fn.blocks.size( ) == 1 );
    REQUIRE( fn.blocks[ fn.entry_block.id ].params_count == 1 );
}

TEST_CASE( "builder: iconst stores immediate", "[ir]" )
{
    Module     m;
    const auto ref = m.new_function( "foo", {}, Type::i64 );
    auto      &fn  = m.get_function( ref );
    IRBuilder  b { fn };

    auto [ id ] = b.iconst( 1234 );
    auto &inst  = fn.instructions[ 0 ];

    REQUIRE( inst.kind == InstKind::iconst );
    REQUIRE( fn.immediates[ inst.imm_idx ] == 1234 );
    REQUIRE( fn.value_types[ id ] == Type::i64 );
}