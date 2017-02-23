#ifndef ELLE_NAMED_HH
# define ELLE_NAMED_HH

# include <utility>

# include <elle/meta.hh>

#define NAMED_ARGUMENT(Name)                                            \
  __attribute__((unused))                                               \
  static struct Name##_type                                             \
    : public elle::named::Argument<Name##_type>                         \
  {                                                                     \
    using elle::named::Argument<Name##_type>::operator=;                \
  } Name;                                                               \

#define NAMED_ARGUMENT_DEFINE(Name, container)                          \
  container::Name##_type container::Name;

#define NAMED_FUNCTION(Name, F, ...)                                    \
  template <typename ... Args>                                          \
  auto                                                                  \
  Name(Args&& ... args) ->                                              \
    decltype(elle::named::prototype(__VA_ARGS__).call(                  \
               F, std::forward<Args>(args)...))                         \
  {                                                                     \
    return elle::named::prototype(__VA_ARGS__).call(                    \
      F, std::forward<Args>(args)...);                                  \
  }                                                                     \

namespace elle
{
  namespace named
  {
    struct BaseArgument
    {};

    /// Base class of all formal arguments
    template <typename Tag>
    struct Argument
      : public BaseArgument
    {
      template <typename E>
      struct Effective
      {
        typedef E Type;
        typedef Tag Formal;
        Type value;
      };

      template <typename E>
      Effective<E&&>
      operator = (E&& v)
      {
        return Effective<E&&>{std::forward<E>(v)};
      }
    };

    /*--------.
    | Applier |
    `--------*/

    // Declaration
    template <typename DefaultStore,
              typename Formal,
              typename Applied,
              typename Remaining,
              typename Store>
    struct Applier;

    template <typename DefaultStore,
              typename Formal,
              typename Applied,
              typename Remaining,
              typename Store,
              bool effective>
    struct NotFoundApplier;

    // Done
    template <typename DefaultStore,
              typename ... Applied,
              typename ... Remaining>
    struct Applier<DefaultStore,
                   meta::List<>,
                   meta::List<Applied...>,
                   meta::List<Remaining...>,
                   meta::List<>>
    {
      template <typename F>
        struct result_of
      {
        typedef
          typename std::result_of<F&(Remaining..., Applied...)>::type type;
      };

      template <typename F>
        static
        typename std::result_of<F&(Remaining..., Applied...)>::type
        apply(DefaultStore&,
              F const& f,
              Applied&& ... applied,
              Remaining&& ... positional)
      {
        return f(std::forward<Remaining>(positional)...,
                 std::forward<Applied>(applied)...);
      }
    };

    // Match
    template <typename DefaultStore,
              typename ... Tail,
              typename ... Applied,
              typename RHead,
              typename ... RTail,
              typename ... Store
              >
    struct Applier<DefaultStore,
                   meta::List<typename RHead::Formal, Tail...>,
                   meta::List<Applied...>,
                   meta::List<RHead, RTail...>,
                   meta::List<Store...>>
    {
      typedef Applier<DefaultStore,
                      meta::List<Tail...>,
                      meta::List<Applied..., typename RHead::Type>,
                      meta::List<Store..., RTail...>,
                      meta::List<>> next;
      template <typename F>
        struct result_of
      {
        typedef typename next::template result_of<F>::type type;
      };

      template <typename F>
        static
        typename next::template result_of<F>::type
        apply(DefaultStore& defaults,
              F const& f,
              Applied&& ... applied,
              RHead&& match,
              RTail&& ... remaining,
              Store&& ... store)
      {
        return next::apply(defaults,
                           f,
                           std::forward<Applied>(applied)...,
                           std::forward<typename RHead::Type>(match.value),
                           std::forward<Store>(store)...,
                           std::forward<RTail>(remaining)...);
      }
    };

