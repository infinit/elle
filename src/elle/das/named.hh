#pragma once

#include <utility>

#include <elle/Option.hh>
#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/log.hh>
#include <elle/meta.hh>
#include <elle/serialization/Serializer.hh>

#include <elle/das/Symbol.hh>
#include <elle/das/tuple.hh>

namespace elle
{
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
      using make_formal =
        std::remove_reference_t<decltype(_make_formal<T>(42))>;

      template <typename T>
      typename T::Symbol
      _make_default(...);

      template <typename T>
      std::enable_if_exists_t<typename T::Passing, T>
      _make_default(int);

      template <typename T>
      using make_default =
        std::remove_reference_t<decltype(_make_default<T>(42))>;

      template <typename T>
      T
      _make_symbol(...);

      template <typename T>
      typename T::Symbol
      _make_symbol(int);

      template <typename T>
      using make_symbol =
        std::remove_reference_t<decltype(_make_symbol<T>(42))>;

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
          return d.DefaultFor<true, Formal>::value;
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
            template Effective<typename T::Default, typename T::Default const&>
      {
        static constexpr bool has = true;
        using type = typename T::Default;
        using Super = typename T::Formal::
          template Effective<typename T::Default, typename T::Default const&>;
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
        {
          static_assert(sizeof ... (Formal) == sizeof ... (Args), "LOLEUH2");
        }

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
        ELLE_LOG_COMPONENT("elle.das.named");
        using DefaultStore = named::DefaultStore<Formal...>;
        DefaultStore defaults;
        Prototype(DefaultStore&& d)
          : defaults(std::move(d))
        {}

        template <typename... NewArgs>
        auto
        extend(NewArgs&& ... args) const
        {
          return Prototype<NewArgs..., Formal...>
            (this->defaults.extend(std::forward<NewArgs>(args)...));
        }

        template <int Index, typename F, typename ... Args>
        using find = named::find<Index,
                                 0,
                                 std::remove_cv_reference_t<F>,
                                 elle::meta::List<>,
                                 elle::meta::List<Args...>>;

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
            return f(find<Index, Formal, Args...>::get(
                       defaults, std::forward<Args>(args)...)...);
          }

