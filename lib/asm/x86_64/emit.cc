#include <rasi/asm/x86_64/emit.hh>

using namespace rasi;
using namespace rasi::x86;
using namespace rasi::azm;
using namespace rasi::regalloc;

static Reg phys( const AllocContext &ctx, const VReg vreg )
{
    assert( !ctx.assignments[ vreg.id ].spilled && "spilled vreg not reloaded bfore emit" );
    return Reg { ctx.assignments[ vreg.id ].reg.id };
}

static Reg opr( const VCode &vc, const AllocContext &ctx, const u32 operand_offset, const u32 idx )
{
    return phys( ctx, vc.operands[ operand_offset + idx ].vreg );
}

Buffer x86_64::emit( const VCode &vcode, const AllocContext &ctx )
{
    Buffer buf;

    for ( const auto &inst : vcode.instructions.as_span( ) )
    {
        switch ( inst.kind )
        {
            case MachInstKind::mov :
            {
                /* mov rd, rs */
                if ( inst.operand_count == 2 )
                {
                    const auto rd = opr( vcode, ctx, inst.operand_offset, 0 );
                    const auto rs = opr( vcode, ctx, inst.operand_offset, 1 );
                    emit_mov_rr64( buf, rd, rs );
                }
                /* mov rd, imm */
                else if ( inst.operand_count == 1 )
                {
                    const auto rd = opr( vcode, ctx, inst.operand_offset, 0 );
                    if ( inst.imm >= 0 && inst.imm <= 0xFFFF'FFFF ) emit_mov_mi32( buf, rd, static_cast< u32 >( inst.imm ) );
                    else
                        emit_mov_ri64( buf, rd, static_cast< u64 >( inst.imm ) );
                }

                break;
            }

            case MachInstKind::add :
            {
                if ( inst.operand_count == 2 )
                {
                    const auto rd = opr( vcode, ctx, inst.operand_offset, 0 );
                    const auto rs = opr( vcode, ctx, inst.operand_offset, 1 );
                    emit_add_rr64( buf, rd, rs );
                }
                else if ( inst.operand_count == 1 )
                {
                    const auto rd = opr( vcode, ctx, inst.operand_offset, 0 );
                    if ( inst.imm >= -128 && inst.imm <= 127 ) emit_add_ri8( buf, rd, static_cast< u8 >( inst.imm ) );
                    else
                        emit_add_ri32( buf, rd, static_cast< u32 >( inst.imm ) );
                }
            }

            case MachInstKind::ret : emit_ret( buf ); break;

            default : break;
        }
    }

    return buf;
}
