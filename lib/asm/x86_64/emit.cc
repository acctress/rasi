#include <print>
#include <rasi/asm/x86_64/dispatcher.hh>
#include <rasi/asm/x86_64/emit.hh>
#include <rasi/isel/x86_64/abi.hh>

using namespace rasi;
using namespace rasi::azm;
using namespace rasi::regalloc;

static x86::Reg phys( const AllocContext &ctx, const VReg vreg )
{
    assert( !ctx.assignments[ vreg.id ].spilled && "spilled vreg not reloaded bfore emit" );
    return x86::Reg { ctx.assignments[ vreg.id ].reg.id };
}

Buffer x86_64::emit( const VCode &vcode, const AllocContext &ctx, FrameLayout &frame )
{
    Buffer buf { 4096 };

    isel::x86_64::emit_prologue( buf, frame, vcode.call_conv );

    for ( const auto &[ kind, operand_offset, operand_count, result, imm, mem, epi_ph ] : vcode.instructions.as_span( ) )
    {
        if ( epi_ph )
        {
            assert( kind == MachInstKind::ret && "epilogue placeholder must be attached to ret" );

            isel::x86_64::emit_epilogue( buf, frame, vcode.call_conv );
        }

        const bool has_result
            = operand_count > 0
           && std::ranges::any_of( vcode.operands.as_span( ).subspan( operand_offset, operand_count ), []( const Operand &op )
        {
            return op.dir == OperandDir::def;
        } );

        const x86::Reg rd = has_result && !ctx.assignments[ result.id ].spilled ? phys( ctx, result ) : x86::Reg {};

        std::vector< x86::Reg > uses;
        for ( u32 i { operand_offset }; i < operand_offset + operand_count; ++i )
        {
            if ( const auto &op = vcode.operands[ i ]; op.dir == OperandDir::use && !ctx.assignments[ op.vreg.id ].spilled )
                uses.push_back( phys( ctx, op.vreg ) );
        }

        const x86::Reg mem_base = mem.base.id != 0 ? phys( ctx, mem.base ) : x86::Reg {};

        dispatch( buf, kind, rd, uses, imm, mem_base );
    }

    return buf;
}
