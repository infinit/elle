#ifndef ELLE_META_HXX
# define ELLE_META_HXX

# include <tuple>
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

    /*----.
    | Map |
    `----*/

    namespace
    {
      template <template <typename> class F, typename ... Elts>
      struct map_helper
      {
        using type = List<>;
      };

      template <template <typename> class F, typename Head>
      static constexpr
      typename std::enable_if_exists<
        decltype(F<Head>::value), bool>::type
      map_runtime(int)
      {
        return true;
      }

      template <template <typename> class F, typename Head>
      static constexpr
      typename std::enable_if_exists<typename F<Head>::type, bool>::type
      map_runtime(...)
      {
        return false;
      }

      template <template <typename> class F, typename Head, typename ... Tail>
      struct map_helper<F, Head, Tail...>
      {
        using type = typename List<Tail...>
          ::template map<F>::type
          ::template prepend<typename F<Head>::type>::type;
      };

      template <typename Res,
                bool runtime,
                template <typename> class F,
                typename ... Elts>
      struct map_value_apply
      {};

      template <template <typename> class F,
                typename ... Elts>
      struct map_value_apply<std::tuple<>, true, F, Elts...>
      {
        template <typename ... Args>
        static
        std::tuple<>
        value(Args&& ...)
        {
          return {};
        }
      };

      template <typename RHead,
                typename ... RTail,
                template <typename> class F,
                typename Head,
                typename ... Tail>
      struct map_value_apply<std::tuple<RHead, RTail...>, true, F, Head, Tail...>
      {
        template <typename ... Args>
        static
        std::tuple<RHead, RTail...>
        value(Args&& ... args)
        {
          return std::tuple_cat(
            std::make_tuple(F<Head>::value(std::forward<Args>(args)...)),
            map_value_apply<std::tuple<RTail...>, true, F, Tail...>::value(
              std::forward<Args>(args)...));
        }
      };

      template <typename Res, template <typename> class F, typename ... Elts>
      struct map_value_helper
      {
        template <typename ... Args>
        static
        std::tuple<>
        value(Args&& ...)
        {
          return {};
        }
      };

      template <typename Res,
                template <typename> class F,
                typename Head,
                typename ... Tail>
      struct map_value_helper<Res, F, Head, Tail...>
        : public map_value_apply<typename Res::template apply<std::tuple>::type,
                                 map_runtime<F, Head>(0), F, Head, Tail...>
      {};
    }

    template <typename ... Elts>
    template <template <typename> class F>
    struct List<Elts...>::map
      : public map_value_helper<
          typename map_helper<F, Elts...>::type, F, Elts...>
    {
      using type = typename map_helper<F, Elts...>::type;
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
