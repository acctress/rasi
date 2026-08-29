#pragma once

#include <rasi/ir/call_conv.hh>
#include <rasi/support/arena.hh>
#include <rasi/support/slab.hh>
#include <rasi/vcode/inst_kind.hh>
#include <rasi/vcode/operand.hh>

namespace rasi
{
    struct MachInst
    {
        MachInstKind kind {};
        u32          operand_offset {};
        u32          operand_count {};
        VReg         result {};
        i64          imm {};
        MemRef       mem {};
    };

    struct VCode
    {
        Arena           *arena;
        Slab< MachInst > instructions;
        Slab< Operand >  operands;
        u32              next_vreg {};
        CallConv         call_conv { CallConv::native };

        explicit VCode( Arena *arena ) : arena { arena }, instructions( *arena, 64 ), operands( *arena, 64 ) { }

        [[nodiscard]] VReg new_vreg( ) noexcept { return VReg { next_vreg++ }; }

        VReg append( MachInstKind kind, VReg result, std::initializer_list< Operand > ops, i64 imm = 0, MemRef mem = {} );
        VReg append( MachInstKind kind, VReg result, std::span<const Operand> ops, i64 imm = 0, MemRef mem = {} );
        void append_void( MachInstKind kind, std::initializer_list< Operand > ops, i64 imm = 0, MemRef mem = {} );
        void append_void( MachInstKind kind, std::span<const Operand> ops, i64 imm = 0, MemRef mem = {} );
    };
} // namespace rasi