    // Mismatch
    template <typename DefaultStore,
              typename Head,
              typename ... Tail,
              typename ... Applied,
              typename RHead,
              typename ... RTail,
              typename ... Store
              >
    struct Applier<DefaultStore,
                   meta::List<Head, Tail...>,
                   meta::List<Applied...>,
                   meta::List<RHead, RTail...>,
                   meta::List<Store...>>
    {
      typedef Applier<DefaultStore,
                      meta::List<Head, Tail...>,
                      meta::List<Applied...>,
                      meta::List<RTail...>,
                      meta::List<Store..., RHead>> next;
      template <typename F>
        struct result_of
      {
        typedef typename next::template result_of<F>::type type;
      };
      template <typename F>
        static
        typename next::template result_of<F>::type
        apply(DefaultStore& defaults,
              F const& f,
              Applied&& ... applied,
              RHead&& mismatch,
              RTail&& ... remaining,
              Store&& ... store)
      {
        return next::apply(defaults,
                           f,
                           std::forward<Applied>(applied)...,
                           std::forward<RTail>(remaining)...,
                           std::forward<Store>(store)...,
                           std::forward<RHead>(mismatch));
      }
    };

    // Positional
    template <typename DefaultStore,
              typename Head,
              typename ... Tail,
              typename ... Applied,
              typename StoreHead,
              typename ... StoreTail>
    struct NotFoundApplier<DefaultStore,
                           meta::List<Head, Tail...>,
                           meta::List<Applied...>,
                           meta::List<>,
                           meta::List<StoreHead, StoreTail ...>,
                           false>
    {
      typedef Applier<DefaultStore,
                      meta::List<Tail...>,
                      meta::List<Applied..., StoreHead>,
                      meta::List<StoreTail...>,
                      meta::List<>> next;
      template <typename F>
        struct result_of
      {
        typedef typename next::template result_of<F>::type type;
      };
      template <typename F>
        static
        typename next::template result_of<F>::type
        apply(DefaultStore& defaults,
              F const& f,
              Applied&& ... applied,
              StoreHead&& store_head,
              StoreTail&& ... store_tail)
      {
        return next::apply(
          defaults,
          f,
          std::forward<Applied>(applied)...,
          std::forward<StoreHead>(store_head),
          std::forward<StoreTail>(store_tail)...);
      }
    };

    // Default
    template <typename DefaultStore,
              typename Head,
              typename ... Tail,
              typename ... Applied,
              typename ... Store>
    struct NotFoundApplier<DefaultStore,
                           meta::List<Head, Tail...>,
                           meta::List<Applied...>,
                           meta::List<>,
                           meta::List<Store...>,
                           true>
    {
      static_assert(DefaultStore::template default_for<Head>::has,
                    "missing argument");
      typedef
        typename DefaultStore::template default_for<Head>::type
        default_type;
      typedef Applier<DefaultStore,
                      meta::List<Tail...>,
                      meta::List<Applied..., typename default_type::Type>,
                      meta::List<Store...>,
                      meta::List<>> next;
      template <typename F>
        struct result_of
      {
        typedef typename next::template result_of<F>::type type;
      };
      template <typename F>
        static
        typename result_of<F>::type
        apply(DefaultStore& defaults,
              F const& f,
              Applied&& ... applied,
              Store&& ... store)
      {
        return next::apply(
          defaults,
          f,
          std::forward<Applied>(applied)...,
          std::forward<typename default_type::Type>
            (defaults.default_type::value),
          std::forward<Store>(store)...);
      }
    };

    template <typename T>
    static
    constexpr std::enable_if_exists_t<typename T::Formal, bool>
    is_named(int)
    {
      return std::is_base_of<BaseArgument, typename T::Formal>::value;
    };

    template <typename T>
    static
    constexpr bool
    is_named(...)
    {
      return false;
    };

