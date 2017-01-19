#pragma once

#include <elle/printf.hh>
#include <das/model.hh>

namespace das
{
  namespace
  {
    template <typename O>
    struct stringify_object
    {
      template <typename S>
      struct stringify
      {
        using type = std::string;
        static
        type
        value(O const& o)
        {
          return elle::meta::static_if<S::template attr_has<O>()>(
            [] (auto const& o)
            {
              return elle::sprintf("%s = %s", S::name(), S::attr_get(o));
            },
            [] (auto const& o)
            {
              return elle::sprintf("%s = %s", S::name(), S::method_call(o));
            })(o);
        }
      };
    };
  }

  template <typename T>
  std::enable_if_exists_t<typename das::DefaultModel<T>::type, std::ostream&>
  operator <<(std::ostream& s, T const& o)
  {
    using Fields = typename DefaultModel<T>::type::Fields;
    s << elle::type_info(o) <<
      Fields::template map<stringify_object<T>::template stringify>::value(o);
    return s;
  }
}
