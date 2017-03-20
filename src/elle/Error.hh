#pragma once

#include <elle/Exception.hh>

namespace elle
{
  /// Exception for runtime errors.
  ///
  /// Type of Exception representing errors.
  class ELLE_API Error
    : public Exception
  {
    /*------.
    | Types |
    `------*/
  public:
    using Self = Error;
    using Super = Exception;

    /*-------------.
    | Construction |
    `-------------*/
  public:
    /// Construct an Error.
    ///
    /// @see Exception::Exception.
    Error(elle::Backtrace const& bt, std::string const& message);
    /// Construct an Error.
    ///
    /// Skip the first StackFrame of the Backtrace (to ignore the Construction
    /// of the Error itself).
    ///
    /// @param message An explanatory string.
    Error(std::string const& message);

    /*--------------.
    | Serialization |
    `--------------*/
  public:
    /// Construct an Error from a Serializer.
    ///
    /// @see Exception::Exception(serialization::SerializerIn&);
    Error(elle::serialization::SerializerIn& input);
  };
}
