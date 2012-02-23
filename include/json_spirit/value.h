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

#include "path_error.h"

// comment out the value types you don't need to reduce build times and intermediate file sizes
#define JSON_SPIRIT_VALUE_ENABLED
#define JSON_SPIRIT_WVALUE_ENABLED

namespace json_spirit {

    struct Null{};

/** BasicValue represents a JSON value. It can contain all the normal
 *  JSON datatypes: null, numbers, bools, strings, arrays, and other
 *  objects. Objects and arrays are filled with other BasicValues.
 */
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
        typedef typename Config::String_type String;
        typedef typename Config::Object_type Object;
        typedef typename Config::Array_type Array;
        typedef typename String::const_pointer Const_str_ptr;  // eg const char*

        typedef BasicPathError<String> PathError;

        BasicValue();  // creates null value
        BasicValue( Const_str_ptr      value );
        BasicValue( const String& value );
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

        bool isNull() const;
        bool isObject() const;
        bool isString() const;
        bool isArray() const;
        bool isBool() const;
        bool isInt() const;
        bool isInt64() const;
        bool isUInt64() const;
        bool isReal() const;

        const String&      getString()    const;
        String& getString();
        const Object&      getObject()    const;
        Object& getObject();
        const Array&       getArray()  const;
        Array&  getArray();
        bool               getBool()   const;
        int                getInt()    const;
        boost::int64_t     getInt64()  const;
        boost::uint64_t    getUInt64() const;
        double             getReal()   const;

        template< typename T > T getValue() const;  // example usage: int    i = value.getValue< int >();
                                                     // or             double d = value.getValue< double >();

        // Object-specific utility methods based on paths such as
        // 'foo.bar.baz' rather than having to traverse manually.

        /** Check if the given path exists within this object.
         *
         *  \param path the path to the value to access
         *  \return true if the path exists, false otherwise
         *  \throw PathError for path related errors that aren't due to missing
         *         objects, e.g. improperly formatted paths.
         */
        bool contains(const String& path, const typename String::value_type delim = '.') const;
        /** Read a value from a path within this object.
         *
         *  \param path the path to the value to access
         *  \return a reference to the requested value
         *  \throw PathError if this value is not an object or one of the
         *         objects along the path does not exist or is not an object
         */
        BasicValue& get(const String& path, const typename String::value_type delim = '.');
        /** Read a value from a path within this object.
         *
         *  \param path the path to the value to access
         *  \return a reference to the requested value
         *  \throw PathError if this value is not an object or one of the
         *         objects along the path does not exist or is not an object
         */
        const BasicValue& get(const String& path, const typename String::value_type delim = '.') const;

        // Utilities that perform a get by path, then extract the
        // actual value.
        const String& getString(const String& path, const typename String::value_type delim = '.') const;
        String& getString(const String& path, const typename String::value_type delim = '.');
        const Object& getObject(const String& path, const typename String::value_type delim = '.') const;
        Object& getObject(const String& path, const typename String::value_type delim = '.');
        const Array& getArray(const String& path, const typename String::value_type delim = '.') const;
        Array& getArray(const String& path, const typename String::value_type delim = '.');
        bool getBool(const String& path, const typename String::value_type delim = '.') const;
        int getInt(const String& path, const typename String::value_type delim = '.') const;
        boost::int64_t getInt64(const String& path, const typename String::value_type delim = '.') const;
        boost::uint64_t getUInt64(const String& path, const typename String::value_type delim = '.') const;
        double getReal(const String& path, const typename String::value_type delim = '.') const;

        // Accessors with alternative defaults. Note that these do *not* throw
        // an exception if they find the path and it is the wrong type. The
        // return the default value instead.
        const String& getString(const String& path, const String& default_, const typename String::value_type delim = '.') const;
        const Object& getObject(const String& path, const Object& default_, const typename String::value_type delim = '.') const;
        const Array& getArray(const String& path, const Array& default_, const typename String::value_type delim = '.') const;
        bool getBool(const String& path, bool default_, const typename String::value_type delim = '.') const;
        int getInt(const String& path, int default_, const typename String::value_type delim = '.') const;
        boost::int64_t getInt64(const String& path, boost::int64_t default_, const typename String::value_type delim = '.') const;
        boost::uint64_t getUInt64(const String& path, boost::uint64_t default_, const typename String::value_type delim = '.') const;
        double getReal(const String& path, double default_, const typename String::value_type delim = '.') const;

