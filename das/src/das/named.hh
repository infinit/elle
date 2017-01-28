#pragma once

#include <utility>

#include <elle/attribute.hh>
#include <elle/log.hh>
#include <elle/meta.hh>

#include <das/Symbol.hh>

namespace das
{
  namespace named
  {
    template <typename ... Args>
    using List = elle::meta::List<Args...>;

    /*--------.
    | Helpers |
    `--------*/

    template <typename T>
    static
    constexpr std::enable_if_exists_t<typename T::Formal, bool>
    is_named(int)
    {
      return std::is_base_of<das::Symbol, typename T::Formal>::value;
    };

    template <typename T>
    static
    constexpr bool
    is_named(...)
    {
      return false;
    };

    template <typename T>
    T
    _make_formal(...);

    template <typename T>
    typename T::Formal
    _make_formal(int);

    template <typename T>
    using make_formal = std::remove_reference_t<decltype(_make_formal<T>(42))>;

    template <typename T>
    struct is_effective
    {
      static constexpr bool value =
        !std::is_same<decltype(_make_formal<T>(42)), T>::value;
    };

    /*-----.
    | Find |
    `-----*/

    template <int Index,
              int Current,
              typename Formal,
              typename LEffective,
              typename REffective>
    struct find;

    template <bool named,
              int Index,
              int Current,
              typename Formal,
              typename LEffective,
              typename REffective>
    struct find_impl;

    // Recurse
    template <bool named,
              int Index,
              int Current,
              typename Formal,
              typename Effective,
              typename ... LEffectives,
              typename ... REffectives>
    struct find_impl<named,
                     Index,
                     Current,
                     Formal,
                     elle::meta::List<LEffectives...>,
                     elle::meta::List<Effective, REffectives...>>
      : public find<Index,
                    Current + 1,
                    Formal,
                    elle::meta::List<LEffectives..., Effective>,
                    elle::meta::List<REffectives...>>
    {};

    // Positional
    template <int Current,
              typename Formal,
              typename Effective,
              typename ... LEffectives,
              typename ... REffectives>
    struct find_impl<false,
                     Current,
                     Current,
                     Formal,
                     elle::meta::List<LEffectives...>,
                     elle::meta::List<Effective, REffectives...>>
    {
      using type = Effective;
      template <typename Default>
      static
      auto
      get(Default&, LEffectives&& ..., Effective&& e, REffectives&& ...)
        -> decltype(std::forward<Effective>(e))
      {
        return std::forward<Effective>(e);
      }
    };

    // Named.
    template <int Index,
              int Current,
              typename Effective,
              typename ... LEffectives,
              typename ... REffectives>
    struct find_impl<true,
                     Index,
                     Current,
                     typename Effective::Formal,
                     elle::meta::List<LEffectives...>,
                     elle::meta::List<Effective, REffectives...>>
    {
      template <typename Default>
      static
      typename Effective::Passing
      get(Default&, LEffectives&& ..., Effective&& e, REffectives&& ...)
      {
        return std::forward<typename Effective::Passing>(e.value);
      }
    };

    // Switch.
    template <int Index,
              int Current,
              typename Formal,
              typename Effective,
              typename ... LEffectives,
              typename ... REffectives>
    struct find<Index,
                Current,
                Formal,
                elle::meta::List<LEffectives...>,
                elle::meta::List<Effective, REffectives...>>
      : public find_impl<is_named<Effective>(0),
                         Index,
                         Current,
                         Formal,
                         elle::meta::List<LEffectives...>,
                         elle::meta::List<Effective, REffectives...>>
    {};

    // Not found, use default.
    template <int Index,
              int Current,
              typename Formal,
              typename ... Effectives>
    struct find<Index,
                Current,
                Formal,
                elle::meta::List<Effectives...>,
                elle::meta::List<>>
    {
      template <typename Default>
      static
      auto&
      get(Default& d, Effectives&& ...)
      {
        using D = typename Default::template default_for<Formal>::type;
        return d.D::value;
      }
    };

    /*-------------.
    | DefaultStore |
    `-------------*/

    // The Tail remainder is there to uniquify
    template <typename T, typename ... Tail>
    struct
    Empty
    {
      Empty(T const&)
      {}
    };

    template <bool effective, typename T, typename D>
    struct DefaultEffective
    {
      using type = D;
    };

    template <typename T, typename D>
    struct DefaultEffective<true, T, D>
    {
      using type =
        typename T::Formal::
        template Effective<typename T::Type, typename T::Type const&>;
    };

