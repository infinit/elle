// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <network/uri.hpp>
#include <sstream>

TEST(uri_stream_test, ostream) {
  std::ostringstream oss;
  oss << network::uri("http://www.example.com/");
  ASSERT_EQ("http://www.example.com/", oss.str());
}
