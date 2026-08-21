#pragma once

#include "registers.hh"

#include <rasi/support/types.hh>

namespace rasi::asm_
{
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
            VReg reg;
            i64  imm;

            struct
            {
                VReg base;
                i32  offset;
            } mem;
        };
    };

    struct OperandConstraint
    {
        ConstraintKind kind { ConstraintKind::any };

        union
        {
            PhysReg fixed_reg;
            u8      reuse_index;
        };
    };
} // namespace rasi::asm_