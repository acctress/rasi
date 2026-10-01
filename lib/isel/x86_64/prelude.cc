#include <rasi/isel/x86_64/prelude.hh>

#include "rasi/vcode/registers.hh"

using namespace rasi;
using namespace rasi::isel;
using namespace rasi::isel::x86_64;

static ExtValueRefPair make_binop( const Function &fn, const Inst &inst, const InstKind kind )
{
    if ( inst.kind != kind ) return std::nullopt;
    return std::pair { fn.operands[ inst.operand_offset ], fn.operands[ inst.operand_offset + 1 ] };
}

ExtValueRefPair x86_64::extract_iadd( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::iadd ); }

ExtValueRefPair x86_64::extract_isub( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::isub ); }

ExtValueRefPair x86_64::extract_imul( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::imul ); }

ExtValueRefPair x86_64::extract_sdiv( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::sdiv ); }

ExtValueRefPair x86_64::extract_udiv( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::udiv ); }

ExtValueRefPair x86_64::extract_and( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::and_ ); }

ExtValueRefPair x86_64::extract_or( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::or_ ); }

ExtValueRefPair x86_64::extract_xor( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::xor_ ); }

ExtValueRefPair x86_64::extract_shl( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::shl ); }

ExtValueRefPair x86_64::extract_shr( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::shr ); }

ExtValueRefPair x86_64::extract_sar( const Function &fn, const Inst &inst ) { return make_binop( fn, inst, InstKind::sar ); }

ExtValueRef x86_64::extract_load( const Function &fn, const Inst &inst )
{
    if ( inst.kind != InstKind::load ) return std::nullopt;
    return fn.operands[ inst.operand_offset ];
}

ExtValueRefPair x86_64::extract_store( const Function &fn, const Inst &inst )
{
    if ( inst.kind != InstKind::store ) return std::nullopt;
    return std::pair { fn.operands[ inst.operand_offset ], fn.operands[ inst.operand_offset + 1 ] };
}

std::optional< u32 > x86_64::extract_iconst( const Inst &inst )
{
    if ( inst.kind != InstKind::iconst ) return std::nullopt;
    return inst.imm_idx;
}

ExtValueRef x86_64::extract_ret( const Function &fn, const Inst &inst )
{
    if ( inst.kind != InstKind::ret || inst.operand_count == 0 ) return std::nullopt;
    return fn.operands[ inst.operand_offset ];
}

ExtValueRef x86_64::extract_ret_void( const Inst &inst )
{
    if ( inst.kind != InstKind::ret || inst.operand_count != 0 ) return std::nullopt;
    return ValueRef {};
}

VReg x86_64::construct_add_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::mov_rr64, rd, { Operand::def( rd ), Operand::use( a ) } );
    vcode.append( MachInstKind::add_rr64, rd, { Operand::def( rd ), Operand::use( b ), Operand::use( rd ) } );
    return rd;
}

VReg x86_64::construct_add_ri( VCode &vcode, const VReg a, const i64 imm )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::mov_rr64, rd, { Operand::def( rd ), Operand::use( a ) } );
    const auto kind = ( imm >= -128 && imm <= 127 )                   ? MachInstKind::add_ri8
                    : ( imm >= -2147483648LL && imm <= 2147483647LL ) ? MachInstKind::add_ri32
                                                                      : MachInstKind::add_ri32;
    vcode.append( kind, rd, { Operand::def( rd ), Operand::use( rd ) }, imm );
    return rd;
}

VReg x86_64::construct_sub_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::sub_rr64, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_sub_ri( VCode &vcode, const VReg a, const i64 imm )
{
    const auto rd   = vcode.new_vreg( );
    const auto kind = ( imm >= -128 && imm <= 127 ) ? MachInstKind::sub_ri8 : MachInstKind::sub_ri32;
    vcode.append( kind, rd, { Operand::def( rd ), Operand::use( a ) }, imm );
    return rd;
}

VReg x86_64::construct_imul_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::imul_rr64, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_idiv_r( VCode &vcode, const VReg a )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::idiv_r64, rd, { Operand::def( rd ), Operand::use( a ) } );
    return rd;
}

VReg x86_64::construct_udiv_r( VCode &vcode, const VReg a )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::udiv_r64, rd, { Operand::def( rd ), Operand::use( a ) } );
    return rd;
}

VReg x86_64::construct_and_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::and_rr64, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_or_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::or_rr64, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_xor_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::xor_rr64, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_shl_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::shl_ri8, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_shr_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::shr_ri8, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_sar_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::sar_ri8, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_mov_ri( VCode &vcode, const i64 imm )
{
    const auto rd   = vcode.new_vreg( );
    const auto kind = ( imm >= -2147483648LL && imm <= 2147483647LL ) ? MachInstKind::mov_mi32 : MachInstKind::mov_ri64;
    vcode.append( kind, rd, { Operand::def( rd ) }, imm );
    return rd;
}

VReg x86_64::construct_mov_rr( VCode &vcode, const VReg src )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::mov_rr64, rd, { Operand::def( rd ), Operand::use( src ) } );
    return rd;
}

VReg x86_64::construct_load( VCode &vcode, const VReg base, const i32 offset )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::load_rm64, rd, { Operand::def( rd ), Operand::use( base ) }, 0, MemRef { base, offset } );
    return rd;
}

void x86_64::construct_store( VCode &vcode, const VReg src, const VReg base, const i32 offset )
{
    vcode.append_void( MachInstKind::store_mr64, { Operand::use( src ), Operand::use( base ) }, 0, MemRef { base, offset } );
}

void x86_64::construct_ret( VCode &vcode, const VReg val )
{
    vcode.append_void_epi( MachInstKind::ret, { Operand::use_fixed( val, PhysReg { registers::RAX } ) } );
}

void x86_64::construct_ret_void( VCode &vcode ) { vcode.append_void( MachInstKind::ret, {} ); }