#pragma once

#include <rasi/support/types.hh>
#include <string_view>

namespace rasi
{
    enum class MachInstKind : u8
    {
        mov,
        movsx,
        movzx,
        lea,
        push,
        pop,
        add,
        sub,
        imul,
        idiv,
        udiv,
        neg,
        inc,
        dec,
        and_,
        or_,
        xor_,
        not_,
        shl,
        shr,
        sar,
        cmp,
        test,
        movss,
        movsd,
        addss,
        addsd,
        subss,
        subsd,
        mulss,
        mulsd,
        divss,
        divsd,
        ucomiss,
        ucomisd,
        cvtsi2ss,
        cvtsi2sd,
        cvttss2si,
        cvttsd2si,
        jmp,
        jcc,
        call,
        ret,
        nop,
        ud2,
    };

    inline std::string_view mach_inst_name( const MachInstKind k ) noexcept
    {
        switch ( k )
        {
            case MachInstKind::mov : return "mov";
            case MachInstKind::movsx : return "movsx";
            case MachInstKind::movzx : return "movzx";
            case MachInstKind::lea : return "lea";
            case MachInstKind::push : return "push";
            case MachInstKind::pop : return "pop";
            case MachInstKind::add : return "add";
            case MachInstKind::sub : return "sub";
            case MachInstKind::imul : return "imul";
            case MachInstKind::idiv : return "idiv";
            case MachInstKind::udiv : return "udiv";
            case MachInstKind::neg : return "neg";
            case MachInstKind::inc : return "inc";
            case MachInstKind::dec : return "dec";
            case MachInstKind::and_ : return "and";
            case MachInstKind::or_ : return "or";
            case MachInstKind::xor_ : return "xor";
            case MachInstKind::not_ : return "not";
            case MachInstKind::shl : return "shl";
            case MachInstKind::shr : return "shr";
            case MachInstKind::sar : return "sar";
            case MachInstKind::cmp : return "cmp";
            case MachInstKind::test : return "test";
            case MachInstKind::movss : return "movss";
            case MachInstKind::movsd : return "movsd";
            case MachInstKind::addss : return "addss";
            case MachInstKind::addsd : return "addsd";
            case MachInstKind::subss : return "subss";
            case MachInstKind::subsd : return "subsd";
            case MachInstKind::mulss : return "mulss";
            case MachInstKind::mulsd : return "mulsd";
            case MachInstKind::divss : return "divss";
            case MachInstKind::divsd : return "divsd";
            case MachInstKind::ucomiss : return "ucomiss";
            case MachInstKind::ucomisd : return "ucomisd";
            case MachInstKind::cvtsi2ss : return "cvtsi2ss";
            case MachInstKind::cvtsi2sd : return "cvtsi2sd";
            case MachInstKind::cvttss2si : return "cvttss2si";
            case MachInstKind::cvttsd2si : return "cvttsd2si";
            case MachInstKind::jmp : return "jmp";
            case MachInstKind::jcc : return "jcc";
            case MachInstKind::call : return "call";
            case MachInstKind::ret : return "ret";
            case MachInstKind::nop : return "nop";
            case MachInstKind::ud2 : return "ud2";
            default : return "unknown";
        }
    }
} // namespace rasi