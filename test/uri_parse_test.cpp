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
using network::detail::v2::iterator_pair;
using network::detail::v2::uri_parts;

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

TEST(uri_test, test_empty_uri) {
  test_uri uri("");
  EXPECT_FALSE(uri.parse_uri());
  EXPECT_TRUE(uri.parsed_till().empty());
}

TEST(uri_test, test_valid_scheme) {
  test_uri uri("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http", uri.scheme());
}

TEST(uri_test, test_scheme_beginning_with_a_colon) {
  test_uri uri(":http://user@www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_scheme());
}

TEST(uri_test, test_scheme_beginning_with_a_number) {
  test_uri uri("8http://user@www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_test, test_scheme_with_a_minus) {
  test_uri uri("ht-tp://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("ht-tp", uri.scheme());
}

TEST(uri_test, test_scheme_with_a_plus) {
  test_uri uri("ht+tp://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("ht+tp", uri.scheme());
}

TEST(uri_test, test_scheme_with_a_dot) {
  test_uri uri("ht.tp://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("ht.tp", uri.scheme());
}

TEST(uri_test, test_scheme_with_a_number) {
  test_uri uri("http1://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("http1", uri.scheme());
}

TEST(uri_test, test_scheme_with_an_invalid_character) {
  test_uri uri("http$://user@www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_test, test_scheme_with_capital_letters) {
  test_uri uri("HTTP://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  EXPECT_EQ("HTTP", uri.scheme());
}

TEST(uri_test, test_hierarchical_part_single_slash) {
  test_uri uri("http:/www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_FALSE(uri.has_host());
  ASSERT_FALSE(uri.has_port());
  // ASSERT_FALSE(uri.has_path());
}

TEST(uri_test, test_hierarchical_part_valid_user_info) {
  test_uri uri("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
}

TEST(uri_test, test_hierarchical_part_empty_user_info) {
  test_uri uri("http://@www.example.com:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
}

TEST(uri_test, test_hierarchical_part_unset_user_info) {
  test_uri uri("http://www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
}

TEST(uri_test, test_hierarchical_part_unset_user_info_and_host) {
  test_uri uri("http://:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_FALSE(uri.has_host());
}

TEST(uri_test, test_hierarchical_part_valid_user_info_and_host) {
  test_uri uri("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
}

TEST(uri_test, test_hierarchical_part_valid_user_info_unset_host) {
  test_uri uri("http://user@:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
  ASSERT_FALSE(uri.has_host());
}

TEST(uri_test, test_hierarchical_part_valid_user_info_host_and_port) {
  test_uri uri("http://user@www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_user_info());
  EXPECT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_TRUE(uri.has_port());
  EXPECT_EQ("80", uri.port());
}

TEST(uri_test, test_hierarchical_part_empty_user_info_valid_host_and_port) {
  test_uri uri("http://www.example.com:80/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_TRUE(uri.has_port());
  EXPECT_EQ("80", uri.port());
}

TEST(uri_test, test_hierarchical_part_empty_user_info_valid_host_empty_port) {
  test_uri uri("http://www.example.com/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
}

TEST(uri_test, test_hierarchical_part_valid_user_info_and_host_empty_port) {
  test_uri uri("http://user@www.example.com/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_user_info());
  ASSERT_EQ("user", uri.user_info());
  ASSERT_TRUE(uri.has_host());
  EXPECT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
}

TEST(uri_test, test_hierarchical_part_valid_user_info_empty_host_valid_port) {
  test_uri uri("http://user@:80/path?query#fragment");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_test, test_hierarchical_part_valid_host_empty_port_empty_path) {
  test_uri uri("http://www.example.com");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  ASSERT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_TRUE(uri.path().empty());
}

TEST(uri_test, test_hierarchical_part_valid_host_valid_port_empty_path) {
  test_uri uri("http://www.example.com:80");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  ASSERT_EQ("www.example.com", uri.host());
  ASSERT_TRUE(uri.has_port());
  ASSERT_EQ("80", uri.port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_TRUE(uri.path().empty());
}

TEST(uri_test, test_hierarchical_part_valid_host_port_path) {
  test_uri uri("http://www.example.com:80/path");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  ASSERT_EQ("www.example.com", uri.host());
  ASSERT_TRUE(uri.has_port());
  ASSERT_EQ("80", uri.port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_EQ("/path", uri.path());
}

TEST(uri_test, test_hierarchical_part_valid_host_path) {
  test_uri uri("http://www.example.com/path");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  ASSERT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_EQ("/path", uri.path());
}

TEST(uri_test, test_hierarchical_part_with_opaque_uri) {
  test_uri uri("mailto:user@example.com");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_scheme());
  ASSERT_EQ("mailto", uri.scheme());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_FALSE(uri.has_host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_EQ("user@example.com", uri.path());
}

TEST(uri_test, test_hierarchical_part_valid_host_path_and_query) {
  test_uri uri("http://www.example.com/path?query");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  ASSERT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_EQ("/path", uri.path());
  ASSERT_TRUE(uri.has_query());
  ASSERT_EQ("query", uri.query());
  ASSERT_FALSE(uri.has_fragment());
}

TEST(uri_test, test_hierarchical_part_valid_host_path_query_and_fragment) {
  test_uri uri("http://www.example.com/path?query#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  ASSERT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_EQ("/path", uri.path());
  ASSERT_TRUE(uri.has_query());
  ASSERT_EQ("query", uri.query());
  ASSERT_TRUE(uri.has_fragment());
  ASSERT_EQ("fragment", uri.fragment());
}

TEST(uri_test, test_hierarchical_part_valid_host_path_and_fragment) {
  test_uri uri("http://www.example.com/path#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_FALSE(uri.has_user_info());
  ASSERT_TRUE(uri.has_host());
  ASSERT_EQ("www.example.com", uri.host());
  ASSERT_FALSE(uri.has_port());
  ASSERT_TRUE(uri.has_path());
  ASSERT_EQ("/path", uri.path());
  ASSERT_FALSE(uri.has_query());
  ASSERT_TRUE(uri.has_fragment());
  ASSERT_EQ("fragment", uri.fragment());
}
