#ifndef DAS_SYMBOL_HH
# define DAS_SYMBOL_HH

namespace das
{
  class Symbol
  {};

  template <typename S>
  class SpecificSymbol
    : public Symbol
  {
  public:
    template <typename E>
    struct Effective
    {
      using Type = E;
      using Formal = S;
      Type value;
    };

    template <typename E>
    Effective<E&&>
    operator =(E&& v) const
    {
      return Effective<E&&>{std::forward<E>(v)};
    }
  };
}

# include <das/Symbol.hxx>

#endif
