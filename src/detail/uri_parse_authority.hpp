// Copyright 2013-2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARSE_AUTHORITY_INC
#define NETWORK_DETAIL_URI_PARSE_AUTHORITY_INC

#include "uri_parts.hpp"

namespace network {
namespace detail {
bool parse_authority(std::string &str,
                     network_boost::optional<std::string> &user_info,
                     network_boost::optional<std::string> &host,
                     network_boost::optional<std::string> &port);

bool parse_authority(string_view::const_iterator &first,
                     string_view::const_iterator last,
                     optional<v2::iterator_pair> &user_info,
                     optional<v2::iterator_pair> &host,
                     optional<v2::iterator_pair> &port);
}  // namespace detail
}  // namespace network

#endif // NETWORK_DETAIL_URI_PARSE_AUTHORITY_INC
