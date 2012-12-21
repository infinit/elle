// Copyright (c) Glyn Matthews 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <gtest/gtest.h>
#include <network/uri.hpp>

// Compare the underlying strings because ``normalize`` is used in the
// ``uri`` equality operator.

TEST(uri_normalization_test, string_comparison) {
  network::uri instance("http://www.example.com/");
  ASSERT_EQ("http://www.example.com/",
	    instance.normalize(network::uri_comparison_level::string_comparison).string());
}

TEST(uri_normalization_test, normalize_case) {
  network::uri instance("HTTP://www.example.com/");
  ASSERT_EQ("http://www.example.com/",
	    instance.normalize(network::uri_comparison_level::case_normalization).string());
}

TEST(uri_normalization_test, string_comparison_with_case) {
  network::uri instance("HTTP://www.example.com/");
  ASSERT_EQ("HTTP://www.example.com/",
	    instance.normalize(network::uri_comparison_level::string_comparison).string());
}

TEST(uri_normalization_test, normalize_case_percent_encode_is_upper_case) {
  network::uri instance("HTTP://www.example.com/%aa");
  ASSERT_EQ("http://www.example.com/%AA",
	    instance.normalize(network::uri_comparison_level::case_normalization).string());
}
