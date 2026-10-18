//
// Created by griffith on 18/10/2026.
//

#pragma once

#include <cstdint>
#include <variant>
#include <vector>
#include <format>

#include <rasi/codegen/operand.hh>
#include <rasi/codegen/vcode.hh>

#define RASI_OPERANDS(...) \
    void get_operands(std::vector<codegen::MachOperand>& ops) const { \
        __VA_ARGS__ \
    }

#define USE(reg)    if ((reg).valid()) ops.push_back(codegen::MachOperand::any((reg).as_vreg(), codegen::OperandKind::Use,    codegen::OperandPos::Before));
#define DEF(reg)    if ((reg).valid()) ops.push_back(codegen::MachOperand::any((reg).as_vreg(), codegen::OperandKind::Def,    codegen::OperandPos::After));
#define USEDEF(reg) if ((reg).valid()) ops.push_back(codegen::MachOperand::any((reg).as_vreg(), codegen::OperandKind::UseDef, codegen::OperandPos::After));

namespace rasi::x86_64 {
    //! @brief x86_64 condition codes
    enum class CC : uint8_t {
        E, //! Equal, ZF=1
        NE, //! Not equal, ZF=0
        L, //! Less, signed
        LE, //! Less or equal, signed
        G, //! Greater, signed
        GE, //! Greater or equal, signed
        B, //! Below, unsigned
        BE, //! Below or equal, unsigned
        A, //! Above, unsigned
        AE, //! Above or equal, unsigned
    };

    //! @brief ALU op kind
    enum class AluOp : uint8_t {
        Add,
        Sub,
        Mul,
        And,
        Or,
        Xor,
        Cmp,
        Test
    };

    //! @brief Operand size for sign or zero extending moves
    enum class OperandSize : uint8_t {
        S8,
        S16,
        S32,
        S64
    };

    //! @brief x86_64 memory addressing mode
    struct AddressMode {
        codegen::Reg base{};
        codegen::Reg index{};
        uint8_t scale{};
        int32_t disp{};
    };

    namespace inst {
        //! @brief reg/reg ALU: dst op= src
        struct Alu {
            AluOp op{};
            codegen::Reg dst{};
            codegen::Reg src{};
            RASI_OPERANDS(USEDEF(dst) USE(src))
        };

        //! @brief reg/imm ALU: dst op= imm
        struct AluImm {
            AluOp op{};
            codegen::Reg dst{};
            int32_t imm{};
            RASI_OPERANDS(USEDEF(dst))
        };

        //! @brief Register to register move
        struct Mov {
            codegen::Reg dst{};
            codegen::Reg src{};
            RASI_OPERANDS(DEF(dst) USE(src))
        };

        //! @brief Load 64bit immediate into register
        struct MovImm {
            codegen::Reg dst{};
            int64_t imm{};
            RASI_OPERANDS(DEF(dst))
        };

        //! @brief Load from memory into register
        struct MovLoad {
            codegen::Reg dst{};
            AddressMode mem{};
            RASI_OPERANDS(DEF(dst) USE(mem.base) USE(mem.index))
        };

        //! @brief Store register into memory
        struct MovStore {
            AddressMode mem{};
            codegen::Reg src{};
            RASI_OPERANDS(USE(mem.base) USE(mem.index) USE(src))
        };

        //! @brief Zero extending move
        struct Movzx {
            codegen::Reg dst{};
            codegen::Reg src{};
            OperandSize size{};
            RASI_OPERANDS(DEF(dst) USE(src))
        };

        //! @brief Sign extending move
        struct Movsx {
            codegen::Reg dst{};
            codegen::Reg src{};
            OperandSize size{};
            RASI_OPERANDS(DEF(dst) USE(src))
        };

        //! @brief Sign extend rax into rdx:rax
        struct Cdq {
            RASI_OPERANDS()
        };

        //! @brief Unsigned division: rax / src -> rax (quotient), rdx (remainder)
        struct Div {
            codegen::Reg src{};
            RASI_OPERANDS(USE(src))
        };

        //! @brief Signed division: rax / src -> rax (quotient), rdx (remainder)
        struct Idiv {
            codegen::Reg src{};
            RASI_OPERANDS(USE(src))
        };

        //! @brief Push register onto stack
        struct Push {
            codegen::Reg src{};
            RASI_OPERANDS(USE(src))
        };

