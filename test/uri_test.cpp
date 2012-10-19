// Copyright 2009-2012 Dean Michael Berris, Jeroen Habraken, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MODULE URI Test
#include <boost/config/warning_disable.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/range/algorithm/equal.hpp>
#include <boost/scoped_ptr.hpp>
#include <network/uri/uri.hpp>
#include <network/uri/uri_io.hpp>
#include <map>
#include <set>
#include <boost/unordered_set.hpp>


BOOST_AUTO_TEST_CASE(basic_uri_scheme_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
}

BOOST_AUTO_TEST_CASE(basic_uri_user_info_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::user_info(instance), "");
}

BOOST_AUTO_TEST_CASE(basic_uri_host_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::host(instance), "www.example.com");
}

BOOST_AUTO_TEST_CASE(basic_uri_port_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::port(instance), "");
}

BOOST_AUTO_TEST_CASE(basic_uri_path_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(basic_uri_query_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::query(instance), "");
}

BOOST_AUTO_TEST_CASE(basic_uri_fragment_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::fragment(instance), "");
}

BOOST_AUTO_TEST_CASE(basic_uri_value_semantics_test) {
  network::uri original;
  network::uri assigned;
  assigned = original;
  BOOST_CHECK(original == assigned);
  assigned = "http://www.example.com/";
  BOOST_CHECK(original != assigned);
  network::uri copy(assigned);
  BOOST_CHECK(copy == assigned);
}

BOOST_AUTO_TEST_CASE(basic_uri_range_scheme_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.scheme_range());
    BOOST_CHECK(instance.begin() == boost::begin(instance.scheme_range()));
    BOOST_CHECK(boost::equal(instance.scheme_range(), boost::as_literal("http")));
}

BOOST_AUTO_TEST_CASE(basic_uri_range_user_info_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(!instance.user_info_range());
    BOOST_CHECK(boost::begin(instance.host_range()) == boost::begin(instance.user_info_range()));
    BOOST_CHECK(boost::begin(instance.host_range()) == boost::end(instance.user_info_range()));
}

BOOST_AUTO_TEST_CASE(basic_uri_range_host_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.host_range());
    BOOST_CHECK(boost::equal(instance.host_range(), boost::as_literal("www.example.com")));
}

BOOST_AUTO_TEST_CASE(basic_uri_range_port_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(!instance.port_range());
    BOOST_CHECK(boost::end(instance.host_range()) == boost::begin(instance.port_range()));
    BOOST_CHECK(boost::end(instance.host_range()) == boost::end(instance.port_range()));
}

BOOST_AUTO_TEST_CASE(basic_uri_range_path_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.path_range());
    BOOST_CHECK(boost::equal(instance.path_range(), boost::as_literal("/")));
    BOOST_CHECK(instance.end() == boost::end(instance.path_range()));
}

BOOST_AUTO_TEST_CASE(basic_uri_range_query_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(!instance.query_range());
    BOOST_CHECK(instance.end() == boost::begin(instance.query_range()));
    BOOST_CHECK(instance.end() == boost::end(instance.query_range()));
}

BOOST_AUTO_TEST_CASE(basic_uri_range_fragment_test) {
    network::uri instance("http://www.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(!instance.fragment_range());
    BOOST_CHECK(instance.end() == boost::begin(instance.fragment_range()));
    BOOST_CHECK(instance.end() == boost::end(instance.fragment_range()));
}

BOOST_AUTO_TEST_CASE(full_uri_scheme_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
}

BOOST_AUTO_TEST_CASE(full_uri_user_info_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::user_info(instance), "user:password");
}

BOOST_AUTO_TEST_CASE(full_uri_host_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::host(instance), "www.example.com");
}

BOOST_AUTO_TEST_CASE(full_uri_port_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::port(instance), "80");
    BOOST_CHECK(network::port_us(instance));
    BOOST_CHECK_EQUAL(network::port_us(instance).get(), 80);
}

