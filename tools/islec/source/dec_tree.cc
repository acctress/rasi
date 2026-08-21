#include "dec_tree.hh"
#include <algorithm>

dec_node dec_tree::build( std::vector< const rule_node * > rules )
{
    std::ranges::sort( rules, []( const rule_node *a, const rule_node *b )
    {
        return a->priority > b->priority;
    } );

    return build_node( rules, 0 );
}

dec_node dec_tree::build_node( const std::vector< const rule_node * > &rules, const std::size_t depth )
{
    if ( rules.empty( ) ) return fail_node {};
    if ( rules.size( ) == 1 ) return leaf_node { rules[ 0 ] };

    children_map                                                             children {};
    std::unordered_map< std::string_view, std::vector< const rule_node * > > groups {};

    for ( const auto *r : rules )
        groups[ r->pattern.head ].push_back( r );

    for ( auto &[ head, group ] : groups )
        children.emplace( head, std::make_unique< dec_node >( build_node( group , depth + 1 ) ) );

    return branch_node { static_cast< int >( depth ), std::move( children ) };
}
