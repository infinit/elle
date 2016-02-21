// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARTS_INC
#define NETWORK_DETAIL_URI_PARTS_INC

#include <string>
#include <boost/range/iterator_range.hpp>
#include <boost/optional.hpp>

namespace network {
namespace detail {
using iterator_pair = boost::iterator_range<std::string::iterator>;

struct hierarchical_part {
  boost::optional<iterator_pair> user_info;
  boost::optional<iterator_pair> host;
  boost::optional<iterator_pair> port;
  boost::optional<iterator_pair> path;
};

struct uri_parts {
  boost::optional<iterator_pair> scheme;
  hierarchical_part hier_part;
  boost::optional<iterator_pair> query;
  boost::optional<iterator_pair> fragment;
};
} // namespace detail
} // namespace network

#endif  // NETWORK_DETAIL_URI_PARTS_INC
