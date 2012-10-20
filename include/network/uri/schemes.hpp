// Copyright 2012 Glyn Matthews.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_SCHEMES_INC
#define NETWORK_URI_SCHEMES_INC

#include <string>
#include <boost/optional.hpp>

namespace network {
class hierarchical_schemes {

public:

    static bool exists(const std::string &scheme);

};

class opaque_schemes {

public:

    static bool exists(const std::string &scheme);

};

boost::optional<std::string> default_port(const std::string &scheme);
} // namespace network

#endif // NETWORK_URI_SCHEMES_INC
