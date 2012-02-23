//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#include "value_test.h"
#include "utils_test.h"
#include <json_spirit/value.h>
#include <limits.h>
#include <list>
#include <boost/assign/list_of.hpp>
#include <boost/integer_traits.hpp>

using namespace json_spirit;
using namespace std;
using namespace boost;
using namespace boost::assign;

namespace
{
#ifdef JSON_SPIRIT_VALUE_ENABLED

    const int64_t max_int64   = integer_traits< int64_t  >::max();
    const uint64_t max_uint64 = integer_traits< uint64_t >::max();

    void test_obj_value()
    {
        const Object::value_type p1( "name1", "value1" );
        const Object::value_type p3( "name3", "value3" );

        Object obj_1; obj_1.insert( p1 );
        Object obj_2; obj_2.insert( p1 );
        Object obj_3; obj_3.insert( p3 );

        Value v1( obj_1 );
        Value v2( obj_2 );
        Value v3( obj_3 );

        assert_eq( v1.type(), Value::OBJECT_TYPE );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        assert_eq( v1.getObject(), obj_1 );
        assert_eq( v3.getObject(), obj_3 );
    }

    void test_array_value()
    {
        Array array_1; array_1.push_back( 1 ); array_1.push_back( "2" );
        Array array_2; array_2.push_back( 1 ); array_2.push_back( "2" );
        Array array_3; array_3.push_back( 1 ); array_3.push_back( "X" );

        Value v1( array_1 );
        Value v2( array_2 );
        Value v3( array_3 );

        assert_eq( v1.type(), Value::ARRAY_TYPE );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        assert_eq( v1.getArray(), array_1 );
        assert_eq( v3.getArray(), array_3 );
    }

    void test_bool_value()
    {
        Value v1( true );
        Value v2( true );
        Value v3( false );

        assert_eq( v1.type(), Value::BOOL_TYPE );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        assert( v1.getBool() );
        assert( !v3.getBool() );
    }

    void test_int_value()
    {
        Value v1( 1 );
        Value v2( 1 );
        Value v3( INT_MAX );

        assert_eq( v1.type(), Value::INT_TYPE );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        unsigned int uint_max = INT_MAX;

        assert_eq( v1.getInt(),    1 );
        assert_eq( v1.getInt64(),  1 );
        assert_eq( v1.getUInt64(), 1u );
        assert_eq( v3.getInt(),    INT_MAX );
        assert_eq( v3.getInt64(),  INT_MAX );
        assert_eq( v3.getUInt64(), uint_max );

        Value v4( max_int64 );

        assert_eq( v4.getInt64(), max_int64 );
        assert_eq( v4.getUInt64(), static_cast< uint64_t >( max_int64 ) );

        const uint64_t max_int64_plus_1 = max_int64 + uint64_t( 1 );

        Value v5( max_int64_plus_1 );

        assert_eq( v5.getUInt64(), max_int64_plus_1 );

        Value v6( max_uint64 );

        assert_eq( v6.getUInt64(), max_uint64 );

        Value v7( 0 );

        assert_eq( v7.getInt(),    0 );
        assert_eq( v7.getInt64(),  0 );
        assert_eq( v7.getUInt64(), 0u );

        Value v8( -1 );

        assert_eq( v8.getInt(),   -1 );
        assert_eq( v8.getInt64(), -1 );
        assert_eq( v8.getUInt64(), max_uint64 );
    }

    void test_real_value()
    {
        Value v1( 1.0 );
        Value v2( 1.0 );
        Value v3( 2.0 );

        assert_eq( v1.type(), Value::REAL_TYPE );
        assert_eq ( v1, v2 );
        assert_neq( v1, v3 );

        assert_eq( v1.getReal(), 1.0 );
        assert_eq( v3.getReal(), 2.0 );
    }

    void test_null_value()
    {
        Value v1;
        Value v2;

        assert_eq( v1.type(), Value::NULL_TYPE );
        assert_eq( v1.isNull(), true );
        assert_eq( v1, v2 );
        assert_eq( v1.isNull(), true );
        assert_eq( Value( 1 ).isNull(), false );
    }

    template< typename T >
    void test_get_value( const T& t )
    {
        assert_eq( Value( t ).getValue< T >(), t );
    }

    void test_get_value()
    {
        test_get_value( 123 );
        test_get_value( max_int64 );
        test_get_value( 1.23 );
        test_get_value( true );
        test_get_value( false );
        test_get_value( string( "test" ) );

        Array a; a.push_back( 1 ); a.push_back( "2" );

        test_get_value( a );

        Object obj; obj.insert( Object::value_type( "name1", "value1" ) );

        test_get_value( obj );
    }

