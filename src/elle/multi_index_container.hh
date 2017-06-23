#pragma once

#include <boost/multi_index_container.hpp>

#include <elle/range.hh>

namespace elle
{
  /// Whether \a e is in \a c.
  template <typename... Args, typename T>
  bool
  contains(boost::multi_index::multi_index_container<Args...> const& c, T const& e)
  {
    return c.find(e) != c.end();
  }

  /// The range of values with equal key.
  template <typename... Args, typename T>
  auto
  equal_range(boost::multi_index::detail::hashed_index<Args...> const& c, T const& e)
  {
    return as_range(c.equal_range(e));
  }

  /// The range of values with equal key.
  template <typename... Args, typename T>
  auto
  equal_range(boost::multi_index::multi_index_container<Args...> const& c, T const& e)
  {
    return as_range(c.equal_range(e));
  }
}

namespace boost
{
  namespace multi_index
  {
    /// Pretty-printing.
    template <typename... Args>
    std::ostream&
    operator <<(std::ostream& out, multi_index_container<Args...> const& s)
    {
      auto const format = is_fixed(out) ? "%s%f" : "%s%s";
      out << "BMI{";
      auto* sep = "";
      for (auto const& e: s)
      {
        elle::fprintf(out, format, sep, e);
        sep = ", ";
      }
      out << '}';
      return out;
    }
  }
}