        /** Insert the value into this object at the given path. Unlike
         *  manipulating objects directly, this uses a 'path' to the
         *  destination, creating objects along the way if they don't
         *  exist. For example insert('foo.bar', 7) would ensure the
         *  current object looks like this:
         *  {
         *     'foo' : {
         *         'bar' : 7
         *     }
         *  }
         *  as long as a value didn't already exist at the position.
         *
         *  \param path the path, through objects, to the destination
         *         location of the value.
         *  \param val the value to insert
         *  \param delim the delimiter to use in parsing the path into
         *         components. Defaults to '.'
         *  \returns true if the insertion was successful, false if
         *         there was already a value at the given path
         *
         *  \note If you want to overwrite a value if it already
         *  exists, use put().
         */
        bool insert(const String& path, const BasicValue& val, const typename String::value_type delim = '.');
        /** Insert the value into this object at the given path. Unlike
         *  manipulating objects directly, this uses a 'path' to the
         *  destination, creating objects along the way if they don't
         *  exist. For example put('foo.bar', 7) would ensure the
         *  current object looks like this:
         *  {
         *     'foo' : {
         *         'bar' : 7
         *     }
         *  }
         *
         *  \param path the path, through objects, to the destination
         *         location of the value.
         *  \param val the value to insert
         *  \param delim the delimiter to use in parsing the path into
         *         components. Defaults to '.'
         *  \returns true if the insertion was successful, false if
         *         there was already a value at the given path
         *
         *  \note If you want to overwrite a value if it already
         *  exists, use put().
         */
        void put(const String& path, const BasicValue& val, const typename String::value_type delim = '.');

        static const BasicValue null;

    private:

#ifndef IGNORE_FOR_DOCUMENTATION
        // Helper for path functions which provides an iterator interface for
        // splitting a path into parts and traversing them
        struct PathIterator {
            // The path passed in must remain valid for the entire use of the
            // PathIterator, it isn't copied
            PathIterator(const String& path_, const typename String::value_type delim_)
             : path(path_), delim(delim_), prev_delim_pos(-1), delim_pos(0)
            {
                // prev_delim_pos = -1 so when we search at prev_delim_pos+1 for
                // the next delimiter, it'll do the right thing by search from
                // the start of the string. Note that we need to be careful with
                // this since this could mean prev_delim_pos == npos.

                // delim_pos just needs to be non-npos here so finished()
                // returns false, even if this is an empty string. The value
                // doesn't matter since as soon as we try to look at the next
                // value we'll just overwrite it.
            }

            // Get the next part of the path. The return value is the subpath
            // and, if we've reached the end of the path, subsequent calls to
            // finished() will return true.
            String next() {
                assert(!finished());

                delim_pos = path.find(delim, prev_delim_pos+1);
                // We should never have delim_pos == npos because of the outer
                // check for stop_pos
                String subpath;
                if (delim_pos == String::npos)
                    subpath = path.substr(prev_delim_pos+1);
                else
                    subpath = path.substr(prev_delim_pos+1, delim_pos-prev_delim_pos-1);

                if (subpath.empty())
                    throw PathError(path, subpath, "Found empty subpath");

                prev_delim_pos = delim_pos;
                return subpath;
            }

            bool finished() const {
                return (prev_delim_pos == String::npos && delim_pos == String::npos);
            }

            const String& path;
            const typename String::value_type delim;
            std::size_t prev_delim_pos;
            std::size_t delim_pos;
        };
#endif

        bool insert_put_impl(const String& path, const BasicValue& val, const typename String::value_type delim, bool force_val);

        void check_type( const Type vtype ) const;

        typedef boost::variant< Null, boost::recursive_wrapper< Object >, boost::recursive_wrapper< Array >,
                                String, bool, boost::int64_t, double, boost::uint64_t > Variant;

        Variant v_;

#ifndef IGNORE_FOR_DOCUMENTATION
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
#endif

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

    typedef Value::PathError PathError;
#endif

    // typedefs for Unicode

#if defined( JSON_SPIRIT_WVALUE_ENABLED ) && !defined( BOOST_NO_STD_WSTRING )
    typedef Config_map< std::wstring > wConfig;

