#pragma once

#include <iterator>
#include <ostream>

#include <elle/printf-fwd.hh>

namespace elle
{
  namespace detail
  {
    /// A pair of iterators complying with the range-based for loop
    /// protocol.
    template <typename It>
    struct range
    {
      using iterator = It;
      using const_iterator = It;
      using value_type = typename std::iterator_traits<iterator>::value_type;
      auto begin() const { return _begin; }
      auto end()   const { return _end; }
      iterator _begin, _end;
    };

    // FIXME: code duplication for these routines.
    template <typename It>
    std::ostream&
    operator << (std::ostream& out, range<It> const& r)
    {
      auto const format = is_fixed(out) ? "%s%f" : "%s%s";
      out << '[';
      auto* sep = "";
      for (auto const& e: r)
      {
        elle::fprintf(out, format, sep, e);
        sep = ", ";
      }
      out << ']';
      return out;
    }
  }

  /// Turn a pair of iterator into a for-loop compatible range.
  ///
  /// Especially useful with the return type of std::equal_range.
  template <typename It>
  detail::range<It>
  as_range(const std::pair<It, It> &p)
  {
    return {p.first, p.second};
  }
}
