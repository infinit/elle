#pragma once

#include <type_traits> // std::integral_constant
#include <utility> // std::forward

namespace elle
{
  namespace meta
  {
    struct Null
    {};

    namespace
    {
      template <typename R, typename L, template <typename> class P>
      struct filter_helper;
    }

    /// Always map to false, but in a lazy way.  Useful for
    /// static_assert, by making a type-dependent false.
    template <typename... Args>
    constexpr auto
    lazy_false(Args&&...)
    {
      return false;
    }

    template <typename ... Elts>
    struct List
    {
      /// T<Args ..., Elts...>
      template <template <typename ...> class T, typename ... Args>
      using apply = T<Args..., Elts...>;

      /// A list containing elements that match P
      template <template <typename> class P>
      using filter = typename filter_helper<List<>, List<Elts...>, P>::type;

      /// The position of T in List
      template <typename T>
      struct
      index_of;

      /// List<F<Elts, Args...>...>
      template <template <typename, typename ...> class F, typename ... Args>
      struct
      map;

      /// List<T, Elts...>
      template <typename T>
      struct
      prepend;

      /// List<Elts[1:]>
      template <int = 0>
      struct
      tail;

      /// Elts[0]
      template <typename Default = Null>
      struct
      head;
    };

    /// Helper to declare `List` from values through `decltype`.
    template<typename ... Args>
    List<Args...>
    list(Args ...);

    /// { value = T[0] && ... && T[N]; }
    template <typename ... T>
    struct All;

    template <>
    struct All<>
    {
      static bool constexpr value = true;
    };

    template <typename Head, typename ... Tail>
    struct All<Head, Tail...>
    {
      static bool constexpr value = Head::value && All<Tail...>::value;
    };


    /*------------.
    | static-if.  |
    `------------*/

    /// Execute the then-clause.
    template <typename Then, typename Else>
    auto constexpr static_if_impl(std::true_type, Then&& then, Else&&)
    {
      return std::forward<Then>(then);
    }

    /// Execute the else-clause.
    template <typename Then, typename Else>
    auto constexpr static_if_impl(std::false_type, Then&&, Else&& else_)
    {
      return std::forward<Else>(else_);
    }

    /// Execute the then- or the else-clause depending on \a cond.
    template <bool Cond, typename Then, typename Else>
    auto constexpr static_if(Then&& then, Else&& else_)
    {
      return static_if_impl(std::integral_constant<bool, Cond>{},
                            std::forward<Then>(then),
                            std::forward<Else>(else_));
    }

    struct Ignore
    {
      template <typename ... Args>
      void constexpr
      operator()(Args&& ...)
      {}
    };

    /// Execute the then-clause if \a cond is verified.
    template <bool Cond, typename Then>
    auto constexpr static_if(Then&& then)
    {
      return static_if<Cond>(std::forward<Then>(then), Ignore{});
    }

    template <typename T>
    struct Identity
    {
      using type = T;
    };
  }
}

#include <elle/meta.hxx>
