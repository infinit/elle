#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include <elle/das/model.hh>

namespace elle
{
  namespace das
  {
    // C++17.
    template <typename... Ts>
    struct void_t_impl
    {
      using type = void;
    };
    template <typename... Ts>
    using void_t = typename void_t_impl<Ts...>::type;


    template <typename ... Formal>
    class tuple
      : public Formal::Symbol::template Field<std::decay_t<std::remove_reference_t<typename Formal::Type>>>...
    {
    public:
      template <typename ... Args,
                typename = void_t<std::enable_if_t<std::is_convertible<Args, typename Formal::Type>::value, int>...>>
      tuple(Args&& ... args)
        : Formal::Symbol::template Field<std::decay_t<std::remove_reference_t<typename Formal::Type>>>{
            std::forward<Args>(args)}...
      {}

      using Model = das::Model<tuple, meta::List<typename Formal::Symbol...>>;
    };

    namespace
    {
      // This should be a lambda in operator <<, but GCC 4.8 says no:
      // "sorry, unimplemented: mangling argument_pack_select"
      template <typename S, typename T>
      inline
      void
      _print_tuple(T const& t, std::ostream& out, bool& first)
      {
        if (first)
          first = false;
        else
          out << ", ";
        elle::fprintf(out, "%s = %s", S::name(), S::attr_get(t));
      };
    }

    template <typename ... Args>
    std::ostream&
    operator <<(std::ostream& out, tuple<Args...> const& t)
    {
      bool first = true;
      out << "(";
      auto ignore = {
        (_print_tuple<typename Args::Symbol>(t, out, first), 0)...
      };
      (void) ignore;
      out << ")";
      return out;
    }

    namespace
    {
      template <typename T>
      T
      get_formal_impl(...);

      template <typename T>
      typename T::Formal::template Formal<typename T::Passing>
      get_formal_impl(int);

      template <typename T>
      using get_formal = decltype(get_formal_impl<T>(0));
    }

    template <typename ... Formals>
    auto
    make_tuple(Formals&& ... formals)
    {
      return tuple<get_formal<std::remove_cv_reference_t<Formals>>...>(
        std::forward<Formals>(formals).value...);
    }
  }
}
