#include <rasi/asm/x86_64/assembler.hh>
#include <rasi/isel/x86_64/abi.hh>
#include <rasi/isel/x86_64/prelude.hh>

namespace rasi::isel::x86_64
{
    using namespace registers;

    namespace detail
    {
        [[nodiscard]] constexpr u32 align_16( const u32 value, const u32 alignment ) noexcept
        {
            return ( value + alignment - 1 ) & ~( alignment - 1 );
        }

        [[nodiscard]] u32 outgoing_size( const FrameLayout &frame, const CallConv call_conv ) noexcept
        {
            const u32 mini = call_conv == CallConv::win_fastcall ? 32u : 0u;
            return std::max( frame.outgoing_args_size, mini );
        }

        [[nodiscard]] u32 stack_alloc_size( const FrameLayout &frame, const CallConv call_conv ) noexcept
        {
            const u32 req = outgoing_size( frame, call_conv ) + frame.fixed_frame_stor_size + frame.clobber_size;
            return align_16( req, 16u );
        }

        [[nodiscard]] constexpr x86::Reg reg( const u8 id ) noexcept { return x86::Reg { id }; }
    } // namespace detail

    void emit_prologue( Buffer &buffer, const FrameLayout &frame, const CallConv call_conv )
    {
        constexpr auto rsp = detail::reg( RSP );
        constexpr auto rbp = detail::reg( RBP );

        x86::emit_push_r64( buffer, rbp );
        x86::emit_mov_rr64( buffer, rbp, rsp );

        if ( const u32 alloc_size = detail::stack_alloc_size( frame, call_conv ); alloc_size <= 127 )
        {
            if ( alloc_size != 0 ) x86::emit_sub_ri8( buffer, rsp, static_cast< u8 >( alloc_size ) );
        }
        else
        {
            x86::emit_sub_ri32( buffer, rsp, alloc_size );
        }

        const u32 clobber_base = detail::outgoing_size( frame, call_conv ) + frame.fixed_frame_stor_size;

        for ( u32 i {}; i < frame.clobbered_callee_saves.size( ); ++i )
        {
            const auto saved_reg = detail::reg( frame.clobbered_callee_saves[ i ].id );
            const u32  offset    = clobber_base + i * 8u;
            x86::emit_store_mr64( buffer, rsp, static_cast< i32 >( offset ), saved_reg );
        }
    }

    void emit_epilogue( Buffer &buffer, const FrameLayout &frame, CallConv call_conv )
    {
        constexpr auto rsp = detail::reg( RSP );
        constexpr auto rbp = detail::reg( RBP );

        const u32 clobber_base = detail::outgoing_size( frame, call_conv ) + frame.fixed_frame_stor_size;

        for ( auto i { frame.clobbered_callee_saves.size(  ) }; i > 0; --i )
        {
            const auto idx = i - 1;
            const auto saved = detail::reg( frame.clobbered_callee_saves[ idx ].id );
            const u32 offset = clobber_base + static_cast<u32>( idx ) * 8u;

            x86::emit_load_rm64(
                buffer,
                saved,
                rsp,
                static_cast<i32>( offset )
            );
        }

        if ( const u32 alloc_size = detail::stack_alloc_size( frame, call_conv ); alloc_size <= 127 )
        {
            if ( alloc_size != 0 ) x86::emit_add_ri8( buffer, rsp, static_cast< u8 >( alloc_size ) );
        }
        else
        {
            x86::emit_add_ri32( buffer, rsp, alloc_size );
        }

        x86::emit_pop_r64( buffer, rbp );
    }

} // namespace rasi::isel::x86_64