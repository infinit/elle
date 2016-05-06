// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_parse_authority.hpp"

namespace network {
namespace detail {
bool parse_authority(uri::string_type &str,
                     network_boost::optional<uri::string_type> &user_info,
                     network_boost::optional<uri::string_type> &host,
                     network_boost::optional<uri::string_type> &port) {
  return false;
}
}  // namespace detail
}  // namespace network
