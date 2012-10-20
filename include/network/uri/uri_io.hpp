// Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_URI_IO_INC
#define NETWORK_URI_URI_IO_INC

#include <network/uri/uri.hpp>

namespace network {
inline
std::ostream &operator << (std::ostream &os, const uri &uri_) {
    return os << uri_.string();
}
} // namespace network

#endif // NETWORK_URI_URI_IO_INC
