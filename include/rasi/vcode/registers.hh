#pragma once

#include <rasi/support/types.hh>
#include <string_view>

namespace rasi::registers
{
    enum class RegClass : u8
    {
        Int,
        Float
    };

    inline constexpr u8 RAX = 0;
    inline constexpr u8 RCX = 1;
    inline constexpr u8 RDX = 2;
    inline constexpr u8 RBX = 3;
    inline constexpr u8 RSP = 4;
    inline constexpr u8 RBP = 5;
    inline constexpr u8 RSI = 6;
    inline constexpr u8 RDI = 7;
    inline constexpr u8 R8  = 8;
    inline constexpr u8 R9  = 9;
    inline constexpr u8 R10 = 10;
    inline constexpr u8 R11 = 11;
    inline constexpr u8 R12 = 12;
    inline constexpr u8 R13 = 13;
    inline constexpr u8 R14 = 14;
    inline constexpr u8 R15 = 15;

    inline constexpr u8 XMM0  = 16;
    inline constexpr u8 XMM1  = 17;
    inline constexpr u8 XMM2  = 18;
    inline constexpr u8 XMM3  = 19;
    inline constexpr u8 XMM4  = 20;
    inline constexpr u8 XMM5  = 21;
    inline constexpr u8 XMM6  = 22;
    inline constexpr u8 XMM7  = 23;
    inline constexpr u8 XMM8  = 24;
    inline constexpr u8 XMM9  = 25;
    inline constexpr u8 XMM10 = 26;
    inline constexpr u8 XMM11 = 27;
    inline constexpr u8 XMM12 = 28;
    inline constexpr u8 XMM13 = 29;
    inline constexpr u8 XMM14 = 30;
    inline constexpr u8 XMM15 = 31;

    [[nodiscard]] constexpr RegClass reg_class( const u8 id ) noexcept { return id < 16 ? RegClass::Int : RegClass::Float; }

    [[nodiscard]] constexpr std::string_view reg_name( const u8 id ) noexcept
    {
        switch (id)
        {
            case RAX: return "rax"; case RCX: return "rcx";
            case RDX: return "rdx"; case RBX: return "rbx";
            case RSP: return "rsp"; case RBP: return "rbp";
            case RSI: return "rsi"; case RDI: return "rdi";
            case R8:  return "r8";  case R9:  return "r9";
            case R10: return "r10"; case R11: return "r11";
            case R12: return "r12"; case R13: return "r13";
            case R14: return "r14"; case R15: return "r15";
            case XMM0:  return "xmm0";  case XMM1:  return "xmm1";
            case XMM2:  return "xmm2";  case XMM3:  return "xmm3";
            case XMM4:  return "xmm4";  case XMM5:  return "xmm5";
            case XMM6:  return "xmm6";  case XMM7:  return "xmm7";
            case XMM8:  return "xmm8";  case XMM9:  return "xmm9";
            case XMM10: return "xmm10"; case XMM11: return "xmm11";
            case XMM12: return "xmm12"; case XMM13: return "xmm13";
            case XMM14: return "xmm14"; case XMM15: return "xmm15";
            default:    return "?";
        }
    }
} // namespace rasi::registers