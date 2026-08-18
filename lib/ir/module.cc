#include <rasi/ir/module.hh>

using namespace rasi;

FunctionRef Module::new_function( const std::string_view name, const std::span< const Type > param_types, const Type return_type )
{
    Function func { name, return_type, ( &arena ) };

    for ( const auto type : param_types )
        func.param_types.push( type );

    const u32 idx = static_cast< u32 >( functions.push( std::move( func ) ) );
    return FunctionRef{ idx };
}

std::optional<
    std::reference_wrapper<const Function>
> Module::get_function( const std::string_view name )
{
    for ( auto& func : functions.as_span( ) )
    {
        if ( func.name == name ) return func;
    }

    return std::nullopt;
}