BOOST_AUTO_TEST_CASE(full_uri_path_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::path(instance), "/path");
}

BOOST_AUTO_TEST_CASE(full_uri_query_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::query(instance), "query");
}

BOOST_AUTO_TEST_CASE(full_uri_fragment_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::fragment(instance), "fragment");
}

BOOST_AUTO_TEST_CASE(full_uri_range_scheme_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.scheme_range());
    BOOST_CHECK(instance.begin() == boost::begin(instance.scheme_range()));
    BOOST_CHECK(boost::equal(instance.scheme_range(), boost::as_literal("http")));
}

BOOST_AUTO_TEST_CASE(full_uri_range_user_info_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.user_info_range());
    BOOST_CHECK(boost::equal(instance.user_info_range(), boost::as_literal("user:password")));
}

BOOST_AUTO_TEST_CASE(full_uri_range_host_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.host_range());
    BOOST_CHECK(boost::equal(instance.host_range(), boost::as_literal("www.example.com")));
}

BOOST_AUTO_TEST_CASE(full_uri_range_port_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.port_range());
    BOOST_CHECK(boost::equal(instance.port_range(), boost::as_literal("80")));
}

BOOST_AUTO_TEST_CASE(full_uri_range_path_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.path_range());
    BOOST_CHECK(boost::equal(instance.path_range(), boost::as_literal("/path")));
}

BOOST_AUTO_TEST_CASE(full_uri_range_query_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.query_range());
    BOOST_CHECK(boost::equal(instance.query_range(), boost::as_literal("query")));
}

BOOST_AUTO_TEST_CASE(full_uri_range_fragment_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(instance.fragment_range());
    BOOST_CHECK(boost::equal(instance.fragment_range(), boost::as_literal("fragment")));
    BOOST_CHECK(instance.end() == boost::end(instance.fragment_range()));
}

BOOST_AUTO_TEST_CASE(mailto_test) {
    network::uri instance("mailto:john.doe@example.com");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "mailto");
    BOOST_CHECK_EQUAL(network::path(instance), "john.doe@example.com");
}

BOOST_AUTO_TEST_CASE(file_test) {
    network::uri instance("file:///bin/bash");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "file");
    BOOST_CHECK_EQUAL(network::path(instance), "/bin/bash");
}

BOOST_AUTO_TEST_CASE(xmpp_test) {
    network::uri instance("xmpp:example-node@example.com?message;subject=Hello%20World");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "xmpp");
    BOOST_CHECK_EQUAL(network::path(instance), "example-node@example.com");
    BOOST_CHECK_EQUAL(network::query(instance), "message;subject=Hello%20World");
}

