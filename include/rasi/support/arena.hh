#pragma once

#include <cassert>
#include <cstddef>

namespace rasi
{
    class Arena
    {
    public:
        explicit Arena( const std::size_t size ) : m_size( size )
        {
            m_buffer = new std::byte[size];
        }

        /// @brief Copy constructor deletes to prevent duplicated arenas.
        Arena( const Arena& ) = delete;

        /// @brief Copy assignment deletes to prevent duplicated arenas.
        Arena& operator=( const Arena& ) = delete;

        /// @brief Move constructor, buffer is taken.
        Arena( Arena &&other ) noexcept : m_buffer( other.m_buffer ), m_size( other.m_size ), m_offset( other.m_offset )
        {
            other.m_buffer = nullptr;
            other.m_size = 0;
            other.m_offset = 0;
        }

        /// @brief Move assignment, own buffer is freed and is taken from `other`.
        Arena &operator= ( Arena &&other ) noexcept
        {
            if ( this == &other ) return *this;
            delete[] m_buffer;

            m_buffer = other.m_buffer;
            m_size   = other.m_size;
            m_offset = other.m_offset;

            other.m_buffer = nullptr;
            other.m_size = 0;
            other.m_offset = 0;

            return *this;
        }

        ~Arena()
        {
            delete[] m_buffer;
        }

        std::byte* alloc( const std::size_t size, const std::size_t alignment ) noexcept
        {
            const auto aligned = (m_offset + alignment - 1) & ~(alignment - 1);
            assert( aligned + size <= m_size && "arena out of memory" );

            m_offset = aligned + size;
            return m_buffer + aligned;
        }
    private:
        std::byte*  m_buffer { nullptr };
        std::size_t m_size   { };
        std::size_t m_offset { };
    };
}