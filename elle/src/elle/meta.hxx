#ifndef ELLE_META_HXX
# define ELLE_META_HXX

# include <type_traits>

namespace elle
{
  namespace meta
  {
    /*------.
    | Apply |
    `------*/

    template <typename ... Elts>
    template <template <typename ...> class T>
    struct List<Elts...>::apply
    {
      typedef T<Elts...> type;
    };

    /*-------.
    | Filter |
    `-------*/

    namespace
    {
      template <typename R, typename L, template <typename> class P>
      struct filter_helper
      {
        typedef R type;
      };

      template <typename ... Res,
                typename Head,
                typename ... Tail,
                template <typename> class P>
      struct filter_helper<List<Res...>, List<Head, Tail...>, P>
      {
        typedef typename filter_helper<
          typename std::conditional<P<Head>::value,
                                    List<Res...,
                                         Head>, List<Res...>>::type,
          List<Tail...>, P>::type type;
      };
    }

    template <typename ... Elts>
    template <template <typename> class P>
    struct List<Elts...>::filter
    {
      typedef typename filter_helper<List<>, List<Elts...>, P>::type type;
    };

    /*---------.
    | Index of |
    `---------*/

    namespace
    {
      template <int I, typename T, typename ... List>
      struct index_of_helper
      {};

      template <int I, typename T, typename Head, typename ... Tail>
      struct index_of_helper<I, T, Head, Tail...>
      {
        static constexpr int value =
          index_of_helper<I + 1, T, Tail...>::value;
      };

      template <int I, typename T, typename ... Tail>
      struct index_of_helper<I, T, T, Tail ...>
      {
        static constexpr int value = I;
      };
    }

    template <typename ... Elts>
    template <typename T>
    struct List<Elts...>::index_of
    {
      static constexpr auto value = index_of_helper<0, T, Elts...>::value;
    };

    /*--------.
    | Prepend |
    `--------*/

    template <typename ... Elts>
    template <typename T>
    struct List<Elts...>::prepend
    {
      typedef List<T, Elts...> type;
    };

    /*-----.
    | Tail |
    `-----*/


    namespace
    {
      template <typename ... List>
      struct tail_helper
      {};

      template <typename Head, typename ... Tail>
      struct tail_helper<Head, Tail...>
      {
        using type = List<Tail...>;
      };
    }

    template <typename ... Elts>
    template <int>
    struct List<Elts...>::tail
    {
      using type = typename tail_helper<Elts...>::type;
    };
  }
}

#endif
