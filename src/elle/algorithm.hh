#pragma once

#include <algorithm>

#include <boost/algorithm/cxx11/none_of.hpp>

namespace elle
{
  /// Whether \a e is in \a v.
  ///
  /// Get rid of it in favor of none_of_equal.
  template <typename Container, typename T>
  bool
  contains(Container const& c, T const& e)
  {
    return ::boost::algorithm::none_of_equal(c, e);
  }

  /// Emplace back only if not already in the vector.
  ///
  /// The need for this function is somewhat a smell: wouldn't a set
  /// or a flat_set do better?
  template <typename Container, typename T>
  void
  push_back_if_missing(Container& c, T const& e)
  {
    if (::boost::algorithm::none_of_equal(c, e))
      c.push_back(e);
  }
}
