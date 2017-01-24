#pragma once

#include <elle/Exception.hh>

namespace elle
{
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
    Exit(int return_code);
    ELLE_ATTRIBUTE_R(int, return_code);
  };
}