          template <typename ... Types, typename ... Args>
          static
          auto
          map(DefaultStore const& defaults, Args&& ... args)
          {
            return das::make_tuple(
              (make_formal<Formal>() =
                Types{find<Index, Formal, Args...>::get(
                  defaults, std::forward<Args>(args)...)})...);
          }
        };

        template <typename F, typename ... Args>
        auto
        call(F const& f, Args&& ... args) const
        {
          return Call<std::make_index_sequence<sizeof ... (Formal)>>::call(
            this->defaults, f, std::forward<Args>(args)...);
        }

        template <typename ... Types, typename ... Args>
        auto
        map(Args&& ... args) const
        {
          return Call<std::make_index_sequence<sizeof ... (Formal)>>::
            template map<Types...>(
              this->defaults, std::forward<Args>(args)...);
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
        return {DefaultStore<Args...>(std::forward<Args>(args)...)};
      }

      /*---------.
      | Function |
      `---------*/

      template <typename Signature>
      class Function;

      namespace
      {
        template <typename T>
        struct get_type_impl
        {
          using type = typename T::Type;
        };

        template <typename T>
        using get_type = typename get_type_impl<T>::type;
      }

      namespace
      {
        template <typename T>
        struct result_type_impl
        {
          using type = elle::Option<T, std::exception_ptr>;
          using mapped_type = T;
        };

        /// To make `void` a regular type, make it an int.
        template <>
        struct result_type_impl<void>
          : result_type_impl<int>
        {};

        /// The result type of a function: return value and exception.
        template <typename T>
        using result_type = typename result_type_impl<T>::type;
      }

      namespace
      {
        template<typename T, typename D>
        class Builder
        {
        public:
          Builder(D&& val)
            : _default(std::move(val))
          {}

          T result()
          {
            if (_content)
              return std::move(*_content);
            else
              return std::move(_default);
          }

          template<typename V>
          void
          operator , (V&& b)
          {
            _content = T(std::move(b));
          }

        private:
          D _default;
          boost::optional<T> _content;
        };

        template<typename T, typename D>
        Builder<T, D>
        make_builder(D &&v)
        {
          return std::move(v);
        }
      }

      template <typename R, typename ... Args>
      class Function<R (Args...)>
        : public Printable::as<Function<R (Args...)>>
      {
      public:
        using F = std::function<R (get_type<Args> ...)>;
        template <typename ... CArgs>
        Function(F f, CArgs&& ... args)
          : _function(std::move(f))
          , _prototype(DefaultStore<make_default<Args>...>(
                         std::forward<CArgs>(args)...))
        {
          static_assert(sizeof ... (Args) <= sizeof ... (args),
                        "too few effective arguments compared to signature");
          static_assert(sizeof ... (Args) >= sizeof ... (args),
                        "too many effective arguments compared to signature");
        }

        template <typename ... Effective>
        auto
        operator() (Effective&& ... effective) const
        {
          return this->_prototype.call(
            this->_function, std::forward<Effective>(effective)...);
        }

        template <typename T>
        using make_effective = typename make_symbol<T>::template Effective<
          std::remove_cv_reference_t<get_type<T>>,
          std::remove_cv_reference_t<get_type<T>>>;

        class Call
          : public make_effective<Args>...
        {
        public:
          Call(get_type<Args> const& ... args)
            : make_effective<Args>(args)...
          {}

          Call(elle::serialization::SerializerIn& s)
            : make_effective<Args>(
                s.deserialize<std::remove_cv_reference_t<get_type<Args>>>(
                  make_symbol<Args>::name()))...
          {}

          void
          serialize(serialization::Serializer& s)
          {
            int ignore[] = {
              (s.serialize(make_symbol<Args>::name(),
                           this->make_effective<Args>::value), 0)...
            };
            (void)ignore;
          }
        };

        class Result
          : public result_type<R>
        {
        public:
          using Super = result_type<R>;
          using Super::Super;
          using Option = typename Super::Option;
          using is_void = std::is_same<R, void>;
          R
          operator()()
          {
            using M = typename result_type_impl<R>::mapped_type;
            if (this->template is<M>())
              return (R)this->template get<M>();
            else
              std::rethrow_exception(this->template get<std::exception_ptr>());
          }
        };

        template <typename ... Effective>
        Call
        call(Effective&& ... args)
        {
          return this->_prototype.call(
            [] (get_type<Args> const& ... args)
            {
              return Call(args...);
            }, std::forward<Effective>(args)...);
        }

        template <typename C>
        std::enable_if_t<
          std::is_same<std::remove_cv_reference_t<C>, Call>::value, Result>
        operator() (C&& c) const
        {
          try
          {
            using M = typename result_type_impl<R>::mapped_type;
            auto builder = make_builder<Result>(M());
            // Clang/GCC think we're not use `builder`.  The point of
            // the `operator,` trick to deal with `void`.
#if defined __GCC__ || defined __clang__
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-value"
#endif
            builder, this->_function(std::move(c.make_effective<Args>::value)...);
#if defined __GCC__ || defined __clang__
# pragma GCC diagnostic pop
#endif
            return builder.result();
          }
          catch (elle::Exception const& e)
          {
            return std::current_exception();
          }
        }

        template <typename C>
        std::enable_if_t<
          std::is_same<std::remove_cv_reference_t<C>, Call>::value, Result>
        operator() (C const& c) const
        {
          try
          {
            return this->_function(c.make_effective<Args>::value...);
          }
          catch (elle::Exception const& e)
          {
            return std::current_exception();
          }
        }
        ELLE_ATTRIBUTE_R(F, function);
        ELLE_ATTRIBUTE_R(
          (Prototype<make_default<Args> ...>), prototype);

        void
        print(std::ostream& out) const
        {
          elle::fprintf(
            out, "%s[%s]", this->_function, this->_prototype);
        }
      };

      namespace
      {
        template <typename T, typename Named>
        typename Named::template Formal<T>
        make_signature_arg(...);

        template <typename T, typename Named>
        typename Named::template rebind<T, T>
        make_signature_arg(int);

        template <typename S, typename ... Named>
        struct make_signature;

        template <typename R, typename ... Args, typename ... Named>
        struct make_signature<R (Args...), Named...>
        {
          using type = R (decltype(make_signature_arg<Args, Named>(0)) ...);
        };
      }

      template <typename F, typename ... Args>
      Function<
        typename make_signature<std::get_signature<F>,
                                std::remove_cv_reference_t<Args>...>::type>
      function(F f, Args&& ... args)
      {
        return Function<
          typename make_signature<
            std::get_signature<F>,
            std::remove_cv_reference_t<Args>...>::type>(
              std::move(f), std::forward<Args>(args)...);
      }
    }
  }
}
