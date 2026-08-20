#include "checker.hh"
#include "parser.hh"

#include <iostream>
#include <ostream>
#include <print>

int main( )
{
    constexpr std::string_view source = R"(
(type Value     (primitive))
)";

    try
    {
        auto       p   = parser( source );
        const auto ast = p.parse( );
        auto       ch  = checker( ast );

        ch.check( );
    } catch( const std::exception& e )
    {
        std::cerr << e.what( ) << std::endl;
    }
}