    void assert_array_eq( const Value& v, const Array& a )
    {
        assert_eq( v.getArray(), a );
    }

    void assert_obj_eq( const Value& v, const Object& obj )
    {
        assert_eq( v.getObject(), obj );
    }

    template< typename T >
    void check_copy( const T& t )
    {
        const Value v1( t );
        const Value v2( v1 );
        Value v3;
        v3 = v1;

        assert_eq( v1, v2 );
        assert_eq( v1, v3 );

        assert_eq( v2.getValue< T >(), t );
        assert_eq( v3.getValue< T >(), t );

        assert_eq( v1.isUInt64(), v2.isUInt64() );
        assert_eq( v1.isUInt64(), v3.isUInt64() );
    }

    void check_copying_null()
    {
        const Value v1;
        const Value v2( v1 );
        Value v3;
        v3 = v1;

        assert_eq( v2.type(), Value::NULL_TYPE );
        assert_eq( v3.type(), Value::NULL_TYPE );
    }

    void test_copying()
    {
        {
            const Array array_1 = list_of(1)(2);

            Value v1( array_1 );
            const Value v2( v1 );

            assert_array_eq( v1, array_1 );
            assert_array_eq( v2, array_1 );

            v1.getArray()[0] = 3;

            assert_array_eq( v1, list_of(3)(2) );
            assert_array_eq( v2, array_1 );
        }
        {
            const Object obj_1 = map_list_of( "a", 1 )( "b", 2 );

            Value v1( obj_1 );
            Value v2;

            v2 = v1;

            assert_obj_eq( v1, obj_1 );
            assert_obj_eq( v2, obj_1 );
        }
        {
            check_copy( 1 );
            check_copy( 2.0 );
            check_copy( max_int64 );
            check_copy( max_uint64 );
            check_copy( string("test") );
            check_copy( true );
            check_copy( false );
            const Array array_1 = list_of(1)(2);
            check_copy( array_1 );
            const Object obj_1 = map_list_of( "a", 1 )( "b", 2 );
            check_copy( obj_1 );
            check_copying_null();
        }
    }

    template< typename ObjectType > void check_pair_typedefs( ObjectType &object )
    {
        typename ObjectType::key_type name = object.begin()->first;
        typename ObjectType::value_type::second_type value = object.begin()->second;
    }

    void check_pair_typedefs()
    {
        Object o;
        check_pair_typedefs( o );

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
        wObject wo;
        check_pair_typedefs( wo );
#endif
    }

    void test_obj_map_implemention()
    {
#ifdef JSON_SPIRIT_VALUE_ENABLED
        Object obj;

        obj[ "name 1" ] = 1;
        obj[ "name 2" ] = "two";

        assert_eq( obj.size(), 2u );

        assert_eq( obj.find( "name 1" )->second.getInt(), 1 );
        assert_eq( obj.find( "name 2" )->second.getString(), "two" );
#endif
    }

    template< typename Int >
    void check_an_int_is_a_real( Int i, bool expected_result )
    {
        assert_eq( Value( i ).isUInt64(), expected_result );
    }

    void test_is_uint64()
    {
        check_an_int_is_a_real( 1,                            false );
        check_an_int_is_a_real( static_cast< int64_t  >( 1 ), false );
        check_an_int_is_a_real( static_cast< uint64_t >( 1 ), true );
    }

    template< typename Int >
    void check_an_int_is_a_real( Int i, double expected_result )
    {
        assert_eq( Value( i ).getReal(), expected_result );
    }

    void test_an_int_is_a_real()
    {
        check_an_int_is_a_real( -1, -1.0 );
        check_an_int_is_a_real(  0,  0.0 );
        check_an_int_is_a_real(  1,  1.0 );
        check_an_int_is_a_real( max_int64,  9223372036854775800.0 );
        check_an_int_is_a_real( max_uint64, 18446744073709552000.0 );
    }

    template< typename T >
    void check_wrong_type_exceptions( const Value::Type vtype )
    {
        Value v;

        assert_eq( v.type(), Value::NULL_TYPE );

        try
        {
            v.getValue< T >();

            assert( false );
        }
        catch( const runtime_error& e )
        {
            ostringstream os;

            os << "value type is " << (int)Value::NULL_TYPE << " not " << (int)vtype;

            assert_eq( e.what(), os.str() );
        }
    }

