#include "dec_tree.hh"
#include <algorithm>

dec_node dec_tree::build( std::vector< const rule_node * > rules )
{
    std::ranges::sort( rules, []( const rule_node *a, const rule_node *b )
    {
        return a->priority > b->priority;
    } );

    return build_node( rules );
}

dec_node dec_tree::build_node( const std::vector< const rule_node * > &rules )
{
    if ( rules.empty( ) ) return fail_node {};

    std::unordered_map< std::string_view, std::vector< const rule_node * > > groups;

    for ( const auto *r : rules )
        groups[ r->pattern.head ].push_back( r );

    if ( groups.size( ) == 1 ) return leaf_node { groups.begin( )->second[ 0 ] };

    children_map children;
    for ( auto &[ head, group ] : groups )
        children.emplace( head, std::make_unique< dec_node >( leaf_node { group[ 0 ] } ) );

    return branch_node { .children = std::move( children ) };
}

const expr_node *dec_tree::at_path( const expr_node &root, const path_t &path )
{
    const auto *cur = &root;
    for ( const auto idx : path )
    {
        if ( idx >= cur->args.size( ) ) return nullptr;
        cur = &cur->args[ idx ];
    }

    return cur;
}
