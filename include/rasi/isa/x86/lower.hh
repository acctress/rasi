//
// Created by griffith on 19/10/2026.
//

#pragma once

#include <rasi/ir/visitor.hh>
#include <rasi/ir/function.hh>
#include <rasi/isa/x86/inst.hh>
#include <rasi/isa/x86/reg.hh>
#include <rasi/codegen/abi.hh>
#include <rasi/codegen/machenv.hh>
#include <rasi/codegen/vcode.hh>

#include <cassert>
#include <vector>

namespace rasi::x86_64 {
    class IRLowerer : public ir::IRVisitor<IRLowerer> {
    public:
        explicit IRLowerer(
            const MachineEnv &env,
            const ABISpec &abi
        ) noexcept
            : m_env{env}
              , m_abi{abi} {
        }

        [[nodiscard]] VCode<X86Inst> lower(
            ir::IRFunction &fn,
            CallConv cc = CallConv::SystemV
        ) {
            m_fn = &fn;
            m_vreg_c = 0;
            m_value_regs.clear();
            m_builder = codegen::VCodeBuilder<X86Inst>{};

            std::vector<ir::Type> param_tys;
            std::vector<ir::Type> ret_tys;

            param_tys.reserve(fn.sig.params_count);
            for (uint16_t i = 0; i < fn.sig.params_count; ++i)
                param_tys.push_back(fn.values[i].type);

            if (fn.sig.ret != ir::Type::unit)
                ret_tys.push_back(fn.sig.ret);

            m_sig = m_abi.compute(cc, param_tys, ret_tys);

            m_value_regs.resize(fn.values.size());
            for (auto i{0u}; i < fn.values.size(); ++i)
                m_value_regs[i] = alloc_vreg(fn.values[i].type);

            visit_function(fn);

            m_fn = nullptr;
            return std::move(m_builder).finalise();
        }

        void on_function(ir::IRFunction &) {
        }

        void on_function_end(ir::IRFunction &) {
        }

        void on_block(ir::IRFunction &, ir::IRBlock &) {
            m_builder.start_block();
        }

        void on_block_end(ir::IRFunction &, ir::IRBlock &) {
            m_builder.end_block({});
        }

        void on_inst(ir::IRFunction &, ir::IRBlock &, ir::IRInst &inst) {
            using enum ir::IROp;
            switch (inst.op) {
                case iconst: lower_iconst(inst);
                    break;
                case iadd: lower_binop(inst, AluOp::Add);
                    break;
                case isub: lower_binop(inst, AluOp::Sub);
                    break;
                case imul: lower_binop(inst, AluOp::Mul);
                    break;
                case ret: lower_ret(inst);
                    break;
                case idiv:
                case icmp:
                case br:
                case brif:
                case call: std::unreachable();
            }
        }

    private:
        ir::IRFunction *m_fn{nullptr};
        const MachineEnv &m_env;
        const ABISpec &m_abi;
        ABISig m_sig{};
        VCodeBuilder<X86Inst> m_builder{};
        std::vector<VReg> m_value_regs{};
        uint32_t m_vreg_c{};

        [[nodiscard]] VReg alloc_vreg(const ir::Type type) noexcept {
            const auto rc = (type == ir::Type::f64)
                                ? RegClass::Float
                                : RegClass::Int;
            return VReg::make(m_vreg_c++, rc);
        }

        [[nodiscard]] VReg vreg_for(const ir::ValueRef ref) const noexcept {
            assert(ref.id < m_value_regs.size());
            return m_value_regs[ref.id];
        }

        [[nodiscard]] Reg reg_for(const ir::ValueRef ref) const noexcept {
            return Reg::from_vreg(vreg_for(ref));
        }

        void lower_iconst(const ir::IRInst &inst) {
            m_builder.push(inst::MovImm{
                .dst = reg_for(inst.result),
                .imm = static_cast<int64_t>(inst.imm),
            });
        }

        void lower_binop(const ir::IRInst &inst, AluOp op) {
            const auto dst = reg_for(inst.result);
            const auto lhs = reg_for(m_fn->operands[inst.ops_offset]);
            const auto rhs = reg_for(m_fn->operands[inst.ops_offset + 1]);

            m_builder.push(inst::Mov{.dst = dst, .src = lhs});
            m_builder.push(inst::Alu{.op = op, .dst = dst, .src = rhs});
        }

        void lower_ret(const ir::IRInst &inst) {
            if (inst.ops_count > 0) {
                assert(!m_sig.rets.empty() && m_sig.rets[0].is_reg());

                const auto src = reg_for(m_fn->operands[inst.ops_offset]);
                const auto ret_preg = m_sig.rets[0].reg();

                m_builder.push(inst::Mov{
                    .dst = codegen::Reg::from_preg(ret_preg),
                    .src = src,
                });
            }

            m_builder.push(inst::Ret{});
        }
    };
} // namespace rasi::x86_64
