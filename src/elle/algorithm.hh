#pragma once

#include <algorithm>
#include <map>
#include <unordered_map>

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/cxx11/none_of.hpp>

namespace elle
{
  /// Whether \a e is in \a c.
  template <typename Container, typename T>
  bool
  contains(Container const& c, T const& e)
  {
    return ::boost::algorithm::any_of_equal(c, e);
  }

  /// Whether \a e is in \a c.
  template <typename... Args, typename T>
  bool
  contains(std::unordered_map<Args...> const& c, T const& e)
  {
    return c.find(e) != c.end();
  }

  /// Whether \a e is in \a v.
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
}
