#pragma once

#include <algorithm>

#include <elle/BoolIterator.hh>

namespace elle
{
  template <typename C, typename E>
  auto
  find(C& c, E const& e)
    -> BoolIterator<decltype(c.find(e))>
  {
    auto it = c.find(e);
    return BoolIterator<decltype(it)>(it, it != c.end());
  }

  template <typename C, typename F>
  auto
  find(C& c, F const& f)
    -> std::enable_if_t<
      sizeof(decltype(f(*std::begin(c)))) >= 0,
      BoolIterator<decltype(std::find_if(std::begin(c), std::end(c), f))>>
  {
    auto it = std::find_if(std::begin(c), std::end(c), f);
    return BoolIterator<decltype(it)>(it, it != c.end());
  }
}
