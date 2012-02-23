#ifndef JSON_SPIRIT_VALUE
#define JSON_SPIRIT_VALUE

//          Copyright John W. Wilkinson 2007 - 2011
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.05

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "platform.h"
#include <vector>
#include <map>
#include <string>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <boost/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/variant.hpp>

// comment out the value types you don't need to reduce build times and intermediate file sizes
#define JSON_SPIRIT_VALUE_ENABLED
#define JSON_SPIRIT_WVALUE_ENABLED

namespace json_spirit {

    struct Null{};

    template< class Config >    // Config determines whether the value uses std::string or std::wstring and
                                // whether JSON Objects are represented as vectors or maps
    class BasicValue
    {
    public:

        /** The type held within a variant. */
        // Note: This order needs to match the order of the variant so
        // variant::which() matches.
        enum Type {
            // Ensuring NULL_TYPE is 0 makes for easy null checks:
            // if (!Value.type())
            //   do something with value
            NULL_TYPE = 0,
            OBJECT_TYPE,
            ARRAY_TYPE,
            STRING_TYPE,
            BOOL_TYPE,
            INT_TYPE,
            REAL_TYPE,

            // Don't use externally
            UINT64_TYPE
        };

        typedef Config Config_type;
        typedef typename Config::String_type String_type;
        typedef typename Config::Object_type Object;
        typedef typename Config::Array_type Array;
        typedef typename String_type::const_pointer Const_str_ptr;  // eg const char*

        BasicValue();  // creates null value
        BasicValue( Const_str_ptr      value );
        BasicValue( const String_type& value );
        BasicValue( const Object&      value );
        BasicValue( const Array&       value );
        BasicValue( bool               value );
        BasicValue( int                value );
        BasicValue( boost::int64_t     value );
        BasicValue( boost::uint64_t    value );
        BasicValue( double             value );

        template< class Iter >
        BasicValue( Iter first, Iter last );    // constructor from containers, e.g. std::vector or std::list

        template< BOOST_VARIANT_ENUM_PARAMS( typename T ) >
        BasicValue( const boost::variant< BOOST_VARIANT_ENUM_PARAMS(T) >& variant ); // constructor for compatible variant types

        BasicValue( const BasicValue& other );

        bool operator==( const BasicValue& lhs ) const;

        BasicValue& operator=( const BasicValue& lhs );

        Type type() const;

        bool is_uint64() const;
        bool is_null() const;

        const String_type& get_str()    const;
        const Object&      get_obj()    const;
        const Array&       get_array()  const;
        bool               get_bool()   const;
        int                get_int()    const;
        boost::int64_t     get_int64()  const;
        boost::uint64_t    get_uint64() const;
        double             get_real()   const;

        Object& get_obj();
        Array&  get_array();

        template< typename T > T get_value() const;  // example usage: int    i = value.get_value< int >();
                                                     // or             double d = value.get_value< double >();

        static const BasicValue null;

    private:

        void check_type( const Type vtype ) const;

        typedef boost::variant< Null, boost::recursive_wrapper< Object >, boost::recursive_wrapper< Array >,
                                String_type, bool, boost::int64_t, double, boost::uint64_t > Variant;

        Variant v_;

        class Variant_converter_visitor : public boost::static_visitor< Variant >
        {
        public:

              template< typename T, typename A, template< typename, typename > class Cont >
              Variant operator()( const Cont< T, A >& cont ) const
              {
                  return Array( cont.begin(), cont.end() );
              }

              Variant operator()( int i ) const
              {
                  return static_cast< boost::int64_t >( i );
              }

              template<class T>
              Variant operator()( const T& t ) const
              {
                  return t;
              }
        };
    };

    // map objects

#if defined( JSON_SPIRIT_VALUE_ENABLED ) || defined( JSON_SPIRIT_WVALUE_ENABLED )
    template< class String >
    struct Config_map
    {
        typedef String String_type;
        typedef BasicValue< Config_map > ValueType;
        typedef std::vector< ValueType > Array_type;
        typedef std::map< String_type, ValueType > Object_type;
        typedef std::pair< String_type, ValueType > Pair_type;

        static ValueType& add( Object_type& obj, const String_type& name, const ValueType& value )
        {
            return obj[ name ] = value;
        }

        static String_type get_name( const Pair_type& pair )
        {
            return pair.first;
        }

        static ValueType get_value( const Pair_type& pair )
        {
            return pair.second;
        }
    };
#endif

    // typedefs for ASCII

#ifdef JSON_SPIRIT_VALUE_ENABLED
    typedef Config_map< std::string > Config;

    typedef Config::ValueType  Value;
    typedef Config::Object_type Object;
    typedef Config::Array_type  Array;
#endif

    // typedefs for Unicode

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    typedef Config_map< std::wstring > wConfig;

    typedef wConfig::ValueType  wValue;
    typedef wConfig::Object_type wObject;
    typedef wConfig::Array_type  wArray;
#endif

    ///////////////////////////////////////////////////////////////////////////////////////////////
    //
    // implementation

    inline bool operator==( const Null&, const Null& )
    {
        return true;
    }

    template< class Config >
    const BasicValue< Config > BasicValue< Config >::null;

    template< class Config >
    BasicValue< Config >::BasicValue()
    :   v_( Null() )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( const Const_str_ptr value )
    :  v_( String_type( value ) )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( const String_type& value )
    :   v_( value )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( const Object& value )
    :   v_( value )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( const Array& value )
    :   v_( value )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( bool value )
    :   v_( value )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( int value )
    :   v_( static_cast< boost::int64_t >( value ) )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( boost::int64_t value )
    :   v_( value )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( boost::uint64_t value )
    :   v_( value )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( double value )
    :   v_( value )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( const BasicValue< Config >& other )
    :   v_( other.v_ )
    {
    }

