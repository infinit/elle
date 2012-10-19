// Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_DIRECTIVES_USER_INFO_INC
#define NETWORK_URI_DIRECTIVES_USER_INFO_INC

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

namespace network {
struct user_info_directive {

    explicit user_info_directive(const std::string &user_info)
        : user_info(user_info)
    {}

    template <
        class Uri
        >
    void operator () (Uri &uri) const {
        uri.append(user_info);
        uri.append("@");
    }

    std::string user_info;

};

inline
user_info_directive user_info(const std::string &user_info)  {
    return user_info_directive(user_info);
}
} // namespace network

#endif // NETWORK_URI_DIRECTIVES_USER_INFO_INC
