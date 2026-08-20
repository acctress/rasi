#include "parser.hh"

std::vector< top_level > parser::parse( )
{
    std::vector< top_level > result;

    while ( !matches( token_type::EOF_ ) )
    {
        expect( token_type::LPAREN );
        const auto val = lexeme( );
        consume( );

        if ( val == "type" ) result.push_back( parse_type_decl( ) );
        else if ( val == "decl" )
            result.push_back( parse_extern_decl( ) );
        else if ( val == "rule" )
            result.push_back( parse_rule( ) );
        else
            throw std::runtime_error( "unknown top-level form: " + std::string( val ) );
    }

    return result;
}

type_decl parser::parse_type_decl( )
{
    const auto type_name = lexeme( );
    consume( );

    expect( token_type::LPAREN );
    const auto ty = lexeme( );
    consume( );

    if ( ty == "primitive" )
    {
        expect( token_type::RPAREN );
        expect( token_type::RPAREN );
        return type_decl { .name = type_name, .kind = prim_type {} };
    }
    if ( ty == "enum" )
    {
        std::vector< std::string_view > variants;
        while ( !matches( token_type::RPAREN ) )
        {
            variants.push_back( lexeme( ) );
            consume( );
        }

        expect( token_type::RPAREN );
        expect( token_type::RPAREN );
        return type_decl { .name = type_name, .kind = enum_type { .variants = std::move( variants ) } };
    }
    throw std::runtime_error( std::format( "unknown top-level type kind '{}'", type_name ) );
}

extern_decl parser::parse_extern_decl( )
{
    const auto kind = lexeme( );
    consume( );

    const auto ek = kind == "extractor" ? extern_kind::ex : extern_kind::co;

    const auto name = lexeme( );
    consume( );

    expect( token_type::LPAREN );

    std::vector< std::string_view > params;
    while ( !matches( token_type::RPAREN ) )
    {
        params.push_back( lexeme( ) );
        consume( );
    }

    expect( token_type::RPAREN );

    const auto ret = lexeme( );
    consume( );

    expect( token_type::RPAREN );
    return extern_decl { .name = name, .params = std::move( params ), .ret = ret, .kind = ek };
}

expr parser::parse_expr( )
{
    if ( matches( token_type::LPAREN ) )
    {
        consume( );
        const auto head = lexeme( );
        consume( );

        std::vector< expr > args;
        while ( !matches( token_type::RPAREN ) )
            args.push_back( parse_expr( ) );

        expect( token_type::RPAREN );
        return expr { .head = head, .args = std::move( args ) };
    }

    const auto head = lexeme( );
    consume( );

    return expr { .head = head, .args = {} };
}

rule parser::parse_rule( )
{
    int prio { 0 };
    if ( matches( token_type::INT ) )
    {
        prio = std::stoi( std::string( lexeme( ) ) );
        consume( );
    }

    const auto pattern = parse_expr( );
    const auto body    = parse_expr( );

    expect( token_type::RPAREN );
    return rule { .priority = prio, .pattern = pattern, .body = body };
}
