#include <rasi/asm/x86_64/dispatcher.hh>
#include <rasi/asm/x86_64/emit.hh>

#include <print>

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
        const bool has_result
            = inst.operand_count > 0
           && std::ranges::any_of( vcode.operands.as_span( ).subspan( inst.operand_offset, inst.operand_count ), []( const Operand &op )
        {
            return op.dir == OperandDir::def;
        } );

        const Reg rd = has_result && !ctx.assignments[ inst.result.id ].spilled ? phys( ctx, inst.result ) : Reg {};

        std::vector< Reg > uses;
        for ( u32 i { inst.operand_offset }; i < inst.operand_offset + inst.operand_count; ++i )
        {
            if ( const auto &op = vcode.operands[ i ]; op.dir == OperandDir::use && !ctx.assignments[ op.vreg.id ].spilled )
                uses.push_back( phys( ctx, op.vreg ) );
        }

        dispatch( buf, inst.kind, rd, uses, inst.imm );
    }

    return buf;
}
