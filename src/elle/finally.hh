#ifndef ELLE_FINALLY_HH
# define ELLE_FINALLY_HH

# include <functional>
# include <cstdlib>

# include <boost/preprocessor/cat.hpp>

# include <elle/With.hh>
# include <elle/attribute.hh>

namespace elle
{
  /// An action to be finally executed.
  ///
  /// This is a cheap alternative to the missing if .. else .. finally.
  ///
  /// This execute the given Action when the SafeFinally is destroyed. Safe
  /// stands for the action must no throw an exception, I repeat:
  ///
  /// "THE ACTION MUST NO THROW AN EXCEPTION.".
  ///
  /// If it can be the case, use Finally, coupled with a With.
  ///
  /// @see Finally.
  ///
  /// \code{.cc}
  ///
  /// void
  /// foo(int& i)
  /// {
  ///   elle::SafeFinally finally_increment([&i] { return ++i });
  ///
  ///   {
  ///     elle::SafeFinally finally_to_abort([] { assert(false); });
  ///     i *= 2;
  ///     // The finally won't be executed.
  ///     finally_to_abort.abort();
  ///   }
  /// }
  ///
  /// int i = 5;
  /// foo(i);
  /// std::cout << i;
  ///
  /// // Result: 11
  ///
  /// \endcode
  class ELLE_API SafeFinally
  {
    using Action = std::function<void ()>;
  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Create a noop SafeFinally.
    SafeFinally();
    /// Create a SafeFinally that run the Action when it's destroyed.
    ///
    /// @param action The Action to run.
    SafeFinally(Action const& action);
    /// Destroy a SafeFinally.
    ///
    /// If not aborted, the Action will be run.
    ~SafeFinally();
    SafeFinally(SafeFinally&& f) = delete;
    SafeFinally(SafeFinally const& f) = delete;

  /*--------.
  | Methods |
  `--------*/
  public:
    /// Abort the current SafeFinally, preventing the Action to be executed.
    void
    abort();

    /// Return whether the SafeFinally has been aborted.
    bool
    aborted() const;
  /*-----------.
  | Attributes |
  `-----------*/
  private:
    ELLE_ATTRIBUTE_RW(Action, action);
  };

  /// Provide a way for a final action to be performed i.e when leaving the
  /// current scope.
  ///
  /// Note that a method is provided (i.e abort()) for cancelling this final
  /// action.
  ///
  /// \code{.cc}
  ///
  /// try
  /// {
  ///   elle::With<elle::Finally>([&] { throw 3; }) << [] (elle::Finally& f)
  ///   {
  ///     // Do stuff.
  ///   };
  /// }
  /// catch (int i)
  /// {
  ///   std::cout << i;
  /// }
  ///
  /// // Result: 3
  ///
  /// \endcode.
  class ELLE_API Finally
  {
  /*-------------.
  | Construction |
  `-------------*/
  protected:
    /// Create a noop Finally.
    Finally();
    /// Create a Finally that run the Action when it's destroyed.
    ///
    /// @param action The Action to be finally executed.
    Finally(std::function<void()> const& action);
    ~Finally() noexcept(false);
    /// Let With manage us.
    template<typename> friend class elle::With;

  /*--------.
  | Methods |
  `--------*/
  public:
    /// Abort the current SafeFinally, preventing the Action to be executed.
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
