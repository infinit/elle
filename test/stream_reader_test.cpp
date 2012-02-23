//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#include "stream_reader_test.h"
#include "utils_test.h"
#include <json_spirit/stream_reader.h>

#include <sstream>
#include <boost/assign/list_of.hpp>

using namespace json_spirit;
using namespace std;
using namespace boost;
using namespace boost::assign;

namespace
{
    template< class Config_type >
    struct Test_runner
    {
        typedef typename Config_type::String_type String_type;
        typedef typename Config_type::Object_type Object_type;
        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::ValueType ValueType;
        typedef typename Config_type::Pair_type Pair_type;
        typedef typename String_type::value_type  Char_type;
        typedef typename String_type::const_iterator Iter_type;
        typedef std::basic_istringstream< Char_type > Istringstream_type;
        typedef std::basic_istream< Char_type > Istream_type;

        String_type to_str( const char* c_str )
        {
            return ::to_str< String_type >( c_str );
        }

        Test_runner()
        {
        }

        void check_stream_reader( BasicStreamReader< Istream_type, ValueType >& reader, const vector< int >& expected_result )
        {
            ValueType v;
            const bool ok = reader.read( v );
            assert_eq( ok, true );
            assert_eq( v.type(), ValueType::ARRAY_TYPE );
            assert_eq( v.get_array().size(), expected_result.size() );
            for( vector< int >::size_type i = 0; i < v.get_array().size(); ++i )
            {
                assert_eq( v.get_array()[i], expected_result[i] );
            }
        }

        void check_stream_read_or_throw( BasicStreamReader< Istream_type, ValueType >& reader, const vector< int >& expected_result )
        {
            ValueType v;

            try
            {
                reader.read( v );
                assert_eq( v.type(), ValueType::ARRAY_TYPE );
                assert_eq( v.get_array().size(), expected_result.size() );
                for( vector< int >::size_type i = 0; i < v.get_array().size(); ++i )
                {
                    assert_eq( v.get_array()[i], expected_result[i] );
                }
            }
            catch( ... )
            {
                assert( false );
            }
        }

        void test_stream_reader( const char* s )
        {
            {
                Istringstream_type is( to_str( s ) );

                BasicStreamReader< Istream_type, ValueType > reader( is, false );

                check_stream_reader( reader, vector< int >() );
                check_stream_reader( reader, list_of( 1 ) );
                check_stream_reader( reader, list_of( 1 )( 2 ) );
                check_stream_reader( reader, list_of( 1 )( 2 )( 3 ) );

                ValueType v;
                const bool ok = reader.read( v );
                assert_eq( ok, false );
            }
            {
                Istringstream_type is( to_str( s ) );

                BasicStreamReader< Istream_type, ValueType > reader( is, true);

                check_stream_read_or_throw( reader, vector< int >() );
                check_stream_read_or_throw( reader, list_of( 1 ) );
                check_stream_read_or_throw( reader, list_of( 1 )( 2 ) );
                check_stream_read_or_throw( reader, list_of( 1 )( 2 )( 3 ) );

                try
                {
                    ValueType v;
                    reader.read( v );
                    assert( false );
                }
                catch( ... )
                {
                }
            }
        }

        void run_tests()
        {
            test_stream_reader( "[][1][1,2][1,2,3]" );
            test_stream_reader( "[] [1] [1,2] [1,2,3]" );
            test_stream_reader( " [] [1] [1,2] [1,2,3] " );
        }
    };
}

void json_spirit::test_stream_reader()
{
#ifdef JSON_SPIRIT_VALUE_ENABLED
    Test_runner< Config >().run_tests();
#endif

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    Test_runner< wConfig >().run_tests();
#endif
}
