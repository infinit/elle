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
//
// TEST(uri_test, test_valid_user_info) {
//   std::string uri("http://user@www.example.com:80/path?query#fragment");
//   network::string_view view(uri);
//   uri_parts parts;
//
//   EXPECT_TRUE(parse(std::begin(view), std::end(view), parts));
//   EXPECT_TRUE(static_cast<bool>(parts.hier_part.user_info));
//   EXPECT_EQ("http", to_string(*parts.hier_part.user_info));
// }
