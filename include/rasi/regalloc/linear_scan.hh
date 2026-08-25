#pragma once
#include "rasi/asm/operand.hh"
#include "rasi/asm/vcode.hh"

#include <array>
#include <unordered_map>
#include <variant>
#include <vector>

using namespace rasi::asm_;

namespace rasi::regalloc
{
    inline constexpr std::array sysv_caller_saved
        = { regs::rax, regs::rcx, regs::rdx, regs::rsi, regs::rdi, regs::r8, regs::r9, regs::r10, regs::r11 };

    inline constexpr std::array sysv_callee_saved = { regs::rbx, regs::r12, regs::r13, regs::r14, regs::r15 };

    inline constexpr std::array win64_caller_saved = { regs::rax, regs::rcx, regs::rdx, regs::r8, regs::r9, regs::r10, regs::r11 };

    inline constexpr std::array win64_callee_saved = { regs::rbx, regs::rsi, regs::rdi, regs::r12, regs::r13, regs::r14, regs::r15 };

#ifdef _WIN32
    inline constexpr auto &caller_saved = win64_caller_saved;
    inline constexpr auto &callee_saved = win64_callee_saved;
#else
    inline constexpr auto &caller_saved = sysv_caller_saved;
    inline constexpr auto &callee_saved = sysv_callee_saved;
#endif

    inline constexpr std::array allocatable = []
    {
        std::array< PhysReg, caller_saved.size( ) + callee_saved.size( ) > result {};

        std::ranges::copy( caller_saved, result.begin( ) );
        std::ranges::copy( callee_saved, result.begin( ) + caller_saved.size( ) );

        return result;
    }( );

    using AllocResult = std::variant< PhysReg, SpillSlot >;

    struct LiveInterval
    {
        VReg        reg {};
        u32         start {};
        u32         end {};
        AllocResult assign;
        bool        fixed {};
    };

    class LinearScan
    {
    public:
        std::unordered_map< u32, AllocResult > run( const VCode &vcode );

    private:
        std::vector< LiveInterval > m_intervals;
        u32                         m_nx_spill_offset {};

        void build_intervals( const VCode &vcode );
        void alloc( );
    };

} // namespace rasi::regalloc