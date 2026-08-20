#include "lexer.hh"

token lexer::next( )
{
    while ( true )
    {
        skip_ws( );

        if ( !not_eof( ) ) return tok( token_type::EOF_, m_pos, m_pos );

        switch ( current( ) )
        {
            case '\n' :
                advance( );
                ++m_ln;
                m_col = 0;
                continue;

            case ';' :
                while ( not_eof( ) && current( ) != '\n' )
                    advance( );
                continue;

            case '(' :
            {
                const auto start = m_pos;
                advance( );
                return tok( token_type::LPAREN, start, m_pos );
            }

            case ')' :
            {
                const auto start = m_pos;
                advance( );
                return tok( token_type::RPAREN, start, m_pos );
            }

            default : break;
        }

        if ( is_digit( current( ) ) || current( ) == '-' )
        {
            const auto start = m_pos;

            if ( current( ) == '-' ) advance( );

            while ( not_eof( ) && is_digit( current( ) ) )
                advance( );

            return tok( token_type::INT, start, m_pos );
        }

        if ( is_alpha( current( ) ) || current( ) == '_' )
        {
            const auto start = m_pos;

            while ( not_eof( ) && ( is_alphanum( current( ) ) || current( ) == '_' || current( ) == '-' ) )
            {
                advance( );
            }

            return tok( token_type::IDENT, start, m_pos );
        }

        advance( );
    }
}

token lexer::tok( const token_type ty, const std::size_t start, const std::size_t end ) const
{
    return token {
        .type  = ty,
        .start = start,
        .end   = end,
        .line  = m_ln,
        .col   = m_col,
    };
}

char lexer::current( ) const { return m_source.at( m_pos ); }

void lexer::skip_ws( )
{
    while ( not_eof( ) && ( current( ) == ' ' || current( ) == '\t' ) )
        advance( );
}

void lexer::advance( )
{
    if ( not_eof( ) )
    {
        m_pos++;
        m_col++;
    }
}

bool lexer::not_eof( ) const { return m_pos < m_source.length( ); }
