//            Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE URI encoding test
#include <boost/config/warning_disable.hpp>
#include <boost/test/unit_test.hpp>
#include <network/uri/encode.hpp>
#include <network/uri/decode.hpp>
#include <iterator>


BOOST_AUTO_TEST_CASE(encoding_test) {
    const std::string unencoded(" !\"#$%&\'()*");
    const std::string encoded("%20%21%22%23%24%25%26%27%28%29%2A");

    std::string instance;
    network::encode(unencoded, std::back_inserter(instance));
    BOOST_CHECK_EQUAL(instance, encoded);
}

BOOST_AUTO_TEST_CASE(decoding_test) {
    const std::string unencoded(" !\"#$%&\'()*");
    const std::string encoded("%20%21%22%23%24%25%26%27%28%29%2A");

    std::string instance;
    network::decode(encoded, std::back_inserter(instance));
    BOOST_CHECK_EQUAL(instance, unencoded);
}
