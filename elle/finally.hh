#ifndef ELLE_FINALLY_HH
# define ELLE_FINALLY_HH

# include <functional>
# include <cstdlib>

# include <boost/preprocessor/cat.hpp>

/// Provide a lambda-based skeleton for creating Finally instances based
/// on the name of a variable.
# define ELLE_FINALLY_LAMBDA(_variable_, _lambda_)                      \
  auto BOOST_PP_CAT(_elle_finally_lambda_, __LINE__) = _lambda_;        \
  elle::Finally BOOST_PP_CAT(_elle_finally_variable_, _variable_)(      \
    std::bind(BOOST_PP_CAT(_elle_finally_lambda_, __LINE__), _variable_));

/// Make it super easy to abort the final action based on the name of
/// the variable it relates to.
# define ELLE_FINALLY_ABORT(_variable_)                                 \
  BOOST_PP_CAT(_elle_finally_variable_, _variable_).abort();

/// Make it extremely simple to call delete on a pointer when leaving the
/// variable's scope.
# define ELLE_FINALLY_DELETE(_variable_)                                \
  ELLE_FINALLY_LAMBDA(                                                  \
    _variable_,                                                         \
    [] (decltype(_variable_) pointer) { delete pointer; });

/// Make it extremely simple to call free on a pointer when leaving a scope.
# define ELLE_FINALLY_FREE(_variable_)                                  \
  ELLE_FINALLY_LAMBDA(                                                  \
    _variable_,                                                         \
    [] (void* pointer) { ::free(pointer); });

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
