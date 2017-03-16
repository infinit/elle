#pragma once

#include <functional>

#include <boost/functional/hash.hpp>

namespace elle
{
  /// Combine the hash of a number.
  inline
  void
  hash_combine(std::size_t& s, unsigned long v)
  {
    boost::hash_combine(s, boost::hash_value(v));
  }

  /// Combine the hash of a string.
  inline
  void
  hash_combine(std::size_t& s, std::string const& v)
  {
    boost::hash_combine(s, boost::hash_value(v));
  }

  /// Combine the hashes of a range.
  template <typename FwdIter>
  void
  hash_combine(std::size_t& s, FwdIter begin, FwdIter end)
  {
    for (; begin != end; ++begin)
      hash_combine(s, *begin);
  }

  /// Combine the hashes of a std::array.
  template <typename T, size_t S>
  void
  hash_combine(std::size_t& s, std::array<T, S> const& a)
  {
    hash_combine(s, a.begin(), a.end());
  }
}
