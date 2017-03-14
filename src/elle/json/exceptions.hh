#pragma once

#include <elle/Error.hh>

namespace elle
{
  namespace json
  {
    class ParseError:
      public elle::Error
    {
    public:
      using Super = elle::Error;
      ParseError(std::string const& message);
    };
  }
}
