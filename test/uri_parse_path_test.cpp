// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include "test_uri.hpp"
#include "string_utility.hpp"

// path          = path-abempty    ; begins with "/" or is empty
//               / path-absolute   ; begins with "/" but not "//"
//               / path-noscheme   ; begins with a non-colon segment
//               / path-rootless   ; begins with a segment
//               / path-empty      ; zero characters
//
// path-abempty  = *( "/" segment )
// path-absolute = "/" [ segment-nz *( "/" segment ) ]
// path-noscheme = segment-nz-nc *( "/" segment )
// path-rootless = segment-nz *( "/" segment )
// path-empty    = 0<pchar>

TEST(uri_parse_path_test, test_empty_path) {
  test::uri uri("http://123.34.23.56");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_path());
  EXPECT_TRUE(uri.path().empty());
}

TEST(uri_parse_path_test, test_empty_path_with_query) {
  test::uri uri("http://123.34.23.56?query");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_path());
  EXPECT_TRUE(uri.path().empty());
}

TEST(uri_parse_path_test, test_empty_path_with_fragment) {
  test::uri uri("http://123.34.23.56#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_path());
  EXPECT_TRUE(uri.path().empty());
}

TEST(uri_parse_path_test, test_single_slash) {
  test::uri uri("http://123.34.23.56/");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/", uri.path());
}

TEST(uri_parse_path_test, test_single_slash_with_query) {
  test::uri uri("http://123.34.23.56/?query");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/", uri.path());
}

TEST(uri_parse_path_test, test_single_slash_with_fragment) {
  test::uri uri("http://123.34.23.56/#fragment");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("/", uri.path());
}

TEST(uri_parse_path_test, test_empty_path_empty_everything) {
  test::uri uri("file://");
  EXPECT_FALSE(uri.parse_uri());
}

TEST(uri_parse_path_test, test_single_slash_empty_everything) {
  test::uri uri("file:///");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_path());
  EXPECT_TRUE(uri.path().empty());
}

TEST(uri_parse_path_test, test_rootless_1) {
  test::uri uri("mailto:john.doe@example.com");
  EXPECT_TRUE(uri.parse_uri());
  ASSERT_TRUE(uri.has_path());
  EXPECT_EQ("john.doe@example.com", uri.path());
}
