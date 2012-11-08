// Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <network/uri/encode.hpp>
#include <network/uri/decode.hpp>
#include <iterator>


TEST(uri_encoding_test, encode_user_info_iterator_test) {
  const std::string unencoded(" !\"#$%&\'()*");

  std::string instance;
  network::encode_user_info(std::begin(unencoded), std::end(unencoded),
			    std::back_inserter(instance));
  ASSERT_EQ(instance, "%20%21%22%23%24%25%26%27%28%29%2A");
}


TEST(uri_encoding_test, encode_user_info_test) {
  const std::string unencoded(" !\"#$%&\'()*");
  ASSERT_EQ(network::encode_user_info(unencoded), "%20%21%22%23%24%25%26%27%28%29%2A");
}

//TEST(uri_encoding_test, decoding_test) {
//  const std::string unencoded(" !\"#$%&\'()*");
//  const std::string encoded("%20%21%22%23%24%25%26%27%28%29%2A");
//
//  std::string instance;
//  network::decode(encoded, std::back_inserter(instance));
//  ASSERT_EQ(instance, unencoded);
//}
