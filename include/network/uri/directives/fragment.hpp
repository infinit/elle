// Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_DIRECTIVES_FRAGMENT_INC__
#define NETWORK_URI_DIRECTIVES_FRAGMENT_INC__

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

namespace network {
struct fragment_directive {

    explicit fragment_directive(const std::string &fragment)
        : fragment(fragment)
    {}

    template <
        class Uri
        >
    void operator () (Uri &uri) const {
        uri.append("#");
        uri.append(fragment);
    }

    std::string fragment;

};

inline
fragment_directive fragment(const std::string &fragment)  {
    return fragment_directive(fragment);
}
} // namespace network

#endif // NETWORK_URI_DIRECTIVES_FRAGMENT_INC__
