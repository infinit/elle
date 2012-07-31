#ifndef  ELLE_SERIALIZE_NAMEDVALUE_HXX
# define ELLE_SERIALIZE_NAMEDVALUE_HXX

namespace elle
{
  namespace serialize
  {

    template<typename T>
    struct NamedValue
    {
      std::string name;
      T& value;

      NamedValue(std::string const& name, T& value) :
        name(name), value(value)
      {}

      NamedValue(NamedValue&& o) :
        name(std::move(o.name)),
        value(o.value)
      {}
    };


    namespace detail
    {
      // assume clean type
      template<typename T> struct IsNamedValue
      { static bool const value = false; };

      template<typename T> struct IsNamedValue<NamedValue<T>>
      { static bool const value = true;  };

    }

    template<typename T> struct IsNamedValue
    {
      static bool const value = detail::IsNamedValue<
          typename std::remove_cv<
            typename std::remove_reference<T>::type
          >::type
      >::value;
    };

    template<typename T> inline
    NamedValue<T>
    named(std::string const& name,
          T& value)
    {
      return NamedValue<T>(name, value);
    }

    template<typename T> inline
    NamedValue<T const>
    named(std::string const& name,
          T const& value)
    {
      return NamedValue<T const>(name, value);
    }

  }
}

#endif

