#include <catch2/catch_test_macros.hpp>
#include <print>
#include <rasi/asm/x86_64/emit.hh>
#include <rasi/regalloc/regalloc.hh>
#include <rasi/vcode/vcode.hh>

using namespace rasi;
using namespace rasi::regalloc;
using namespace rasi::azm::x86_64;

static Arena g_arena { 1024 * 1024 };

static VCode make_vcode( ) { return VCode { &g_arena }; }

static void print_bytes( const x86::Buffer &buf )
{
    for ( const auto b : buf.bytes )
        std::print( "{:02X} ", b );
    std::println( "" );
}

static x86::Buffer compile( const VCode &vc, const u8 n_regs = 4 )
{
    AllocContext ctx;
    ctx.num_regs = n_regs;
    compute_liveness( ctx, vc );
    alloc( ctx );
    return emit( vc, ctx );
}

TEST_CASE( "emit: ret encodes 0xc3", "[emit]" )
{
    auto vc = make_vcode( );
    vc.append_void( MachInstKind::ret, {} );

    const auto [ bytes ] = compile( vc );
    REQUIRE(  bytes.size( ) == 1 );
    REQUIRE(  bytes[ 0 ] == 0xc3 );
}
