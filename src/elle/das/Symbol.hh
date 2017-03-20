#pragma once

#include <string>

#include <elle/printf-fwd.hh>

namespace elle
{
  namespace das
  {
    class Symbol
    {};

    /// Reduce symbol sizes.
    ///
    /// Symbol size can get so big that some compiler (mingw5.8 at least) fail.
    template <typename S>
    class SS;

    template <typename S>
    using SpecificSymbol = SS<S>;

    /// A symbol, a class with introspection capabilities.
    ///
    /// Symbols are the base of Das, that uses symbol-based meta-programming.
    ///
    /// Use ELLE_DAS_SYMBOL(name) to create a Symbol.
    ///
    /// \code{.cc}
    /// namespace symbols
    /// {
    ///   ELLE_DAS_SYMBOL(foo);
    ///   ELLE_DAS_SYMBOL(baz);
    /// }
    ///
    /// struct S
    /// {
    ///   S()
    ///     : foo(5)
    ///   {}
    ///   int foo;
    ///
    ///   void
    ///   baz()
    ///   {
    ///     return 3;
    ///   }
    /// };
    ///
    /// assert(decltype(symbols::foo)::attr_has<S>());
    /// assert(!decltype(symbols::baz)::attr_has<S>());
    /// assert(!decltype(symbols::baz)::method_has<S>());
    /// S s;
    /// assert(symbols::foo.attr_get(s) == 5);
    /// assert(++symbols::foo.attr_get(s) == 6);
    /// assert(symbols::baz.method_call(s) == 3);
    ///
    /// \endcode
    template <typename S>
    class SS
      : public Symbol
    {
    public:
      using Symbol = S;

      template <typename T>
      struct Formal
      {
        using Type = T;
        using Symbol = S;
      };

      /// A valued symbol
      ///
      /// @param E The value type.
      /// @param P The type through which the value was passed.
      /// @param T XXX[doc].
      template <typename E, typename P, typename T = E>
      struct Effective
      {
        using Type = T;
        using Default = E;
        using Formal = S;
        using Passing = P;
        using Symbol = S;
        E value;

        template <typename ER, typename PR>
        using rebind = Effective<ER, PR>;

        template <typename OE, typename OP>
        Effective(Effective<OE, OP>&& e)
          : value(std::forward<OP>(e.value))
        {}

        Effective(Effective const& e)
          : value(e.value)
        {}

        Effective(Passing v)
          : value(std::forward<Passing>(v))
        {}

        using ByConstRef = Effective<E, std::remove_cv_reference_t<P> const&>;
        using ByRef = Effective<E, std::remove_cv_reference_t<P>&>;
        using ByRValueRef = Effective<E, std::remove_cv_reference_t<P>&&>;
        using ByValue = Effective<E, std::remove_cv_reference_t<P>>;

        static
        std::string
        name()
        {
          return S::name();
        }

        friend
        std::ostream&
        operator<<(std::ostream& out, Effective const& e)
        {
          elle::fprintf(out, "%s = %s", e.name(), e.value);
          return out;
        }
      };

      template <typename E>
      Effective<E, E&&>
      operator =(E&& v) const
      {
        return Effective<E, E&&>{std::forward<E>(v)};
      }
    };
  }
}

#include <elle/das/Symbol.hxx>
