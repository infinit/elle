#ifndef JSON_SPIRIT_READ_STREAM
#define JSON_SPIRIT_READ_STREAM

//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "reader_template.h"

namespace json_spirit
{
    // these classes allows you to read multiple top level contiguous values from a stream,
    // the normal stream read functions have a bug that prevent multiple top level values
    // from being read unless they are separated by spaces

/** StreamReader lets you read multiple top-level values from a stream. Normally
 *  you are only able to parse one top-level object (e.g. a top-level object),
 *  but this lets you place them in a stream instead. This class can work with
 *  exceptions or by just returning a success code.
 */
    template< class Istream_type, class ValueType >
    class BasicStreamReader
    {
    public:
        BasicStreamReader( Istream_type& is, bool throw_on_error = false)
         :   throw_(throw_on_error),
             iters_( is ),
             posn_begin_( iters_.begin_, iters_.end_ ),
             posn_end_( iters_.end_, iters_.end_ )
        {
        }

        /** Read the next value from the stream, inserting it into value if
         *  successful.
         *
         *  \param value reference to a Value to parse
         *  \returns true if another value was parsed
         *  \throw ParseError on failure to parse, if the StreamReader is using
         *         exceptions
         */
        bool read( ValueType& value )
        {
            if (!throw_)
                return read_range( iters_.begin_, iters_.end_, value );
            else {
                posn_begin_ = read_range_or_throw( posn_begin_, posn_end_, value );
                return true;
            }
        }

    private:

        typedef Multi_pass_iters< Istream_type > Mp_iters;
        typedef spirit_namespace::position_iterator< typename Mp_iters::Mp_iter > Posn_iter_t;

        const bool throw_;
        Mp_iters iters_;
        Posn_iter_t posn_begin_, posn_end_;
    };


// typedefs for ASCII
#ifdef JSON_SPIRIT_VALUE_ENABLED
template<typename Istream_type>
class StreamReader : public BasicStreamReader<Istream_type, Config::String_type> {
public:
    StreamReader( Istream_type& is, bool throw_on_error = false)
     : BasicStreamReader<Istream_type, Config::String_type>(is, throw_on_error)
    {}
};
#endif
// typedefs for Unicode
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
template<typename Istream_type>
class wStreamReader : public BasicStreamReader<Istream_type, wConfig::String_type> {
public:
    wStreamReader( Istream_type& is, bool throw_on_error = false)
     : BasicStreamReader<Istream_type, wConfig::String_type>(is, throw_on_error)
    {}
};
#endif

}

#endif