    template <typename ... Args>
    struct DefaultStore
    //: public DefaultEffective<is_effective<Args>::value, std::decay_t<Args>, Empty<Args>>::type...
    {
      template <typename T>
      struct default_for
      {
        static constexpr bool has = false;
        using type = void;
      };
    };

    template <typename Head, typename ... Tail>
    struct DefaultStore<Head, Tail...>
      : public DefaultStore<Tail...>
      , public DefaultEffective<
          is_effective<Head>::value,
          std::decay_t<Head>,
          Empty<Head, Tail ...>>::type
    {
      using SuperHead = typename DefaultEffective<
        is_effective<Head>::value,
        std::decay_t<Head>,
        Empty<Head, Tail ...>>::type;

      template <typename CHead, typename ... CTail>
      DefaultStore(CHead&& head, CTail&& ... tail)
        : DefaultStore<Tail...>(std::forward<CTail>(tail)...)
        , SuperHead(std::forward<CHead>(head))
      {}

      template <typename T>
        struct default_for
      {
        static constexpr bool has =
          std::is_same<T, make_formal<Head>>::value ?
          is_effective<Head>::value :
          DefaultStore<Tail...>::template default_for<T>::has;
        using type =
          std::conditional_t<
            std::is_same<T, make_formal<Head>>::value,
            SuperHead,
            typename DefaultStore<Tail...>::template default_for<T>::type
            >;
      };
    };

    /*----------.
    | Prototype |
    `----------*/

    template <typename ... Formal>
    struct Prototype
      : public elle::Printable::as<Prototype<Formal...>>
    {
      ELLE_LOG_COMPONENT("das.named");
      using DefaultStore = named::DefaultStore<Formal...>;
      DefaultStore defaults;
      Prototype(DefaultStore&& d)
        : defaults(std::move(d))
      {}

      template <typename Sequence>
      struct Call;

      template <typename I, I ... Index>
        struct Call<std::integer_sequence<I, Index ...>>
      {
        template <typename Default, typename F, typename ... Args>
        static
        auto
        call(Default& defaults, F const& f, Args&& ... args)
        {
          return f(
            find<Index,
                 0,
                 make_formal<std::remove_cv_reference_t<Formal>>,
                 elle::meta::List<>,
                 elle::meta::List<Args...>>::get(
                   defaults, std::forward<Args>(args)...)...);
        }
      };

      template <typename F, typename ... Args>
      auto
      call(F const& f, Args&& ... args) const
      {
        return Call<std::make_index_sequence<sizeof ... (Formal)>>::call(
          this->defaults, f, std::forward<Args>(args)...);
      }

      template <typename Arg>
      struct print_prototype
      {
        using type = bool;
        static
        bool
        value(std::ostream& o, bool& first)
        {
          if (first)
            first = false;
          else
            o << ", ";
          o << Arg::name();
          return true;
        }
      };

      void
      print(std::ostream& o) const
      {
        bool first = true;
        elle::meta::List<std::remove_cv_reference_t<Formal>...>::
          template map<print_prototype>::value(o, first);
      }
    };

    template <typename ... Args>
    Prototype<Args...>
    prototype(Args&& ... args)
    {
      return Prototype<Args...>
        (DefaultStore<Args...>(std::forward<Args>(args)...));
    }

    /*---------.
    | Function |
    `---------*/

    template <typename F, typename ... Args>
    class Function
      : public elle::Printable::as<Function<F, Args...>>
    {
    public:
      template <typename ... CArgs>
      Function(F f, CArgs&& ... args)
        : _function(std::move(f))
        , _prototype(DefaultStore<Args...>(std::forward<CArgs>(args)...))
      {}

      template <typename ... Effective>
      auto
      operator() (Effective&& ... effective) const
      {
        return this->_prototype.call(
          this->_function, std::forward<Effective>(effective)...);
      }

      ELLE_ATTRIBUTE_R(F, function);
      ELLE_ATTRIBUTE_R((Prototype<Args...>), prototype);

      void
      print(std::ostream& out) const
      {
        // elle::fprintf(out, "%s[%s]", this->_function, this->_prototype);
        elle::fprintf(
          out, "%s[%s]", this->_function, this->_prototype);
      }
    };

    template <typename F, typename ... Args>
    Function<F, std::remove_cv_reference_t<Args>...>
    function(F f, Args&& ... args)
    {
      return Function<F, std::remove_cv_reference_t<Args>...>(
        std::move(f), std::forward<Args>(args)...);
    }
  }
}
