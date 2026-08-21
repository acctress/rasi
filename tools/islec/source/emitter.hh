#pragma once

#include "dec_tree.hh"
#include <ostream>

class emitter
{
public:
    explicit emitter( const dec_node &root_node, std::ostream &os ) : m_root { root_node }, m_out { os } { }

    void emit( );

private:
    const dec_node &m_root;
    std::ostream   &m_out;
    int             m_indent {};

    void emit_node( const dec_node &node );
    void emit_branch( const branch_node &node );
    void emit_leaf( const leaf_node &node ) const;
    void emit_fail( ) const;

    void indent( ) const;
};
