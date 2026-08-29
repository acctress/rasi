#include <rasi/isel/x86_64/prelude.hh>

using namespace rasi;
using namespace rasi::isel;
using namespace rasi::isel::x86_64;

static ExtValueRefPair make_binop( const Inst &inst, const InstKind kind )
{
    if ( inst.kind != kind ) return std::nullopt;
    return std::pair { ValueRef { inst.operand_offset }, ValueRef { inst.operand_offset + 1 } };
}

ExtValueRefPair x86_64::extract_iadd( const Inst &inst ) { return make_binop( inst, InstKind::iadd ); }

ExtValueRefPair x86_64::extract_isub( const Inst &inst ) { return make_binop( inst, InstKind::isub ); }

ExtValueRefPair x86_64::extract_imul( const Inst &inst ) { return make_binop( inst, InstKind::imul ); }

ExtValueRefPair x86_64::extract_sdiv( const Inst &inst ) { return make_binop( inst, InstKind::sdiv ); }

ExtValueRefPair x86_64::extract_udiv( const Inst &inst ) { return make_binop( inst, InstKind::udiv ); }

ExtValueRefPair x86_64::extract_and( const Inst &inst ) { return make_binop( inst, InstKind::and_ ); }

ExtValueRefPair x86_64::extract_or( const Inst &inst ) { return make_binop( inst, InstKind::or_ ); }

ExtValueRefPair x86_64::extract_xor( const Inst &inst ) { return make_binop( inst, InstKind::xor_ ); }

ExtValueRefPair x86_64::extract_shl( const Inst &inst ) { return make_binop( inst, InstKind::shl ); }

ExtValueRefPair x86_64::extract_shr( const Inst &inst ) { return make_binop( inst, InstKind::shr ); }

ExtValueRefPair x86_64::extract_sar( const Inst &inst ) { return make_binop( inst, InstKind::sar ); }

std::optional< i64 > x86_64::extract_iconst( const Inst &inst )
{
    if ( inst.kind != InstKind::iconst ) return std::nullopt;
    return inst.imm_idx;
}

ExtValueRef x86_64::extract_ret( const Inst &inst )
{
    if ( inst.kind != InstKind::ret || inst.operand_count == 0 ) return std::nullopt;
    return ValueRef { inst.operand_offset };
}

ExtValueRef x86_64::extract_ret_void( const Inst &inst )
{
    if ( inst.kind != InstKind::ret || inst.operand_count != 0 ) return std::nullopt;
    return ValueRef { 0 };
}

VReg x86_64::construct_add_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::add, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_add_ri( VCode &vcode, const VReg a, const i64 imm )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::add, rd, { Operand::def( rd ), Operand::use( a ) }, imm );
    return rd;
}

VReg x86_64::construct_sub_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::sub, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_sub_ri( VCode &vcode, const VReg a, const i64 imm )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::sub, rd, { Operand::def( rd ), Operand::use( a ) }, imm );
    return rd;
}

VReg x86_64::construct_imul_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::imul, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_idiv_r( VCode &vcode, const VReg a )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::idiv, rd, { Operand::def( rd ), Operand::use( a ) } );
    return rd;
}

VReg x86_64::construct_udiv_r( VCode &vcode, const VReg a )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::udiv, rd, { Operand::def( rd ), Operand::use( a ) } );
    return rd;
}

VReg x86_64::construct_and_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::and_, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_or_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::or_, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_xor_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::xor_, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_shl_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::shl, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_shr_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::shr, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_sar_rr( VCode &vcode, const VReg a, const VReg b )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::sar, rd, { Operand::def( rd ), Operand::use( a ), Operand::use( b ) } );
    return rd;
}

VReg x86_64::construct_mov_ri( VCode &vcode, const i64 imm )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::mov, rd, { Operand::def( rd ) }, imm );
    return rd;
}

VReg x86_64::construct_mov_rr( VCode &vcode, const VReg src )
{
    const auto rd = vcode.new_vreg( );
    vcode.append( MachInstKind::mov, rd, { Operand::def( rd ), Operand::use( src ) } );
    return rd;
}

void x86_64::construct_ret( VCode &vcode, const VReg val ) { vcode.append_void( MachInstKind::ret, { Operand::use( val ) } ); }

void x86_64::construct_ret_void( VCode &vcode ) { vcode.append_void( MachInstKind::ret, {} ); }