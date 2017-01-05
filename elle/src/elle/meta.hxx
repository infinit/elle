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
    template <template <typename ...> class T, typename ... Args>
    struct List<Elts...>::apply
    {
      typedef T<Args..., Elts...> type;
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

    namespace _details
    {
      template <template <typename, typename ...> class F,
                typename L,
                typename Args>
      struct map_helper
      {
        using type = List<>;
      };

      template <template <typename, typename ...> class F,
                typename Head,
                typename Args>
      static constexpr
      std::enable_if_exists_t<
        decltype(Args::template apply<F, Head>::type::value),
        bool>
      map_runtime(int)
      {
        return true;
      }

      template <template <typename, typename ...> class F,
                typename Head,
                typename Args>
      static constexpr
      std::enable_if_exists_t<
        typename Args::template apply<F, Head>::type::type, bool>
      map_runtime(...)
      {
        return false;
      }

      template <template <typename, typename ...> class F,
                typename Head,
                typename ... Tail,
                typename ... Args>
      struct map_helper<F, List<Head, Tail...>, List<Args...>>
      {
        using type = typename std::conditional<
          std::is_same<typename F<Head, Args...>::type, void>::value,
          typename List<Tail...>::template map<F, Args...>::type,
          typename List<Tail...>::template map<F, Args...>::type
          ::template prepend<typename F<Head, Args...>::type>::type>::type;
      };

      template <typename Res,
                bool runtime,
                template <typename, typename ...> class F,
                typename Elts,
                typename ... Args>
      struct map_value_apply
      {};

      template <template <typename, typename ...> class F,
                typename ... Elts,
                typename ... ExtraArgs>
      struct map_value_apply<std::tuple<>, true, F, List<Elts...>, ExtraArgs...>
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
                template <typename, typename ...> class F,
                typename Head,
                typename ... Tail,
                typename ... ExtraArgs>
      struct map_value_apply<std::tuple<RHead, RTail...>,
                             true,
                             F,
                             List<Head, Tail...>,
                             ExtraArgs...>
      {
        template <typename ... Args>
        static
        std::tuple<RHead, RTail...>
        value(Args&& ... args)
        {
          auto head = std::tuple<typename F<Head, ExtraArgs...>::type>(
            F<Head, ExtraArgs...>::value(std::forward<Args>(args)...));
          return std::tuple_cat(
            std::move(head),
            map_value_apply<std::tuple<RTail...>,
                            true,
                            F,
                            List<Tail...>,
                            ExtraArgs...>::value(
              std::forward<Args>(args)...));
        }
      };

      template <typename Res,
                template <typename, typename ...> class F,
                typename Elts,
                typename ExtraArgs>
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
                template <typename, typename ...> class F,
                typename Head,
                typename ... Tail,
                typename ... Args>
      struct map_value_helper<Res, F, List<Head, Tail...>, List<Args...>>
        : public map_value_apply<
            typename Res::template apply<std::tuple>::type,
            map_runtime<F, Head, List<Args...>>(0), F, List<Head, Tail...>, Args...>
      {};
    }

    template <typename ... Elts>
    template <template <typename, typename ...> class F, typename ... Args>
    struct List<Elts...>::map
      : public _details::map_value_helper<
          typename _details::map_helper<F, List<Elts...>, List<Args...>>::type,
          F,
          List<Elts...>,
          List<Args...>>
    {
      using type = typename _details::map_helper<
        F, List<Elts...>, List<Args...>>::type;
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
    | Head |
    `-----*/

    namespace
    {
      template <typename Default, typename ... List>
      struct head_helper
      {
        using type = Default;
      };

      template <typename Default, typename Head, typename ... Tail>
      struct head_helper<Default, Head, Tail...>
      {
        using type = Head;
      };

      template <typename ... List>
      struct head_helper<Null, List ...>
      {};
    }

    template <typename ... Elts>
    template <typename Default>
    struct List<Elts...>::head
    {
      using type = typename head_helper<Default, Elts...>::type;
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
