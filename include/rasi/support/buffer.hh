#pragma once

#include <cassert>
#include <rasi/support/types.hh>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#else
#include <sys/mman.h>
#endif

#include <array>
#include <cstdint>
#include <cstring>
#include <memory>
#include <span>
#include <stdexcept>

namespace rasi
{
    /// A literal for casting generic integers to std::byte cleanly.
    constexpr std::byte operator""_b ( unsigned long long value ) { return static_cast< std::byte >( value ); }

    /// Custom deleter
    struct VirtualDeleter
    {
        std::size_t size {};

        void operator( ) ( std::byte *ptr ) const noexcept
        {
            if ( ptr == nullptr ) return;
#ifdef _WIN32
            VirtualFree( ptr, 0, MEM_RELEASE );
#else
            munmap( ptr, size );
#endif
        }
    };

    using BufferMemory = std::unique_ptr< std::byte[], VirtualDeleter >;

    class Buffer
    {
    public:

        explicit Buffer( const std::size_t size ) :
            m_size( size ),
            m_memory( [ & ]( ) -> std::byte *
        {
#ifdef _WIN32
            return static_cast< std::byte * >( VirtualAlloc( nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ) );
#else
            void *p = mmap( nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0 );
            return p == MAP_FAILED ? nullptr : static_cast< std::byte * >( p );
#endif
        }( ), VirtualDeleter { size } )
        {
            assert( m_memory && "failed to allocate executable buffer" );
        }

        [[nodiscard]] std::byte *data( ) const noexcept { return m_memory.get( ); }

        [[nodiscard]] std::size_t size( ) const noexcept { return m_size; }

        [[nodiscard]] std::size_t pos( ) const noexcept { return m_pos; }

        [[nodiscard]] std::span< const std::byte > bytes( ) const noexcept { return { m_memory.get( ), m_pos }; }

        void write( const std::vector< std::uint8_t > &data )
        {
            write_bytes( { reinterpret_cast< const std::byte * >( data.data( ) ), data.size( ) } );
        }

        void emit( const std::uint8_t b ) { write_byte( static_cast< std::byte >( b ) ); }

        void emit_u16( const std::uint16_t v ) { write_imm< std::uint16_t >( v ); }

        void emit_u32( const std::uint32_t v ) { write_imm< std::uint32_t >( v ); }

        void emit_u64( const std::uint64_t v ) { write_imm< std::uint64_t >( v ); }

        void write_byte( const std::byte byte ) { write_bytes( { &byte, 1 } ); }

        void write_bytes( const std::span< const std::byte > bytes )
        {
            assert( m_pos + bytes.size() <= m_size && "buffer overflow" );
            std::memcpy( m_memory.get( ) + m_pos, bytes.data( ), bytes.size( ) );
            m_pos += bytes.size( );
        }

        void write_bytes( const std::initializer_list< std::byte > bytes ) { write_bytes( std::span( bytes.begin( ), bytes.size( ) ) ); }

        template < typename T >
            requires std::integral< T >
        void write_imm( const T value )
        {
            assert( m_pos + sizeof(T) <= m_size && "buffer overflow" );
            std::memcpy( m_memory.get( ) + m_pos, &value, sizeof( T ) );
            m_pos += sizeof( T );
        }

        auto patch_at( const std::size_t pos, const std::span< const std::byte > bytes ) const
        {
            assert( pos + bytes.size() <= m_size && "patch out of range" );
            std::memcpy( m_memory.get( ) + pos, bytes.data( ), bytes.size( ) );
        }

        auto skip( const std::size_t amount )
        {
            assert( m_pos + amount <= m_size && "buffer overflow in skip()" );
            m_pos += amount;
        }

        template < typename T >
            requires std::integral< T >
        [[nodiscard]] std::size_t reverse_immediate( )
        {
            const auto offset = m_pos;  
            skip( sizeof( T ) );
            return offset;
        }

        template < typename T >
            requires std::integral< T >
        void patch_immediate( const std::size_t pos, T value )
        {
            assert( pos + sizeof(T) <= m_size && "patch out of range" );
            std::memcpy( m_memory.get( ) + pos, &value, sizeof( T ) );
        }

        [[nodiscard]] bool make_exec( ) const
        {
#ifdef _WIN32
            DWORD old {};
            return VirtualProtect( m_memory.get( ), m_size, PAGE_EXECUTE_READ, &old );
#else
            return mprotect( m_memory.get( ), m_size, PROT_READ | PROT_EXEC ) == 0;
#endif
        }

    private:
        std::size_t  m_size {};
        std::size_t  m_pos {};
        BufferMemory m_memory {};
    };
} // namespace rasi