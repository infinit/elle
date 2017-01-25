#pragma once

#include <elle/Exception.hh>

namespace elle
{
  /// Exception for runtime errors.
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
    Error(std::string const& message);
    Error(elle::Backtrace const& bt, std::string const& message);

  /*--------------.
  | Serialization |
  `--------------*/
  public:
    Error(elle::serialization::SerializerIn& input);
  };
}
