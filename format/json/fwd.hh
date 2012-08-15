#ifndef ELLE_FORMAT_JSON_FWD_HH
# define ELLE_FORMAT_JSON_FWD_HH

# include "_internal.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {
      namespace detail
      {
        template <typename T>
        class BasicObject;
      }

      typedef detail::BasicObject<internal::Integer>      Integer;
      typedef detail::BasicObject<internal::Bool>         Bool;
      typedef detail::BasicObject<internal::Float>        Float;
      typedef detail::BasicObject<internal::String>       String;

      class Object;
      class Array;
      class Dictionary;
      class Null;

      template <typename StringType>
      class Parser;

    }
  }
}

#endif
