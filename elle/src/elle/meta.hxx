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

    /*--------.
    | Prepend |
    `--------*/

    template <typename ... Elts>
    template <typename T>
    struct List<Elts...>::prepend
    {
      typedef List<T, Elts...> type;
    };

  }
}

#endif
