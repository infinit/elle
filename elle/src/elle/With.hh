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
    With(With<T>&& model);
    ~With();
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

    template <typename F>
    auto
    operator <<(F const& action) -> decltype(action());

  private:
    template <typename F>
    auto
    _run(F const& action) -> decltype(action(*(T*)(nullptr)));
  };
}

# include <elle/With.hxx>

#endif
