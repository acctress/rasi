#include <rasi/asm/x86_64/emit.hh>
#include <rasi/asm/x86_64/dispatcher.hh>

using namespace rasi;
using namespace rasi::x86;
using namespace rasi::azm;
using namespace rasi::regalloc;

static Reg phys( const AllocContext &ctx, const VReg vreg )
{
    assert( !ctx.assignments[ vreg.id ].spilled && "spilled vreg not reloaded bfore emit" );
    return Reg { ctx.assignments[ vreg.id ].reg.id };
}

Buffer x86_64::emit( const VCode &vcode, const AllocContext &ctx )
{
    Buffer buf;

    for ( const auto &inst : vcode.instructions.as_span( ) )
    {
        std::vector< Reg > regs;
        for ( u32 i { inst.operand_offset }; i < inst.operand_offset + inst.operand_count; ++i )
        {
            if ( const auto &op = vcode.operands[ i ]; !ctx.assignments[ op.vreg.id ].spilled ) regs.push_back( phys( ctx, op.vreg ) );
        }

        dispatch( buf, mach_inst_name( inst.kind ), regs, inst.imm );
    }

    return buf;
}
