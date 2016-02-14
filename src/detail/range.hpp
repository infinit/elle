// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_RANGE_INC
#define NETWORK_DETAIL_RANGE_INC

#include <algorithm>
#include <iterator>
#include <utility>

namespace network {
namespace detail {
template <class Rng, class Pred>
inline void for_each(Rng& rng, Pred&& pred) {
  std::for_each(std::begin(rng), std::end(rng), pred);
}

template <class Rng, class Iter, class Pred>
inline void transform(const Rng& rng, Iter out, Pred&& pred) {
  std::transform(std::begin(rng), std::end(rng), out, pred);
}

template <class Rng>
inline typename Rng::size_type distance(const Rng& rng) {
  return std::distance(std::begin(rng), std::end(rng));
}

template <class Rng1, class Rng2>
inline bool equal(const Rng1& rng1, const Rng2 &rng2) {
  return std::equal(std::begin(rng1), std::end(rng1), std::begin(rng2));
}
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_RANGE_INC
