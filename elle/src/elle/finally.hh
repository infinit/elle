#ifndef ELLE_FINALLY_HH
# define ELLE_FINALLY_HH

# include <functional>
# include <cstdlib>

# include <boost/preprocessor/cat.hpp>

# include <elle/With.hh>
# include <elle/attribute.hh>

namespace elle
{
  class SafeFinally
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    SafeFinally();
    SafeFinally(std::function<void()> const& action);
    ~SafeFinally();
    SafeFinally(SafeFinally&& f) = delete;
    SafeFinally(SafeFinally const& f) = delete;

  /*--------.
  | Methods |
  `--------*/
  public:
    void
    abort();

  /*-----------.
  | Attributes |
  `-----------*/
  private:
    ELLE_ATTRIBUTE_RW(std::function<void()>, action);
  };

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
  private:
    Finally();
    Finally(std::function<void()> const& action);
    ~Finally() noexcept(false);
    /// Let With manage us.
    friend class elle::With<Finally>;

  /*--------.
  | Methods |
  `--------*/
  public:
    void
    abort();

  /*-----------.
  | Attributes |
  `-----------*/
  private:
    ELLE_ATTRIBUTE_RW(std::function<void()>, action);
  };
}

#endif
