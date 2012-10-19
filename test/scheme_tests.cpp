//              Copyright 2012 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE URI Scheme Test
#include <boost/config/warning_disable.hpp>
#include <boost/test/unit_test.hpp>
#include <network/uri.hpp>

BOOST_AUTO_TEST_CASE(http_has_default_port) {
	BOOST_CHECK(network::default_port("http"));
}

BOOST_AUTO_TEST_CASE(http_default_port) {
	BOOST_CHECK_EQUAL(std::string("80"), network::default_port("http"));
}

BOOST_AUTO_TEST_CASE(https_has_default_port) {
	BOOST_CHECK(network::default_port("https"));
}

BOOST_AUTO_TEST_CASE(https_default_port) {
	BOOST_CHECK_EQUAL(std::string("443"), network::default_port("https"));
}
