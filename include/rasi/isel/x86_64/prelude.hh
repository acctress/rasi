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

    ExtValueRefPair      extract_iadd( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_isub( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_imul( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_sdiv( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_udiv( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_and( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_or( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_xor( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_shl( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_shr( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_sar( const Function &fn, const Inst &inst );
    std::optional< u32 > extract_iconst( const Inst &inst );
    ExtValueRef          extract_load( const Function &fn, const Inst &inst );
    ExtValueRefPair      extract_store( const Function &fn, const Inst &inst );
    ExtValueRef          extract_ret( const Function &fn, const Inst &inst );
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
    VReg construct_load( VCode &vcode, VReg base, i32 offset );
    void construct_store( VCode &vcode, VReg src, VReg base, i32 offset );
    void construct_ret( VCode &vcode, VReg val );
    void construct_ret_void( VCode &vcode );

} // namespace rasi::isel::x86_64