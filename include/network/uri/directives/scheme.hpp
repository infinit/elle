// Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_DIRECTIVES_SCHEME_INC
#define NETWORK_URI_DIRECTIVES_SCHEME_INC

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <network/uri/schemes.hpp>

namespace network {
struct scheme_directive {

    explicit scheme_directive(const std::string &scheme)
        : scheme(scheme)
    {}

    template <
        class Uri
        >
    void operator () (Uri &uri) const {
        uri.append(scheme);
        if (opaque_schemes::exists(scheme)) {
            uri.append(":");
        }
        else {
            uri.append("://");
        }
    }

    std::string scheme;

};

inline
scheme_directive scheme(const std::string &scheme) {
    return scheme_directive(scheme);
}

namespace schemes {
inline
uri &http(uri &uri_) {
    return uri_ << scheme("http");
}

inline
uri &https(uri &uri_) {
    return uri_ << scheme("https");
}

inline
uri &file(uri &uri_) {
    return uri_ << scheme("file");
}
} // namespace schemes
} // namespace network

#endif // NETWORK_URI_DIRECTIVES_SCHEME_INC
