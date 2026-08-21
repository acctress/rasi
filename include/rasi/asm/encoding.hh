#pragma once

#include "rasi/support/types.hh"
#include "registers.hh"

namespace rasi::asm_
{
    [[nodiscard]] constexpr u8 rex( const bool W, const bool R, const bool X, const bool B ) noexcept
    {
        return static_cast< u8 >( 0x40 | W << 3 | R << 2 | X << 1 | B );
    }

    [[nodiscard]] constexpr u8 modrm( const u8 mod, const u8 reg, const u8 rm ) noexcept
    {
        return static_cast< u8 >( mod << 6 | ( reg & 0x7 ) << 3 | rm & 0x7 );
    }

    [[nodiscard]] constexpr bool rex_ext( const PhysReg r ) noexcept { return r.index( ) >= 8; }

    [[nodiscard]] constexpr u8 rex_rr( const PhysReg dest, const PhysReg src ) noexcept
    {
        return rex( true, rex_ext( dest ), false, rex_ext( src ) );
    }

    [[nodiscard]] constexpr u8 modrm_rr( const PhysReg dest, const PhysReg src ) noexcept
    {
        return modrm( 0b11, dest.index( ) & 0x7, src.index( ) & 0x7 );
    }
} // namespace rasi::asm_