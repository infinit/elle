#ifndef ELLE_WITH_HH
# define ELLE_WITH_HH

namespace elle
{
  template <typename T>
  class With
  {
  public:
    template <typename ... Args>
    With(Args&&... args);
    template <typename F>
    void operator <<(F const& action);

  private:
    char _value[sizeof(T)];
    bool _used;
  };
}

# include <elle/With.hxx>

#endif