    template< class Config >
    template< class Iter >
    BasicValue< Config >::BasicValue( Iter first, Iter last )
    :   v_( Array( first, last ) )
    {
    }

    template< class Config >
    template< BOOST_VARIANT_ENUM_PARAMS( typename T ) >
    BasicValue< Config >::BasicValue( const boost::variant< BOOST_VARIANT_ENUM_PARAMS(T) >& variant )
    :   v_( boost::apply_visitor( Variant_converter_visitor(), variant) )
    {
    }

    template< class Config >
    BasicValue< Config >& BasicValue< Config >::operator=( const BasicValue& lhs )
    {
        BasicValue tmp( lhs );

        std::swap( v_, tmp.v_ );

        return *this;
    }

    template< class Config >
    bool BasicValue< Config >::operator==( const BasicValue& lhs ) const
    {
        if( this == &lhs ) return true;

        if( type() != lhs.type() ) return false;

        return v_ == lhs.v_;
    }

    template< class Config >
    typename BasicValue<Config>::Type BasicValue< Config >::type() const
    {
        if( is_uint64() )
        {
            return INT_TYPE;
        }

        return static_cast< Type >( v_.which() );
    }

    template< class Config >
    bool BasicValue< Config >::is_uint64() const
    {
        return v_.which() == UINT64_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::is_null() const
    {
        return type() == NULL_TYPE;
    }

    template< class Config >
    void BasicValue< Config >::check_type( const Type vtype ) const
    {
        if( type() != vtype )
        {
            std::ostringstream os;

            os << "value type is " << (int)type() << " not " << (int)vtype;

            throw std::runtime_error( os.str() );
        }
    }

    template< class Config >
    const typename Config::String_type& BasicValue< Config >::get_str() const
    {
        check_type( STRING_TYPE );

        return *boost::get< String_type >( &v_ );
    }

    template< class Config >
    const typename BasicValue< Config >::Object& BasicValue< Config >::get_obj() const
    {
        check_type( OBJECT_TYPE );

        return *boost::get< Object >( &v_ );
    }

    template< class Config >
    const typename BasicValue< Config >::Array& BasicValue< Config >::get_array() const
    {
        check_type( ARRAY_TYPE );

        return *boost::get< Array >( &v_ );
    }

    template< class Config >
    bool BasicValue< Config >::get_bool() const
    {
        check_type( BOOL_TYPE );

        return boost::get< bool >( v_ );
    }

    template< class Config >
    int BasicValue< Config >::get_int() const
    {
        check_type( INT_TYPE );

        return static_cast< int >( get_int64() );
    }

    template< class Config >
    boost::int64_t BasicValue< Config >::get_int64() const
    {
        check_type( INT_TYPE );

        if( is_uint64() )
        {
            return static_cast< boost::int64_t >( get_uint64() );
        }

        return boost::get< boost::int64_t >( v_ );
    }

    template< class Config >
    boost::uint64_t BasicValue< Config >::get_uint64() const
    {
        check_type( INT_TYPE );

        if( !is_uint64() )
        {
            return static_cast< boost::uint64_t >( get_int64() );
        }

        return boost::get< boost::uint64_t >( v_ );
    }

    template< class Config >
    double BasicValue< Config >::get_real() const
    {
        if( type() == INT_TYPE )
        {
            return is_uint64() ? static_cast< double >( get_uint64() )
                               : static_cast< double >( get_int64() );
        }

        check_type( REAL_TYPE );

        return boost::get< double >( v_ );
    }

    template< class Config >
    typename BasicValue< Config >::Object& BasicValue< Config >::get_obj()
    {
        check_type( OBJECT_TYPE );

        return *boost::get< Object >( &v_ );
    }

    template< class Config >
    typename BasicValue< Config >::Array& BasicValue< Config >::get_array()
    {
        check_type( ARRAY_TYPE );

        return *boost::get< Array >( &v_ );
    }

    // converts a C string, ie. 8 bit char array, to a string object
    //
    template < class String_type >
    String_type to_str( const char* c_str )
    {
        String_type result;

        for( const char* p = c_str; *p != 0; ++p )
        {
            result += *p;
        }

        return result;
    }


#ifndef IGNORE_FOR_DOCUMENTATION

    namespace internal_
    {
        template< typename T >
        struct Type_to_type
        {
        };

        template< class Value >
        int get_value( const Value& value, Type_to_type< int > )
        {
            return value.get_int();
        }

        template< class Value >
        boost::int64_t get_value( const Value& value, Type_to_type< boost::int64_t > )
        {
            return value.get_int64();
        }

        template< class Value >
        boost::uint64_t get_value( const Value& value, Type_to_type< boost::uint64_t > )
        {
            return value.get_uint64();
        }

        template< class Value >
        double get_value( const Value& value, Type_to_type< double > )
        {
            return value.get_real();
        }

        template< class Value >
        typename Value::String_type get_value( const Value& value, Type_to_type< typename Value::String_type > )
        {
            return value.get_str();
        }

        template< class Value >
        typename Value::Array get_value( const Value& value, Type_to_type< typename Value::Array > )
        {
            return value.get_array();
        }

        template< class Value >
        typename Value::Object get_value( const Value& value, Type_to_type< typename Value::Object > )
        {
            return value.get_obj();
        }

        template< class Value >
        bool get_value( const Value& value, Type_to_type< bool > )
        {
            return value.get_bool();
        }
    }

#endif

    template< class Config >
    template< typename T >
    T BasicValue< Config >::get_value() const
    {
        return internal_::get_value( *this, internal_::Type_to_type< T >() );
    }
}

#endif
