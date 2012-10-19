//            Copyright (c) Glyn Matthews 2012.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE URI builder test
#include <boost/config/warning_disable.hpp>
#include <boost/test/unit_test.hpp>
#include <network/uri/uri.hpp>
#include <network/uri/uri_io.hpp>


BOOST_AUTO_TEST_CASE(builder_test)
{
    network::uri instance;
    network::builder builder(instance);
    builder
        .scheme("http")
        .host("www.example.com")
        .path("/")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/", instance.string());
}

BOOST_AUTO_TEST_CASE(full_uri_builder_test)
{
    network::uri instance;
    network::builder builder(instance);
    builder
        .scheme("http")
        .user_info("user:password")
        .host("www.example.com")
        .port("80")
        .path("/path")
        .query("query")
        .fragment("fragment")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://user:password@www.example.com:80/path?query#fragment", instance.string());
}

BOOST_AUTO_TEST_CASE(port_test)
{
    network::uri instance;
    network::builder(instance).scheme("http").host("www.example.com").port(8000).path("/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com:8000/", instance.string());
}

BOOST_AUTO_TEST_CASE(encoded_path_test)
{
    network::uri instance;
    network::builder builder(instance);
    builder
        .scheme("http")
        .host("www.example.com")
        .port(8000)
        .encoded_path("/Path With (Some) Encoded Characters!")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com:8000/Path%20With%20%28Some%29%20Encoded%20Characters%21", instance.string());
}

BOOST_AUTO_TEST_CASE(query_test)
{
    network::uri instance;
    network::builder builder(instance);
    builder
       .scheme("http")
       .host("www.example.com")
       .path("/")
       .query("key", "value")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/?key=value", instance.string());
}

BOOST_AUTO_TEST_CASE(query_2_test)
{
    network::uri instance;
    network::builder builder(instance);
    builder
       .scheme("http")
       .host("www.example.com")
       .path("/")
       .query("key1", "value1")
       .query("key2", "value2")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/?key1=value1&key2=value2", instance.string());
}

BOOST_AUTO_TEST_CASE(fragment_test)
{
    network::uri instance;
    network::builder builder(instance);
    builder
       .scheme("http")
       .host("www.example.com")
       .path("/")
       .fragment("fragment")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/#fragment", instance.string());
}

BOOST_AUTO_TEST_CASE(from_base_test)
{
    network::uri instance("http://www.example.com");
    network::builder builder(instance);
    builder
        .path("/")
        .fragment("fragment")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/#fragment", instance.string());
}

BOOST_AUTO_TEST_CASE(encoded_null_char_test)
{
    // there is a potential bug in the way we process ranges if the
    // strings are null terminated.
    network::uri instance;
    network::builder builder(instance);
    builder
       .scheme("http")
       .host("www.example.com")
       .encoded_path("/")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://www.example.com/", instance.string());
}

BOOST_AUTO_TEST_CASE(mailto_builder_test)
{
    network::uri instance;
    network::builder builder(instance);
    builder
       .scheme("mailto")
       .path("cpp-netlib@example.com")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("mailto:cpp-netlib@example.com", instance.string());
}

BOOST_AUTO_TEST_CASE(ipv4_address) {
    using namespace boost::asio::ip;
    network::uri instance;
    network::builder builder(instance);
    builder
        .scheme("http")
        .host(address_v4::loopback())
        .path("/")
        ;
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL("http://127.0.0.1/", instance.string());
}

//BOOST_AUTO_TEST_CASE(ipv6_address) {
//    using namespace boost::asio::ip;
//    network::uri instance;
//    network::builder builder(instance);
//    builder
//        .scheme("http")
//        .host(address_v6::loopback())
//        .path("/")
//        ;
//    BOOST_REQUIRE(network::valid(instance));
//    BOOST_CHECK_EQUAL("http://[::1]/", instance.string());
//}
