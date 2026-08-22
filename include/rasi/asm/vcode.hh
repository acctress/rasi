#pragma once
#include "operand.hh"
#include "rasi/ir/instruction.hh"
#include "rasi/support/arena.hh"
#include "rasi/support/slab.hh"

namespace rasi::asm_
{
    struct MachInst
    {
        InstKind          kind {};
        u32               operand_offset {};
        u16               operand_count {};
        OperandConstraint output_constraint {};
        VReg              result {};
    };

    struct VCode
    {
        Arena           *arena;
        Slab< MachInst > insts;
        Slab< Operand >  operands;
        u32              next_vreg {};

        explicit VCode( Arena *arena ) : arena { arena }, insts( *arena, 64 ), operands( *arena, 64 ) { }

        VReg append( InstKind kind, std::initializer_list< Operand > ops, OperandConstraint constraint );
        void append( InstKind kind, std::initializer_list< Operand > ops );

        [[nodiscard]] VReg new_vreg( ) noexcept { return VReg { next_vreg++ }; }
    };
} // namespace rasi::asm_