// Copyright (c) Glyn Matthews 2012, 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <gtest/gtest.h>
#include <network/uri.hpp>


TEST(uri_relativize_test, opaque_uri) {
  network::uri uri_1("mailto:glynos@example.com");
  network::uri uri_2("mailto:john.doe@example.com");
  ASSERT_EQ(uri_2, uri_1.relativize(uri_2, network::uri_comparison_level::string_comparison));
}

TEST(uri_relativize_test, simple_test) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/path/");
  ASSERT_EQ("/path/",
	    uri_1.relativize(uri_2, network::uri_comparison_level::string_comparison).string());
}

TEST(uri_relativize_test, simple_test_with_different_authority) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.org/path/");
  ASSERT_EQ("http://www.example.org/path/",
	    uri_1.relativize(uri_2, network::uri_comparison_level::string_comparison).string());
}

TEST(uri_relativize_test, simple_test_is_relative) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/path/");
  ASSERT_FALSE(uri_1.relativize(uri_2, network::uri_comparison_level::string_comparison).absolute());
}

TEST(uri_relativize_test, simple_test_is_hierarchical) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/path/");
  ASSERT_FALSE(uri_1.relativize(uri_2, network::uri_comparison_level::string_comparison).opaque());
}

TEST(uri_relativize_test, simple_test_with_query) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/path/?key=value");
  auto result = uri_1.relativize(uri_2, network::uri_comparison_level::string_comparison);
  ASSERT_EQ("/path/?key=value", result.string());
}

TEST(uri_relativize_test, simple_test_with_fragment) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/path/#fragment");
  ASSERT_EQ("/path/#fragment",
	    uri_1.relativize(uri_2, network::uri_comparison_level::string_comparison).string());
}

TEST(uri_relativize_test, relativize_with_case_normalization) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/%aa");
  ASSERT_EQ("/%AA",
	    uri_1.relativize(uri_2, network::uri_comparison_level::case_normalization).string());
}

TEST(uri_relativize_test, relativize_with_percent_encoding_normalization) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/%7E%46%4F%4F%42%41%52%5F%36%39/");
  ASSERT_EQ("/~foobar_69/",
	    uri_1.relativize(uri_2, network::uri_comparison_level::percent_encoding_normalization).string());
}

TEST(uri_relativize_test, relativize_with_percent_encoding_normalization_with_query) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/%7E%46%4F%4F%42%41%52%5F%36%39/?key=value");
  ASSERT_EQ("/~foobar_69/?key=value",
	    uri_1.relativize(uri_2, network::uri_comparison_level::percent_encoding_normalization).string());
}

TEST(uri_relativize_test, relativize_with_percent_encoding_normalization_with_fragment) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/%7E%46%4F%4F%42%41%52%5F%36%39/#fragment");
  ASSERT_EQ("/~foobar_69/#fragment",
	    uri_1.relativize(uri_2, network::uri_comparison_level::percent_encoding_normalization).string());
}
