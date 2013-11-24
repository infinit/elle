// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_DETAIL_URI_PARSE_INC
#define NETWORK_DETAIL_URI_PARSE_INC

#include <network/uri/uri.hpp>

namespace network {
  namespace detail {
    bool parse_authority(uri::string_type::iterator first,
                         uri::string_type::iterator last,
                         boost::optional<uri::string_type> &user_info,
                         boost::optional<uri::string_type> &host,
                         boost::optional<uri::string_type> &port);
  } // namespace detail
} // namespace network

#endif // NETWORK_DETAIL_URI_PARSE_INC
