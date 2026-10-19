//
// Created by griffith on 19/10/2026.
//

#pragma once

#include <rasi/ir/visitor.hh>
#include <rasi/ir/function.hh>
#include <rasi/isa/x86/inst.hh>
#include <rasi/isa/x86/regs.hh>
#include <rasi/codegen/machenv.hh>
#include <rasi/codegen/vcode.hh>

#include <cassert>
#include <vector>

#include "reg.hh"

namespace rasi::x86_64 {

    //! @brief Lowers a single IR function to x86_64 VCode
    class IRLowerer : public ir::IRVisitor<IRLowerer> {
    public:
        explicit IRLowerer(const codegen::MachineEnv& env) noexcept
            : m_env{ env } {}

        //! @brief Lower one function
        [[nodiscard]] codegen::VCode<X86Inst> lower(ir::IRFunction& fn) {
            m_fn        = &fn;
            m_vreg_c    = 0;
            m_value_regs.clear();
            m_builder   = codegen::VCodeBuilder<X86Inst>{};

            m_value_regs.resize(fn.values.size());
            for (auto i { 0u }; i < fn.values.size(); ++i) m_value_regs[i] = alloc_vreg(fn.values[i].type);

            visit_function(fn);

            m_fn = nullptr;
            return std::move(m_builder).finalise();
        }

        void on_function(ir::IRFunction& fn) {

        }

        void on_function_end(ir::IRFunction&) {}

        void on_block(ir::IRFunction&, ir::IRBlock& block) {
            m_builder.start_block();
        }

        void on_block_end(ir::IRFunction&, ir::IRBlock&) {
            m_builder.end_block({});
        }

        void on_inst(ir::IRFunction& fn, ir::IRBlock&, ir::IRInst& inst) {
            using enum ir::IROp;
            switch (inst.op) {
                case iconst: lower_iconst(inst);               break;
                case iadd:   lower_binop(inst, AluOp::Add);    break;
                case isub:   lower_binop(inst, AluOp::Sub);    break;
                case imul:   lower_binop(inst, AluOp::Mul);    break;
                case ret:    lower_ret(inst);                  break;
                case idiv:
                case icmp:
                case br:
                case brif:
                case call: std::unreachable();
            }
        }
    private:
        ir::IRFunction*                 m_fn { nullptr };
        const codegen::MachineEnv&      m_env;
        codegen::VCodeBuilder<X86Inst>  m_builder {};
        std::vector<codegen::VReg>      m_value_regs {};
        uint32_t                        m_vreg_c {};

        [[nodiscard]] codegen::VReg alloc_vreg(const ir::Type type) noexcept {
            const auto rc = (type == ir::Type::f64)
                ? codegen::RegClass::Float : codegen::RegClass::Int;
            return codegen::VReg::make(m_vreg_c++, rc);
        }

        [[nodiscard]] codegen::VReg vreg_for(const ir::ValueRef ref) const noexcept {
            assert(ref.id < m_value_regs.size());
            return m_value_regs[ref.id];
        }

        [[nodiscard]] codegen::Reg reg_for(const ir::ValueRef ref) const noexcept {
            return codegen::Reg::from_vreg(vreg_for(ref));
        }

        void lower_iconst(const ir::IRInst& inst) {
            m_builder.push(inst::MovImm{
                .dst = reg_for(inst.result),
                .imm = static_cast<int64_t>(inst.imm),
            });
        }

        void lower_binop(const ir::IRInst& inst, AluOp op) {
            const auto dst = reg_for(inst.result);
            const auto lhs = reg_for(m_fn->operands[inst.ops_offset]);
            const auto rhs = reg_for(m_fn->operands[inst.ops_offset + 1]);

            m_builder.push(inst::Mov{ .dst = dst, .src = lhs });
            m_builder.push(inst::Alu{ .op = op, .dst = dst, .src = rhs });
        }

        void lower_ret(const ir::IRInst& inst) {
            if (inst.ops_count > 0) {
                const auto v = reg_for(m_fn->operands[inst.ops_offset]);
                m_builder.push(inst::Mov{
                    .dst = codegen::Reg::from_preg(RAX),
                    .src = v,
                });
            }

            m_builder.push(inst::Ret {});
        }
    };
}
