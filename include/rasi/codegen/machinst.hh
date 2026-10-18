//
// Created by griffith on 18/10/2026.
//

#pragma once

#include <rasi/codegen/operand.hh>
#include <vector>
#include <cstdint>
#include <span>

namespace rasi::codegen {
    //! @brief CRTP trait for a target specific machine instruction.
    //! @tparam Derived The concrete backend instruction type.
    template <typename Derived>
    class MachInst {
    public:
        //! @brief Collect all register operands into `ops` for the register allocator
        void get_operands(std::vector<MachOperand>& ops) {
            self().get_operands(ops);
        }

        //! @brief True if this instruction ends a basic block.
        [[nodiscard]] bool is_terminator() const noexcept { return self().is_terminator(); }

        //! @brief True if this instruction is a branch.
        [[nodiscard]] bool is_branch() const noexcept { return self().is_branch(); }

        //! @brief True if this is a pure reg2reg move.
        [[nodiscard]] bool is_move() const noexcept { return self().is_move(); }
    private:
        [[nodiscard]] Derived& self()             noexcept { return static_cast<Derived&>(*this); }
        [[nodiscard]] const Derived& self() const noexcept { return static_cast<const Derived&>(*this); }
    };
}