    void test_wrong_type_exceptions()
    {
        check_wrong_type_exceptions< Object >( Value::OBJECT_TYPE );
        check_wrong_type_exceptions< Array >( Value::ARRAY_TYPE );
        check_wrong_type_exceptions< string >( Value::STRING_TYPE );
        check_wrong_type_exceptions< bool >( Value::BOOL_TYPE );
        check_wrong_type_exceptions< boost::int64_t >( Value::INT_TYPE );
        check_wrong_type_exceptions< int >( Value::INT_TYPE );
        check_wrong_type_exceptions< double >( Value::REAL_TYPE );
    }

void test_path_insert() {
    Object n;
    Value v1(n);
    assert_eq( v1.type(), Value::OBJECT_TYPE);

    bool inserted;

    // Single element path, integer
    inserted = v1.insert("a", 7);
    assert_eq(inserted, true);
    assert( v1.getObject().find("a") != v1.getObject().end());
    assert_eq( v1.getObject()["a"].type(), Value::INT_TYPE );
    assert_eq( v1.getObject()["a"].getInt(), 7);

    // Two element path, string
    inserted = v1.insert("b.cd", "XXX");
    assert_eq(inserted, true);
    assert( v1.getObject().find("b") != v1.getObject().end());
    assert_eq( v1.getObject()["b"].type(), Value::OBJECT_TYPE );
    assert( v1.getObject()["b"].getObject().find("cd") != v1.getObject()["b"].getObject().end());
    assert_eq( v1.getObject()["b"].getObject()["cd"].type(), Value::STRING_TYPE);
    assert_eq( v1.getObject()["b"].getObject()["cd"].getString(), "XXX");

    // Two elements, preserve previous elements
    inserted = v1.insert("b.e", "YYY");
    assert_eq(inserted, true);
    assert_eq( v1.getObject()["b"].getObject()["cd"].getString(), "XXX");
    assert( v1.getObject()["b"].getObject().find("e") != v1.getObject()["b"].getObject().end());
    assert_eq( v1.getObject()["b"].getObject()["e"].type(), Value::STRING_TYPE);
    assert_eq( v1.getObject()["b"].getObject()["e"].getString(), "YYY");

    // Fail to insert over existing element
    inserted = v1.insert("b.e", "ZZZ");
    assert_eq(inserted, false);
    assert_eq( v1.getObject()["b"].getObject()["e"].getString(), "YYY");
}

void test_path_put() {
    // Assume test_path_insert() passes. We only need to check the changes in
    // behavior: overwriting data.

    Object n;
    Value v1(n);
    assert_eq( v1.type(), Value::OBJECT_TYPE);

    // Prep some test data
    v1.put("b.e", "YYY");
    assert( v1.getObject()["b"].getObject().find("e") != v1.getObject()["b"].getObject().end());
    assert_eq( v1.getObject()["b"].getObject()["e"].type(), Value::STRING_TYPE);
    assert_eq( v1.getObject()["b"].getObject()["e"].getString(), "YYY");

    // Overwrite test
    v1.put("b.e", 2);
    assert( v1.getObject()["b"].getObject().find("e") != v1.getObject()["b"].getObject().end());
    assert_eq( v1.getObject()["b"].getObject()["e"].type(), Value::INT_TYPE);
    assert_eq( v1.getObject()["b"].getObject()["e"].getInt(), 2);
}

#endif

    template< class Config_type >
    class Container_constructor_runner
    {
    public:

        Container_constructor_runner()
        {
            vector< double > vd = list_of( 1.2 )( 1.3 );  test_container_constructor( vd );
            vector< int >    vi = list_of( 1 );           test_container_constructor( vi );
                             vi = list_of( 1 )( 2 );      test_container_constructor( vi );
                             vi = list_of( 1 )( 2 )( 3 ); test_container_constructor( vi );
            list< double >   ld = list_of( 1.2 )( 1.3 );  test_container_constructor( ld );
            list< int >      li = list_of( 1 );           test_container_constructor( li );
                             li = list_of( 1 )( 2 );      test_container_constructor( li );
                             li = list_of( 1 )( 2 )( 3 ); test_container_constructor( li );
        }

    private:

        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::ValueType ValueType;

        template< class Cont >
        void test_container_constructor( const Cont& cont )
        {
            typedef typename Cont::value_type Cont_value_type;
            const ValueType val( cont.begin(), cont.end() );
            const Array_type& arr = val.getArray();
            Cont result;
            for( unsigned int i = 0; i < arr.size(); ++i )
            {
                result.push_back( arr[i].template getValue< Cont_value_type>() );
            }
            assert_eq( result, cont );
        }
    };

