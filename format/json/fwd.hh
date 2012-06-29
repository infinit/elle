#ifndef ELLE_FORMAT_JSON_FWD_HH
# define ELLE_FORMAT_JSON_FWD_HH

# include <string>

namespace elle
{
  namespace format
  {
    namespace json
    {
      namespace detail
      {
        template<typename T>
        class BasicObject;
      }

      typedef detail::BasicObject<int32_t> Integer;
      typedef detail::BasicObject<bool> Bool;
      typedef detail::BasicObject<double> Float;
      typedef detail::BasicObject<std::string> String;

      class Object;
      class Array;
      class Dictionary;
      class Null;
      template<typename StringType>
      class Parser;

    }
  }
}

#endif
