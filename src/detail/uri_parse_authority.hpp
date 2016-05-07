// Copyright 2013-2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARSE_AUTHORITY_INC
#define NETWORK_DETAIL_URI_PARSE_AUTHORITY_INC

#include <network/uri/uri.hpp>
#include <network/optional.hpp>
#include "../boost/optional.hpp"

namespace network {
namespace detail {
bool parse_authority(uri::string_type &str,
                     network_boost::optional<uri::string_type> &user_info,
                     network_boost::optional<uri::string_type> &host,
                     network_boost::optional<uri::string_type> &port);


bool parse_authority(uri::string_type &str,
                     optional<uri::string_type> &user_info,
                     optional<uri::string_type> &host,
                     optional<uri::string_type> &port);
}  // namespace detail
}  // namespace network

#endif // NETWORK_DETAIL_URI_PARSE_AUTHORITY_INC
