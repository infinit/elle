// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARTS_INC
#define NETWORK_DETAIL_URI_PARTS_INC

#include <string>
#include <utility>
#include <iterator>
#include <network/optional.hpp>
#include <network/string_view.hpp>

namespace network {
namespace detail {
struct uri_part {

  typedef string_view::value_type value_type;
  typedef string_view::iterator iterator;
  typedef string_view::const_iterator const_iterator;

  uri_part() {}

  uri_part(const_iterator first, const_iterator last)
      : first(first), last(last) {}

  const_iterator begin() const {
    return first;
  }

  const_iterator end() const {
    return last;
  }

  bool empty() const {
    return first == last;
  }

  std::string to_string() const {
    return std::string(first, last);
  }

private:

  const_iterator first, last;

};


struct hierarchical_part {
  optional<uri_part> user_info;
  optional<uri_part> host;
  optional<uri_part> port;
  optional<uri_part> path;
};

struct uri_parts {
  optional<uri_part> scheme;
  hierarchical_part hier_part;
  optional<uri_part> query;
  optional<uri_part> fragment;
};

// With the parser, we use string_views, which are mutable. However,
// there are times (e.g. during normalization), when we want a part
// to be mutable. This function returns a pair of
// std::string::iterators in the same range as the URI part.
//
inline std::pair<std::string::iterator, std::string::iterator> mutable_part(
    std::string &str, uri_part part) {
  auto view = string_view(str);

  auto first_index = std::distance(std::begin(view), std::begin(part));
  auto first = std::begin(str);
  std::advance(first, first_index);

  auto last_index = std::distance(std::begin(view), std::end(part));
  auto last = std::begin(str);
  std::advance(last, last_index);

  return std::make_pair(first, last);
}

// This is a convenience function that converts a part of a
// std::string to a string_view.
inline string_view to_string_view(const std::string &uri,
                                  uri_part part) {
  if (!part.empty()) {
    const char *c_str = uri.c_str();
    const char *part_begin = &(*(std::begin(part)));
    std::advance(c_str, std::distance(c_str, part_begin));
    return string_view(c_str, std::distance(std::begin(part), std::end(part)));
  }
  return string_view();
}
} // namespace detail
} // namespace network

#endif  // NETWORK_DETAIL_URI_PARTS_INC
