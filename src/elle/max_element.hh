#pragma once

#include <boost/range/algorithm/max_element.hpp>

#include <elle/BoolIterator.hh>

namespace elle
{
  template<class ForwardRange>
  auto
  max_element(ForwardRange&& rng)
  {
    auto it = boost::max_element(std::forward<ForwardRange>(rng));
    return BoolIterator<decltype(it)>(it, it != end(rng));
  }

  template<class ForwardRange, class BinaryPredicate>
  auto
  max_element(ForwardRange&& rng, BinaryPredicate const& pred)
  {
    auto it = boost::max_element(std::forward<ForwardRange>(rng), pred);
    return BoolIterator<decltype(it)>(it, it != end(rng));
  }
}
