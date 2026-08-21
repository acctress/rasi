#pragma once

#include "parser.hh"
#include <string_view>
#include <unordered_map>

struct dec_node;

using children_map = std::unordered_map< std::string_view, std::unique_ptr< dec_node > >;

using path_t = std::vector< std::size_t >;

struct branch_node
{
    children_map children;
};

struct leaf_node
{
    const rule_node *matched_rule;
};

struct fail_node
{
};

struct dec_node : std::variant< branch_node, leaf_node, fail_node >
{
    using variant::variant;
};

class dec_tree
{
public:
    dec_tree( )  = default;
    ~dec_tree( ) = default;

    static dec_node         build( std::vector< const rule_node * > rules );
    static dec_node         build_node( const std::vector< const rule_node * > &rules );
    static const expr_node *at_path( const expr_node &root, const path_t &path );
};
