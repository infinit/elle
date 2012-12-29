// Copyright (c) Glyn Matthews 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <gtest/gtest.h>
#include <network/uri.hpp>


TEST(uri_test, equality_test) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/");
  ASSERT_EQ(uri_1, uri_2);
}
//
//TEST(uri_test, equality_test_capitalized_scheme) {
//  network::uri uri_1("http://www.example.com/");
//  network::uri uri_2("HTTP://www.example.com/");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_capitalized_host) {
//  network::uri uri_1("http://www.example.com/");
//  network::uri uri_2("http://WWW.EXAMPLE.COM/");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_user_info) {
//  network::uri uri_1("ftp://john.doe@ftp.example.com/");
//  network::uri uri_2("ftp://JOHN.DOE@ftp.example.com/");
//  ASSERT_NE(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_default_http_port) {
//  network::uri uri_1("http://www.example.com/");
//  network::uri uri_2("http://www.example.com:80/");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_default_http_port_2) {
//  network::uri uri_1("http://www.example.com:80/");
//  network::uri uri_2("http://www.example.com/");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_default_https_port) {
//  network::uri uri_1("https://www.example.com/");
//  network::uri uri_2("https://www.example.com:443/");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_default_https_port_2) {
//  network::uri uri_1("https://www.example.com:443/");
//  network::uri uri_2("https://www.example.com/");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_empty_path_with_trailing_slash) {
//  network::uri uri_1("http://www.example.com/");
//  network::uri uri_2("http://www.example.com");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_with_single_dot_segment) {
//  network::uri uri_1("http://www.example.com/./path");
//  network::uri uri_2("http://www.example.com/path");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_with_double_dot_segment) {
//  network::uri uri_1("http://www.example.com/1/../2/");
//  network::uri uri_2("http://www.example.com/2/");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_with_trailing_slash) {
//  network::uri uri_1("http://www.example.com/path/");
//  network::uri uri_2("http://www.example.com/path");
//  ASSERT_EQ(uri_1, uri_2);
//}
//
//TEST(uri_test, equality_test_with_file_ext) {
//  network::uri uri_1("http://www.example.com/filename.txt");
//  network::uri uri_2("http://www.example.com/filename.txt/");
//  ASSERT_NE(uri_1, uri_2);
//}

TEST(uri_test, inequality_test) {
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.com/");
  ASSERT_TRUE(!(uri_1 != uri_2));
}

TEST(uri_test, less_than_test) {
  // uri_1 is lexicographically less than uri_2
  network::uri uri_1("http://www.example.com/");
  network::uri uri_2("http://www.example.org/");
  ASSERT_LT(uri_1, uri_2);
}
