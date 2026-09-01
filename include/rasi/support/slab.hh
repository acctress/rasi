#pragma once

#include <algorithm>
#include <cstring>
#include <rasi/support/arena.hh>
#include <span>

namespace rasi
{

    /// @brief A Slab<T> owns a growable array of T allocated from an arena.
    template < typename T > class Slab
    {
    public:
        Slab( Arena &arena, const std::size_t init_capacity ) : m_arena( &arena ), m_capacity( init_capacity )
        {
            m_arr = reinterpret_cast< T * >( arena.alloc( init_capacity * sizeof( T ), alignof( T ) ) );
        }

        /// @brief Pushes a value into the slab, growing if necessary.
        /// @return Index of the pushed value.
        std::size_t push( T value ) noexcept
        {
            if ( m_count == m_capacity )
            {
                auto new_block = reinterpret_cast< T * >( m_arena->alloc( m_capacity * sizeof( T ) * 2, alignof( T ) ) );
                std::memcpy( new_block, m_arr, m_count * sizeof( T ) );
                m_arr       = new_block;
                m_capacity *= 2;
            }

            new ( m_arr + m_count ) T { std::move( value ) };
            return m_count++;
        }

        [[nodiscard]] T &operator[] ( const std::size_t index ) noexcept
        {
            assert( index < m_count );
            return m_arr[ index ];
        }

        [[nodiscard]] const T &operator[] ( const std::size_t index ) const noexcept
        {
            assert( index < m_count );
            return m_arr[ index ];
        }

        [[nodiscard]] T &back( ) noexcept
        {
            assert( m_count > 0 );
            return m_arr[ m_count - 1 ];
        }

        [[nodiscard]] const T &back( ) const noexcept
        {
            assert( m_count > 0 );
            return m_arr[ m_count - 1 ];
        }

        T *begin( ) noexcept { return m_arr; }

        T *end( ) noexcept { return m_arr + m_count; }

        const T *begin( ) const noexcept { return m_arr; }

        const T *end( ) const noexcept { return m_arr + m_count; }

        std::span< T > as_span( ) noexcept { return std::span( m_arr, m_count ); }

        std::span< const T > as_span( ) const noexcept { return std::span( m_arr, m_count ); }

        [[nodiscard]] std::size_t size( ) const noexcept { return m_count; };

    private:
        Arena      *m_arena { nullptr };
        T          *m_arr { nullptr };
        std::size_t m_count {};
        std::size_t m_capacity {};
    };
} // namespace rasi