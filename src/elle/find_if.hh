#pragma once

#include <boost/range/algorithm/find_if.hpp>

#include <elle/BoolIterator.hh>

namespace elle
{
  template <typename SinglePassRange, typename UnaryPredicate>
  auto
  find_if(SinglePassRange&& rng, UnaryPredicate const& pred)
  {
    auto it = boost::find_if(std::forward<SinglePassRange>(rng), pred);
    return BoolIterator<decltype(it)>(it, it != end(rng));
  }
}
