// Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_DIRECTIVES_PORT_INC
#define NETWORK_URI_DIRECTIVES_PORT_INC


#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

namespace network {
struct port_directive {

    explicit port_directive(const std::string &port)
        : port(port)
    {}

    explicit port_directive(boost::uint16_t port)
        : port(boost::lexical_cast<std::string>(port))
    {}

    template <
        class Uri
        >
    void operator () (Uri &uri) const {
        uri.append(":");
        uri.append(port);
    }

    std::string port;

};

inline
port_directive port(const std::string &port)  {
    return port_directive(port);
}

inline
port_directive port(boost::uint16_t port) {
    return port_directive(port);
}
} // namespace network

#endif // NETWORK_URI_DIRECTIVES_PORT_INC
