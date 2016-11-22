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
    template <typename E, typename Passing>
    struct Effective
    {
      using Type = E;
      using Formal = S;
      Type value;

      Effective(Effective&& e)
        : value(std::forward<Passing>(e.value))
      {}

      Effective(Passing v)
        : value(std::forward<Passing>(v))
      {}

      static inline
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

# include <das/Symbol.hxx>

#endif
