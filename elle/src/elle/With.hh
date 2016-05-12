#ifndef ELLE_WITH_HH
# define ELLE_WITH_HH

# include <utility>

# include <elle/compiler.hh>

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
    ELLE_COMPILER_ALIGN(128)
    char _data[sizeof(T)];
    bool _used;
    T* _value;

  /*--------.
  | Running |
  `--------*/
  public:
    template <typename F>
    auto
    operator <<(F const& action) -> decltype(action(std::declval<T&>()));

    template <typename F>
    auto
    operator <<(F const& action) -> decltype(action());

  private:
    template <typename F>
    auto
    _run(F const& action) -> decltype(action(std::declval<T&>()));
  };
}

# include <elle/With.hxx>

#endif
