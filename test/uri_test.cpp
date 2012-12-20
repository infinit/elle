// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2012 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <network/uri.hpp>
#include <algorithm>
#include <memory>
#include <map>
#include <set>
#include <unordered_set>
#include <cstring>

TEST(uri_test, construct_invalid_uri) {
  ASSERT_THROW(network::uri("I am not a valid URI."), std::system_error);
}

TEST(uri_test, make_invalid_uri) {
  std::error_code ec;
  network::uri uri = network::make_uri("I am not a valid URI.", ec);
  ASSERT_TRUE(static_cast<bool>(ec));
}

TEST(uri_test, construct_uri_from_char_array) {
  ASSERT_NO_THROW(network::uri("http://www.example.com/"));
}

TEST(uri_test, make_uri_from_char_array) {
  std::error_code ec;
  network::uri uri = network::make_uri("http://www.example.com/", ec);
  ASSERT_FALSE(ec);
}

TEST(uri_test, construct_uri_from_wchar_t_array) {
  ASSERT_NO_THROW(network::uri(L"http://www.example.com/"));
}

TEST(uri_test, make_uri_from_wchar_t_array) {
  std::error_code ec;
  network::uri uri = network::make_uri(L"http://www.example.com/", ec);
  ASSERT_FALSE(ec);
}

TEST(uri_test, construct_uri_from_string) {
  ASSERT_NO_THROW(network::uri(std::string("http://www.example.com/")));
}

TEST(uri_test, make_uri_from_string) {
  std::error_code ec;
  network::uri uri = network::make_uri(std::string("http://www.example.com/"), ec);
  ASSERT_FALSE(ec);
}

TEST(uri_test, construct_uri_from_wstring) {
  ASSERT_NO_THROW(network::uri(std::wstring(L"http://www.example.com/")));
}

TEST(uri_test, make_uri_from_wstring) {
  std::error_code ec;
  network::uri uri = network::make_uri(std::wstring(L"http://www.example.com/"), ec);
  ASSERT_FALSE(ec);
}

TEST(uri_test, basic_uri_scheme_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.scheme());
  ASSERT_EQ(*instance.scheme(), "http");
}

TEST(uri_test, basic_uri_user_info_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.user_info());
}

TEST(uri_test, basic_uri_host_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.host());
  ASSERT_EQ(*instance.host(), "www.example.com");
}

TEST(uri_test, basic_uri_port_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.port());
}

TEST(uri_test, basic_uri_path_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.path());
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, basic_uri_query_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.query());
}

TEST(uri_test, basic_uri_fragment_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.fragment());
}

TEST(uri_test, basic_uri_value_semantics_test) {
  network::uri original;
  network::uri assigned;
  assigned = original;
  ASSERT_EQ(original, assigned);
  assigned = network::uri("http://www.example.com/");
  ASSERT_NE(original, assigned);
  network::uri copy(assigned);
  ASSERT_EQ(copy, assigned);
}

TEST(uri_test, basic_uri_range_scheme_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.scheme());
  ASSERT_TRUE(std::begin(instance) == std::begin(*instance.scheme()));
  ASSERT_EQ(*instance.scheme(), "http");
}

TEST(uri_test, basic_uri_range_user_info_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.user_info());
}

TEST(uri_test, basic_uri_range_host_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.host());
  ASSERT_EQ(*instance.host(), "www.example.com");
}

TEST(uri_test, basic_uri_range_port_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.port());
  //ASSERT_TRUE(std::end(*instance.host()) == std::begin(*instance.port()));
  //ASSERT_TRUE(std::end(*instance.host()) == std::end(*instance.port()));
}

TEST(uri_test, basic_uri_range_path_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.path());
  ASSERT_EQ(*instance.path(), "/");
  ASSERT_TRUE(std::end(instance) == std::end(*instance.path()));
}

TEST(uri_test, basic_uri_range_query_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.query());
  //ASSERT_TRUE(std::end(instance) == std::begin(*instance.query()));
  //ASSERT_TRUE(std::end(instance) == std::end(*instance.query()));
}

TEST(uri_test, basic_uri_range_fragment_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.fragment());
  //ASSERT_TRUE(std::end(instance) == std::begin(instance.fragment()));
  //ASSERT_TRUE(std::end(instance) == std::end(instance.fragment()));
}

