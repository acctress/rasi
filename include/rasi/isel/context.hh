#pragma once

#include <optional>
#include <rasi/ir/function.hh>
#include <rasi/vcode/vcode.hh>
#include <unordered_map>
#include <vector>

namespace rasi::isel
{
    struct ISELCtx
    {
        Function &fn;
        VCode    &vcode;

        /// @brief ValueRef.id -> lowered VReg
        std::unordered_map< u32, VReg > value_cache;

        /// @brief Use counts for each ValueRef.id, built before lowering
        /// > if use_counts[v.id] == 1, then sinking is legal
        std::vector< u32 > use_counts;

        explicit ISELCtx( Function &fn, VCode &vcode ) : fn( fn ), vcode( vcode ) { init_use_counts( ); }

        VReg lower_value( ValueRef ref ) noexcept
        {

        }

        [[nodiscard]] bool legal_sink( const ValueRef ref ) const noexcept
        {
            return ref.id < use_counts.size( ) && use_counts[ ref.id ] == 1;
        }

        /// @brief Find a cached VReg for a ValueRef
        [[nodiscard]] std::optional< VReg > cached( const ValueRef ref ) const noexcept
        {
            const auto iter = value_cache.find( ref.id );
            if ( iter == value_cache.end( ) ) return std::nullopt;
            return iter->second;
        }

        void cache( const ValueRef ref, const VReg reg ) noexcept { value_cache[ ref.id ] = reg; }

        /// @brief Returns the instruction which produced the ValueRef
        [[nodiscard]] const Inst *producer( const ValueRef ref ) const noexcept
        {
            for ( const auto &inst : fn.instructions.as_span( ) )
                if ( inst.result.id == ref.id ) return &inst;
            return nullptr;
        }

    private:
        void init_use_counts( ) noexcept
        {
            use_counts.resize( fn.value_types.size( ), 0 );
            for ( const auto &inst : fn.instructions.as_span( ) )
                for ( u32 i { inst.operand_offset }; i < inst.operand_offset + inst.operand_count; ++i )
                    ++use_counts[ fn.operands[ i ].id ];
        }
    };
} // namespace rasi::isel