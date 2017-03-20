#pragma once

#include <algorithm>
#include <vector>

namespace elle
{
  /// This is really the `map` function in functional languages:
  /// a vector of the values of `fun(e)` for e in c.
  ///
  /// @param A container to apply the function to.
  /// @param The function to apply.
  /// @returns A vector containing the result of the map.
  template <typename Cont, typename Fun>
  auto
  make_vector(const Cont& c, Fun&& fun)
    -> std::vector<decltype(fun(*c.begin()))> // SFINAE
  {
    using Res = decltype(fun(*c.begin()));
    auto res = std::vector<Res>{};
    res.reserve(c.size());
    std::transform(c.begin(), c.end(),
                   std::back_inserter(res),
                   std::forward<Fun>(fun));
    return res;
  }

  /// The content of \a cont as a vector.
  ///
  /// @param A container.
  /// @results A vector.
  template <typename Cont>
  auto
  make_vector(const Cont& c)
    -> std::vector<typename Cont::value_type>
  {
    using std::begin;
    using std::end;
    return {begin(c), end(c)};
  }
}
