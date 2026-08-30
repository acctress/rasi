#include <catch2/catch_test_macros.hpp>
#include <print>
#include <rasi/asm/x86_64/emit.hh>
#include <rasi/regalloc/regalloc.hh>
#include <rasi/vcode/vcode.hh>

using namespace rasi;
using namespace rasi::regalloc;
using namespace rasi::azm::x86_64;

static x86::Buffer compile( const VCode &vc )
{
    AllocContext ctx;
    ctx.num_regs = 4;
    compute_liveness( ctx, vc );
    alloc( ctx );
    return emit( vc, ctx );
}

static void print_bytes( const x86::Buffer &buf )
{
    for ( const auto b : buf.bytes )
        std::print( "{:02X} ", b );
    std::println( "" );
}

TEST_CASE( "emit: ret encodes 0xc3", "[emit]" )
{
    Arena arena { 1024 * 1024 };
    auto  vc = VCode { &arena };
    vc.append_void( MachInstKind::ret, {} );

    const auto [ bytes ] = compile( vc );
    REQUIRE( bytes.size( ) == 1 );
    REQUIRE( bytes[ 0 ] == 0xc3 );
}

TEST_CASE( "emit: mov rr64", "[emit]" )
{
    Arena arena { 1024 * 1024 };
    auto  vc = VCode { &arena };
    auto  v0 = vc.new_vreg( );
    auto  v1 = vc.new_vreg( );
    auto  v2 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) } );
    vc.append( MachInstKind::mov, v1, { Operand::def( v1 ) } );
    vc.append( MachInstKind::mov, v2, { Operand::def( v2 ), Operand::use( v0 ), Operand::use( v1 ) } );
    vc.append_void( MachInstKind::ret, {} );

    const auto [ bytes ] = compile( vc );
    REQUIRE_FALSE( bytes.empty( ) );
    REQUIRE( bytes.back( ) == 0xC3 );
}

TEST_CASE( "emit: mov ri64 small imm uses mi32", "[emit]" )
{
    Arena arena { 1024 * 1024 };
    auto  vc = VCode { &arena };
    auto  v0 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) }, 42 );
    vc.append_void( MachInstKind::ret, {} );

    const auto [ bytes ] = compile( vc );
    REQUIRE( bytes.size( ) == 8 );
    REQUIRE( bytes.back( ) == 0xC3 );

    const u32 imm = bytes[ 3 ] | ( bytes[ 4 ] << 8 ) | ( bytes[ 5 ] << 16 ) | ( bytes[ 6 ] << 24 );
    REQUIRE( imm == 42 );
}

TEST_CASE( "emit: mov ri64 large imm uses ri64", "[emit]" )
{
    Arena arena { 1024 * 1024 };
    auto  vc = VCode { &arena };
    auto  v0 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) }, 0x1'0000'0000LL );
    vc.append_void( MachInstKind::ret, {} );

    const auto [ bytes ] = compile( vc );
    REQUIRE( bytes.size( ) == 11 );
    REQUIRE( bytes.back( ) == 0xC3 );
}

TEST_CASE( "emit: add rr64", "[emit]" )
{
    Arena arena { 1024 * 1024 };
    auto  vc = VCode { &arena };
    auto  v0 = vc.new_vreg( );
    auto  v1 = vc.new_vreg( );
    auto  v2 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) }, 1 );
    vc.append( MachInstKind::mov, v1, { Operand::def( v1 ) }, 2 );
    vc.append( MachInstKind::add, v2, { Operand::def( v2 ), Operand::use( v0 ), Operand::use( v1 ) } );
    vc.append_void( MachInstKind::ret, {} );

    const auto buf = compile( vc );
    REQUIRE_FALSE( buf.bytes.empty( ) );
    REQUIRE( buf.bytes.back( ) == 0xC3 );

    const auto it = std::ranges::find( buf.bytes, static_cast< u8 >( 0x01 ) );
    REQUIRE( it != buf.bytes.end( ) );
}

TEST_CASE( "emit: add ri8 for small immediates", "[emit]" )
{
    Arena arena{ 1024 * 1024 };
    auto  vc = VCode{ &arena };
    auto  v0 = vc.new_vreg( );
    auto  v1 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def(v0) }, 10 );
    vc.append( MachInstKind::mov, v1, { Operand::def(v1), Operand::use(v0) } ); // copy v0 into v1
    vc.append( MachInstKind::add, v1, { Operand::def(v1) }, 5 );                // add imm in place
    vc.append_void( MachInstKind::ret, {} );

    const auto buf = compile( vc );
    const auto it  = std::ranges::find( buf.bytes, static_cast<u8>( 0x83 ) );
    REQUIRE( it != buf.bytes.end( ) );
}

TEST_CASE( "emit: add ri32 for large immediates", "[emit]" )
{
    Arena arena{ 1024 * 1024 };
    auto  vc = VCode{ &arena };
    auto  v0 = vc.new_vreg( );
    auto  v1 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) }, 10 );
    vc.append( MachInstKind::mov, v1, { Operand::def( v1 ), Operand::use( v0 ) } );
    vc.append( MachInstKind::add, v1, { Operand::def( v1 ) }, 1000 );
    vc.append_void( MachInstKind::ret, {} );

    const auto [ bytes ] = compile( vc );
    const auto it        = std::ranges::find( bytes, static_cast<u8>( 0x81 ) );
    REQUIRE( it != bytes.end( ) );
}

TEST_CASE( "emit: full pipeline add two values", "[emit][.]" )
{
    Arena arena { 1024 * 1024 };
    auto  vc = VCode { &arena };
    auto  v0 = vc.new_vreg( );
    auto  v1 = vc.new_vreg( );
    auto  v2 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) }, 10 );
    vc.append( MachInstKind::mov, v1, { Operand::def( v1 ) }, 20 );
    vc.append( MachInstKind::add, v2, { Operand::def( v2 ), Operand::use( v0 ), Operand::use( v1 ) } );
    vc.append_void( MachInstKind::ret, {} );

    const auto buf = compile( vc );
    std::print( "bytes: " );
    print_bytes( buf );
    REQUIRE( buf.bytes.back( ) == 0xC3 );
}