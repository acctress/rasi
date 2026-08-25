#include <rasi/codegen/codegen.hh>

using namespace rasi::codegen;
using namespace rasi::x86_64;

void CodeGen::emit( const VCode &vcode, const std::unordered_map< u32, AllocResult > &alloc, Buffer &buf )
{
    emit_prologue( buf );

    for ( u32 i = 0; i < vcode.insts.size( ); ++i )
    {
        const auto &inst = vcode.insts[ i ];

        auto op_reg = [ & ]( const u32 idx ) -> Reg
        {
            return resolve( vcode.operands[ inst.operand_offset + idx ].vreg, alloc );
        };

        switch ( inst.kind )
        {
            case InstKind::copy :
            {
                if (inst.operand_count == 0)
                    break;

                const auto dst = resolve( inst.result, alloc );

                if ( const auto src = op_reg( 0 ); dst.idx != src.idx )
                    emit_mov_rr64( buf, dst, src );

                break;
            }
            case InstKind::iadd :
            {
                const auto dst = resolve( inst.result, alloc );
                const auto lhs = op_reg( 0 );
                const auto rhs = op_reg( 1 );
                emit_mov_rr64( buf, dst, lhs );
                emit_add_rr64( buf, dst, rhs );
                std::printf(
                    "iadd: dst=%u lhs=%u rhs=%u\n",
                    dst.idx,
                    lhs.idx,
                    rhs.idx
                );
                break;
            }

            case InstKind::isub :
            {
                const auto dst = resolve( inst.result, alloc );
                const auto rhs = op_reg( 1 );
                emit_sub_rr64( buf, dst, rhs );
                break;
            }

            case InstKind::imul :
            {
                const auto dst = resolve( inst.result, alloc );
                const auto rhs = op_reg( 1 );
                emit_imul_rr64( buf, dst, rhs );
                break;
            }

            case InstKind::ret :
            {
                if ( const auto src = op_reg( 0 ); src.idx != 0 ) emit_mov_rr64( buf, Reg { 0 }, src );
                break;
            }

            default : break;
        }
    }

    emit_epilogue( buf );
}

void CodeGen::emit_prologue( Buffer &buf )
{
    emit_push_r64( buf, Reg { 5 } );
    emit_mov_rr64( buf, Reg { 5 }, Reg { 4 } );
}

void CodeGen::emit_epilogue( Buffer &buf )
{
    emit_pop_r64( buf, Reg { 5 } );
    emit_ret( buf );
}

Reg CodeGen::resolve( const VReg vreg, const std::unordered_map< u32, AllocResult > &alloc )
{
    const auto &a = alloc.at( vreg.id );
    assert( std::holds_alternative< PhysReg >( a ) && "spills not implemented" );
    return Reg { std::get< PhysReg >( a ).index( ) };
}
