#pragma once

#include <utility>

#include <elle/compiler.hh>

namespace elle
{
  /// Destructor throwing guard.
  ///
  /// elle::With helps in situations where a destructor can throw an exception.
  ///
  /// Quote: "https://akrzemi1.wordpress.com/2011/09/21/destructors-that-throw"
  ///
  /// It is impossible to automatically detect in a destructor if it was called
  /// due to a throw from its scope or due to the normal scope exit.
  ///
  /// That's why elle::With takes the responsibility for destroying the object,
  /// by explicitly calling its destructor.
  ///
  /// We can distinguish 4 cases.
  /// - Neither the function nor the destructor threw. In that case, do nothing
  ///   specific.
  /// - The function threw, the destructor didn't. In that case, the exception
  ///   is rethrown normally.
  /// - The destructor threw but the function didn't. In that case, the
  ///   exception is also rethrown.
  /// - If both threw, we chose to overwrite the exception, rethrowing only
  ///   the exception thrown by the destructor. Unfortunately, the exception
  ///   cannot be nested (because of the missing information of its concrete
  ///   type), and, therefore is lost.
  ///
  /// @code{.cc}
  ///
  /// struct DThrower
  /// {
  ///   DThrower(bool destructor_throws)
  ///     : _destructor_throws(destructor_throws)
  ///   {}
  ///
  ///   ~DThrower() noexcept(false)
  ///   {
  ///     if (_destructor_throws)
  ///       throw 1;
  ///   };
  ///
  ///   bool _destructor_throws;
  /// };
  ///
  /// void
  /// test(bool body_throws, bool destructor_throws)
  /// {
  ///   try
  ///   {
  ///     elle::With<DThrower>(destructor_throws) << [&] (DThrower& d)
  ///       {
  ///         throw body_throws ? 2 : 3;
  ///       };
  ///   }
  ///   catch (int value)
  ///   {
  ///     std::cout << value << '\n';
  ///   }
  ///   elle::unreachable();
  /// }
  ///
  /// test(false, false); // Prints 3
  /// test(true, false);  // Prints 2
  /// test(false, true);  // Prinst 1
  /// test(true, true);   // Prints 1
  ///
  /// @endcode
  template <typename T>
  class With
  {
  /*------.
  | Types |
  `------*/
  public:
    using Self = With<T>;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    template <typename ... Args>
    /// Create a With<T>.
    With(Args&&... args);
    With(With<T>&& model);
    ~With();
  private:
    ELLE_COMPILER_ALIGN(alignof(T))
    char _data[sizeof(T)];
    bool _used;
    T* _value;

  /*--------.
  | Running |
  `--------*/
  public:
    /// "Insert" an action to the With.
    ///
    /// @param A function.
    /// @returns The value returned by the function.
    template <typename F>
    auto
    operator <<(F const& action) -> decltype(action(std::declval<T&>()));
    /// "Insert" an action to the With.
    ///
    /// @param A function.
    /// @returns The value returned by the function.
    template <typename F>
    auto
    operator <<(F const& action) -> decltype(action());

  private:
    static
    ELLE_COMPILER_ATTRIBUTE_NO_INLINE
    void
    destroy_it(T* ptr);
    template <typename F>
    auto
    _run(F const& action) -> decltype(action(std::declval<T&>()));
  };
}

#include <elle/With.hxx>
