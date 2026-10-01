#include <rasi/vcode/printer.hh>

#include <rasi/vcode/registers.hh>
#include <string>

using namespace rasi;
using namespace rasi::vcode;
using namespace rasi::regalloc;
using namespace rasi::registers;

static std::string fmt_vreg( const AllocContext &ctx, const VReg vreg )
{
    if ( vreg.id >= ctx.assignments.size( ) ) return "v" + std::to_string( vreg.id ) + "(??)";
    const auto &a = ctx.assignments[ vreg.id ];
    if ( a.spilled ) return "v" + std::to_string( vreg.id ) + "(spill@" + std::to_string( a.slot.offset ) + ")";
    return "v" + std::to_string( vreg.id ) + "(" + std::string( reg_name( a.reg.id ) ) + ")";
}

void vcode::print_vcode( std::ostream &os, const VCode &vcode, const AllocContext &ctx )
{
    for ( u32 i {}; i < vcode.instructions.size( ); ++i )
    {
        const auto &inst = vcode.instructions[ i ];
        os << i << ": " << mach_inst_name( inst.kind );

        if ( inst.result.id != 0 || inst.operand_count > 0 )
        {
            os << " " << fmt_vreg( ctx, inst.result ) << " <-";
            for ( u32 j { inst.operand_offset }; j < inst.operand_offset + inst.operand_count; ++j )
            {
                const auto &op = vcode.operands[ j ];
                os << " [" << ( op.dir == OperandDir::def ? "def" : "use" );
                if ( op.policy == OperandPolicy::fixed ) os << ",fixed";
                os << " " << fmt_vreg( ctx, op.vreg ) << "]";
            }
        }

        os << " imm=" << inst.imm << "\n";
    }
}
