// Copyright 2013-2016 Glyn Matthews.
// Copyright 2013 Hannes Kamecke.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_RESOLVE_INC
#define NETWORK_DETAIL_RESOLVE_INC

#include <network/uri/uri.hpp>

namespace network {
namespace detail {
// implementation of http://tools.ietf.org/html/rfc3986#section-5.2.4
uri::string_type remove_dot_segments(uri::string_view input);

// implementation of http://tools.ietf.org/html/rfc3986#section-5.2.3
uri::string_type merge_paths(const uri &base, const uri &reference);
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_RESOLVE_INC
