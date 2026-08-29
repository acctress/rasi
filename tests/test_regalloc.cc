#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <print>
#include <rasi/regalloc/regalloc.hh>
#include <rasi/vcode/vcode.hh>
#include <sstream>

using namespace rasi;
using namespace rasi::regalloc;

static Arena g_arena { 1024 * 1024 };

static VCode make_vcode( ) { return VCode { &g_arena }; }

static std::string visualise( const AllocContext &ctx )
{
    constexpr auto RESET  = "\033[0m";
    constexpr auto BOLD   = "\033[1m";
    constexpr auto CYAN   = "\x1b[38;5;147m";
    constexpr auto GREEN  = "\x1b[38;5;157m";
    constexpr auto RED    = "\x1b[38;5;211m";
    constexpr auto YELLOW = "\x1b[38;5;229m";
    constexpr auto DIM    = "\033[2m";

    constexpr int VREG_WIDTH   = 6;
    constexpr int START_WIDTH  = 7;
    constexpr int END_WIDTH    = 5;
    constexpr int USES_WIDTH   = 6;
    constexpr int WEIGHT_WIDTH = 10;

    std::ostringstream os;

    os << BOLD << CYAN << std::left << std::setw( VREG_WIDTH ) << "VReg" << std::setw( START_WIDTH ) << "Start" << std::setw( END_WIDTH )
       << "End" << std::setw( USES_WIDTH ) << "Uses" << std::setw( WEIGHT_WIDTH ) << "Weight"
       << "Assignment\n"
       << std::setw( VREG_WIDTH ) << "----" << std::setw( START_WIDTH ) << "-----" << std::setw( END_WIDTH ) << "---"
       << std::setw( USES_WIDTH ) << "----" << std::setw( WEIGHT_WIDTH ) << "--------"
       << "----------\n"
       << RESET;

    for ( u32 i = 0; i < ctx.live_ranges.size( ); ++i )
    {
        const auto &lr = ctx.live_ranges[ i ];

        os << YELLOW << std::setw( VREG_WIDTH ) << ( "v" + std::to_string( lr.vreg.id ) ) << RESET

           << DIM << std::setw( START_WIDTH ) << lr.start << std::setw( END_WIDTH ) << lr.end << RESET

           << std::setw( USES_WIDTH ) << lr.uses << std::setw( WEIGHT_WIDTH ) << lr.spill_weight( );

        if ( i < ctx.assignments.size( ) )
        {
            if ( const auto &assignment = ctx.assignments[ i ]; assignment.spilled )
            {
                os << RED << "spill[" << assignment.slot.offset << "]" << RESET;
            }
            else
            {
                os << GREEN << "r" << static_cast< int >( assignment.reg.id ) << RESET;
            }
        }

        os << '\n';
    }

    auto result = os.str( );
    std::print( "{}", result );
    return result;
}

TEST_CASE( "liveness: single vreg use", "[regalloc]" )
{
    auto vc = make_vcode( );
    auto v0 = vc.new_vreg( );
    auto v1 = vc.new_vreg( );
    vc.append( MachInstKind::add, v0, { Operand::def( v0 ), Operand::use( v1 ) } );

    AllocContext ctx;
    ctx.num_regs = 4;
    compute_liveness( ctx, vc );

    REQUIRE( ctx.live_ranges.size( ) >= 2 );
    REQUIRE( ctx.live_ranges[ v0.id ].uses == 1 );
    REQUIRE( ctx.live_ranges[ v1.id ].uses == 1 );
}

TEST_CASE( "liveness: vreg used across multiple insts", "[regalloc]" )
{
    auto vc = make_vcode( );
    auto v0 = vc.new_vreg( );
    auto v1 = vc.new_vreg( );
    auto v2 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) } );
    vc.append( MachInstKind::add, v1, { Operand::def( v1 ), Operand::use( v0 ) } );
    vc.append( MachInstKind::add, v2, { Operand::def( v2 ), Operand::use( v0 ) } );

    AllocContext ctx;
    ctx.num_regs = 4;
    compute_liveness( ctx, vc );

    const auto &lr = ctx.live_ranges[ v0.id ];
    REQUIRE( lr.start == 0 );
    REQUIRE( lr.end == 3 );
    REQUIRE( lr.uses == 3 );
}

TEST_CASE( "liveness: non-overlapping ranges", "[regalloc]" )
{
    auto vc = make_vcode( );
    auto v0 = vc.new_vreg( );
    auto v1 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) } );
    vc.append( MachInstKind::add, v0, { Operand::use( v0 ) } );
    vc.append( MachInstKind::mov, v1, { Operand::def( v1 ) } );
    vc.append( MachInstKind::add, v1, { Operand::use( v1 ) } );

    AllocContext ctx;
    ctx.num_regs = 4;
    compute_liveness( ctx, vc );

    REQUIRE_FALSE( ctx.live_ranges[ v0.id ].overlaps( ctx.live_ranges[ v1.id ] ) );
}

