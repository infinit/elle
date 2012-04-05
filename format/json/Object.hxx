#ifndef ELLE_FORMAT_JSON_OBJECT_HXX
# define ELLE_FORMAT_JSON_OBJECT_HXX

# include <typeinfo>

# include "Object.hh"
# include "Float.hh"
# include "String.hh"
# include "Integer.hh"
# include "Bool.hh"

namespace elle { namespace format { namespace json {

    ///
    /// This files might be included when using the Load mecanism of an object
    ///

    namespace detail {

        /// Helper to select the right cast according to the out type
        struct ObjectLoader
          {
          private:
            /// Method enabler
            template<typename T, typename Type = void> struct Select
              {
                typedef typename std::enable_if<
                    std::is_same<T, std::string>::value
                  , Type
                > String;

                typedef typename std::enable_if<
                    std::is_floating_point<T>::value
                  , Type
                > Float;

                typedef typename std::enable_if<
                    std::is_integral<T>::value
                  , Type
                > Integer;

                typedef typename std::enable_if<
                    std::is_same<T, bool>::value
                  , Type
                > Bool;
              };

          public:
            /// Simple objects have a cast operator
#define LOAD_SIMPLE_OBJECT_FOR(JSONType)                                      \
            template<typename T> static inline typename                       \
              Select<T>::JSONType::type Load(Object const& o, T& out)         \
              {                                                               \
                out = static_cast<typename JSONType::CastType>(               \
                    dynamic_cast<JSONType const&>(o)                          \
                );                                                            \
              }                                                               \
            template<typename T> static inline typename                       \
              Select<T, bool>::JSONType::type TryLoad(Object const& o, T& out)\
              {                                                               \
                if (auto ptr = dynamic_cast<JSONType const*>(&o))             \
                  {                                                           \
                    out = static_cast<typename JSONType::CastType>(*ptr);     \
                    return true;                                              \
                  }                                                           \
                return false;                                                 \
              }                                                               \

            /// Generate Load() method for simple objects
            LOAD_SIMPLE_OBJECT_FOR(String)
            LOAD_SIMPLE_OBJECT_FOR(Float)
            LOAD_SIMPLE_OBJECT_FOR(Integer)
            LOAD_SIMPLE_OBJECT_FOR(Bool)
#undef LOAD_SIMPLE_OBJECT_FOR

//            template<typename T> static inline
//              void Load(T& out)
//                {
//#ifdef INFINIT_DEBUG
//                  if (dynamic_cast<json::Dictionary const*>(this) == nullptr)
//                    throw std::bad_cast(
//                        "Asked type '" + typeid(T).name() + "' is a class, but json element '" +
//                        this->repr() +
//                        "' is not a dictionary"
//                    );
//#endif
//                }

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
        detail::ObjectLoader::Load(*this, out);
      }

    template<typename T> bool Object::TryLoad(T& out) const
      {
        return detail::ObjectLoader::TryLoad(*this, out);
      }

}}} // !namespace elle::format::json

#endif

