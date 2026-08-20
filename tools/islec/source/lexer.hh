#pragma once

#include <cstdint>
#include <string_view>

[[nodiscard]] static constexpr bool is_digit( const char c ) noexcept { return static_cast< unsigned >( c - '0' ) < 10; }

[[nodiscard]] static constexpr bool is_alpha( const char c ) noexcept { return static_cast< unsigned >( ( c | 32 ) - 'a' ) < 26; }

[[nodiscard]] static constexpr bool is_alphanum( const char c ) noexcept { return is_digit( c ) || is_alpha( c ); }


enum class token_type : std::uint8_t
{
    LPAREN,
    RPAREN,
    IDENT,
    INT,
    EOF_
};

struct token
{
    token_type type{};
    std::size_t start{}, end{};
    std::size_t line{}, col{};
};

class lexer
{
public:
    explicit lexer( const std::string_view source ) : m_source { source } {}

    token next();
private:
    std::string_view m_source {};
    std::size_t      m_pos {}, m_ln {}, m_col {};

    [[nodiscard]]
    token tok( token_type ty, size_t start, size_t end ) const;


    [[nodiscard]]
    char current( ) const;
    void advance();
    void skip_ws();
    [[nodiscard]]
    bool not_eof( ) const;
};
