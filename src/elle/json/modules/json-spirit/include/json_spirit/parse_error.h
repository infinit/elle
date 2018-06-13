#ifndef JSON_SPIRIT_PARSE_ERROR
#define JSON_SPIRIT_PARSE_ERROR

//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <string>
#include "platform.h"

namespace json_spirit
{
    // An ParseError exception is thrown by the "read_or_throw" functions below on finding an error.
    // Note the "read_or_throw" functions are around 3 times slower than the standard functions "read"
    // functions that return a bool.
    //
    struct JSON_SPIRIT_EXPORT ParseError
    {
        ParseError();
        ParseError( unsigned int line, unsigned int column, const std::string& reason );
        bool operator==( const ParseError& lhs ) const;
        unsigned int line_;
        unsigned int column_;
        std::string reason_;
    };

    inline ParseError::ParseError()
    :   line_( 0 )
    ,   column_( 0 )
    {
    }

    inline ParseError::ParseError( unsigned int line, unsigned int column, const std::string& reason )
    :   line_( line )
    ,   column_( column )
    ,   reason_( reason )
    {
    }

    inline bool ParseError::operator==( const ParseError& lhs ) const
    {
        if( this == &lhs ) return true;

        return ( reason_ == lhs.reason_ ) &&
               ( line_   == lhs.line_ ) &&
               ( column_ == lhs.column_ );
    }
}

#endif
