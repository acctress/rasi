#include "rasi/asm/x86_64/emit.hh"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <print>
#include <ranges>
#include <rasi/asm/x86_64/assembler.hh>
#include <rasi/isel/x86_64/abi.hh>

using namespace rasi;
using namespace rasi::registers;
using namespace rasi::isel::x86_64;
using namespace rasi::x86;

TEST_CASE( "x86_64 sysv empty stack frame", "[abi]" )
{
    Buffer      buffer { 64 };
    FrameLayout frame {};

    emit_prologue( buffer, frame, CallConv::sys_v );
    emit_epilogue( buffer, frame, CallConv::sys_v );
    emit_ret( buffer );

    constexpr std::array expected {
        0x55_b,                 // push rbp
        0x48_b, 0x89_b, 0xe5_b, // mov rbp, rsp
        0x5d_b,                 // pop rbp
        0xc3_b,                 // ret
    };

    for ( const auto byte : buffer.bytes( ) )
    {
        std::print( "{:02x} ", std::to_integer< u8 >( byte ) );
    }

    std::println( );

    REQUIRE( buffer.pos( ) == expected.size( ) );
    REQUIRE( std::ranges::equal( buffer.bytes( ), expected ) );
}

TEST_CASE( "x86_64 windows empty stack frame", "[abi]" )
{
    Buffer      buffer { 64 };
    FrameLayout frame {};

    emit_prologue( buffer, frame, CallConv::win_fastcall );
    emit_epilogue( buffer, frame, CallConv::win_fastcall );

    emit_ret( buffer );

    constexpr std::array expected {
        0x55_b,                         // push rbp
        0x48_b, 0x89_b, 0xe5_b,         // mov rbp, rsp
        0x48_b, 0x83_b, 0xec_b, 0x20_b, // sub rsp, 32
        0x48_b, 0x83_b, 0xc4_b, 0x20_b, // add rsp, 32
        0x5d_b,                         // pop rbp
        0xc3_b,                         // ret
    };

    REQUIRE( buffer.pos( ) == expected.size( ) );
    REQUIRE( std::ranges::equal( buffer.bytes( ), expected ) );
}

TEST_CASE( "x86_64 sysv preserves rbx", "[abi]" )
{
    Buffer      buffer { 64 };
    FrameLayout frame {};

    frame.clobbered_callee_saves.push_back( PhysReg { RBX } );
    frame.clobber_size = 8;

    emit_prologue( buffer, frame, CallConv::sys_v );
    emit_epilogue( buffer, frame, CallConv::sys_v );
    emit_ret( buffer );

    constexpr std::array expected {
        0x55_b,                         // push rbp
        0x48_b, 0x89_b, 0xe5_b,         // mov rbp, rsp
        0x48_b, 0x83_b, 0xec_b, 0x10_b, // sub rsp, 16
        0x48_b, 0x89_b, 0x9c_b, 0x24_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x48_b, 0x8b_b,
        0x9c_b, 0x24_b, 0x00_b, 0x00_b, 0x00_b, 0x00_b, 0x48_b, 0x83_b, 0xc4_b, 0x10_b, // add rsp, 16
        0x5d_b,                                                                         // pop rbp
        0xc3_b,                                                                         // ret
    };

    REQUIRE( buffer.pos( ) == expected.size( ) );
    REQUIRE( std::ranges::equal( buffer.bytes( ), expected ) );
}

TEST_CASE( "x86_64 emit expadns epilogue placeholder", "[abi]" )
{
    Arena arena { 1024 * 1024 };
    VCode vcode { &arena };
    vcode.call_conv = CallConv::sys_v;

    vcode.append_void_epi( MachInstKind::ret, {} );

    regalloc::AllocContext alloc {};
    FrameLayout            frame {};

    const Buffer buffer = azm::x86_64::emit( vcode, alloc, frame );

    constexpr std::array expected {
        0x55_b,                 // push rbp
        0x48_b, 0x89_b, 0xe5_b, // mov rbp, rsp
        0x5d_b,                 // pop rbp
        0xc3_b,                 // ret
    };

    REQUIRE( buffer.pos( ) == expected.size( ) );
    REQUIRE( std::ranges::equal( buffer.bytes( ), expected ) );
}

