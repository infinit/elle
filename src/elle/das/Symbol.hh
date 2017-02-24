#pragma once

#include <string>

namespace elle
{
  namespace das
  {
    class Symbol
    {};

    /// Reduce symbol sizes.
    template <typename S>
    class SS;

    template <typename S>
    using SpecificSymbol = SS<S>;

    template <typename S>
    class SS
      : public Symbol
    {
    public:
      template <typename T>
      struct Formal
      {
        using Type = T;
        using Symbol = S;
      };

      /** A valued symbol
       *
       *  @param E The value type.
       *  @param P The type through which the value was passed.
       */
      template <typename E, typename P>
      struct Effective
      {
        using Type = E;
        using Formal = S;
        using Passing = P;
        Type value;

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
