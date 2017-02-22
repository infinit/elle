#pragma once

#include <memory>

namespace elle
{
  template <typename Cloned>
  class Clonable
  {
  public:
    std::unique_ptr<Cloned>
    clone() const = 0;
  protected:
    using _elle_Cloned = Cloned;
  };
}

#define ELLE_CLONABLE()                                                 \
  std::unique_ptr<typename Self::_elle_Cloned>                          \
  clone() const override                                                \
  {                                                                     \
    return                                                              \
      std::unique_ptr<typename Self::_elle_Cloned>(new Self(*this));    \
  }
