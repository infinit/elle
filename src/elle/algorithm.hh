#pragma once

#include <algorithm>
#include <map>
#include <unordered_map>

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>
#include <boost/algorithm/string/predicate.hpp> // starts_with
#include <boost/optional.hpp>

#include <elle/random.hh> // random_engine.

namespace elle
{
  /// Whether @a e is in @a c.
  template <typename Container, typename T>
  bool
  contains(Container const& c, T const& e)
  {
    return ::boost::algorithm::any_of_equal(c, e);
  }

  /// Whether @a e is in @a c.
  template <typename... Args, typename T>
  bool
  contains(std::unordered_map<Args...> const& c, T const& e)
  {
    return c.find(e) != c.end();
  }

  /// Whether @a e is in @a v.
  template <typename... Args, typename T>
  bool
  contains(std::map<Args...> const& c, T const& e)
  {
    return c.find(e) != c.end();
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

  /// Copy-append the contents of a vector.
  ///
  /// @return iterator to the first inserted value.
  ///
  /// http://stackoverflow.com/a/37210097/1353549
  template <typename T>
  typename std::vector<T>::iterator
  push_back(std::vector<T>& dest, std::vector<T> const& src)
  {
    if (dest.empty())
    {
      dest = src;
      return std::begin(dest);
    }
    else
      return dest.insert(std::end(dest), std::begin(src), std::end(src));
  }

  /// Copy-append values to a vector.
  ///
  /// @return iterator to the first inserted value.
  template <typename T, typename Arg, typename... Args>
  auto
  push_back(std::vector<T>& dest, Arg&& arg, Args&&... args)
    -> typename std::vector<T>::iterator
  {
    dest.emplace_back(std::forward<Arg>(arg));
    auto res = std::prev(dest.end());
    using swallow = int[];
    (void) swallow
      {
        (dest.emplace_back(std::forward<Args>(args)),
         0)...
      };
    return res;
  }

  /// Move-append the contents of a vector.
  ///
  /// @return iterator to the first inserted value.
  ///
  /// http://stackoverflow.com/a/37210097/1353549
  template <typename T>
  typename std::vector<T>::iterator
  push_back(std::vector<T>& dest, std::vector<T>&& src)
  {
    if (dest.empty())
    {
      dest = std::move(src);
      return std::begin(dest);
    }
    else
      return dest.insert(std::end(dest),
                         std::make_move_iterator(std::begin(src)),
                         std::make_move_iterator(std::end(src)));
  }

  /// Reorder `r` randomly.
  ///
  /// Does not appear to be offered by Boost.Range.
  template <typename Range, typename URBG>
  auto shuffle(Range& r, URBG&& g)
    -> Range&
  {
    std::shuffle(std::begin(r), std::end(r), std::forward<URBG>(g));
    return r;
  }

  /// Reorder `r` randomly using elle's random engine singleton.
  ///
  /// Does not appear to be offered by Boost.Range.
  template <typename Range>
  auto shuffle(Range& r)
    -> Range&
  {
    return shuffle(r, random_engine());
  }

  /// If `range` starts with `prefix`, the remainder of the range,
  /// otherwise none.
  template <typename Range, typename Prefix>
  boost::optional<Range>
  tail(Range const& range, Prefix const& prefix)
  {
    using std::begin;
    using std::end;
    using boost::size;
    if (boost::starts_with(range, prefix))
      return Range{begin(range) + prefix.size(), end(range)};
    else
      return {};
  }

  template <typename Range>
  boost::optional<Range>
  tail(Range const& range, char const prefix[])
  {
    return tail(range, std::string{prefix});
  }
}