    // Not found: bounce to positional or default value
    template <typename DefaultStore,
              typename FormalHead,
              typename ... FormalTail,
              typename Applied,
              typename StoreHead,
              typename ... StoreTail>
    struct Applier<DefaultStore,
                   meta::List<FormalHead, FormalTail...>,
                   Applied,
                   meta::List<>,
                   meta::List<StoreHead, StoreTail...>>
      : public NotFoundApplier<DefaultStore,
                               meta::List<FormalHead, FormalTail...>,
                               Applied,
                               meta::List<>,
                               meta::List<StoreHead, StoreTail...>,
                               is_named<StoreHead>(42)>
    {};

    // Not found without any value left: bounce to default value
    template <typename DefaultStore,
              typename FormalHead,
              typename ... FormalTail,
              typename Applied>
    struct Applier<DefaultStore,
                   meta::List<FormalHead, FormalTail...>,
                   Applied,
                   meta::List<>,
                   meta::List<>>
      : public NotFoundApplier<DefaultStore,
                               meta::List<FormalHead, FormalTail...>,
                               Applied,
                               meta::List<>,
                               meta::List<>,
                               true>
    {};

    template <typename T>
    T
    _make_formal(...);

    template <typename T>
    typename T::Formal
    _make_formal(int);

    template <typename T>
    struct make_formal
    {
      typedef
        typename std::remove_reference<decltype(_make_formal<T>(42))>::type
        type;
    };

    template <typename T>
    struct is_effective
    {
      static constexpr bool value =
        !std::is_same<decltype(_make_formal<T>(42)), T>::value;
    };

    template <typename DefaultStore, typename ... Formal>
    struct Prototype
    {
      DefaultStore defaults;
      Prototype(DefaultStore&& d)
        : defaults(std::move(d))
      {}

      template <typename F, typename ... Args>
      typename Applier<DefaultStore,
                       meta::List<typename make_formal<Formal>::type...>,
                       meta::List<>,
                       meta::List<Args...>,
                       meta::List<>>::template result_of<F>::type
        call(F const& f, Args&& ... args)
      {
        return Applier<
          DefaultStore,
          meta::List<typename make_formal<Formal>::type...>,
          meta::List<>,
          meta::List<Args...>,
          meta::List<>>::apply(defaults, f, std::forward<Args>(args)...);
      }
    };


    // The Tail remainder is there to uniquify
    template <typename T, typename ... Tail>
    struct
    Empty
    {
      Empty(T&)
      {}
    };

    template <typename ... Args>
    struct DefaultStore
    {
      typedef meta::List<> defaults_type;
      template <typename T>
      struct default_for
      {
        static constexpr bool has = false;
        typedef void type;
      };
    };

    template <typename Head, typename ... Tail>
    struct DefaultStore<Head, Tail...>
      : public DefaultStore<Tail...>
      , public std::conditional<is_effective<Head>::value, Head, Empty<Head, Tail ...>>::type
    {
      typedef typename std::conditional<
        is_effective<Head>::value,
        typename DefaultStore<Tail...>::defaults_type::template prepend<Head>::type,
        typename DefaultStore<Tail...>::defaults_type
        >::type
        defaults_type;

      DefaultStore(Head&& head, Tail&& ... tail)
        : DefaultStore<Tail...>(std::forward<Tail>(tail)...)
        , std::conditional<is_effective<Head>::value, Head, Empty<Head, Tail ...>>::type(std::forward<Head>(head))
      {}

      template <typename T>
        struct default_for
      {
        static constexpr bool has =
          std::is_same<T, typename make_formal<Head>::type>::value ?
          is_effective<Head>::value :
          DefaultStore<Tail...>::template default_for<T>::has;
        typedef typename
        std::conditional<
          std::is_same<T, typename make_formal<Head>::type>::value,
          Head,
          typename DefaultStore<Tail...>::template default_for<T>::type
          >::type type;
      };
    };

    template <typename ... Args>
    Prototype<DefaultStore<Args...>,
              typename std::remove_reference<Args>::type...>
    prototype(Args&& ... args)
    {
      return Prototype<DefaultStore<Args...>,
                       typename std::remove_reference<Args>::type...>
        (DefaultStore<Args...>(std::forward<Args>(args)...));
    }
  }
}

#endif
