#ifndef ELLE_FINALLY_HH
# define ELLE_FINALLY_HH

namespace elle
{
  class Finally
  {
  public:
    Finally(std::function<void()> const& action)
      : _action(action)
    {}

    ~Finally()
    {
      if (_action)
        _action();
    }

    void
    release()
    {
      _action = std::function<void()>();
    }

  private:
    std::function<void()> _action;
  };
}

#endif
