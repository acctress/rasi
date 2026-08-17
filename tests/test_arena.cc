#include <Catch2/catch_test_macros.hpp>
#include <rasi/support/arena.hh>

TEST_CASE( "arena basic allocation", "[arena]" )
{
    rasi::Arena arena { 1024 };

    auto p = arena.alloc( 100, 4 );

    REQUIRE(p != nullptr);
    // Check if the address of p is divisible by 4
    REQUIRE( reinterpret_cast< std::uintptr_t >( p ) % 4 == 0 );
}

TEST_CASE( "allocates dont overlap", "[arena]" )
{
    rasi::Arena arena { 1024 };

    auto p1 = arena.alloc( 24, 4 );
    auto p2 = arena.alloc( 49, 4 );

    REQUIRE(p1 != nullptr);
    REQUIRE(p2 != nullptr);
    // do the addresses of p1 and p2 overlap
    REQUIRE( reinterpret_cast< std::uintptr_t >( p2 ) >= reinterpret_cast<std::uintptr_t>( p1 ) + 24 );
}

TEST_CASE( "allocate an int", "[arena]" )
{
    rasi::Arena arena { 1024 };

    auto i = reinterpret_cast< int* >( arena.alloc( sizeof(int), alignof(int) ) );
    REQUIRE(i != nullptr);

    *i = 5;

    REQUIRE( *i == 5 );
}