#pragma once

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
      It begin() const { return _begin; }
      It end() const { return _end; }
      It _begin, _end;
    };
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
