#pragma once

#include <functional>

#include <boost/container/flat_set.hpp>

namespace elle
{
  /// Whether \a e is member of \a s.
  template <typename... Args, typename T>
  bool
  contains(const boost::container::flat_set<Args...>& s,
           const T& e)
  {
    return s.find(e) != s.end();
  }

  template <typename... Args>
  void
  hash_combine(std::size_t& seed,
               boost::container::flat_set<Args...> const& s)
  {
    for (auto const& e: s)
      boost::hash_combine(seed, hash_value(e));
  }
}

/// Boost compliance.
namespace boost
{
  namespace container
  {
    template <typename... Args>
    std::size_t
    hash_value(boost::container::flat_set<Args...> const& s)
    {
      std::size_t res = 0;
      elle::hash_combine(res, s);
      return res;
    }
  }
}

/// std compliance.
namespace std
{
  template <typename... Args>
  struct hash<boost::container::flat_set<Args...>>
  {
    using Type = boost::container::flat_set<Args...>;
    size_t operator()(Type const& v) const
    {
      return boost::hash_value(v);
    }
  };
}
