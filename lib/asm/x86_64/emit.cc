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

    for ( const auto &[ kind, operand_offset, operand_count, result, imm, mem ] : vcode.instructions.as_span( ) )
    {
        const bool has_result
            = operand_count > 0
           && std::ranges::any_of( vcode.operands.as_span( ).subspan( operand_offset, operand_count ), []( const Operand &op )
        {
            return op.dir == OperandDir::def;
        } );

        const Reg rd = has_result && !ctx.assignments[ result.id ].spilled ? phys( ctx, result ) : Reg {};

        std::vector< Reg > uses;
        for ( u32 i { operand_offset }; i < operand_offset + operand_count; ++i )
        {
            if ( const auto &op = vcode.operands[ i ]; op.dir == OperandDir::use && !ctx.assignments[ op.vreg.id ].spilled )
                uses.push_back( phys( ctx, op.vreg ) );
        }

        const Reg mem_base = mem.base.id != 0
            ? phys( ctx, mem.base )
            : Reg {};

        dispatch( buf,  kind, rd, uses, imm );
    }

    return buf;
}
