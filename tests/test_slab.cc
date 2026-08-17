#include <Catch2/catch_test_macros.hpp>
#include <rasi/support/arena.hh>
#include <rasi/support/slab.hh>

using namespace rasi;

TEST_CASE( "push one element and check idx and value", "[slab]" )
{
    Arena arena { 1024 };
    Slab<int> nums { arena, 5 };

    const auto idx = nums.push( 5 );
    const auto val = nums[idx];

    REQUIRE(idx == 0);
    REQUIRE(val == 5);
}