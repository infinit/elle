// Copyright 2010 Jeroen Habraken.
// Copyright 2009-2016 Dean Michael Berris, Glyn Matthews.
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
#include "string_utility.hpp"

TEST(uri_test, construct_invalid_uri) {
  ASSERT_THROW(network::uri("I am not a valid URI."), network::uri_syntax_error);
}

TEST(uri_test, make_invalid_uri) {
  std::error_code ec;
  network::uri uri = network::make_uri("I am not a valid URI.", ec);
  ASSERT_TRUE(static_cast<bool>(ec));
}

TEST(uri_test, construct_uri_from_char_array) {
  ASSERT_NO_THROW(network::uri("http://www.example.com/"));
}

TEST(uri_test, construct_uri_starting_with_ipv4_like) {
  ASSERT_NO_THROW(network::uri("http://198.51.100.0.example.com/"));
}

TEST(uri_test, construct_uri_like_short_ipv4) {
  ASSERT_NO_THROW(network::uri("http://198.51.100/"));
}

TEST(uri_test, construct_uri_like_long_ipv4) {
  ASSERT_NO_THROW(network::uri("http://198.51.100.0.255/"));
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
  ASSERT_TRUE(static_cast<bool>(instance.scheme()));
  ASSERT_EQ("http", *instance.scheme());
}

TEST(uri_test, basic_uri_user_info_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.user_info());
}

TEST(uri_test, basic_uri_host_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(static_cast<bool>(instance.host()));
  ASSERT_EQ("www.example.com", *instance.host());
}

TEST(uri_test, basic_uri_port_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.port());
}

TEST(uri_test, basic_uri_port_as_int_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_FALSE(instance.port<int>());
}

TEST(uri_test, basic_uri_path_test) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(static_cast<bool>(instance.path()));
  ASSERT_EQ("/", *instance.path());
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

TEST(uri_test, full_uri_scheme_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ("http", *instance.scheme());
}

TEST(uri_test, full_uri_user_info_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ("user:password", *instance.user_info());
}

TEST(uri_test, full_uri_host_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ("www.example.com", *instance.host());
}

TEST(uri_test, full_uri_port_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ("80", *instance.port());
}

TEST(uri_test, full_uri_port_as_int_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ(80, *instance.port<int>());
}

TEST(uri_test, full_uri_path_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ("/path", *instance.path());
}

TEST(uri_test, full_uri_query_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ("query", *instance.query());
}

TEST(uri_test, full_uri_fragment_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_EQ("fragment", *instance.fragment());
}

TEST(uri_test, full_uri_range_scheme_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.scheme()));
  ASSERT_EQ("http", *instance.scheme());
}

TEST(uri_test, full_uri_range_user_info_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.user_info()));
  ASSERT_EQ("user:password", *instance.user_info());
}

TEST(uri_test, full_uri_range_host_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.host()));
  ASSERT_EQ("www.example.com", *instance.host());
}

TEST(uri_test, full_uri_range_port_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.port()));
  ASSERT_EQ("80", *instance.port());
}

TEST(uri_test, full_uri_range_path_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.path()));
  ASSERT_EQ("/path", *instance.path());
}

TEST(uri_test, full_uri_range_query_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.query()));
  ASSERT_EQ("query", *instance.query());
}

TEST(uri_test, full_uri_range_fragment_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.fragment()));
  ASSERT_EQ("fragment", *instance.fragment());
}

TEST(uri_test, mailto_test) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_EQ("mailto", *instance.scheme());
  ASSERT_EQ("john.doe@example.com", *instance.path());
}

TEST(uri_test, file_test) {
  network::uri instance("file:///bin/bash");
  ASSERT_EQ("file", *instance.scheme());
  ASSERT_EQ("/bin/bash", *instance.path());
}

TEST(uri_test, xmpp_test) {
  network::uri instance("xmpp:example-node@example.com?message;subject=Hello%20World");
  ASSERT_EQ("xmpp", *instance.scheme());
  ASSERT_EQ("example-node@example.com", *instance.path());
  ASSERT_EQ("message;subject=Hello%20World", *instance.query());
}

