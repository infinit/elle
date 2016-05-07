// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARSE_SCHEME_INC
#define NETWORK_DETAIL_URI_PARSE_SCHEME_INC

#include <network/uri/uri.hpp>
#include "../uri_parts.hpp"

namespace network {
namespace detail {
bool parse_scheme(uri::const_iterator &it, uri::const_iterator last,
                  v2::uri_parts &parts);
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_URI_PARSE_SCHEME_INC