TEST_CASE( "x86_64 emit expands Windows epilogue placeholder", "[abi]" )
{
    Arena arena { 1024 * 1024 };
    VCode vcode { &arena };

    vcode.call_conv = CallConv::win_fastcall;

    vcode.append_void_epi( MachInstKind::ret, {} );

    regalloc::AllocContext alloc {};
    FrameLayout            frame {};

    const Buffer buffer = azm::x86_64::emit( vcode, alloc, frame );

    constexpr std::array expected {
        0x55_b,                         // push rbp
        0x48_b, 0x89_b, 0xe5_b,         // mov rbp, rsp
        0x48_b, 0x83_b, 0xec_b, 0x20_b, // sub rsp, 32
        0x48_b, 0x83_b, 0xc4_b, 0x20_b, // add rsp, 32
        0x5d_b,                         // pop rbp
        0xc3_b,                         // ret
    };

    REQUIRE( buffer.pos( ) == expected.size( ) );
    REQUIRE( std::ranges::equal( buffer.bytes( ), expected ) );
}

TEST_CASE( "x86_64 sysv preserves multiple registers", "[abi]" )
{
    Buffer      buffer { 64 };
    FrameLayout frame {};

    frame.clobbered_callee_saves.push_back( PhysReg { RBX } );
    frame.clobbered_callee_saves.push_back( PhysReg { R12 } );
    frame.clobber_size = 16;

    emit_prologue( buffer, frame, CallConv::sys_v );
    emit_epilogue( buffer, frame, CallConv::sys_v );
    emit_ret( buffer );

    constexpr std::array expected {
        0x55_b, // push rbp
        0x48_b,
        0x89_b,
        0xe5_b, // mov rbp, rsp
        0x48_b,
        0x83_b,
        0xec_b,
        0x10_b, // sub rsp, 16

        // mov [rsp + 0], rbx
        0x48_b,
        0x89_b,
        0x9c_b,
        0x24_b,
        0x00_b,
        0x00_b,
        0x00_b,
        0x00_b,

        // mov [rsp + 8], r12
        0x4c_b,
        0x89_b,
        0xa4_b,
        0x24_b,
        0x08_b,
        0x00_b,
        0x00_b,
        0x00_b,

        // mov r12, [rsp + 8]
        0x4c_b,
        0x8b_b,
        0xa4_b,
        0x24_b,
        0x08_b,
        0x00_b,
        0x00_b,
        0x00_b,

        // mov rbx, [rsp + 0]
        0x48_b,
        0x8b_b,
        0x9c_b,
        0x24_b,
        0x00_b,
        0x00_b,
        0x00_b,
        0x00_b,

        0x48_b,
        0x83_b,
        0xc4_b,
        0x10_b, // add rsp, 16
        0x5d_b, // pop rbp
        0xc3_b, // ret
    };

    REQUIRE( buffer.pos( ) == expected.size( ) );
    REQUIRE( std::ranges::equal( buffer.bytes( ), expected ) );
}

TEST_CASE( "x86_64 sysv separates spill storage and callee saves", "[abi]" )
{
    Buffer      buffer { 64 };
    FrameLayout frame {};

    frame.fixed_frame_stor_size = 16;
    frame.clobbered_callee_saves.push_back( PhysReg { RBX } );
    frame.clobber_size = 8;

    emit_prologue( buffer, frame, CallConv::sys_v );
    emit_epilogue( buffer, frame, CallConv::sys_v );
    emit_ret( buffer );

    constexpr std::array expected {
        0x55_b,                         // push rbp
        0x48_b, 0x89_b, 0xe5_b,         // mov rbp, rsp
        0x48_b, 0x83_b, 0xec_b, 0x20_b, // sub rsp, 32
        0x48_b, 0x89_b, 0x9c_b, 0x24_b, 0x10_b, 0x00_b, 0x00_b, 0x00_b, 0x48_b, 0x8b_b,
        0x9c_b, 0x24_b, 0x10_b, 0x00_b, 0x00_b, 0x00_b, 0x48_b, 0x83_b, 0xc4_b, 0x20_b, // add rsp, 32
        0x5d_b,                                                                         // pop rbp
        0xc3_b,                                                                         // ret
    };

    REQUIRE( buffer.pos( ) == expected.size( ) );
    REQUIRE( std::ranges::equal( buffer.bytes( ), expected ) );
}