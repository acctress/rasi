#include "checker.hh"
#include "dec_tree.hh"
#include "emitter.hh"
#include "parser.hh"

#include <exception>
#include <fstream>
#include <iostream>
#include <print>
#include <string_view>
#include <vector>

int main( int argc, char *argv[] )
{
    if ( argc < 2 ) return 1;

    const auto input_filename  = std::string { argv[ 1 ] };
    const auto output_filename = std::string { argv[ 2 ] };

    std::ifstream file { input_filename };
    std::string   input_source( ( std::istreambuf_iterator( file ) ), std::istreambuf_iterator< char >( ) );

    try
    {
        parser     p { input_source };
        const auto ast = p.parse( );

        checker ch { ast };
        ch.check( );

        std::vector< const rule_node * > rules;

        for ( const auto &node : ast )
        {
            if ( const auto *rule = std::get_if< rule_node >( &node ) ) rules.push_back( rule );
        }

        const auto root = dec_tree::build( std::move( rules ) );

        std::unordered_map< std::string_view, type_decl_node >   types;
        std::unordered_map< std::string_view, extern_decl_node > decls;

        for ( const auto &node : ast )
        {
            std::visit( [ & ]< typename Ty >( const Ty &n )
            {
                using T = std::decay_t< Ty >;
                if constexpr ( std::is_same_v< T, type_decl_node > ) types[ n.name ] = n;
                if constexpr ( std::is_same_v< T, extern_decl_node > ) decls[ n.name ] = n;
            }, node );
        }

        std::ofstream output { output_filename };
        if ( !output.is_open( ) ) throw std::runtime_error( "Could not open output file" );

        emitter out { root, output, types, decls };
        out.emit( );

        output.close( );
    }
    catch ( const std::exception &error )
    {
        std::cerr << "error: " << error.what( ) << '\n';
        return 1;
    }
}