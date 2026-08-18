#pragma once

#include "function.hh"
#include "rasi/support/slab.hh"
#include <span>
#include <functional>
#include <optional>

namespace rasi
{
    struct Module
    {
        explicit Module( ) : arena( 4 * 1024 * 1024 ), functions( arena, 64 ) { }

        Arena          arena;
        Slab<Function> functions;

        FunctionRef new_function( std::string_view name, std::initializer_list< Type > param_types, Type return_type );
        Function& get_function( FunctionRef fn );
        std::optional<
            std::reference_wrapper<const Function>
        >   get_function( std::string_view name );
    };
}