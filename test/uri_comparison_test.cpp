// Copyright (c) Glyn Matthews 2012, 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <gtest/gtest.h>
#include <network/uri.hpp>


TEST(uri_comparison_test, equality_test) {
  network::uri lhs("http://www.example.com/");
  network::uri rhs("http://www.example.com/");
  ASSERT_EQ(lhs, rhs);
}

TEST(uri_comparison_test, equality_test_capitalized_scheme) {
  network::uri lhs("http://www.example.com/");
  network::uri rhs("HTTP://www.example.com/");
  ASSERT_NE(lhs.compare(rhs, network::uri_comparison_level::string_comparison), 0);
}

TEST(uri_comparison_test, equality_test_capitalized_scheme_with_case_normalization) {
  network::uri lhs("http://www.example.com/");
  network::uri rhs("HTTP://www.example.com/");
  ASSERT_EQ(lhs.compare(rhs, network::uri_comparison_level::syntax_based), 0);
}

//TEST(uri_comparison_test, equality_test_capitalized_host) {
//  network::uri lhs("http://www.example.com/");
//  network::uri rhs("http://WWW.EXAMPLE.COM/");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_user_info) {
//  network::uri lhs("ftp://john.doe@ftp.example.com/");
//  network::uri rhs("ftp://JOHN.DOE@ftp.example.com/");
//  ASSERT_NE(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_default_http_port) {
//  network::uri lhs("http://www.example.com/");
//  network::uri rhs("http://www.example.com:80/");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_default_http_port_2) {
//  network::uri lhs("http://www.example.com:80/");
//  network::uri rhs("http://www.example.com/");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_default_https_port) {
//  network::uri lhs("https://www.example.com/");
//  network::uri rhs("https://www.example.com:443/");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_default_https_port_2) {
//  network::uri lhs("https://www.example.com:443/");
//  network::uri rhs("https://www.example.com/");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_empty_path_with_trailing_slash) {
//  network::uri lhs("http://www.example.com/");
//  network::uri rhs("http://www.example.com");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_with_single_dot_segment) {
//  network::uri lhs("http://www.example.com/./path");
//  network::uri rhs("http://www.example.com/path");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_with_double_dot_segment) {
//  network::uri lhs("http://www.example.com/1/../2/");
//  network::uri rhs("http://www.example.com/2/");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_with_trailing_slash) {
//  network::uri lhs("http://www.example.com/path/");
//  network::uri rhs("http://www.example.com/path");
//  ASSERT_EQ(lhs, rhs);
//}
//
//TEST(uri_comparison_test, equality_test_with_file_ext) {
//  network::uri lhs("http://www.example.com/filename.txt");
//  network::uri rhs("http://www.example.com/filename.txt/");
//  ASSERT_NE(lhs, rhs);
//}

TEST(uri_comparison_test, equality_empty_lhs) {
  network::uri lhs;
  network::uri rhs("http://www.example.com/");
  ASSERT_NE(lhs, rhs);
}

TEST(uri_comparison_test, equality_empty_rhs) {
  network::uri lhs("http://www.example.com/");
  network::uri rhs;
  ASSERT_NE(lhs, rhs);
}

TEST(uri_comparison_test, inequality_test) {
  network::uri lhs("http://www.example.com/");
  network::uri rhs("http://www.example.com/");
  ASSERT_FALSE(lhs != rhs);
}

TEST(uri_comparison_test, less_than_test) {
  // lhs is lexicographically less than rhs
  network::uri lhs("http://www.example.com/");
  network::uri rhs("http://www.example.org/");
  ASSERT_LT(lhs, rhs);
}
