#ifndef ELLE_META_HH
# define ELLE_META_HH

namespace elle
{
  namespace meta
  {
    template <typename ... Elts>
    struct List
    {
      /// T<Args ..., Elts...>
      template <template <typename ...> class T, typename ... Args>
      struct
      apply;

      /// A list containing elements that match P
      template <template <typename> class P>
      struct filter;

      /// The position of T in List
      template <typename T>
      struct
      index_of;

      /// List<F<Elts>...>
      template <template <typename> class F>
      struct
      map;

      /// List<T, Elts...>
      template <typename T>
      struct
      prepend;

      /// List<F<Elts>...>
      template <int = 0>
      struct
      tail;
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
  }
}

# include <elle/meta.hxx>

#endif
