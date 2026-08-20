#include "lexer.hh"

token lexer::next( )
{
    skip_ws( );

    switch ( current( ) )
    {
        case '\n' :
        {
            m_ln++;
            m_col = 0;
            advance( );
            return next( );
        }
        case '(' :
        {
            advance( );
            return tok( token_type::LPAREN, m_pos - 1, m_pos );
        }
        case ')' :
        {
            advance( );
            return tok( token_type::RPAREN, m_pos - 1, m_pos );
        }
        case ';' :
        {
            while ( not_eof( ) && current( ) != '\n' )
                advance( );
            return next( );
        }
        default :
        {
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
                    advance( );
                return tok( token_type::IDENT, start, m_pos );
            }

            advance( );

            return next( );
        }
    }
}

token lexer::tok( const token_type ty, const std::size_t start, const std::size_t end ) const
{
    return token {
        .type  = ty,
        .start = start == 0 ? m_pos : start,
        .end   = end == 0 ? m_pos : end,
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
