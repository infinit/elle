// Copyright 2013-2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_NORMALIZE_INC
#define NETWORK_DETAIL_NORMALIZE_INC

#include <network/uri/uri.hpp>

namespace network {
namespace detail {
uri::string_type normalize_path_segments(uri::string_view path);

uri::string_type normalize_path(uri::string_view path,
                                uri_comparison_level level);
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_NORMALIZE_INC
