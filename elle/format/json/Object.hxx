#ifndef ELLE_FORMAT_JSON_OBJECT_HXX
# define ELLE_FORMAT_JSON_OBJECT_HXX

# include "Object.hh"
# include "Float.hh"
# include "String.hh"
# include "Integer.hh"

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
              };

          public:
            /// Simple objects have a cast operator
#define LOAD_SIMPLE_OBJECT_FOR(Type)                                          \
            template<typename T> static inline typename                       \
              Select<T>::Type::type Load(Object const& o, T& out)             \
              {                                                               \
                out = dynamic_cast<Type const&>(o);                           \
              }                                                               \
            template<typename T> static inline typename                       \
              Select<T, bool>::Type::type TryLoad(Object const& o, T& out)    \
              {                                                               \
                if (auto ptr = dynamic_cast<Type const*>(&o))                 \
                  {                                                           \
                    out = *ptr;                                               \
                    return true;                                              \
                  }                                                           \
                return false;                                                 \
              }                                                               \

            /// Generate Load() method for simple objects
            LOAD_SIMPLE_OBJECT_FOR(String)
            LOAD_SIMPLE_OBJECT_FOR(Float)
            LOAD_SIMPLE_OBJECT_FOR(Integer)
#undef LOAD_SIMPLE_OBJECT_FOR

          };

    } // !namespace detail

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

