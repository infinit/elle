// Copyright (c) Glyn Matthews 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <string>
#include <network/uri/optional.hpp>
#include <network/uri/string_view.hpp>

TEST(optional_test, empty_optional) {
  network::optional<int> opt;
  ASSERT_FALSE(opt);
}

TEST(optional_test, empty_optional_constructed_with_nullopt) {
  network::optional<int> opt{network::nullopt};
  ASSERT_FALSE(opt);
}

TEST(optional_test, empty_optional_string) {
  network::optional<std::string> opt{};
  ASSERT_FALSE(opt);
}

TEST(optional_test, empty_optional_string_with_nullopt) {
  network::optional<std::string> opt{network::nullopt};
  ASSERT_FALSE(opt);
}
