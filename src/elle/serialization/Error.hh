#pragma once

#include <elle/Error.hh>

namespace elle
{
  namespace serialization
  {
    class ELLE_API Error
      : public elle::Error
    {
    public:
      Error(std::string const& message);
      Error(SerializerIn& input);
    };
  }
}
