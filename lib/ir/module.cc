#include <rasi/ir/module.hh>

using namespace rasi;

FunctionRef Module::new_function( const std::string_view name, const std::initializer_list< Type > param_types, const Type return_type,
                                  const CallConv conv )
{
    Function  func { name, param_types, return_type, &arena, conv };
    const u32 idx = static_cast< u32 >( functions.push( std::move( func ) ) );
    return FunctionRef { idx };
}

Function &Module::get_function( const FunctionRef fn ) { return functions[ fn.id ]; }

std::optional< std::reference_wrapper< const Function > > Module::get_function( const std::string_view name )
{
    for ( auto &func : functions.as_span( ) )
    {
        if ( func.name == name ) return func;
    }

    return std::nullopt;
}