TEST(uri_test, full_uri_scheme_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(*instance.scheme(), "http");
}

TEST(uri_test, full_uri_user_info_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(*instance.user_info(), "user:password");
}

TEST(uri_test, full_uri_host_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(*instance.host(), "www.example.com");
}

TEST(uri_test, full_uri_port_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(*instance.port(), "80");
}

TEST(uri_test, full_uri_path_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(*instance.path(), "/path");
}

TEST(uri_test, full_uri_query_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(*instance.query(), "query");
}

TEST(uri_test, full_uri_fragment_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(*instance.fragment(), "fragment");
}

TEST(uri_test, full_uri_range_scheme_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.scheme());
  ASSERT_TRUE(std::begin(instance) == std::begin(*instance.scheme()));
  ASSERT_EQ(*instance.scheme(), "http");
}

TEST(uri_test, full_uri_range_user_info_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.user_info());
  ASSERT_EQ(*instance.user_info(), "user:password");
}

TEST(uri_test, full_uri_range_host_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.host());
  ASSERT_EQ(*instance.host(), "www.example.com");
}

TEST(uri_test, full_uri_range_port_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.port());
  ASSERT_EQ(*instance.port(), "80");
}

TEST(uri_test, full_uri_range_path_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.path());
  ASSERT_EQ(*instance.path(), "/path");
}

TEST(uri_test, full_uri_range_query_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.query());
  ASSERT_EQ(*instance.query(), "query");
}

TEST(uri_test, full_uri_range_fragment_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.fragment());
  ASSERT_EQ(*instance.fragment(), "fragment");
  ASSERT_TRUE(std::end(instance) == std::end(*instance.fragment()));
}

TEST(uri_test, mailto_test) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_EQ(*instance.scheme(), "mailto");
  ASSERT_EQ(*instance.path(), "john.doe@example.com");
}

TEST(uri_test, file_test) {
  network::uri instance("file:///bin/bash");
  ASSERT_EQ(*instance.scheme(), "file");
  ASSERT_EQ(*instance.path(), "/bin/bash");
}

TEST(uri_test, xmpp_test) {
  network::uri instance("xmpp:example-node@example.com?message;subject=Hello%20World");
  ASSERT_EQ(*instance.scheme(), "xmpp");
  ASSERT_EQ(*instance.path(), "example-node@example.com");
  ASSERT_EQ(*instance.query(), "message;subject=Hello%20World");
}

