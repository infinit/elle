// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <gtest/gtest.h>
#include "../src/detail/uri_parse.hpp"
#include "../src/detail/uri_parts.hpp"
#include "string_utility.hpp"

using network::detail::parse;
using network::detail::iterator_pair;
using network::detail::uri_parts;

namespace {
inline
std::string to_string(iterator_pair part) {
  return std::string(part.first, part.second);
}

struct test_uri {

  explicit test_uri(const std::string &uri_)
    : uri(uri_), view(uri) {
    std::tie(it, last) = iterator_pair(std::begin(view), std::end(view));
  }

  bool parse_uri() {
    return parse(it, last, parts);
  }

  std::string parsed_till() const {
    return std::string(std::begin(view), it);
  }

  bool has_scheme() const {
    return static_cast<bool>(parts.scheme);
  }

  std::string scheme() const {
    return to_string(*parts.scheme);
  }

  bool has_user_info() const {
    return static_cast<bool>(parts.hier_part.user_info);
  }

  std::string user_info() const {
    return to_string(*parts.hier_part.user_info);
  }

  bool has_host() const {
    return static_cast<bool>(parts.hier_part.host);
  }

  std::string host() const {
    return to_string(*parts.hier_part.host);
  }

  bool has_port() const {
    return static_cast<bool>(parts.hier_part.port);
  }

  std::string port() const {
    return to_string(*parts.hier_part.port);
  }

  bool has_path() const {
    return static_cast<bool>(parts.hier_part.path);
  }

  std::string path() const {
    return to_string(*parts.hier_part.path);
  }

  bool has_query() const {
    return static_cast<bool>(parts.query);
  }

  std::string query() const {
    return to_string(*parts.query);
  }

  bool has_fragment() const {
    return static_cast<bool>(parts.fragment);
  }

  std::string fragment() const {
    return to_string(*parts.fragment);
  }

  std::string uri;
  network::string_view view;
  network::string_view::const_iterator it, last;

  uri_parts parts;

};
} // namespace

TEST(uri_parse_test, test_empty_uri) {
  test_uri uri("");
  EXPECT_FALSE(uri.parse_uri());
  EXPECT_TRUE(uri.parsed_till().empty());
}

