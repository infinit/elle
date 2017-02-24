#pragma once

#include <elle/printf.hh>
#include <elle/das/model.hh>

namespace elle
{
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
            return elle::sprintf("%s = %s",
                                 S::name(),
                                 elle::meta::static_if<S::template attr_has<O>()>
                                 ([] (auto const& o) { return S::attr_get(o); },
                                  [] (auto const& o) { return S::method_call(o); })
                                 (o));
          }
        };
      };
    }

    template <typename T>
    std::enable_if_exists_t<typename elle::das::DefaultModel<T>::type,
                            std::ostream&>
    operator <<(std::ostream& s, T const& o)
    {
      using Fields = typename DefaultModel<T>::type::Fields;
      s << elle::type_info(o) <<
        Fields::template map<stringify_object<T>::template stringify>::value(o);
      return s;
    }
  }
}
