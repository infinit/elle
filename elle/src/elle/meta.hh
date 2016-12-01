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

      /// List<F<Elts, Args...>...>
      template <template <typename, typename ...> class F, typename ... Args>
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

    template <bool C, typename T, typename E>
    struct _static_if
    {
      using type = T;
      static
      T const&
      value(T const& t, E const&)
      {
        return t;
      }
    };

    template <typename T, typename E>
    struct _static_if<false, T, E>
    {
      using type = E;
      static
      E const&
      value(T const&, E const& e)
      {
        return e;
      }
    };

    template <bool Cond, typename T, typename E>
    typename _static_if<Cond, T, E>::type const&
    static_if(T const& then, E const& else_)
    {
      return _static_if<Cond, T, E>::value(then, else_);
    }

    struct Ignore
    {
      template <typename ... Args>
      void
      operator()(Args&& ...)
      {}
    };

    template <bool Cond, typename T>
    typename _static_if<Cond, T, Ignore>::type
    static_if(T const& then)
    {
      return static_if<Cond>(then, Ignore());
    }
  }
}

# include <elle/meta.hxx>

#endif