TEST(uri_test, ipv4_address_test) {
  network::uri instance("http://129.79.245.252/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("129.79.245.252", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv4_loopback_test) {
  network::uri instance("http://127.0.0.1/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("127.0.0.1", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_1) {
  network::uri instance("http://[1080:0:0:0:8:800:200C:417A]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[1080:0:0:0:8:800:200C:417A]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_2) {
  network::uri instance("http://[2001:db8:85a3:8d3:1319:8a2e:370:7348]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:db8:85a3:8d3:1319:8a2e:370:7348]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_3) {
  network::uri instance("http://[2001:db8:85a3:0:0:8a2e:370:7334]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:db8:85a3:0:0:8a2e:370:7334]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_4) {
  network::uri instance("http://[2001:db8:85a3::8a2e:370:7334]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:db8:85a3::8a2e:370:7334]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_5) {
  network::uri instance("http://[2001:0db8:0000:0000:0000:0000:1428:57ab]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:0db8:0000:0000:0000:0000:1428:57ab]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_6) {
  network::uri instance("http://[2001:0db8:0000:0000:0000::1428:57ab]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:0db8:0000:0000:0000::1428:57ab]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_7) {
  network::uri instance("http://[2001:0db8:0:0:0:0:1428:57ab]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:0db8:0:0:0:0:1428:57ab]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_8) {
  network::uri instance("http://[2001:0db8:0:0::1428:57ab]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:0db8:0:0::1428:57ab]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_9) {
  network::uri instance("http://[2001:0db8::1428:57ab]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:0db8::1428:57ab]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_10) {
  network::uri instance("http://[2001:db8::1428:57ab]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[2001:db8::1428:57ab]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_11) {
  network::uri instance("http://[::ffff:0c22:384e]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[::ffff:0c22:384e]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_12) {
  network::uri instance("http://[fe80::]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[fe80::]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_address_test_13) {
  network::uri instance("http://[::ffff:c000:280]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[::ffff:c000:280]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_loopback_test) {
  network::uri instance("http://[::1]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[::1]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_loopback_test_1) {
  network::uri instance("http://[0000:0000:0000:0000:0000:0000:0000:0001]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[0000:0000:0000:0000:0000:0000:0000:0001]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_v4inv6_test_1) {
  network::uri instance("http://[::ffff:12.34.56.78]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[::ffff:12.34.56.78]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ipv6_v4inv6_test_2) {
  network::uri instance("http://[::ffff:192.0.2.128]/");
  ASSERT_EQ("http", *instance.scheme());
  ASSERT_EQ("[::ffff:192.0.2.128]", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, ftp_test) {
  network::uri instance("ftp://john.doe@ftp.example.com/");
  ASSERT_EQ("ftp", *instance.scheme());
  ASSERT_EQ("john.doe", *instance.user_info());
  ASSERT_EQ("ftp.example.com", *instance.host());
  ASSERT_EQ("/", *instance.path());
}

TEST(uri_test, news_test) {
  network::uri instance("news:comp.infosystems.www.servers.unix");
  ASSERT_EQ("news", *instance.scheme());
  ASSERT_EQ("comp.infosystems.www.servers.unix", *instance.path());
}

TEST(uri_test, tel_test) {
  network::uri instance("tel:+1-816-555-1212");
  ASSERT_EQ("tel", *instance.scheme());
  ASSERT_EQ("+1-816-555-1212", *instance.path());
}

TEST(uri_test, ldap_test) {
  network::uri instance("ldap://[2001:db8::7]/c=GB?objectClass?one");
  ASSERT_EQ("ldap", *instance.scheme());
  ASSERT_EQ("[2001:db8::7]", *instance.host());
  ASSERT_EQ("/c=GB", *instance.path());
  ASSERT_EQ("objectClass?one", *instance.query());
}

TEST(uri_test, urn_test) {
  network::uri instance("urn:oasis:names:specification:docbook:dtd:xml:4.1.2");
  ASSERT_EQ("urn", *instance.scheme());
  ASSERT_EQ("oasis:names:specification:docbook:dtd:xml:4.1.2", *instance.path());
}

TEST(uri_test, svn_ssh_test) {
  network::uri instance("svn+ssh://example.com/");
  ASSERT_EQ("svn+ssh", *instance.scheme());
  ASSERT_EQ("example.com", *instance.host());
  ASSERT_EQ("/", *instance.path());
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
  ASSERT_EQ("http://www.example.org/", instance);
  ASSERT_EQ("http://www.example.com/", copy);
}

TEST(uri_test, authority_test) {
  network::uri instance("http://user:password@www.example.com:80/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.authority()));
  ASSERT_EQ("user:password@www.example.com:80", *instance.authority());
}

TEST(uri_test, partial_authority_test) {
  network::uri instance("http://www.example.com/path?query#fragment");
  ASSERT_TRUE(static_cast<bool>(instance.authority()));
  ASSERT_EQ("www.example.com", *instance.authority());
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
  ASSERT_EQ("http", *copy.scheme());
}

TEST(uri_test, uri_set_test) {
  std::set<network::uri> uri_set;
  uri_set.insert(network::uri("http://www.example.com/"));
  ASSERT_FALSE(uri_set.empty());
  ASSERT_EQ(network::uri("http://www.example.com/"), (*std::begin(uri_set)));
}

TEST(uri_test, uri_unordered_set_test) {
  std::unordered_set<network::uri> uri_set;
  uri_set.insert(network::uri("http://www.example.com/"));
  ASSERT_FALSE(uri_set.empty());
  ASSERT_EQ(network::uri("http://www.example.com/"), (*std::begin(uri_set)));
}

TEST(uri_test, empty_uri) {
  network::uri instance;
  ASSERT_TRUE(instance.empty());
}

TEST(uri_test, empty_uri_has_no_scheme) {
  network::uri instance;
  ASSERT_FALSE(instance.scheme());
}

TEST(uri_test, empty_uri_has_no_user_info) {
  network::uri instance;
  ASSERT_FALSE(instance.user_info());
}

TEST(uri_test, empty_uri_has_no_host) {
  network::uri instance;
  ASSERT_FALSE(instance.host());
}

TEST(uri_test, empty_uri_has_no_port) {
  network::uri instance;
  ASSERT_FALSE(instance.port());
}

TEST(uri_test, empty_uri_has_no_path) {
  network::uri instance;
  ASSERT_FALSE(instance.path());
}

TEST(uri_test, empty_uri_has_no_query) {
  network::uri instance;
  ASSERT_FALSE(instance.query());
}

TEST(uri_test, empty_uri_has_no_fragment) {
  network::uri instance;
  ASSERT_FALSE(instance.fragment());
}

TEST(uri_test, http_is_absolute) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(instance.is_absolute());
}

TEST(uri_test, mailto_has_no_user_info) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_FALSE(instance.user_info());
}

TEST(uri_test, mailto_has_no_host) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_FALSE(instance.host());
}

TEST(uri_test, mailto_has_no_port) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_FALSE(instance.port());
}

TEST(uri_test, mailto_has_no_authority) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_FALSE(instance.authority());
}

TEST(uri_test, http_is_hierarchical) {
  network::uri instance("http://www.example.com/");
  ASSERT_TRUE(!instance.is_opaque());
}

TEST(uri_test, file_is_hierarchical) {
  network::uri instance("file:///bin/bash");
  ASSERT_TRUE(!instance.is_opaque());
}

TEST(uri_test, mailto_is_absolute) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_TRUE(instance.is_absolute());
}

TEST(uri_test, mailto_is_opaque) {
  network::uri instance("mailto:john.doe@example.com");
  ASSERT_TRUE(instance.is_opaque());
}

TEST(uri_test, whitespace_no_throw) {
  ASSERT_NO_THROW(network::uri(" http://www.example.com/ "));
}

TEST(uri_test, whitespace_is_trimmed) {
  network::uri instance(" http://www.example.com/ ");
  ASSERT_EQ("http://www.example.com/", instance);
}

TEST(uri_test, unnormalized_invalid_path_doesnt_throw) {
  ASSERT_NO_THROW(network::uri("http://www.example.com/.."));
}

TEST(uri_test, unnormalized_invalid_path_is_valid) {
  network::uri instance("http://www.example.com/..");
  ASSERT_TRUE(static_cast<bool>(instance.path()));
}

TEST(uri_test, unnormalized_invalid_path_value) {
  network::uri instance("http://www.example.com/..");
  ASSERT_EQ("/..", *instance.path());
}

TEST(uri_test, git) {
  network::uri instance("git://github.com/cpp-netlib/cpp-netlib.git");
  ASSERT_EQ("git", *instance.scheme());
  ASSERT_EQ("github.com", *instance.host());
  ASSERT_EQ("/cpp-netlib/cpp-netlib.git", *instance.path());
}

TEST(uri_test, invalid_port_test) {
  ASSERT_THROW(network::uri("http://123.34.23.56:6662626/"), network::uri_syntax_error);
}

TEST(uri_test, valid_empty_port_test) {
  ASSERT_NO_THROW(network::uri("http://123.34.23.56:/"));
}

TEST(uri_test, empty_port_test) {
  network::uri instance("http://123.34.23.56:/");
  ASSERT_TRUE(static_cast<bool>(instance.port()));
  ASSERT_EQ("", *instance.port());
}

TEST(uri_test, empty_port_as_int_test) {
  network::uri instance("http://123.34.23.56:/");
  ASSERT_TRUE(!instance.port<int>());
}

TEST(uri_test, full_copy_uri_scheme_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("http", *instance.scheme());
}

TEST(uri_test, full_copy_uri_user_info_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("user:password", *instance.user_info());
}

TEST(uri_test, full_copy_uri_host_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("www.example.com", *instance.host());
}

TEST(uri_test, full_copy_uri_port_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("80", *instance.port());
}

TEST(uri_test, full_copy_uri_path_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("/path", *instance.path());
}

TEST(uri_test, full_copy_uri_query_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("query", *instance.query());
}

TEST(uri_test, full_copy_uri_fragment_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("fragment", *instance.fragment());
}

TEST(uri_test, full_move_uri_scheme_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_EQ("http", *instance.scheme());
}

TEST(uri_test, full_move_uri_user_info_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_EQ("user:password", *instance.user_info());
}

TEST(uri_test, full_move_uri_host_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_EQ("www.example.com", *instance.host());
}

TEST(uri_test, full_move_uri_port_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_EQ("80", *instance.port());
}

TEST(uri_test, full_move_uri_path_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_EQ("/path", *instance.path());
}

TEST(uri_test, full_move_uri_query_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_EQ("query", *instance.query());
}

TEST(uri_test, full_move_uri_fragment_test) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_EQ("fragment", *instance.fragment());
}

TEST(uri_test, mailto_uri_path) {
  network::uri origin("mailto:john.doe@example.com?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("john.doe@example.com", *instance.path());
}

TEST(uri_test, mailto_uri_query) {
  network::uri origin("mailto:john.doe@example.com?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("query", *instance.query());
}

TEST(uri_test, mailto_uri_fragment) {
  network::uri origin("mailto:john.doe@example.com?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("fragment", *instance.fragment());
}

TEST(uri_test, opaque_uri_with_one_slash) {
  network::uri instance("scheme:/path/");
  ASSERT_TRUE(instance.is_opaque());
}

TEST(uri_test, opaque_uri_with_one_slash_scheme) {
  network::uri instance("scheme:/path/");
  ASSERT_EQ("scheme", *instance.scheme());
}

TEST(uri_test, opaque_uri_with_one_slash_path) {
  network::uri instance("scheme:/path/");
  ASSERT_EQ("/path/", *instance.path());
}

TEST(uri_test, opaque_uri_with_one_slash_query) {
  network::uri instance("scheme:/path/?query#fragment");
  ASSERT_EQ("query", *instance.query());
}

TEST(uri_test, opaque_uri_with_one_slash_fragment) {
  network::uri instance("scheme:/path/?query#fragment");
  ASSERT_EQ("fragment", *instance.fragment());
}

TEST(uri_test, opaque_uri_with_one_slash_copy) {
  network::uri origin("scheme:/path/");
  network::uri instance = origin;
  ASSERT_TRUE(instance.is_opaque());
}

TEST(uri_test, opaque_uri_with_one_slash_copy_query) {
  network::uri origin("scheme:/path/?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("query", *instance.query());
}

TEST(uri_test, opaque_uri_with_one_slash_copy_fragment) {
  network::uri origin("scheme:/path/?query#fragment");
  network::uri instance = origin;
  ASSERT_EQ("fragment", *instance.fragment());
}

TEST(uri_test, move_empty_uri_check_scheme) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_FALSE(origin.scheme());
}

TEST(uri_test, move_empty_uri_check_user_info) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_FALSE(origin.user_info());
}

TEST(uri_test, move_empty_uri_check_host) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_FALSE(origin.host());
}

TEST(uri_test, move_empty_uri_check_port) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_FALSE(origin.port());
}

TEST(uri_test, move_empty_uri_check_path) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_FALSE(origin.path());
}

TEST(uri_test, move_empty_uri_check_query) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_FALSE(origin.query());
}

TEST(uri_test, move_empty_uri_check_fragment) {
  network::uri origin("http://user:password@www.example.com:80/path?query#fragment");
  network::uri instance = std::move(origin);
  ASSERT_FALSE(origin.fragment());
}

TEST(uri_test, empty_username_in_user_info) {
  network::uri instance("ftp://:@localhost");
  ASSERT_TRUE(static_cast<bool>(instance.user_info()));
  ASSERT_EQ(":", *instance.user_info());
  ASSERT_EQ("localhost", *instance.host());
}

TEST(uri_test, uri_begins_with_a_colon) {
  ASSERT_THROW(network::uri("://example.com"), network::uri_syntax_error);
}

TEST(uri_test, uri_begins_with_a_number) {
  ASSERT_THROW(network::uri("3http://example.com"), network::uri_syntax_error);
}

TEST(uri_test, uri_scheme_contains_an_invalid_character) {
  ASSERT_THROW(network::uri("ht%tp://example.com"), network::uri_syntax_error);
}
