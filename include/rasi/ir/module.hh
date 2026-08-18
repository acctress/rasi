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

        FunctionRef new_function( std::string_view name, std::span< const Type > param_types, const Type return_type );
        std::optional<
            std::reference_wrapper<const Function>
        >   get_function( std::string_view name );
    };
}