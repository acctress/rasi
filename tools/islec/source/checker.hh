#pragma once
#include "parser.hh"

#include <string_view>
#include <unordered_map>
#include <unordered_set>

class checker
{
public:
    explicit checker( const std::vector< top_level > &ast ) : m_ast( ast ) { }

    void check( );

private:
    const std::vector< top_level > &m_ast;

    std::unordered_map< std::string_view, const extern_decl * > m_extractors_sym_table;
    std::unordered_map< std::string_view, const extern_decl * > m_constructors_sym_table;
    std::unordered_set< std::string_view >                      m_types;

    void collect_decls( );
    void check_rule( const rule & );
    void check_expr_patt( const expr & );
    void check_expr_body( const expr & );
};
