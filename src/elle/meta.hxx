#include <tuple>
#include <type_traits>

namespace elle
{
  namespace meta
  {
    /*-------.
    | Filter |
    `-------*/

    namespace
    {
      template <typename R, typename L, template <typename> class P>
      struct filter_helper
      {
        using type = R;
      };

      template <typename ... Res,
                typename Head,
                typename ... Tail,
                template <typename> class P>
      struct filter_helper<List<Res...>, List<Head, Tail...>, P>
      {
        using type = typename filter_helper<
          std::conditional_t<P<Head>::value,
                                    List<Res...,
                                         Head>, List<Res...>>,
          List<Tail...>, P>::type;
      };
    }


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
                typename ... Tail,
                typename ... Args>
      struct map_helper<F, List<Head, Tail...>, List<Args...>>
      {
        using type = typename std::conditional<
          std::is_void<typename F<Head, Args...>::type>::value,
          typename List<Tail...>::template map<F, Args...>::type,
          typename List<Tail...>::template map<F, Args...>::type
          ::template prepend<typename F<Head, Args...>::type>::type>::type;
      };

      template <template <typename, typename ...> class F,
                typename Elts,
                typename ... Args>
      struct map_value_apply
      {};

      template <template <typename, typename ...> class F,
                typename ... Elts,
                typename ... Args>
      struct map_value_apply<F, List<Elts...>, Args...>
      {
        template <typename ... RTArgs>
        static
        auto
        value(RTArgs&& ... args)
        {
          return std::tuple<typename F<Elts, Args...>::type...>{
            F<Elts, Args...>::value(std::forward<RTArgs>(args)...)...};
        }
      };
    }

    template <typename ... Elts>
    template <template <typename, typename ...> class F, typename ... Args>
    struct List<Elts...>::map
      : public _details::map_value_apply<F, List<Elts...>, Args...>
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
      using type = List<T, Elts...>;
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
