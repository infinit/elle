#pragma once

#include <ostream>
#include <type_traits>
#include <utility>

#include <boost/functional/hash.hpp>

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
      bool __attribute__((unused)) first = true;
      out << "(";
      int __attribute__((unused)) ignore[] = {
        (_print_tuple<typename Args::Symbol>(t, out, first), 0)...
      };
      out << ")";
      return out;
    }

    namespace
    {
      // This should be a lambda in hash_value, but GCC 4.8 says no:
      // "sorry, unimplemented: mangling argument_pack_select"
      template <typename S, typename T>
      void
      _hash_tuple(T const& t, std::size_t& seed)
      {
        using boost::hash_value;
        boost::hash_combine(seed, hash_value(S::attr_get(t)));
      };
    }

    template <typename ... Args>
    std::size_t
    hash_value(tuple<Args...> const& t)
    {
      std::size_t seed = 0;
      int ignore[] = {
        (_hash_tuple<typename Args::Symbol>(t, seed), 0)...
      };
      (void) ignore;
      return seed;
    }

    namespace
    {
      // This should be a lambda in operator ==, but GCC 4.8 says no:
      // "sorry, unimplemented: mangling argument_pack_select"
      template <typename S, typename T>
      inline
      void
      _compare_tuple(T const& lhs, T const& rhs, bool& res)
      {
        if (res)
          res = S::attr_get(lhs) == S::attr_get(rhs);
      };
    }

    template <typename ... Args>
    bool
    operator ==(tuple<Args...> const& lhs, tuple<Args...> const& rhs)
    {
      bool res = true;
      int __attribute__((unused)) ignore[] = {
        (_compare_tuple<typename Args::Symbol>(lhs, rhs, res), 0)...
      };
      return res;
    }

    template <typename ... Args>
    bool
    operator !=(tuple<Args...> const& lhs, tuple<Args...> const& rhs)
    {
      return !(lhs == rhs);
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
