// Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_DIRECTIVES_AUTHORITY_INC
#define NETWORK_URI_DIRECTIVES_AUTHORITY_INC

namespace network {
struct authority_directive {

    explicit authority_directive(const std::string &authority)
        : authority(authority)
    {}

    template <
        class Uri
        >
    void operator () (Uri &uri) const {
        uri.append(authority);
    }

    std::string authority;

};

inline
authority_directive authority(const std::string &authority) {
    return authority_directive(authority);
}
} // namespace network

#endif // NETWORK_URI_DIRECTIVES_AUTHORITY_INC
