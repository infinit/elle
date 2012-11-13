#ifndef ELLE_FINALLY_HH
# define ELLE_FINALLY_HH

# include <functional>
# include <cstdlib>

# include <boost/preprocessor/cat.hpp>

/// Make it extremely simple to call free on a pointer when leaving a scope.
# define ELLE_FINALLY_FREE(_name_, _pointer_)                           \
  elle::Finally _name_(std::bind(::free, _pointer_));

namespace elle
{
  /// Provide a way for a final action to be performed i.e when leaving the
  /// current scope.
  ///
  /// Note that a method is provided (i.e abort()) for cancelling this final
  /// action.
  class Finally
  {
    /*-------------.
    | Construction |
    `-------------*/
  public:
    Finally(std::function<void()> const& action):
      _action(action)
    {}

    ~Finally()
    {
      if (this->_action)
        this->_action();
    }

    /*--------.
    | Methods |
    `--------*/
  public:
    void
    abort()
    {
      this->_action = std::function<void()>();
    }

    /*-----------.
    | Attributes |
    `-----------*/
  private:
    std::function<void()> _action;
  };
}

#endif
