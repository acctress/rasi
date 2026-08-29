#pragma once

#include <optional>
#include <rasi/ir/instruction.hh>
#include <rasi/isel/context.hh>
#include <rasi/vcode/vcode.hh>
#include <utility>

namespace rasi::isel::x86_64
{
    using ExtValueRefPair = std::optional< std::pair< ValueRef, ValueRef > >;
    using ExtValueRef     = std::optional< ValueRef >;

    ExtValueRefPair      extract_iadd( const Inst &inst );
    ExtValueRefPair      extract_isub( const Inst &inst );
    ExtValueRefPair      extract_imul( const Inst &inst );
    ExtValueRefPair      extract_sdiv( const Inst &inst );
    ExtValueRefPair      extract_udiv( const Inst &inst );
    ExtValueRefPair      extract_and( const Inst &inst );
    ExtValueRefPair      extract_or( const Inst &inst );
    ExtValueRefPair      extract_xor( const Inst &inst );
    ExtValueRefPair      extract_shl( const Inst &inst );
    ExtValueRefPair      extract_shr( const Inst &inst );
    ExtValueRefPair      extract_sar( const Inst &inst );
    std::optional< i64 > extract_iconst( const Inst &inst );
    ExtValueRef          extract_ret( const Inst &inst );
    ExtValueRef          extract_ret_void( const Inst &inst );

    VReg construct_add_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_add_ri( VCode &vcode, VReg a, i64 imm );
    VReg construct_sub_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_sub_ri( VCode &vcode, VReg a, i64 imm );
    VReg construct_imul_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_idiv_r( VCode &vcode, VReg a );
    VReg construct_udiv_r( VCode &vcode, VReg a );
    VReg construct_and_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_or_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_xor_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_shl_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_shr_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_sar_rr( VCode &vcode, VReg a, VReg b );
    VReg construct_mov_ri( VCode &vcode, i64 imm );
    VReg construct_mov_rr( VCode &vcode, VReg src );
    void construct_ret( VCode &vcode, VReg val );
    void construct_ret_void( VCode &vcode );

} // namespace rasi::isel::x86_64