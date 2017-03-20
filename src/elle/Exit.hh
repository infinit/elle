#pragma once

#include <elle/Exception.hh>

namespace elle
{
  /// An specialized Exception representing the will of exiting a program.
  ///
  /// \code{.cc}
  ///
  /// static
  /// void
  /// foo()
  /// {
  ///   throw Exit(1);
  /// }
  ///
  /// int
  /// main()
  /// {
  ///   try
  ///   {
  ///     foo();
  ///   }
  ///   catch (Exit const& e)
  ///   {
  ///     std::cerr << "Program terminated with status " << e.return_code();
  ///               << "\n";
  ///     return e.return_code();
  ///   }
  ///   return 0;
  /// }
  ///
  /// \endcode
  class Exit
    : public Exception
  {
  /*------.
  | Types |
  `------*/
  public:
    using Super = Exception;
    using Self = Exit;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Construct an Exit Exception with a given return code.
    ///
    /// @parma return_code The return code.
    Exit(int return_code);
    ELLE_ATTRIBUTE_R(int, return_code);
  };
}
