#ifndef  ELLE_FORMAT_JSON_FACTORY_HH
# define ELLE_FORMAT_JSON_FACTORY_HH

# include <boost/call_traits.hpp>
# include <type_traits>

# include "Object.hh"

// Helper to enable Factory::construct() method when passing argument by value.
# define _ELLE_FORMAT_JSON_FACTORY_ENABLE_BY_VALUE(T)                         \
  typename std::enable_if<                                                    \
        !std::is_pointer<T>::value                                            \
    &&  std::is_same<                                                         \
            T const                                                           \
          , typename boost::call_traits<T>::param_type                        \
        >::value                                                              \
    , std::unique_ptr<Object>                                                 \
  >::type                                                                     \
  /**/

// Helper to enable Factory::construct() method when passing argument by ref.
# define _ELLE_FORMAT_JSON_FACTORY_ENABLE_BY_REF(T)                           \
  typename std::enable_if<                                                    \
         std::is_array<T>::value                                              \
    ||  std::is_same<                                                         \
            T const&                                                          \
          , typename boost::call_traits<T>::param_type                        \
        >::value                                                              \
    , std::unique_ptr<Object>                                                 \
  >::type                                                                     \
  /**/

namespace elle
{
  namespace format
  {
    namespace json
    {

      ///
      /// Build json objects from an arbitrary value.
      ///
      struct Factory
      {
      public:
        template <typename T>
        static inline
        _ELLE_FORMAT_JSON_FACTORY_ENABLE_BY_VALUE(T)
        construct(T value);

        template <typename T>
        static inline
        _ELLE_FORMAT_JSON_FACTORY_ENABLE_BY_REF(T)
        construct(T const& value);

      protected:
        struct Impl;
      };

    }
  }
}

# include "Factory.hxx"
#endif