TEST(uri_parse_test, test_valid_scheme) {
  test_uri uri("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
}

TEST(uri_parse_test, test_scheme_beginning_with_a_colon) {
  test_uri uri(":http://user@www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_scheme());
}

TEST(uri_parse_test, test_scheme_beginning_with_a_number) {
  test_uri uri("8http://user@www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_test, test_scheme_with_a_minus) {
  test_uri uri("ht-tp://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("ht-tp", uri.scheme());
}

TEST(uri_parse_test, test_scheme_with_a_plus) {
  test_uri uri("ht+tp://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("ht+tp", uri.scheme());
}

TEST(uri_parse_test, test_scheme_with_a_dot) {
  test_uri uri("ht.tp://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("ht.tp", uri.scheme());
}

TEST(uri_parse_test, test_scheme_with_a_number) {
  test_uri uri("http1://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http1", uri.scheme());
}

TEST(uri_parse_test, test_scheme_with_an_invalid_character) {
  test_uri uri("http$://user@www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_test, test_scheme_with_capital_letters) {
  test_uri uri("HTTP://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("HTTP", uri.scheme());
}

TEST(uri_parse_test, test_hierarchical_part_valid_user_info) {
  test_uri uri("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
}

TEST(uri_parse_test, test_hierarchical_part_empty_user_info) {
  test_uri uri("http://@www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
}

TEST(uri_parse_test, test_hierarchical_part_unset_user_info) {
  test_uri uri("http://www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
}

TEST(uri_parse_test, test_hierarchical_part_unset_user_info_and_host) {
  test_uri uri("http://:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_FALSE(uri.has_host());
}

TEST(uri_parse_test, test_hierarchical_part_valid_user_info_and_host) {
  test_uri uri("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
}

TEST(uri_parse_test, test_hierarchical_part_valid_user_info_unset_host) {
  test_uri uri("http://user@:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_host());
}

TEST(uri_parse_test, test_hierarchical_part_valid_user_info_host_and_port) {
  test_uri uri("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_TRUE(uri.has_port());
  EXPECT_EQ("80", uri.port());
}

TEST(uri_parse_test, test_hierarchical_part_empty_user_info_valid_host_and_port) {
  test_uri uri("http://www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_TRUE(uri.has_port());
  EXPECT_EQ("80", uri.port());
}

TEST(uri_parse_test, test_hierarchical_part_empty_user_info_valid_host_empty_port) {
  test_uri uri("http://www.example.com/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
}

TEST(uri_parse_test, test_hierarchical_part_valid_user_info_and_host_empty_port) {
  test_uri uri("http://user@www.example.com/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
}

TEST(uri_parse_test, test_hierarchical_part_valid_user_info_empty_host_valid_port) {
  test_uri uri("http://user@:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_test, test_hierarchical_part_valid_host_empty_port_empty_path) {
  test_uri uri("http://www.example.com");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_TRUE(uri.path().empty());
}

TEST(uri_parse_test, test_hierarchical_part_valid_host_valid_port_empty_path) {
  test_uri uri("http://www.example.com:80");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_TRUE(uri.has_port());
  EXPECT_EQ("80", uri.port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_TRUE(uri.path().empty());
}

TEST(uri_parse_test, test_hierarchical_part_valid_host_port_path) {
  test_uri uri("http://www.example.com:80/path");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_TRUE(uri.has_port());
  EXPECT_EQ("80", uri.port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/path", uri.path());
}

TEST(uri_parse_test, test_hierarchical_part_valid_host_path) {
  test_uri uri("http://www.example.com/path");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/path", uri.path());
}

TEST(uri_parse_test, test_hierarchical_part_with_opaque_uri) {
  test_uri uri("mailto:user@example.com");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("mailto", uri.scheme());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_FALSE(uri.has_host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("user@example.com", uri.path());
}

TEST(uri_parse_test, test_hierarchical_part_valid_host_path_and_query) {
  test_uri uri("http://www.example.com/path?query");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/path", uri.path());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("query", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_parse_test, test_hierarchical_part_valid_host_path_query_and_fragment) {
  test_uri uri("http://www.example.com/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/path", uri.path());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("query", uri.query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("fragment", uri.fragment());
}

TEST(uri_parse_test, test_hierarchical_part_valid_host_path_and_fragment) {
  test_uri uri("http://www.example.com/path#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/path", uri.path());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("fragment", uri.fragment());
}

TEST(uri_parse_test, test_invalid_fragment) {
  test_uri uri("http://www.example.com/path#%fragment");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_test, test_valid_fragment_with_pct_encoded_char) {
  test_uri uri("http://www.example.com/path#%ffragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("%ffragment", uri.fragment());
}

TEST(uri_parse_test, test_valid_fragment_with_unreserved_char) {
  test_uri uri("http://www.example.com/path#fragment-");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("fragment-", uri.fragment());
}

TEST(uri_parse_test, test_invalid_fragment_with_gen_delim) {
  test_uri uri("http://www.example.com/path#frag#ment");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_test, test_valid_fragment_with_sub_delim) {
  test_uri uri("http://www.example.com/path#frag$ment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("frag$ment", uri.fragment());
}

TEST(uri_parse_test, test_valid_fragment_with_forward_slash_and_question_mark) {
  test_uri uri("http://www.example.com/path#frag/ment?");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("frag/ment?", uri.fragment());
}

TEST(uri_parse_test, test_invalid_query) {
  test_uri uri("http://www.example.com/path?%query");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_test, test_valid_query_with_pct_encoded_char) {
  test_uri uri("http://www.example.com/path?%00query");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("%00query", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_parse_test, test_valid_query_with_unreserved_char) {
  test_uri uri("http://www.example.com/path?query-");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("query-", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_parse_test, test_valid_query_with_sub_delim) {
  test_uri uri("http://www.example.com/path?qu$ery");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("qu$ery", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_parse_test, test_invalid_port_with_path) {
  test_uri uri("http://123.34.23.56:6662626/");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_test, test_invalid_port) {
  test_uri uri("http://123.34.23.56:6662626");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_test, test_empty_port_with_path) {
  test_uri uri("http://123.34.23.56:/");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_port());
  ASSERT_EQ("", uri.port());
}

TEST(uri_parse_test, test_empty_port) {
  test_uri uri("http://123.34.23.56:");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_port());
  ASSERT_EQ("", uri.port());
}

TEST(uri_parse_test, test_ipv6_address) {
  test_uri uri("http://[1080:0:0:0:8:800:200C:417A]");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("[1080:0:0:0:8:800:200C:417A]", uri.host());
}

TEST(uri_parse_test, test_ipv6_address_with_path) {
  test_uri uri("http://[1080:0:0:0:8:800:200C:417A]/");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("[1080:0:0:0:8:800:200C:417A]", uri.host());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/", uri.path());
}

TEST(uri_parse_test, test_invalid_ipv6_address) {
  test_uri uri("http://[1080:0:0:0:8:800:200C:417A");
  EXPECT_FALSE(!uri.parse_uri());
}

TEST(uri_parse_test, test_invalid_ipv6_address_with_path) {
  test_uri uri("http://[1080:0:0:0:8:800:200C:417A/");
  EXPECT_FALSE(!uri.parse_uri());
}

TEST(uri_parse_test, test_opaque_uri_with_one_slash) {
  test_uri uri("scheme:/path/");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("scheme", uri.scheme());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/path/", uri.path());
}

TEST(uri_parse_test, test_query_with_empty_path) {
  test_uri uri("http://www.example.com?query");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("", uri.path());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("query", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_parse_test, test_query_with_user_info_and_empty_path) {
  test_uri uri("http://user@www.example.com?query");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("", uri.path());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("query", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_parse_test, test_fragment_with_empty_path) {
  test_uri uri("http://www.example.com#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("", uri.path());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("fragment", uri.fragment());
}

TEST(uri_parse_test, test_fragment_with_user_info_and_empty_path) {
  test_uri uri("http://user@www.example.com#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("", uri.path());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("fragment", uri.fragment());
}

TEST(uri_parse_test, test_query_with_empty_path_and_ipv6_address) {
  test_uri uri("http://[1080:0:0:0:8:800:200C:417A]?query");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("[1080:0:0:0:8:800:200C:417A]", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("", uri.path());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("query", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_parse_test, test_query_with_user_info_empty_path_and_ipv6_address) {
  test_uri uri("http://user@[1080:0:0:0:8:800:200C:417A]?query");
  EXPECT_TRUE(uri.parse_uri());
  EXPECT_EQ("http://user@[1080:0:0:0:8:800:200C:417A]?query", uri.parsed_till());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("[1080:0:0:0:8:800:200C:417A]", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("", uri.path());
  ASSERT_TRUE(uri.has_query());
  EXPECT_EQ("query", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_parse_test, test_fragment_with_empty_path_and_ipv6_address) {
  test_uri uri("http://[1080:0:0:0:8:800:200C:417A]#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("[1080:0:0:0:8:800:200C:417A]", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("", uri.path());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("fragment", uri.fragment());
}

TEST(uri_parse_test, test_fragment_with_user_info_empty_path_and_ipv6_address) {
  test_uri uri("http://user@[1080:0:0:0:8:800:200C:417A]#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("[1080:0:0:0:8:800:200C:417A]", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("", uri.path());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  EXPECT_EQ("fragment", uri.fragment());
}
