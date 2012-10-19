//            Copyright (c) Glyn Matthews 2011.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE URI builder stream test
#include <boost/config/warning_disable.hpp>
#include <boost/test/unit_test.hpp>
#include <network/uri/uri.hpp>
#include <network/uri/directives.hpp>
#include <network/uri/uri_io.hpp>


BOOST_AUTO_TEST_CASE(builder_test)
{
    network::uri instance;
    instance << network::scheme("http") << network::host("www.example.com") << network::path("/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/", instance.string());

}

BOOST_AUTO_TEST_CASE(full_uri_builder_test)
{
    network::uri instance;
    instance << network::scheme("http")
             << network::user_info("user:password")
             << network::host("www.example.com")
             << network::port("80")
             << network::path("/path")
             << network::query("query")
             << network::fragment("fragment")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://user:password@www.example.com:80/path?query#fragment", instance.string());
}

BOOST_AUTO_TEST_CASE(port_test)
{
    network::uri instance;
    instance << network::scheme("http") << network::host("www.example.com") << network::port(8000) << network::path("/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com:8000/", instance.string());
}

BOOST_AUTO_TEST_CASE(encoded_path_test)
{
    network::uri instance;
    instance << network::scheme("http")
             << network::host("www.example.com")
             << network::port(8000)
             << network::encoded_path("/Path With (Some) Encoded Characters!")
        ;
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com:8000/Path%20With%20%28Some%29%20Encoded%20Characters%21", instance.string());
}

BOOST_AUTO_TEST_CASE(query_test)
{
    network::uri instance;
    instance << network::scheme("http") << network::host("www.example.com") << network::path("/")
             << network::query("key", "value")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/?key=value", instance.string());
}

BOOST_AUTO_TEST_CASE(query_2_test)
{
    network::uri instance;
    instance << network::scheme("http") << network::host("www.example.com") << network::path("/")
             << network::query("key1", "value1") << network::query("key2", "value2")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/?key1=value1&key2=value2", instance.string());
}

BOOST_AUTO_TEST_CASE(fragment_test)
{
    network::uri instance;
    instance << network::scheme("http") << network::host("www.example.com") << network::path("/") << network::fragment("fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/#fragment", instance.string());
}

BOOST_AUTO_TEST_CASE(from_base_test)
{
    network::uri base_uri("http://www.example.com");
    network::uri instance;
    instance << base_uri << network::path("/") << network::fragment("fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/#fragment", instance.string());
}

BOOST_AUTO_TEST_CASE(scheme_http_test)
{
    network::uri instance;
    instance << network::schemes::http << network::host("www.example.com") << network::path("/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/", instance.string());
}

BOOST_AUTO_TEST_CASE(scheme_https_test)
{
    network::uri instance;
    instance << network::schemes::https << network::host("www.example.com") << network::path("/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("https://www.example.com/", instance.string());
}

BOOST_AUTO_TEST_CASE(encoded_null_char_test)
{
    // there is a potential bug in the way we process ranges if the
    // strings are null terminated.
    network::uri instance;
    instance << network::scheme("http")
             << network::host("www.example.com")
             << network::encoded_path("/")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/", instance.string());
}

BOOST_AUTO_TEST_CASE(mailto_builder_test)
{
    network::uri instance;
    instance << network::scheme("mailto") << network::path("cpp-netlib@example.com");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("mailto:cpp-netlib@example.com", instance.string());
}
