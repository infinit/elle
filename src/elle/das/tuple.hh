#pragma once

#include <type_traits>
#include <utility>

namespace elle
{
  namespace das
  {
    template <typename ... Formal>
    class tuple
      : public Formal::Symbol::template Field<std::decay_t<std::remove_reference_t<typename Formal::Type>>>...
    {
    public:
      template <typename ... Args>
      tuple(Args&& ... args)
        : Formal::Symbol::template Field<std::decay_t<std::remove_reference_t<typename Formal::Type>>>{
            std::forward<Args>(args)}...
      {}
    };

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
