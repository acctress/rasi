#include "checker.hh"

void checker::check( )
{
    collect_decls( );
    for ( const auto &n : m_ast )
        if ( const auto *r = std::get_if< rule >( &n ) ) check_rule( *r );
}

void checker::collect_decls( )
{
    for ( const auto &no : m_ast )
    {
        std::visit( [ & ]< typename Ty >( const Ty &n )
        {
            using T = std::decay_t< Ty >;
            if constexpr ( std::is_same_v< T, type_decl > )
            {
                if ( !m_types.insert( n.name ).second ) throw std::runtime_error( std::format( "duplicate type decl '{}'", n.name ) );
            }
            else if constexpr ( std::is_base_of_v< T, extern_decl > )
            {
                if ( n.kind == extern_kind::ex )
                {
                    if ( !m_extractors_sym_table.emplace( n.name, &n ).second )
                        throw std::runtime_error( std::format( "duplicate extractor decl '{}'", n.name ) );
                }
                else if ( n.kind == extern_kind::co )
                {
                    if ( !m_constructors_sym_table.emplace( n.name, &n ).second )
                        throw std::runtime_error( std::format( "duplicate constructor decl '{}'", n.name ) );
                }
            }
        }, no );
    }
}

void checker::check_rule( const rule &r )
{
    check_expr_patt( r.pattern );
    check_expr_body( r.body );
}

void checker::check_expr_patt( const expr &e )
{
    if ( e.args.empty( ) ) return;

    if ( !m_extractors_sym_table.contains( e.head ) )
        throw std::runtime_error( std::format( "unknown extractor '{}' in pattern", e.head ) );

    if ( const auto *d = m_extractors_sym_table.at( e.head ); e.args.size( ) != d->params.size( ) )
        throw std::runtime_error( std::format( "extractor '{}' expects {} args, but got {}", e.head, d->params.size( ), e.args.size( ) ) );

    for ( const auto &a : e.args )
        check_expr_patt( a );
}

void checker::check_expr_body( const expr &e )
{
    if (e.args.empty( ) ) return;

    if (!m_extractors_sym_table.contains( e.head ) )
        throw std::runtime_error( std::format( "unknown constructor '{}' in rule body", e.head ) );

    if ( const auto *d = m_constructors_sym_table.at( e.head ); e.args.size( ) != d->params.size( ) )
        throw std::runtime_error( std::format( "constructor '{}' expects {} args, but got {}", e.head, d->params.size( ), e.args.size( ) ) );

    for ( const auto &a : e.args )
        check_expr_body( a );
}
