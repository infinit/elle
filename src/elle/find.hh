#pragma once

#include <elle/BoolIterator.hh>

namespace elle
{
  template <typename C, typename E>
  auto
  find(C& c, E const& e)
  {
    auto it = c.find(e);
    return BoolIterator<decltype(it)>(it, it != c.end());
  }
}