    void test_container_constructor()
    {
#ifdef JSON_SPIRIT_VALUE_ENABLED
       Container_constructor_runner< Config >();
#endif
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
        Container_constructor_runner< wConfig >();
#endif
    }

    template< class Config_type >
    class Variant_constructor_runner
    {
    public:

        Variant_constructor_runner()
        {
            test_variant_constructor< variant< int, double > >( 1.23 );
            test_variant_constructor< variant< int, double > >( 123 );
            test_variant_constructor< variant< int, double, String_type > >( to_str< String_type >( "foo" ) );
            test_variant_constructor< variant< int, double, String_type, bool > >( true );
            test_variant_constructor< variant< int, double, String_type, bool, boost::int64_t > >( boost::int64_t( 123 ) );
            test_variant_constructor< variant< int, double, String_type, bool, boost::uint64_t > >( boost::uint64_t( 123 ) );

            {
                variant< int, Null > variant = Null();
                const ValueType val( variant );
                assert( val.isNull() );
            }

            vector< double > vd = list_of( 1.2 )( 1.3 );   test_variant_array_constructor< double > ( vd );
            vector< int >    vi = list_of( 1 );            test_variant_array_constructor< int >( vi );
                             vi = list_of( 1 )( 2 );       test_variant_array_constructor< int >( vi );
                             vi = list_of( 1 )( 2 )( 3 );  test_variant_array_constructor< int >( vi );
            list< double >   ld = list_of( 1.2 )( 1.3 );   test_variant_array_constructor< double >( ld );
            list< int >      li = list_of( 1 );            test_variant_array_constructor< int >( li );
                             li = list_of( 1 )( 2 );       test_variant_array_constructor< int >( li );
                             li = list_of( 1 )( 2 )( 3 );  test_variant_array_constructor< int >( li );
        }

    private:

        typedef typename Config_type::String_type String_type;
        typedef typename Config_type::Array_type Array_type;
        typedef typename Config_type::ValueType ValueType;

        template< class Variant_t, typename T >
        void test_variant_constructor( const T& t )
        {
            const Variant_t variant( t );
            const ValueType val( variant );
            assert_eq( val.template getValue< T >(), t );
        }

        template< typename T, typename A, template< typename, typename > class Cont >
        void test_variant_array_constructor( const Cont< T, A >& cont )
        {
            const variant< int, Cont< T, A > > variant = cont;
            const ValueType val( variant );
            const Array_type& arr = val.getArray();
            Cont< T, A > result;
            for( unsigned int i = 0; i < arr.size(); ++i )
            {
                result.push_back( arr[i].template getValue< T >() );
            }
            assert_eq( result, cont );
        }
    };

    void test_variant_constructor()
    {
#ifdef JSON_SPIRIT_VALUE_ENABLED
        Variant_constructor_runner< Config >();
#endif
#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
       Variant_constructor_runner< wConfig >();
#endif
    }
}

void json_spirit::test_value()
{
#ifdef JSON_SPIRIT_VALUE_ENABLED
    Object obj;
    Value value_str ( "value" );
    Value value_obj ( obj );
    Value value_bool( true );

    Value value_str_2 ( string( "value" ) );
    Value value_obj_2 ( obj );
    Value value_bool_2( false );

    const char* str( "value" );
    Value value_str_2b ( str );

    assert_eq( value_str, value_str );
    assert_eq( value_str, value_str_2 );
    assert_eq( value_str, value_str_2b );
    assert_eq( value_obj, value_obj );
    assert_eq( value_obj, value_obj_2 );
    assert_neq( value_str, value_obj );
    assert_neq( value_str, value_bool );

    Object obj_2;
    obj_2.insert( Object::value_type( "name", value_str ) );
    Value value_str_3( "xxxxx" );
    Value value_obj_3( obj_2 );

    assert_neq( value_str, value_str_3 );
    assert_neq( value_obj, value_obj_3 );

    test_obj_value();
    test_array_value();
    test_bool_value();
    test_int_value();
    test_real_value();
    test_null_value();
    test_get_value();
    test_copying();
    test_obj_map_implemention();
    test_is_uint64();
    test_an_int_is_a_real();
    test_wrong_type_exceptions();
    test_path_insert();
    test_path_put();
#endif
    test_container_constructor();
    test_variant_constructor();
}
