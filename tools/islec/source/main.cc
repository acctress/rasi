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

        std::ofstream output { output_filename };
        if ( !output.is_open( ) ) throw std::runtime_error( "Could not open output file" );

        emitter out { root, output };
        out.emit( );

        output.close( );
    }
    catch ( const std::exception &error )
    {
        std::cerr << "error: " << error.what( ) << '\n';
        return 1;
    }
}