//            Copyright (c) Glyn Matthews 2012.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE Relative URL Test
#include <boost/config/warning_disable.hpp>
#include <boost/test/unit_test.hpp>
#include <network/uri/uri.hpp>
#include <network/uri/uri_io.hpp>

BOOST_AUTO_TEST_CASE(relative_uri_test) {
    // don't yet support relative URIs
    network::uri instance("example.com");
    BOOST_REQUIRE(!network::valid(instance));
}
