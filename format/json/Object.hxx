#ifndef ELLE_FORMAT_JSON_OBJECT_HXX
# define ELLE_FORMAT_JSON_OBJECT_HXX

# include <typeinfo>

# include "Object.hh"
# include "Float.hh"
# include "String.hh"
# include "Integer.hh"
# include "Bool.hh"
# include "Null.hh"

namespace elle { namespace format { namespace json {

    ///
    /// This files might be included when using the Load mecanism of an object
    ///

    namespace detail {

        template<typename T>
        struct SelectJSONType
          {
            typedef typename StaticIf<
              IsString<T>::value
            , String
            , typename StaticIf<
                std::is_same<T, bool>::value
              , Bool
              , typename StaticIf<
                  std::is_floating_point<T>::value
                , Float
                , typename StaticIf<
                    std::is_integral<T>::value
                  , Integer
                  , typename StaticIf<
                      IsArray<T>::value
                    , Array
                    , typename StaticIf<
                        IsStringMap<T>::value
                      , Dictionary
                      , void
                      >::type
                    >::type
                  >::type
                >::type
              >::type
            >::type type;
          };

    } // !namespace detail

    template<typename T> struct Object::CanLoad
      {
        static bool const value = (
              std::is_arithmetic<T>::value
          ||  std::is_same<T, std::string>::value
          ||  std::is_same<T, bool>::value
        );
      };


    template<typename T> void Object::Load(T& out) const
      {
        static_assert(
            !std::is_base_of<T, Object>::value,
            "Cannot load into a json object"
        );
        typedef typename detail::SelectJSONType<T>::type SelfType;
        out = dynamic_cast<SelfType const&>(*this);
      }

    template<typename T> bool Object::TryLoad(T& out) const
      {
        typedef typename detail::SelectJSONType<T>::type SelfType;
        if (auto ptr = dynamic_cast<SelfType const*>(this))
          {
            out = *ptr;
            return true;
          }
        return false;
      }


      template<typename T> typename std::enable_if<
            !std::is_base_of<T, Object>::value
          , bool
      >::type Object::operator ==(T const& other) const
        {
          typedef typename detail::SelectJSONType<T>::type SelfType;
          if (auto self = dynamic_cast<SelfType const*>(this))
            return *self == other;
          return false;
        }

    struct Object::Factory
      {
        template<typename T> static inline typename std::enable_if<
            std::is_same<T, Null>::value
          , std::unique_ptr<Null>
        >::type Construct(T const&)
          { return std::unique_ptr<Null>(new Null); }

        template<typename T> static inline typename std::enable_if<
            std::is_same<T , bool>::value
          , std::unique_ptr<Bool>
        >::type Construct(T value)
          { return std::unique_ptr<Bool>(new Bool(value)); }

        template<typename T> static inline typename std::enable_if<
                std::is_integral<T>::value
            &&  !std::is_same<T , bool>::value
          , std::unique_ptr<Integer>
        >::type Construct(T value)
          { return std::unique_ptr<Integer>(new Integer(value)); }

        template<typename T> static inline typename std::enable_if<
            std::is_floating_point<T>::value
          , std::unique_ptr<Float>
        >::type Construct(T value)
          { return std::unique_ptr<Float>(new Float(value)); }

        template<typename T> static inline typename std::enable_if<
            detail::IsString<T>::value
          , std::unique_ptr<String>
        >::type Construct(T const& value)
          { return std::unique_ptr<String>(new String(value)); }

        template<typename T> static inline typename std::enable_if<
            detail::IsArray<T>::value
          , std::unique_ptr<Array>
        >::type Construct(T const& value)
          { return std::unique_ptr<Array>(new Array(value)); }

        template<typename T> static inline typename std::enable_if<
            detail::IsStringMap<T>::value
          , std::unique_ptr<Dictionary>
        >::type Construct(T const& value)
          { return std::unique_ptr<Dictionary>(new Dictionary(value)); }

        template<typename T> static inline typename std::enable_if<
              std::is_base_of<Object, T>::value
          &&  !std::is_same<T, Null>::value
          , std::unique_ptr<T>
        >::type Construct(T const& value)
          {
            Object* ptr = value.Clone().release();
            assert(dynamic_cast<T*>(ptr) != nullptr);
            return std::unique_ptr<T>(static_cast<T*>(ptr));
          }

        template<typename T> static inline typename std::enable_if<
              std::is_pointer<T>::value
          &&  !std::is_array<T>::value
        >::type Construct(T const& value)
          { static_assert(!std::is_pointer<T>::value, "You cannot build JSON Object from a pointer"); }
      };

}}} // !namespace elle::format::json

#endif

