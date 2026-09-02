#include <rasi/regalloc/regalloc.hh>

#include <numeric>

using namespace rasi::regalloc;

void rasi::regalloc::compute_liveness( AllocContext &ctx, const VCode &vcode )
{
    for ( u32 i {}; i < vcode.instructions.size( ); ++i )
    {
        const auto &inst = vcode.instructions[ i ];

        for ( u32 j { inst.operand_offset }; j < inst.operand_offset + inst.operand_count; ++j )
        {
            const auto &op = vcode.operands[ j ];
            const u32   id = op.vreg.id;

            if ( id >= ctx.live_ranges.size( ) ) ctx.live_ranges.resize( id + 1 );

            auto &[ vreg, start, end, uses ] = ctx.live_ranges[ id ];
            vreg                             = op.vreg;
            start                            = ( uses == 0 ) ? i : std::min( start, i );
            end                              = std::max( end, i + 1 );

            ++uses;
        }
    }
}

void rasi::regalloc::alloc( AllocContext &ctx, VCode &vcode )
{
    /* all live ranges are sorted by start */
    std::vector< u32 > order( ctx.live_ranges.size( ) );

    std::iota( order.begin( ), order.end( ), 0 );
    std::ranges::sort( order, [ & ]( const u32 a, const u32 b )
    {
        return ctx.live_ranges[ a ].start < ctx.live_ranges[ b ].start;
    } );

    ctx.assignments.resize( ctx.live_ranges.size( ) );

    std::vector< u32 > active {};
    const auto        &cc = conv_regs( vcode.call_conv );

    std::vector allocatable( ctx.num_regs, false );
    for ( const u8 id : cc.allocatable.span( ) )
    {
        if ( id < allocatable.size( ) ) allocatable[ id ] = true;
    }

    std::vector free = allocatable;

    /* pre colour any operands with the policy fixed */
    for ( const auto &op : vcode.operands.as_span( ) )
    {
        if ( op.policy != OperandPolicy::fixed ) continue;
        const u32 id = op.vreg.id;

        ctx.assignments[ id ] = { .vreg = op.vreg, .spilled = false, .pre_coloured = true, .reg = op.fixed_reg };

        assert( free[ op.fixed_reg.id ] && "conflicting fixed preg assignments" );
        free[ op.fixed_reg.id ] = false;
    }

    for ( const u32 idx : order )
    {
        if ( ctx.assignments[ idx ].pre_coloured || ctx.assignments[ idx ].spilled ) continue;

        const auto &range = ctx.live_ranges[ idx ];

        /* remove / expire all over intervals */
        std::erase_if( active, [ & ]( const u32 a )
        {
            if ( ctx.live_ranges[ a ].end <= range.start )
            {
                const u8 id = ctx.assignments[ a ].reg.id;
                free[ id ]  = allocatable[ id ];
                return true;
            }

            return false;
        } );

        if ( auto iter = std::ranges::find( free, true ); iter != free.end( ) )
        {
            const auto reg = static_cast< u8 >( iter - free.begin( ) );

            free[ reg ]            = false;
            ctx.assignments[ idx ] = { .vreg = range.vreg, .spilled = false, .reg = PhysReg { reg } };
            active.push_back( idx );
        }
        else
        {
            auto evicted = std::ranges::min_element( active, [ & ]( const u32 a, const u32 b )
            {
                return ctx.live_ranges[ a ].spill_weight( ) < ctx.live_ranges[ b ].spill_weight( );
            } );

            if ( ctx.live_ranges[ *evicted ].spill_weight( ) < range.spill_weight( ) )
            {
                const auto reg = ctx.assignments[ *evicted ].reg.id;

                ctx.assignments[ *evicted ]
                    = { .vreg = ctx.live_ranges[ *evicted ].vreg, .spilled = true, .slot = SpillSlot { ctx.nx_spill_slot++ } };
                ctx.assignments[ idx ] = { .vreg = range.vreg, .spilled = false, .reg = PhysReg { reg } };

                active.erase( evicted );
                active.push_back( idx );
            }
            else
            {
                ctx.assignments[ idx ] = { .vreg = range.vreg, .spilled = true, .slot = SpillSlot { ctx.nx_spill_slot++ } };
            }
        }
    }
}

void rasi::regalloc::apply( AllocContext &ctx, VCode &vcode )
{
    for ( auto &op : vcode.operands.as_span( ) )
    {
        if ( op.policy != OperandPolicy::fixed ) continue;

        const u32 id = op.vreg.id;
        if ( id >= ctx.assignments.size( ) ) continue;

        if ( !ctx.assignments[ id ].spilled ) op.vreg = VReg { ctx.assignments[ id ].reg.id };
    }
}
