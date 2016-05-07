// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARTS_INC
#define NETWORK_DETAIL_URI_PARTS_INC

#include <string>
#include <utility>
#include "../boost/range/iterator_range.hpp"
#include "../boost/optional.hpp"
#include <network/optional.hpp>
#include <network/string_view.hpp>

namespace network {
namespace detail {
using iterator_pair = network_boost::iterator_range<std::string::iterator>;

struct hierarchical_part {
  network_boost::optional<iterator_pair> user_info;
  network_boost::optional<iterator_pair> host;
  network_boost::optional<iterator_pair> port;
  network_boost::optional<iterator_pair> path;
};

struct uri_parts {
  network_boost::optional<iterator_pair> scheme;
  hierarchical_part hier_part;
  network_boost::optional<iterator_pair> query;
  network_boost::optional<iterator_pair> fragment;
};

namespace v2 {
using iterator_pair = std::pair<uri::const_iterator, uri::const_iterator>;

struct hierarchical_part {
  optional<iterator_pair> user_info;
  optional<iterator_pair> host;
  optional<iterator_pair> port;
  optional<iterator_pair> path;
};

struct uri_parts {
  optional<iterator_pair> scheme;
  hierarchical_part hier_part;
  optional<iterator_pair> query;
  optional<iterator_pair> fragment;
};
}
} // namespace detail
} // namespace network

#endif  // NETWORK_DETAIL_URI_PARTS_INC
