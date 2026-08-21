#pragma once

#include "buffer.hh"
#include "rasi/ir/instruction.hh"

#include <rasi/asm/encoding.hh>
#include <rasi/asm/operand.hh>
#include <rasi/support/types.hh>
#include <unordered_map>
#include <vector>

namespace rasi::asm_
{
    struct MachInst
    {
        InstKind               kind {};
        std::vector< Operand > operands {};
        OperandConstraint      output_constraint {};
        VReg                   result {};
    };

    class Assembler
    {
    public:
        void emit_bytes( const std::unordered_map< VReg, PhysReg > &alloc, Buffer &buf );

        VReg iadd( VReg lhs, VReg rhs ) noexcept;
        VReg isub( VReg lhs, VReg rhs ) noexcept;
        VReg imul( VReg lhs, VReg rhs ) noexcept;
        void ret( VReg value ) noexcept;

    private:
        std::vector< MachInst > m_insts {};
        u32                     m_next_vreg {};

        VReg vreg( ) { return VReg { m_next_vreg++ }; }
    };
} // namespace rasi::asm_