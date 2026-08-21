#pragma once

#include <rasi/support/types.hh>

namespace rasi::asm_
{
    // clang-format off
    enum class RegClass : u8
    {
        gpr,
        xmm
    };

    enum class GPR : u8
    {
        rax, rcx, rdx, rbx,
        rsp, rbp, rsi, rdi,
        r8 ,  r9, r10, r11,
        r12, r13, r14, r15,
    };

    enum class XMM : u8
    {
        xmm0 , xmm1 , xmm2 ,  xmm3,
        xmm4 , xmm5 , xmm6 ,  xmm7,
        xmm8 , xmm9 , xmm10, xmm11,
        xmm12, xmm13, xmm14, xmm15,
    };

    /// @brief Encodes a class and index into a single byte, the high bit is the class, low seven bits are the index
    struct PhysReg
    {
        u8 raw { };

        static constexpr PhysReg gpr( GPR r ) noexcept
        { return { static_cast< u8 >( r ) }; }

        static constexpr PhysReg xmm( XMM r ) noexcept
        { return { static_cast< u8 >( 0x80 | static_cast<u8>( r ) ) }; }

        [[nodiscard]] constexpr RegClass clazz( ) const noexcept
        { return raw & 0x80 ? RegClass::xmm : RegClass::gpr; }

        [[nodiscard]] constexpr u8 index( ) const noexcept
        { return raw & 0x7f; }

        [[nodiscard]] constexpr bool operator==( const PhysReg& ot ) const noexcept = default;
    };

    // clang-format on

    namespace regs
    {
        inline constexpr PhysReg rax   = PhysReg::gpr( GPR::rax );
        inline constexpr PhysReg rcx   = PhysReg::gpr( GPR::rcx );
        inline constexpr PhysReg rdx   = PhysReg::gpr( GPR::rdx );
        inline constexpr PhysReg rsp   = PhysReg::gpr( GPR::rsp );
        inline constexpr PhysReg rbp   = PhysReg::gpr( GPR::rbp );
        inline constexpr PhysReg rsi   = PhysReg::gpr( GPR::rsi );
        inline constexpr PhysReg rdi   = PhysReg::gpr( GPR::rdi );
        inline constexpr PhysReg r8    = PhysReg::gpr( GPR::r8 );
        inline constexpr PhysReg r9    = PhysReg::gpr( GPR::r9 );
        inline constexpr PhysReg r10   = PhysReg::gpr( GPR::r10 );
        inline constexpr PhysReg r11   = PhysReg::gpr( GPR::r11 );
        inline constexpr PhysReg r12   = PhysReg::gpr( GPR::r12 );
        inline constexpr PhysReg r13   = PhysReg::gpr( GPR::r13 );
        inline constexpr PhysReg r14   = PhysReg::gpr( GPR::r14 );
        inline constexpr PhysReg r15   = PhysReg::gpr( GPR::r15 );
        inline constexpr PhysReg xmm0  = PhysReg::xmm( XMM::xmm0 );
        inline constexpr PhysReg xmm1  = PhysReg::xmm( XMM::xmm1 );
        inline constexpr PhysReg xmm2  = PhysReg::xmm( XMM::xmm2 );
        inline constexpr PhysReg xmm3  = PhysReg::xmm( XMM::xmm3 );
        inline constexpr PhysReg xmm4  = PhysReg::xmm( XMM::xmm4 );
        inline constexpr PhysReg xmm5  = PhysReg::xmm( XMM::xmm5 );
        inline constexpr PhysReg xmm6  = PhysReg::xmm( XMM::xmm6 );
        inline constexpr PhysReg xmm7  = PhysReg::xmm( XMM::xmm7 );
        inline constexpr PhysReg xmm8  = PhysReg::xmm( XMM::xmm8 );
        inline constexpr PhysReg xmm9  = PhysReg::xmm( XMM::xmm9 );
        inline constexpr PhysReg xmm10 = PhysReg::xmm( XMM::xmm10 );
        inline constexpr PhysReg xmm11 = PhysReg::xmm( XMM::xmm11 );
        inline constexpr PhysReg xmm12 = PhysReg::xmm( XMM::xmm12 );
        inline constexpr PhysReg xmm13 = PhysReg::xmm( XMM::xmm13 );
        inline constexpr PhysReg xmm14 = PhysReg::xmm( XMM::xmm14 );
        inline constexpr PhysReg xmm15 = PhysReg::xmm( XMM::xmm15 );
    } // namespace regs

} // namespace rasi::asm_