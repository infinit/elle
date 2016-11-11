#ifndef DAS_PRINTER_HH
# define DAS_PRINTER_HH

# include <elle/printf.hh>
# include <das/model.hh>

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
          return elle::sprintf("%s = %s", S::name(), S::attr_get(o));
         }
      };
    };
  }

  template <typename T>
  typename std::enable_if_exists<
    typename das::DefaultModel<T>::type, std::ostream&>::type
  operator <<(std::ostream& s, T const& o)
  {
    using Fields = typename DefaultModel<T>::type::Fields;
    s << elle::type_info(o) <<
      Fields::template map<stringify_object<T>::template stringify>::value(o);
    return s;
  }
}

#endif