TEST_CASE( "alloc: fits in registers", "[regalloc]" )
{
    auto vc = make_vcode( );
    auto v0 = vc.new_vreg( );
    auto v1 = vc.new_vreg( );
    auto v2 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) } );
    vc.append( MachInstKind::mov, v1, { Operand::def( v1 ) } );
    vc.append( MachInstKind::add, v2, { Operand::use( v0 ), Operand::use( v1 ) } );

    AllocContext ctx;
    ctx.num_regs = 4;
    compute_liveness( ctx, vc );
    alloc( ctx );

    INFO( visualise( ctx ) );
    REQUIRE( ctx.assignments[ v0.id ].spilled == false );
    REQUIRE( ctx.assignments[ v1.id ].spilled == false );
}

TEST_CASE( "alloc: spills when regs exhausted", "[regalloc]" )
{
    auto                vc = make_vcode( );
    std::vector< VReg > vregs;
    for ( int i = 0; i < 6; ++i )
    {
        auto v = vc.new_vreg( );
        vregs.push_back( v );
        vc.append( MachInstKind::mov, v, { Operand::def( v ) } );
    }
    std::vector< Operand > ops;
    for ( const auto &v : vregs )
        ops.push_back( Operand::use( v ) );
    vc.append_void( MachInstKind::ret, ops );

    AllocContext ctx;
    ctx.num_regs = 4;
    compute_liveness( ctx, vc );
    alloc( ctx );

    INFO( visualise( ctx ) );
    u32 spilled = 0;
    for ( auto &a : ctx.assignments )
        if ( a.spilled ) ++spilled;
    REQUIRE( spilled >= 2 );
}

TEST_CASE( "alloc: non-overlapping ranges share a register", "[regalloc]" )
{
    auto vc = make_vcode( );
    auto v0 = vc.new_vreg( );
    auto v1 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) } );
    vc.append( MachInstKind::nop, v0, { Operand::use( v0 ) } );
    vc.append( MachInstKind::mov, v1, { Operand::def( v1 ) } );
    vc.append_void( MachInstKind::ret, { Operand::use( v1 ) } );

    AllocContext ctx;
    ctx.num_regs = 1;
    compute_liveness( ctx, vc );
    alloc( ctx );

    INFO( visualise( ctx ) );
    REQUIRE( ctx.assignments[ v0.id ].spilled == false );
    REQUIRE( ctx.assignments[ v1.id ].spilled == false );
    REQUIRE( ctx.assignments[ v0.id ].reg.id == ctx.assignments[ v1.id ].reg.id );
}

TEST_CASE( "alloc: evicts lowest weight", "[regalloc]" )
{
    auto vc   = make_vcode( );
    auto hot  = vc.new_vreg( );
    auto cold = vc.new_vreg( );
    auto tmp  = vc.new_vreg( );

    vc.append( MachInstKind::mov, hot, { Operand::def( hot ) } );
    vc.append( MachInstKind::mov, cold, { Operand::def( cold ) } );
    for ( int i = 0; i < 5; ++i )
        vc.append( MachInstKind::add, tmp, { Operand::use( hot ) } );
    vc.append_void( MachInstKind::ret, { Operand::use( cold ) } );

    AllocContext ctx;
    ctx.num_regs = 1;
    compute_liveness( ctx, vc );
    alloc( ctx );

    INFO( visualise( ctx ) );
    REQUIRE( ctx.assignments[ hot.id ].spilled == false );
    REQUIRE( ctx.assignments[ cold.id ].spilled == true );
}

TEST_CASE( "visualise: allocation table", "[regalloc][.]" )
{
    auto vc = make_vcode( );
    auto v0 = vc.new_vreg( );
    auto v1 = vc.new_vreg( );
    auto v2 = vc.new_vreg( );
    auto v3 = vc.new_vreg( );

    vc.append( MachInstKind::mov, v0, { Operand::def( v0 ) } );
    vc.append( MachInstKind::mov, v1, { Operand::def( v1 ) } );
    vc.append( MachInstKind::add, v2, { Operand::def( v2 ), Operand::use( v0 ), Operand::use( v1 ) } );
    vc.append( MachInstKind::mov, v3, { Operand::def( v3 ) } );
    vc.append_void( MachInstKind::ret, { Operand::use( v2 ), Operand::use( v3 ) } );

    AllocContext ctx;
    ctx.num_regs = 2;
    compute_liveness( ctx, vc );
    alloc( ctx );

    std::println( "{}", visualise( ctx ) );
    SUCCEED( );
}