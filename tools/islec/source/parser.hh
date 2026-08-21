#pragma once

#include "lexer.hh"

#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

struct expr_node
{
    std::string_view    head;
    std::vector< expr_node > args;
};

enum class extern_kind : bool
{
    ex,
    co
};

struct prim_type
{
};

struct enum_type
{
    std::vector< std::string_view > variants;
};

struct type_decl_node
{
    std::string_view                     name;
    std::variant< prim_type, enum_type > kind;
};

struct extern_decl_node
{
    std::string_view                name;
    std::vector< std::string_view > params;
    std::string_view                ret;
    extern_kind                     kind;
};

struct rule_node
{
    int  priority {};
    expr_node pattern;
    expr_node body;
};

using top_level = std::variant< type_decl_node, extern_decl_node, rule_node >;

class parser
{
public:
    explicit parser( const std::string_view source ) : m_source( source ), m_lexer( source ), m_current( m_lexer.next( ) ) { }

    std::vector< top_level > parse( );

private:
    std::string_view m_source;
    lexer            m_lexer;
    token            m_current;

    expr_node        parse_expr( );
    type_decl_node   parse_type_decl( );
    extern_decl_node parse_extern_decl( );
    rule_node        parse_rule( );

    [[nodiscard]] std::string_view lexeme( ) const noexcept { return m_source.substr( m_current.start, m_current.end - m_current.start ); }

    [[nodiscard]] bool matches( const token_type t ) const noexcept { return m_current.type == t; }

    [[nodiscard]] token peek( ) const noexcept { return m_current; }

    token consume( ) { return std::exchange( m_current, m_lexer.next( ) ); }

    token expect( const token_type ty )
    {
        if ( m_current.type != ty ) throw std::runtime_error( std::format( "unexpected token at {}:{}", m_current.line, m_current.col ) );
        return consume( );
    }
};
