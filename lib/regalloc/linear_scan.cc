#include <rasi/regalloc/linear_scan.hh>

using namespace rasi::regalloc;

std::unordered_map< u32, AllocResult > LinearScan::run( const VCode &vcode )
{
    build_intervals( vcode );
    alloc( );

    std::unordered_map< u32, AllocResult > result;
    for ( const auto &inter : m_intervals )
        result[ inter.reg.id ] = inter.assign;

    return result;
}

void LinearScan::build_intervals( const VCode &vcode )
{
    for ( u32 i { 0 }; i < vcode.insts.size( ); i++ )
    {
        const auto &inst = vcode.insts[ i ];

        if ( inst.result.id != ~0u )
        {
            auto interval = LiveInterval { .reg = inst.result, .start = i, .end = i };
            if ( inst.output_constraint.kind == ConstraintKind::fixed )
            {
                interval.assign = inst.output_constraint.fixed_reg;
                interval.fixed  = true;
            }

            m_intervals.push_back( interval );
        }

        for ( u32 j = inst.operand_offset; j < inst.operand_offset + inst.operand_count; ++j )
        {
            const auto &o = vcode.operands[ j ];
            if ( o.kind != OperandKind::reg ) continue;

            for ( auto &inter : m_intervals )
            {
                if ( inter.reg.id == o.vreg.id )
                {
                    inter.end = i;
                    break;
                }
            }
        }
    }
}

void LinearScan::alloc( )
{
    std::ranges::sort( m_intervals, []( const LiveInterval &a, const LiveInterval &b )
    {
        return a.start < b.start;
    } );

    std::vector< LiveInterval * > alive;
    std::vector< PhysReg >        free { allocatable.begin( ), allocatable.end( ) };

    for ( auto &inter : m_intervals )
    {
        std::erase_if( alive, [ & ]( const LiveInterval *a )
        {
            if ( a->end < inter.start )
            {
                if ( !a->fixed ) free.push_back( std::get< PhysReg >( a->assign ) );
                return true;
            }

            return false;
        } );

        if ( inter.fixed )
        {
            std::erase( free, std::get< PhysReg >( inter.assign ) );
            alive.push_back( &inter );
            continue;
        }

        if ( free.empty( ) )
        {
            // find the interval with the furthest end for spill
            auto *spill = *std::ranges::max_element( alive, {}, []( const LiveInterval *a )
            {
                return a->end;
            } );

            if ( spill->end > inter.end )
            {
                inter.assign       = std::get< PhysReg >( spill->assign );
                spill->assign      = SpillSlot { m_nx_spill_offset };
                m_nx_spill_offset += 8;
                alive.push_back( &inter );
            }
            else
            {
                inter.assign       = SpillSlot { m_nx_spill_offset };
                m_nx_spill_offset += 8;
            }
        }
        else
        {
            inter.assign = free.back( );
            free.pop_back( );
            alive.push_back( &inter );
        }
    }
}
