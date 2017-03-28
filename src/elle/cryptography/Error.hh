#pragma once

#include <elle/Error.hh>

namespace elle
{
  namespace cryptography
  {
    /// An error triggered by a cryptographic operation.
    class Error:
      public elle::Error
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Error(std::string const& message);
      Error(elle::serialization::SerializerIn& input);
    };
  }
}
