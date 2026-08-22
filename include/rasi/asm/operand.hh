#pragma once

#include "registers.hh"

#include <rasi/support/types.hh>

namespace rasi::asm_
{
    struct SpillSlot
    {
        u32 offset;
    };

    struct VReg
    {
        u32 id;
    };

    enum class OperandKind : u8
    {
        reg,
        imm,
        mem
    };

    enum class ConstraintKind : u8
    {
        any,
        fixed,
        reuse
    };

    struct Operand
    {
        OperandKind kind {};

        union
        {
            VReg vreg {};
            i64  imm;

            struct
            {
                VReg base;
                i32  offset;
            } mem;
        };

        static constexpr Operand from_reg( const VReg r ) noexcept { return { .kind = OperandKind::reg, .vreg = r }; }

        static constexpr Operand from_imm( const i64 v ) noexcept { return { .kind = OperandKind::imm, .imm = v }; }

        static constexpr Operand from_mem( const VReg base, const i32 offset ) noexcept
        {
            return {
                .kind = OperandKind::mem,
                .mem  = { base, offset }
            };
        }
    };

    struct OperandConstraint
    {
        ConstraintKind kind { ConstraintKind::any };

        union
        {
            PhysReg fixed_reg;
            u8      reuse_index{};
        };
    };
} // namespace rasi::asm_