    typedef wConfig::ValueType  wValue;
    typedef wConfig::Object_type wObject;
    typedef wConfig::Array_type  wArray;

    typedef wValue::PathError wPathError;
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
    :  v_( String( value ) )
    {
    }

    template< class Config >
    BasicValue< Config >::BasicValue( const String& value )
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
        if( isUInt64() )
        {
            return INT_TYPE;
        }

        return static_cast< Type >( v_.which() );
    }

    template< class Config >
    bool BasicValue< Config >::isNull() const
    {
        return type() == NULL_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::isObject() const
    {
        return v_.which() == OBJECT_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::isString() const
    {
        return v_.which() == STRING_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::isArray() const
    {
        return v_.which() == ARRAY_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::isBool() const
    {
        return v_.which() == BOOL_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::isInt() const
    {
        return v_.which() == INT_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::isInt64() const
    {
        // We treat int64 and uint64 the same
        return v_.which() == UINT64_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::isUInt64() const
    {
        return v_.which() == UINT64_TYPE;
    }

    template< class Config >
    bool BasicValue< Config >::isReal() const
    {
        return v_.which() == REAL_TYPE;
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
    const typename Config::String_type& BasicValue< Config >::getString() const
    {
        check_type( STRING_TYPE );

        return *boost::get< String >( &v_ );
    }

    template< class Config >
    typename BasicValue< Config >::String& BasicValue< Config >::getString()
    {
        check_type( STRING_TYPE );

        return *boost::get< String >( &v_ );
    }

    template< class Config >
    const typename BasicValue< Config >::Object& BasicValue< Config >::getObject() const
    {
        check_type( OBJECT_TYPE );

        return *boost::get< Object >( &v_ );
    }

    template< class Config >
    typename BasicValue< Config >::Object& BasicValue< Config >::getObject()
    {
        check_type( OBJECT_TYPE );

        return *boost::get< Object >( &v_ );
    }

    template< class Config >
    const typename BasicValue< Config >::Array& BasicValue< Config >::getArray() const
    {
        check_type( ARRAY_TYPE );

        return *boost::get< Array >( &v_ );
    }

    template< class Config >
    typename BasicValue< Config >::Array& BasicValue< Config >::getArray()
    {
        check_type( ARRAY_TYPE );

        return *boost::get< Array >( &v_ );
    }

    template< class Config >
    bool BasicValue< Config >::getBool() const
    {
        check_type( BOOL_TYPE );

        return boost::get< bool >( v_ );
    }

    template< class Config >
    int BasicValue< Config >::getInt() const
    {
        check_type( INT_TYPE );

        return static_cast< int >( getInt64() );
    }

    template< class Config >
    boost::int64_t BasicValue< Config >::getInt64() const
    {
        check_type( INT_TYPE );

        if( isUInt64() )
        {
            return static_cast< boost::int64_t >( getUInt64() );
        }

        return boost::get< boost::int64_t >( v_ );
    }

    template< class Config >
    boost::uint64_t BasicValue< Config >::getUInt64() const
    {
        check_type( INT_TYPE );

        if( !isUInt64() )
        {
            return static_cast< boost::uint64_t >( getInt64() );
        }

        return boost::get< boost::uint64_t >( v_ );
    }

    template< class Config >
    double BasicValue< Config >::getReal() const
    {
        if( type() == INT_TYPE )
        {
            return isUInt64() ? static_cast< double >( getUInt64() )
                               : static_cast< double >( getInt64() );
        }

        check_type( REAL_TYPE );

        return boost::get< double >( v_ );
    }

template<class Config>
bool BasicValue<Config>::contains(const String& path, const typename String::value_type delim) const {
    if (!isObject())
        throw PathError(path, "<root>", "Value isn't an object.");

    const Object* obj = &(getObject());

    PathIterator path_it(path, delim);
    do {
        String subpath = path_it.next();

        // We need to find the element in the object either way
        typename Object::const_iterator sub_obj_it = obj->find(subpath);
        if (sub_obj_it == obj->end())
            return false;

        // Then we either extract the next object, or the result value
        if (!path_it.finished())
            obj = &(sub_obj_it->second.getObject());
    } while(!path_it.finished());

    return true;
}

    template< class Config >
    BasicValue<Config>& BasicValue< Config >::get(const String& path, const typename String::value_type delim) {
        if (!isObject())
            throw PathError(path, "<root>", "Value isn't an object.");

        Object* obj = &(getObject());

        PathIterator path_it(path, delim);
        do {
            String subpath = path_it.next();

            // We need to find the element in the object either way
            typename Object::iterator sub_obj_it = obj->find(subpath);
            if (sub_obj_it == obj->end())
                throw PathError(path, subpath, "Path component not found");

            // Then we either extract the next object, or the result value
            if (!path_it.finished())
                obj = &(sub_obj_it->second.getObject());
            else
                return sub_obj_it->second;
        } while(!path_it.finished());

        // Shouldn't get here, just have the return to make the compiler happy.
        throw PathError(path, "", "Shouldn't have reached this point.");
    }

    template< class Config >
    const BasicValue<Config>& BasicValue< Config >::get(const String& path, const typename String::value_type delim) const {
        if (!isObject())
            throw PathError(path, "<root>", "Value isn't an object.");

        const Object* obj = &(getObject());

        PathIterator path_it(path, delim);
        do {
            String subpath = path_it.next();

            // We need to find the element in the object either way
            typename Object::const_iterator sub_obj_it = obj->find(subpath);
            if (sub_obj_it == obj->end())
                throw PathError(path, subpath, "Path component not found");

            // Then we either extract the next object, or the result value
            if (!path_it.finished())
                obj = &(sub_obj_it->second.getObject());
            else
                return sub_obj_it->second;
        } while(!path_it.finished());

        // Shouldn't get here, just have the return to make the compiler happy.
        throw PathError(path, "", "Shouldn't have reached this point.");
    }


// Shortcut accessors

template<class Config>
const typename BasicValue<Config>::String& BasicValue<Config>::getString(const String& path, const typename String::value_type delim) const {
    return get(path, delim).getString();
}
template<class Config>
typename BasicValue<Config>::String& BasicValue<Config>::getString(const String& path, const typename String::value_type delim) {
    return get(path, delim).getString();
}

template<class Config>
const typename BasicValue<Config>::Object& BasicValue<Config>::getObject(const String& path, const typename String::value_type delim) const {
    return get(path, delim).getObject();
}
template<class Config>
typename BasicValue<Config>::Object& BasicValue<Config>::getObject(const String& path, const typename String::value_type delim) {
    return get(path, delim).getObject();
}

template<class Config>
const typename BasicValue<Config>::Array& BasicValue<Config>::getArray(const String& path, const typename String::value_type delim) const {
    return get(path, delim).getArray();
}
template<class Config>
typename BasicValue<Config>::Array& BasicValue<Config>::getArray(const String& path, const typename String::value_type delim) {
    return get(path, delim).getArray();
}

template<class Config>
bool BasicValue<Config>::getBool(const String& path, const typename String::value_type delim) const {
    return get(path, delim).getBool();
}

template<class Config>
int BasicValue<Config>::getInt(const String& path, const typename String::value_type delim) const {
    return get(path, delim).getInt();
}

template<class Config>
boost::int64_t BasicValue<Config>::getInt64(const String& path, const typename String::value_type delim) const {
    return get(path, delim).getInt64();
}

template<class Config>
boost::uint64_t BasicValue<Config>::getUInt64(const String& path, const typename String::value_type delim) const {
    return get(path, delim).getUInt64();
}

template<class Config>
double BasicValue<Config>::getReal(const String& path, const typename String::value_type delim) const {
    return get(path, delim).getReal();
}


// Shortcut accessors with defaults
template<class Config>
const typename BasicValue<Config>::String& BasicValue<Config>::getString(const String& path, const String& default_, const typename String::value_type delim) const {
    if (!contains(path, delim)) return default_;
    const BasicValue& val = get(path, delim);
    if (val.type() != STRING_TYPE) return default_;
    return val.getString();
}

template<class Config>
const typename BasicValue<Config>::Object& BasicValue<Config>::getObject(const String& path, const Object& default_, const typename String::value_type delim) const {
    if (!contains(path, delim)) return default_;
    const BasicValue& val = get(path, delim);
    if (val.type() != OBJECT_TYPE) return default_;
    return val.getObject();
}

template<class Config>
const typename BasicValue<Config>::Array& BasicValue<Config>::getArray(const String& path, const Array& default_, const typename String::value_type delim) const {
    if (!contains(path, delim)) return default_;
    const BasicValue& val = get(path, delim);
    if (val.type() != ARRAY_TYPE) return default_;
    return val.getArray();
}

template<class Config>
bool BasicValue<Config>::getBool(const String& path, bool default_, const typename String::value_type delim) const {
    if (!contains(path, delim)) return default_;
    const BasicValue& val = get(path, delim);
    if (val.type() != BOOL_TYPE) return default_;
    return val.getBool();
}

template<class Config>
int BasicValue<Config>::getInt(const String& path, int default_, const typename String::value_type delim) const {
    if (!contains(path, delim)) return default_;
    const BasicValue& val = get(path, delim);
    if (val.type() != INT_TYPE) return default_;
    return val.getInt();
}

template<class Config>
boost::int64_t BasicValue<Config>::getInt64(const String& path, boost::int64_t default_, const typename String::value_type delim) const {
    if (!contains(path, delim)) return default_;
    const BasicValue& val = get(path, delim);
    if (val.type() != INT_TYPE) return default_;
    return val.getInt64();
}

template<class Config>
boost::uint64_t BasicValue<Config>::getUInt64(const String& path, boost::uint64_t default_, const typename String::value_type delim) const {
    if (!contains(path, delim)) return default_;
    const BasicValue& val = get(path, delim);
    if (val.type() != INT_TYPE) return default_;
    return val.getUInt64();
}

template<class Config>
double BasicValue<Config>::getReal(const String& path, double default_, const typename String::value_type delim) const {
    if (!contains(path, delim)) return default_;
    const BasicValue& val = get(path, delim);
    if (val.type() != REAL_TYPE) return default_;
    return val.getReal();
}



    template< class Config >
    bool BasicValue< Config >::insert_put_impl(const String& path, const BasicValue& val, const typename String::value_type delim, bool force_val) {
        if (!isObject())
            throw PathError(path, "<root>", "Value isn't an object.");

        Object* obj = &(getObject());

        PathIterator path_it(path, delim);
        do {
            String subpath = path_it.next();
            if (!path_it.finished()) {
                // Not the last component, get the next subobject

                // Insert a new object. If an object was already
                // there, we just get back the existing object to work
                // with.

                typename Object::iterator sub_obj_it = (obj->insert( std::make_pair(subpath, BasicValue(Object())) )).first;
                if (!sub_obj_it->second.isObject())
                    throw PathError(path, subpath, "Path component isn't an object.");
                obj = &(sub_obj_it->second.getObject());
            }
            else {
                // Finally, insert the value
                if (force_val)
                    obj->erase(subpath);
                return obj->insert( std::make_pair(subpath, val) ).second;
            }
        } while(!path_it.finished());

        // Shouldn't get here, just have the return to make the compiler happy.
        return false;
    }

    template< class Config >
    bool BasicValue< Config >::insert(const String& path, const BasicValue& val, const typename String::value_type delim) {
        return insert_put_impl(path, val, delim, false);
    }
    template< class Config >
    void BasicValue< Config >::put(const String& path, const BasicValue& val, const typename String::value_type delim) {
        insert_put_impl(path, val, delim, true);
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
            return value.getInt();
        }

        template< class Value >
        boost::int64_t get_value( const Value& value, Type_to_type< boost::int64_t > )
        {
            return value.getInt64();
        }

        template< class Value >
        boost::uint64_t get_value( const Value& value, Type_to_type< boost::uint64_t > )
        {
            return value.getUInt64();
        }

        template< class Value >
        double get_value( const Value& value, Type_to_type< double > )
        {
            return value.getReal();
        }

        template< class Value >
        typename Value::String get_value( const Value& value, Type_to_type< typename Value::String > )
        {
            return value.getString();
        }

        template< class Value >
        typename Value::Array get_value( const Value& value, Type_to_type< typename Value::Array > )
        {
            return value.getArray();
        }

        template< class Value >
        typename Value::Object get_value( const Value& value, Type_to_type< typename Value::Object > )
        {
            return value.getObject();
        }

        template< class Value >
        bool get_value( const Value& value, Type_to_type< bool > )
        {
            return value.getBool();
        }
    }

#endif

    template< class Config >
    template< typename T >
    T BasicValue< Config >::getValue() const
    {
        return internal_::get_value( *this, internal_::Type_to_type< T >() );
    }
}

#endif
