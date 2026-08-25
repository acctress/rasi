#pragma once

#include "rasi/support/types.hh"
#include "types.hh"

namespace rasi
{
    enum class InstKind : u8
    {
        iconst,
        iadd,
        isub,
        imul,
        sdiv,
        udiv,
        srem,
        urem,
        fadd,
        fsub,
        fmul,
        fdiv,
        and_,
        or_,
        xor_,
        shl,
        shr,
        sar,
        icmp,
        fcmp,
        load,
        store,
        alloca_,
        call,
        ret,
        br,
        cbr,
        copy,
    };

    enum class IntCC : u8
    {
        eq,
        ne,
        slt,
        sle,
        ult,
        ule
    };

    enum class FloatCC : u8
    {
        eq,
        lt,
        le,
        gt,
        ge,
        ne
    };

    struct Inst
    {
        InstKind kind {};
        ValueRef result {};
        Type     result_type {};
        Type     t_type {};
        u32      operand_offset {};
        u16      operand_count {};
        BlockRef target_block {};
        BlockRef false_block {};
        u32      imm_idx {};

        union
        {
            IntCC   icc;
            FloatCC fcc;
        } predicate {};
    };
} // namespace rasi