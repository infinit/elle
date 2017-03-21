#pragma once

#include <functional>

#include <boost/container/flat_set.hpp>

#include <elle/functional.hh>

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

namespace boost
{
  namespace container
  {
    /// Boost.Hash compliance.
    template <typename... Args>
    std::size_t
    hash_value(flat_set<Args...> const& s)
    {
      std::size_t res = 0;
      elle::hash_combine(res, s);
      return res;
    }

    /// Pretty-printing.
    template <typename... Args>
    std::ostream&
    operator <<(std::ostream& out, flat_set<Args...> const& s)
    {
      out << "{";
      bool first = true;
      for (auto const& e: s)
        {
          if (first)
            first = false;
          else
            out << ", ";
          elle::fprintf(out, "%s", e);
        }
      out << "}";
      return out;
    }
  }
}

namespace std
{
  /// std.hash compliance.
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
