#include <catch2/catch_test_macros.hpp>
#include <rasi/ir/builder.hh>
#include <rasi/ir/module.hh>
#include <rasi/isel/x86_64/lower.hh>
#include <rasi/regalloc/regalloc.hh>
#include <rasi/vcode/vcode.hh>
#include <sstream>
#include <print>

using namespace rasi;
using namespace rasi::isel;
using namespace rasi::isel::x86_64;
using namespace rasi::regalloc;

static VCode lower( Function& fn, Arena& arena )
{
    VCode   vcode{ &arena };
    ISELCtx ctx{ fn, vcode, lower_inst };
    lower_function( ctx );
    return vcode;
}

static std::string dump_vcode( const VCode& vc )
{
    std::ostringstream os;
    for ( const auto& inst : vc.instructions.as_span() )
        os << mach_inst_name( inst.kind ) << " (operands: " << inst.operand_count << ")\n";
    return os.str();
}

TEST_CASE( "isel: iconst lowers to mov_ri", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", {}, Type::i64 );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    auto c = b.iconst( 42 );
    b.ret( c );

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    REQUIRE( vc.instructions.size() == 2 );
    REQUIRE( vc.instructions[0].kind == MachInstKind::mov );
    REQUIRE( vc.instructions[0].imm  == 42 );
    REQUIRE( vc.instructions[1].kind == MachInstKind::ret );
}

TEST_CASE( "isel: iadd rr lowers to add_rr", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", { Type::i64, Type::i64 }, Type::i64 );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    auto r = b.iadd( b.param(0), b.param(1) );
    b.ret( r );

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    const auto has_add = std::ranges::any_of( vc.instructions.as_span(),
        []( const auto& i ) { return i.kind == MachInstKind::add; } );
    REQUIRE( has_add );
    REQUIRE( vc.instructions.back().kind == MachInstKind::ret );
}

TEST_CASE( "isel: iadd with iconst rhs sinks to add_ri", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", { Type::i64 }, Type::i64 );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    auto c = b.iconst( 10 );
    auto r = b.iadd( b.param(0), c );
    b.ret( r );

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    const auto add_inst = std::ranges::find_if( vc.instructions.as_span(),
        []( const auto& i ) { return i.kind == MachInstKind::add; } );
    REQUIRE( add_inst != vc.instructions.as_span().end() );
    REQUIRE( add_inst->imm == 10 );

    const auto mov_count = std::ranges::count_if( vc.instructions.as_span(),
        []( const auto& i ) { return i.kind == MachInstKind::mov; } );
    REQUIRE( mov_count == 0 );
}

TEST_CASE( "isel: iadd with multi-use iconst does not sink", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", { Type::i64 }, Type::i64 );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    auto c  = b.iconst( 5 );
    auto r1 = b.iadd( b.param(0), c );
    auto r2 = b.iadd( r1, c );
    b.ret( r2 );

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    const auto mov_count = std::ranges::count_if( vc.instructions.as_span(),
        []( const auto& i ) { return i.kind == MachInstKind::mov; } );
    REQUIRE( mov_count >= 1 );
}

TEST_CASE( "isel: isub rr lowers to sub_rr", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", { Type::i64, Type::i64 }, Type::i64 );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    auto r = b.isub( b.param(0), b.param(1) );
    b.ret( r );

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    const auto has_sub = std::ranges::any_of( vc.instructions.as_span(),
        []( const auto& i ) { return i.kind == MachInstKind::sub; } );
    REQUIRE( has_sub );
}

TEST_CASE( "isel: isub with iconst rhs sinks to sub_ri", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", { Type::i64 }, Type::i64 );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    auto c = b.iconst( 3 );
    auto r = b.isub( b.param(0), c );
    b.ret( r );

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    const auto sub_inst = std::ranges::find_if( vc.instructions.as_span(),
        []( const auto& i ) { return i.kind == MachInstKind::sub; } );
    REQUIRE( sub_inst != vc.instructions.as_span().end() );
    REQUIRE( sub_inst->imm == 3 );
}

TEST_CASE( "isel: imul lowers to imul_rr", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", { Type::i64, Type::i64 }, Type::i64 );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    auto r = b.imul( b.param(0), b.param(1) );
    b.ret( r );

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    const auto has_imul = std::ranges::any_of( vc.instructions.as_span(),
        []( const auto& i ) { return i.kind == MachInstKind::imul; } );
    REQUIRE( has_imul );
}

TEST_CASE( "isel: ret void lowers to ret", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", {}, Type::void_ );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    b.ret();

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    REQUIRE( vc.instructions.size() == 1 );
    REQUIRE( vc.instructions[0].kind == MachInstKind::ret );
}

TEST_CASE( "isel: sunk value not emitted twice", "[isel]" )
{
    Arena  arena{ 4 * 1024 * 1024 };
    Module m;
    auto   ref = m.new_function( "f", { Type::i64 }, Type::i64 );
    auto&  fn  = m.get_function( ref );
    IRBuilder b{ fn };
    auto c = b.iconst( 99 );
    auto r = b.iadd( b.param(0), c );
    b.ret( r );

    const auto vc = lower( fn, arena );
    INFO( dump_vcode( vc ) );

    REQUIRE( vc.instructions.size() == 2 );
    REQUIRE( vc.instructions[0].kind == MachInstKind::add );
    REQUIRE( vc.instructions[1].kind == MachInstKind::ret );
}