TEST(uri_test, ipv4_address_test) {
  network::uri instance("http://129.79.245.252/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "129.79.245.252");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv4_loopback_test) {
  network::uri instance("http://127.0.0.1/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "127.0.0.1");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_1) {
  network::uri instance("http://[1080:0:0:0:8:800:200C:417A]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[1080:0:0:0:8:800:200C:417A]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_2) {
  network::uri instance("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:db8:85a3:8d3:1319:8a2e:370:7348]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_3) {
  network::uri instance("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:db8:85a3:0:0:8a2e:370:7334]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_4) {
  network::uri instance("http://[2001:db8:85a3::8a2e:370:7334]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:db8:85a3::8a2e:370:7334]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_5) {
  network::uri instance("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:0db8:0000:0000:0000:0000:1428:57ab]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_6) {
  network::uri instance("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:0db8:0000:0000:0000::1428:57ab]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_7) {
  network::uri instance("http://[2001:0db8:0:0:0:0:1428:57ab]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:0db8:0:0:0:0:1428:57ab]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_8) {
  network::uri instance("http://[2001:0db8:0:0::1428:57ab]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:0db8:0:0::1428:57ab]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_9) {
  network::uri instance("http://[2001:0db8::1428:57ab]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:0db8::1428:57ab]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_10) {
  network::uri instance("http://[2001:db8::1428:57ab]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[2001:db8::1428:57ab]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_11) {
  network::uri instance("http://[::ffff:0c22:384e]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[::ffff:0c22:384e]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_12) {
  network::uri instance("http://[fe80::]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[fe80::]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_address_test_13) {
  network::uri instance("http://[::ffff:c000:280]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[::ffff:c000:280]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_loopback_test) {
  network::uri instance("http://[::1]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[::1]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_loopback_test_1) {
  network::uri instance("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[0000:0000:0000:0000:0000:0000:0000:0001]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_v4inv6_test_1) {
  network::uri instance("http://[::ffff:12.34.56.78]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[::ffff:12.34.56.78]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ipv6_v4inv6_test_2) {
  network::uri instance("http://[::ffff:192.0.2.128]/");
  ASSERT_EQ(*instance.scheme(), "http");
  ASSERT_EQ(*instance.host(), "[::ffff:192.0.2.128]");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, ftp_test) {
  network::uri instance("ftp://john.doe@ftp.example.com/");
  ASSERT_EQ(*instance.scheme(), "ftp");
  ASSERT_EQ(*instance.user_info(), "john.doe");
  ASSERT_EQ(*instance.host(), "ftp.example.com");
  ASSERT_EQ(*instance.path(), "/");
}

TEST(uri_test, news_test) {
  network::uri instance("news:comp.infosystems.www.servers.unix");
  ASSERT_EQ(*instance.scheme(), "news");
  ASSERT_EQ(*instance.path(), "comp.infosystems.www.servers.unix");
}

TEST(uri_test, tel_test) {
  network::uri instance("tel:+1-816-555-1212");
  ASSERT_EQ(*instance.scheme(), "tel");
  ASSERT_EQ(*instance.path(), "+1-816-555-1212");
}

TEST(uri_test, copy_constructor_test) {
  network::uri instance("http://www.example.com/");
  network::uri copy = instance;
  ASSERT_EQ(instance, copy);
}

TEST(uri_test, assignment_test) {
  network::uri instance("http://www.example.com/");
  network::uri copy;
  copy = instance;
  ASSERT_EQ(instance, copy);
}

TEST(uri_test, swap_test) {
  network::uri instance("http://www.example.com/");
  network::uri copy("http://www.example.org/");
  network::swap(instance, copy);
  ASSERT_EQ(instance.string(), "http://www.example.org/");
  ASSERT_EQ(copy.string(), "http://www.example.com/");
}

TEST(uri_test, authority_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(instance.authority());
  ASSERT_EQ(*instance.authority(), "user:password@www.example.com:80");
}

TEST(uri_test, partial_authority_test) {
  network::uri instance("http://www.example.com/path?query#fragment");
  ASSERT_TRUE(instance.authority());
  ASSERT_EQ(*instance.authority(), "www.example.com");
}

TEST(uri_test, DISABLED_mailto_has_no_authority) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_FALSE(instance.authority());
}

TEST(uri_test, range_test) {
  const std::string url("http://www.example.com/");
  network::uri instance(url);
  ASSERT_TRUE(std::equal(std::begin(instance), std::end(instance),
			 std::begin(url)));
}

TEST(uri_test, issue_104_test) {
  // https://github.com/cpp-netlib/cpp-netlib/issues/104
  std::unique_ptr<network::uri> instance(new network::uri("http://www.example.com/"));
  network::uri copy = *instance;
  instance.reset();
  ASSERT_EQ(*copy.scheme(), "http");
}

TEST(uri_test, uri_set_test) {
  std::set<network::uri> uri_set;
  uri_set.insert(network::uri("http://www.example.com/"));
  ASSERT_FALSE(uri_set.empty());
  ASSERT_EQ((*std::begin(uri_set)), network::uri("http://www.example.com/"));
}

TEST(uri_test, uri_unordered_set_test) {
  std::unordered_set<network::uri> uri_set;
  uri_set.insert(network::uri("http://www.example.com/"));
  ASSERT_FALSE(uri_set.empty());
  ASSERT_EQ((*std::begin(uri_set)), network::uri("http://www.example.com/"));
}

TEST(uri_test, http_is_absolute) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.absolute());
}

TEST(uri_test, mailto_is_absolute) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_TRUE(instance.absolute());
}

TEST(uri_test, http_is_hierarchical) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(!instance.opaque());
}

TEST(uri_test, DISABLED_mailto_is_opaque) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_TRUE(instance.opaque());
}

TEST(uri_test, DISABLED_whitespace_test) {
  // todo trim whitespace in parser.
  network::uri instance(" http://www.example.com/");
}
