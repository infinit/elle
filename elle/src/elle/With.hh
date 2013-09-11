#ifndef ELLE_WITH_HH
# define ELLE_WITH_HH

namespace elle
{
  template <typename T>
  class With
  {
  /*------.
  | Types |
  `------*/
  public:
    typedef With<T> Self;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    template <typename ... Args>
    With(Args&&... args);
  private:
    char _value[sizeof(T)];
    bool _used;

  /*--------.
  | Running |
  `--------*/
  public:
    template <typename F>
    auto
    operator <<(F const& action) -> decltype(action(*(T*)(nullptr)));
  };
}

# include <elle/With.hxx>

#endif