BOOST_AUTO_TEST_CASE(ipv4_address_test) {
    network::uri instance("http://129.79.245.252/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "129.79.245.252");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv4_loopback_test) {
    network::uri instance("http://127.0.0.1/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "127.0.0.1");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_1) {
    network::uri instance("http://[1080:0:0:0:8:800:200C:417A]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[1080:0:0:0:8:800:200C:417A]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_2) {
    network::uri instance("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:db8:85a3:8d3:1319:8a2e:370:7348]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_3) {
    network::uri instance("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:db8:85a3:0:0:8a2e:370:7334]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_4) {
    network::uri instance("http://[2001:db8:85a3::8a2e:370:7334]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:db8:85a3::8a2e:370:7334]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_5) {
    network::uri instance("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:0db8:0000:0000:0000:0000:1428:57ab]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_6) {
    network::uri instance("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:0db8:0000:0000:0000::1428:57ab]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_7) {
    network::uri instance("http://[2001:0db8:0:0:0:0:1428:57ab]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:0db8:0:0:0:0:1428:57ab]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_8) {
    network::uri instance("http://[2001:0db8:0:0::1428:57ab]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:0db8:0:0::1428:57ab]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_9) {
    network::uri instance("http://[2001:0db8::1428:57ab]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:0db8::1428:57ab]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_10) {
    network::uri instance("http://[2001:db8::1428:57ab]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[2001:db8::1428:57ab]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_11) {
    network::uri instance("http://[::ffff:0c22:384e]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[::ffff:0c22:384e]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_12) {
    network::uri instance("http://[fe80::]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[fe80::]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_address_test_13) {
    network::uri instance("http://[::ffff:c000:280]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[::ffff:c000:280]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_loopback_test) {
    network::uri instance("http://[::1]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[::1]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_loopback_test_1) {
    network::uri instance("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[0000:0000:0000:0000:0000:0000:0000:0001]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_v4inv6_test_1) {
    network::uri instance("http://[::ffff:12.34.56.78]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[::ffff:12.34.56.78]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ipv6_v4inv6_test_2) {
    network::uri instance("http://[::ffff:192.0.2.128]/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "[::ffff:192.0.2.128]");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(ftp_test) {
    network::uri instance("ftp://john.doe@ftp.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "ftp");
    BOOST_CHECK_EQUAL(network::user_info(instance), "john.doe");
    BOOST_CHECK_EQUAL(network::host(instance), "ftp.example.com");
    BOOST_CHECK_EQUAL(network::path(instance), "/");
}

BOOST_AUTO_TEST_CASE(news_test) {
    network::uri instance("news:comp.infosystems.www.servers.unix");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "news");
    BOOST_CHECK_EQUAL(network::path(instance), "comp.infosystems.www.servers.unix");
}

BOOST_AUTO_TEST_CASE(tel_test) {
    network::uri instance("tel:+1-816-555-1212");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "tel");
    BOOST_CHECK_EQUAL(network::path(instance), "+1-816-555-1212");
}

BOOST_AUTO_TEST_CASE(encoded_uri_test) {
    network::uri instance("http://www.example.com/Path%20With%20%28Some%29%20Encoded%20Characters%21");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::scheme(instance), "http");
    BOOST_CHECK_EQUAL(network::host(instance), "www.example.com");
    BOOST_CHECK_EQUAL(network::path(instance), "/Path%20With%20%28Some%29%20Encoded%20Characters%21");
    BOOST_CHECK_EQUAL(network::decoded_path(instance), "/Path With (Some) Encoded Characters!");
}

BOOST_AUTO_TEST_CASE(copy_constructor_test) {
    network::uri instance("http://www.example.com/");
    network::uri copy = instance;
    BOOST_CHECK_EQUAL(instance, copy);
}

BOOST_AUTO_TEST_CASE(assignment_test) {
    network::uri instance("http://www.example.com/");
    network::uri copy;
    copy = instance;
    BOOST_CHECK_EQUAL(instance, copy);
}

BOOST_AUTO_TEST_CASE(swap_test) {
    network::uri instance("http://www.example.com/");
    network::uri copy("http://www.example.org/");
    network::swap(instance, copy);
    BOOST_CHECK_EQUAL(instance.string(), "http://www.example.org/");
    BOOST_CHECK_EQUAL(copy.string(), "http://www.example.com/");
}

BOOST_AUTO_TEST_CASE(equality_test) {
    network::uri uri_1("http://www.example.com/");
    network::uri uri_2("http://www.example.com/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_1) {
    network::uri uri_1("http://www.example.com/");
    std::string uri_2("http://www.example.com/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_2) {
    std::string uri_1("http://www.example.com/");
    network::uri uri_2("http://www.example.com/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_3) {
    network::uri uri_1("http://www.example.com/");
    std::string uri_2("http://www.example.com/");
    BOOST_CHECK(uri_1 == uri_2.c_str());
}

BOOST_AUTO_TEST_CASE(equality_test_4) {
    std::string uri_1("http://www.example.com/");
    network::uri uri_2("http://www.example.com/");
    BOOST_CHECK(uri_1.c_str() == uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_reordered_query) {
	network::uri uri_1("http://www.example.com/?a=1&b=2");
	network::uri uri_2("http://www.example.com/?b=2&a=1");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_capitalized_scheme) {
	network::uri uri_1("http://www.example.com/");
	network::uri uri_2("HTTP://www.example.com/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_capitalized_host) {
	network::uri uri_1("http://www.example.com/");
	network::uri uri_2("http://WWW.EXAMPLE.COM/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_user_info) {
    network::uri uri_1("ftp://john.doe@ftp.example.com/");
    network::uri uri_2("ftp://JOHN.DOE@ftp.example.com/");
    BOOST_CHECK_PREDICATE(std::not_equal_to<network::uri>(), (uri_1)(uri_2));
}

BOOST_AUTO_TEST_CASE(equality_test_default_http_port) {
	network::uri uri_1("http://www.example.com/");
	network::uri uri_2("http://www.example.com:80/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_default_http_port_2) {
	network::uri uri_1("http://www.example.com:80/");
	network::uri uri_2("http://www.example.com/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_default_https_port) {
	network::uri uri_1("https://www.example.com/");
	network::uri uri_2("https://www.example.com:443/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_default_https_port_2) {
	network::uri uri_1("https://www.example.com:443/");
	network::uri uri_2("https://www.example.com/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_empty_path_with_trailing_slash) {
	network::uri uri_1("http://www.example.com/");
	network::uri uri_2("http://www.example.com");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_with_single_dot_segment) {
	network::uri uri_1("http://www.example.com/./path");
	network::uri uri_2("http://www.example.com/path");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_with_double_dot_segment) {
	network::uri uri_1("http://www.example.com/1/../2/");
	network::uri uri_2("http://www.example.com/2/");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_with_trailing_slash) {
	network::uri uri_1("http://www.example.com/path/");
	network::uri uri_2("http://www.example.com/path");
    BOOST_CHECK_EQUAL(uri_1, uri_2);
}

BOOST_AUTO_TEST_CASE(equality_test_with_file_ext) {
	network::uri uri_1("http://www.example.com/filename.txt");
	network::uri uri_2("http://www.example.com/filename.txt/");
    BOOST_CHECK_PREDICATE(std::not_equal_to<network::uri>(), (uri_1)(uri_2));
}

BOOST_AUTO_TEST_CASE(inequality_test) {
    network::uri uri_1("http://www.example.com/");
    network::uri uri_2("http://www.example.com/");
	BOOST_CHECK(!(uri_1 != uri_2));
}

BOOST_AUTO_TEST_CASE(less_than_test) {
    // uri_1 is lexicographically less than uri_2
    network::uri uri_1("http://www.example.com/");
    network::uri uri_2("http://www.example.org/");
    BOOST_CHECK_PREDICATE(std::less<network::uri>(), (uri_1)(uri_2));
    //BOOST_CHECK(uri_1 < uri_2);
}

BOOST_AUTO_TEST_CASE(username_test) {
    network::uri instance("ftp://john.doe@ftp.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::username(instance), "john.doe");
}

BOOST_AUTO_TEST_CASE(pasword_test) {
    network::uri instance("ftp://john.doe:password@ftp.example.com/");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::password(instance), "password");
}

BOOST_AUTO_TEST_CASE(hierarchical_part_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::hierarchical_part(instance), "user:password@www.example.com:80/path");
}

BOOST_AUTO_TEST_CASE(partial_hierarchical_part_test) {
    network::uri instance("http://www.example.com?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::hierarchical_part(instance), "www.example.com");
}

BOOST_AUTO_TEST_CASE(authority_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::authority(instance), "user:password@www.example.com:80");
}

BOOST_AUTO_TEST_CASE(partial_authority_test) {
    network::uri instance("http://www.example.com/path?query#fragment");
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK_EQUAL(network::authority(instance), "www.example.com");
}

BOOST_AUTO_TEST_CASE(http_query_map_test) {
    network::uri instance("http://user:password@www.example.com:80/path?query=something#fragment");
    BOOST_REQUIRE(network::valid(instance));

    std::map<std::string, std::string> queries;
    network::query_map(instance, queries);
    BOOST_REQUIRE_EQUAL(queries.size(), std::size_t(1));
    BOOST_CHECK_EQUAL(queries.begin()->first, "query");
    BOOST_CHECK_EQUAL(queries.begin()->second, "something");
}

BOOST_AUTO_TEST_CASE(xmpp_query_map_test) {
    network::uri instance("xmpp:example-node@example.com?message;subject=Hello%20World");
    BOOST_REQUIRE(network::valid(instance));

    std::map<std::string, std::string> queries;
    network::query_map(instance, queries);
    BOOST_REQUIRE_EQUAL(queries.size(), std::size_t(2));
    BOOST_CHECK_EQUAL(queries.begin()->first, "message");
    BOOST_CHECK_EQUAL(queries.begin()->second, "");
    BOOST_CHECK_EQUAL((++queries.begin())->first, "subject");
    BOOST_CHECK_EQUAL((++queries.begin())->second, "Hello%20World");
}

BOOST_AUTO_TEST_CASE(range_test) {
    const std::string url("http://www.example.com/");
    network::uri instance(url);
    BOOST_REQUIRE(network::valid(instance));
    BOOST_CHECK(boost::equal(instance, url));
}

BOOST_AUTO_TEST_CASE(issue_67_test) {
	// https://github.com/cpp-netlib/cpp-netlib/issues/67
    const std::string site_name("http://www.google.com");
    network::uri bar0;
    network::uri bar1 = site_name;
    bar0 = site_name;
    BOOST_CHECK(network::is_valid(bar0));
    BOOST_CHECK(network::is_valid(bar1));
}

BOOST_AUTO_TEST_CASE(from_parts_1) {
    BOOST_CHECK_EQUAL(network::uri("http://www.example.com/path?query#fragment"),
                      network::from_parts(network::uri("http://www.example.com"), "/path", "query", "fragment"));
}

BOOST_AUTO_TEST_CASE(from_parts_2) {
    BOOST_CHECK_EQUAL(network::uri("http://www.example.com/path?query#fragment"),
                      network::from_parts("http://www.example.com", "/path", "query", "fragment"));
}

BOOST_AUTO_TEST_CASE(from_parts_3) {
    BOOST_CHECK_EQUAL(network::uri("http://www.example.com/path?query"),
                      network::from_parts("http://www.example.com", "/path", "query"));
}

BOOST_AUTO_TEST_CASE(from_parts_4) {
    BOOST_CHECK_EQUAL(network::uri("http://www.example.com/path"),
                      network::from_parts("http://www.example.com", "/path"));
}

BOOST_AUTO_TEST_CASE(from_file) {
    boost::filesystem::path path("/a/path/to/a/file.txt");
    BOOST_CHECK_EQUAL(network::uri("file:///a/path/to/a/file.txt"), network::from_file(path));
}

BOOST_AUTO_TEST_CASE(issue_104_test) {
	// https://github.com/cpp-netlib/cpp-netlib/issues/104
	boost::scoped_ptr<network::uri> instance(new network::uri("http://www.example.com/"));
	network::uri copy = *instance;
	instance.reset();
	BOOST_CHECK_EQUAL(network::scheme(copy), "http");
}

BOOST_AUTO_TEST_CASE(uri_set_test) {
    std::set<network::uri> uri_set;
    uri_set.insert(network::uri("http://www.example.com/"));
    BOOST_REQUIRE(!uri_set.empty());
    BOOST_CHECK_EQUAL((*uri_set.begin()), network::uri("http://www.example.com/"));
}

BOOST_AUTO_TEST_CASE(uri_unordered_set_test) {
    boost::unordered_set<network::uri> uri_set;
    uri_set.insert(network::uri("http://www.example.com/"));
    BOOST_REQUIRE(!uri_set.empty());
    BOOST_CHECK_EQUAL((*uri_set.begin()), network::uri("http://www.example.com/"));
}
