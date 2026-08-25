#pragma once

#include <array>
#include <cassert>
#include <ostream>
#include <rasi/asm/vcode.hh>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

namespace rasi
{
    namespace detail
    {
        constexpr std::array< std::string_view, 16 > gpr_names {
            "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15",
        };

        constexpr std::array< std::string_view, 16 > xmm_names {
            "xmm0", "xmm1", "xmm2",  "xmm3",  "xmm4",  "xmm5",  "xmm6",  "xmm7",
            "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15",
        };
    }

    using AllocMap = std::unordered_map< u32, std::variant< asm_::PhysReg, asm_::SpillSlot > >;

    [[nodiscard]] constexpr std::string_view fmt_inst_kind( const InstKind kind ) noexcept
    {
        switch ( kind )
        {
            case InstKind::iadd : return "iadd";
            case InstKind::isub : return "isub";
            case InstKind::imul : return "imul";
            case InstKind::sdiv : return "sdiv";
            case InstKind::udiv : return "udiv";
            case InstKind::srem : return "srem";
            case InstKind::urem : return "urem";
            case InstKind::fadd : return "fadd";
            case InstKind::fsub : return "fsub";
            case InstKind::fmul : return "fmul";
            case InstKind::fdiv : return "fdiv";
            case InstKind::and_ : return "and";
            case InstKind::or_ : return "or";
            case InstKind::xor_ : return "xor";
            case InstKind::shl : return "shl";
            case InstKind::shr : return "shr";
            case InstKind::sar : return "sar";
            case InstKind::icmp : return "icmp";
            case InstKind::fcmp : return "fcmp";
            case InstKind::load : return "load";
            case InstKind::store : return "store";
            case InstKind::alloca_ : return "alloca";
            case InstKind::call : return "call";
            case InstKind::ret : return "ret";
            case InstKind::br : return "br";
            case InstKind::cbr : return "cbr";
            case InstKind::copy : return "copy";
        }

        return "unknown";
    }

    [[nodiscard]] constexpr std::string_view fmt_phys_reg( const PhysReg reg ) noexcept
    {
        assert( reg.index( ) < 16 );
        return reg.clazz( ) == RegClass::gpr ? detail::gpr_names[ reg.index( ) ] : detail::xmm_names[ reg.index( ) ];
    }

    [[nodiscard]] inline std::string fmt_vreg( const asm_::VReg reg, const AllocMap *alloc = nullptr )
    {
        auto result = std::string { "%" } + std::to_string( reg.id );

        if ( alloc == nullptr ) return result;

        const auto iter = alloc->find( reg.id );
        if ( iter == alloc->end( ) ) return result + "<?>";

        result += '<';

        if ( std::holds_alternative< asm_::PhysReg >( iter->second ) )
        {
            result += fmt_phys_reg( std::get< asm_::PhysReg >( iter->second ) );
        }
        else
        {
            result += "spill+";
            result += std::to_string( std::get< asm_::SpillSlot >( iter->second ).offset );
        }

        result += '>';
        return result;
    }

    [[nodiscard]] inline std::string fmt_constraint( const asm_::OperandConstraint constraint )
    {
        switch ( constraint.kind )
        {
            case ConstraintKind::any : return "any";

            case ConstraintKind::fixed : return std::string { "fixed(" } + std::string { fmt_phys_reg( constraint.fixed_reg ) } + ')';

            case ConstraintKind::reuse : return std::string { "reuse(" } + std::to_string( constraint.reuse_index ) + ')';
        }

        return "unknown";
    }

    [[nodiscard]] inline std::string fmt_operand( const asm_::Operand &operand, const AllocMap *alloc = nullptr )
    {
        switch ( operand.kind )
        {
            case asm_::OperandKind::reg : return fmt_vreg( operand.vreg, alloc );

            case asm_::OperandKind::imm : return std::to_string( operand.imm );

            case asm_::OperandKind::mem :
            {
                auto result = std::string { "[" } + fmt_vreg( operand.mem.base, alloc );

                if ( operand.mem.offset > 0 ) result += '+';

                if ( operand.mem.offset != 0 ) result += std::to_string( operand.mem.offset );

                result += ']';
                return result;
            }
        }

        return "<unknown operand>";
    }

    [[nodiscard]] inline constexpr bool mach_inst_has_result( const InstKind kind ) noexcept
    {
        switch ( kind )
        {
            case InstKind::store :
            case InstKind::ret :
            case InstKind::br :
            case InstKind::cbr : return false;

            default : return true;
        }
    }

    [[nodiscard]] inline std::string fmt_mach_inst( const asm_::VCode &vcode, const asm_::MachInst &inst,
                                                    const AllocMap *alloc = nullptr )
    {
        std::ostringstream out;

        if ( mach_inst_has_result( inst.kind ) ) out << fmt_vreg( inst.result, alloc ) << " = ";

        out << fmt_inst_kind( inst.kind );

        for ( u16 i = 0; i < inst.operand_count; ++i )
        {
            out << ( i == 0 ? " " : ", " );
            out << fmt_operand( vcode.operands[ inst.operand_offset + i ], alloc );
        }

        if ( mach_inst_has_result( inst.kind ) && inst.output_constraint.kind != asm_::ConstraintKind::any )
            out << " [" << fmt_constraint( inst.output_constraint ) << ']';

        return out.str( );
    }

    inline void print_vcode( std::ostream &out, const asm_::VCode &vcode, const AllocMap *alloc = nullptr )
    {
        out << "vcode {\n";

        for ( u32 i = 0; i < vcode.insts.size( ); ++i )
            out << "    " << i << ": " << fmt_mach_inst( vcode, vcode.insts[ i ], alloc ) << '\n';

        out << "}\n";
    }

    inline void print_vcode( std::ostream &out, const asm_::VCode &vcode, const AllocMap &alloc )
    {
        print_vcode( out, vcode, &alloc );
    }

    [[nodiscard]] inline std::string fmt_vcode( const asm_::VCode &vcode, const AllocMap *alloc = nullptr )
    {
        std::ostringstream out;
        print_vcode( out, vcode, alloc );
        return out.str( );
    }

    [[nodiscard]] inline std::string fmt_vcode( const asm_::VCode &vcode, const AllocMap &alloc )
    {
        return fmt_vcode( vcode, &alloc );
    }
} // namespace rasi