        //! @brief Pop stack into register
        struct Pop {
            codegen::Reg dst{};
            RASI_OPERANDS(DEF(dst))
        };

        //! @brief Load effective address of a frame slot into a register
        struct LeaFrame {
            codegen::Reg dst{};
            int32_t offset{};
            RASI_OPERANDS(DEF(dst))
        };

        //! @brief Set byte register from condition code
        struct SetCC {
            CC cc{};
            codegen::Reg dst{};
            RASI_OPERANDS(DEF(dst))
        };

        //! @brief Unconditional jump to a block
        struct Jmp {
            codegen::BlockId target{};
            RASI_OPERANDS()
        };

        //! @brief Conditional jump to a block
        struct Jcc {
            CC cc{};
            codegen::BlockId target{};
            RASI_OPERANDS()
        };

        //! @brief Direct call to a function by id
        struct Call {
            uint32_t func_id{};
            RASI_OPERANDS()
        };

        //! @brief Return from function
        struct Ret {
            RASI_OPERANDS()
        };
    }

    using X86Inst = std::variant<
        inst::Alu,
        inst::AluImm,
        inst::Mov,
        inst::MovImm,
        inst::MovLoad,
        inst::MovStore,
        inst::Movzx,
        inst::Movsx,
        inst::Cdq,
        inst::Div,
        inst::Idiv,
        inst::Push,
        inst::Pop,
        inst::LeaFrame,
        inst::SetCC,
        inst::Jmp,
        inst::Jcc,
        inst::Call,
        inst::Ret
    >;

    //! @brief Collect MachOperands from an X86Inst for the register allocator
    inline void get_operands(const X86Inst &inst, std::vector<codegen::MachOperand> &ops) {
        std::visit([&ops](const auto &i) { i.get_operands(ops); }, inst);
    }

    //! @brief True if this instruction ends a basic block
    [[nodiscard]] inline bool is_terminator(const X86Inst &inst) noexcept {
        return std::holds_alternative<inst::Jmp>(inst)
               || std::holds_alternative<inst::Jcc>(inst)
               || std::holds_alternative<inst::Ret>(inst);
    }

    //! @brief True if this instruction is a branch
    [[nodiscard]] inline bool is_branch(const X86Inst &inst) noexcept {
        return std::holds_alternative<inst::Jmp>(inst)
               || std::holds_alternative<inst::Jcc>(inst);
    }

    //! @brief True if this is a pure register to register move
    [[nodiscard]] inline bool is_move(const X86Inst &inst) noexcept {
        return std::holds_alternative<inst::Mov>(inst);
    }
}

template<>
struct std::formatter<rasi::x86_64::CC> {
    constexpr auto parse(auto &ctx) { return ctx.begin(); }

    auto format(const rasi::x86_64::CC cc, auto &ctx) const {
        using enum rasi::x86_64::CC;
        switch (cc) {
            case E: return std::format_to(ctx.out(), "e");
            case NE: return std::format_to(ctx.out(), "ne");
            case L: return std::format_to(ctx.out(), "l");
            case LE: return std::format_to(ctx.out(), "le");
            case G: return std::format_to(ctx.out(), "g");
            case GE: return std::format_to(ctx.out(), "ge");
            case B: return std::format_to(ctx.out(), "b");
            case BE: return std::format_to(ctx.out(), "be");
            case A: return std::format_to(ctx.out(), "a");
            case AE: return std::format_to(ctx.out(), "ae");
        }
    }
};

template<>
struct std::formatter<rasi::x86_64::AluOp> {
    constexpr auto parse(auto &ctx) { return ctx.begin(); }

    auto format(const rasi::x86_64::AluOp op, auto &ctx) const {
        using enum rasi::x86_64::AluOp;
        switch (op) {
            case Add: return std::format_to(ctx.out(), "add");
            case Sub: return std::format_to(ctx.out(), "sub");
            case Mul: return std::format_to(ctx.out(), "imul");
            case And: return std::format_to(ctx.out(), "and");
            case Or: return std::format_to(ctx.out(), "or");
            case Xor: return std::format_to(ctx.out(), "xor");
            case Cmp: return std::format_to(ctx.out(), "cmp");
            case Test: return std::format_to(ctx.out(), "test");
        }
    }
};
