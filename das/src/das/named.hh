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
    T
    _make_formal(...);

    template <typename T>
    typename T::Formal
    _make_formal(int);

    template <typename T>
    using make_formal = std::remove_reference_t<decltype(_make_formal<T>(42))>;

    template <typename Formal, typename Effective>
    static
    constexpr std::enable_if_exists_t<typename Effective::Formal, int>
    lookup(int)
    {
      return std::is_same<make_formal<Formal>,
                          typename Effective::Formal>::value ? 1 : 2;
    };

    template <typename Formal, typename Effective>
    static
    constexpr int
    lookup(...)
    {
      return 0;
    };

    template <typename T>
    struct is_effective
    {
      static constexpr bool value =
        !std::is_same<decltype(_make_formal<T>(42)), T>::value;
    };

    template <bool effective, typename T>
    struct DefaultFor;

    /*-----.
    | Find |
    `-----*/

    template <int Index,
              int Current,
              typename Formal,
              typename LEffective,
              typename REffective>
    struct find;

    template <int lookup,
              int Index,
              int Current,
              typename Formal,
              typename LEffective,
              typename REffective>
    struct find_impl;

    // Recurse
    template <int Lookup,
              int Index,
              int Current,
              typename Formal,
              typename Effective,
              typename ... LEffectives,
              typename ... REffectives>
    struct find_impl<Lookup,
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
    struct find_impl<0,
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
              typename Formal,
              typename Effective,
              typename ... LEffectives,
              typename ... REffectives>
    struct find_impl<1,
                     Index,
                     Current,
                     Formal,
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
      : public find_impl<lookup<Formal, Effective>(0),
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
        using D = DefaultFor<true, Formal>;
        return d.D::value;
      }
    };

    /*-------------.
    | DefaultStore |
    `-------------*/

    template <bool Effective, typename T>
    struct DefaultFor
    {
      static constexpr bool has = false;
      template <typename Formal>
      DefaultFor(Formal const&)
      {}
    };

    template <typename T>
    struct DefaultFor<true, T>
      : public T::Formal::
          template Effective<typename T::Type, typename T::Type const&>
    {
      static constexpr bool has = true;
      using type = typename T::Type;
      using Super = typename T::Formal::
        template Effective<typename T::Type, typename T::Type const&>;
      using Super::Super;
    };

    template <typename ... Formal>
    struct DefaultStore
      : public DefaultFor<is_effective<Formal>::value, Formal>...
    {
      template <typename ... Args>
      DefaultStore(Args&& ... args)
        : DefaultFor<is_effective<Formal>::value, Formal>(
          std::forward<Args>(args))...
      {}

      template <typename T>
      using default_for = DefaultFor<is_effective<T>::value, T>;

      template <typename ... Args>
      auto
      extend(Args&& ... args) const
      {
        return DefaultStore<Args ..., Formal ...>(
          std::forward<Args>(args)...,
          static_cast<DefaultFor<is_effective<Formal>::value, Formal> const&>(
            *this)...);
      }
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

      template <typename... NewArgs>
      auto
      extend(NewArgs&& ... args)
      {
        return Prototype<NewArgs..., Formal...>
          (this->defaults.extend(std::forward<NewArgs>(args)...));
      }

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
                 std::remove_cv_reference_t<Formal>,
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
