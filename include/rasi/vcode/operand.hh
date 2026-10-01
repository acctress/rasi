#pragma once

#include <rasi/support/types.hh>

namespace rasi
{
    struct VReg
    {
        u32 id;
    };

    struct SpillSlot
    {
        u32 offset;
    };

    /* inspired from regalloc2 */

    enum class OperandDir : u8
    {
        use,
        def,
    };

    enum class OperandPolicy : u8
    {
        any,
        fixed,
        reuse,
    };

    struct PhysReg
    {
        u8 id;

        bool operator==(const PhysReg &) const = default;
    };

    struct MemRef
    {
        VReg base;
        i32  offset;
    };

    struct Operand
    {
        VReg          vreg {};
        OperandDir    dir { OperandDir::use };
        OperandPolicy policy { OperandPolicy::any };

        union
        {
            PhysReg fixed_reg {};
            u8      reuse_idx;
        };

        static constexpr Operand use( const VReg r ) noexcept { return { .vreg = r, .dir = OperandDir::use }; }

        static constexpr Operand def( const VReg r ) noexcept { return { .vreg = r, .dir = OperandDir::def }; }

        static constexpr Operand use_fixed( const VReg r, const PhysReg p ) noexcept
        {
            return { .vreg = r, .dir = OperandDir::use, .policy = OperandPolicy::fixed, .fixed_reg = p };
        }

        static constexpr Operand def_fixed( const VReg r, const PhysReg p ) noexcept
        {
            return { .vreg = r, .dir = OperandDir::def, .policy = OperandPolicy::fixed, .fixed_reg = p };
        }

        static constexpr Operand def_reuse( const VReg r, const u8 idx ) noexcept
        {
            return { .vreg = r, .dir = OperandDir::def, .policy = OperandPolicy::reuse, .reuse_idx = idx };
        }
    };
} // namespace rasi