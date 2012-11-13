#ifndef ELLE_FINALLY_HH
# define ELLE_FINALLY_HH

# include <functional>

# include <stdlib.h>

# include <boost/preprocessor/cat.hpp>

/// Make it easy to bind a function with its arguments to the final action.
# define ELLE_FINALLY_FUNCTION(_name_, _function_, ...)                 \
  elle::Finally _name_(std::bind(_function_, ##__VA_ARGS__));

/// Make it extremely simple to call free on a pointer when leaving a scope.
# define ELLE_FINALLY_FREE(_name_, _pointer_)                           \
  ELLE_FINALLY_FUNCTION(_name_, ::free, _pointer_);

/// Make it simple to bind a lambda function to the final action.
# define ELLE_FINALLY_LAMBDA(_name_, _lambda_)                          \
  auto BOOST_PP_CAT(_elle_finally_lambda_, __LINE__) = _lambda_;        \
  ELLE_FINALLY_FUNCTION(_name_,                                         \
                        BOOST_PP_CAT(_elle_finally_lambda_, __LINE__));

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
      // Is the action still valid.
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
