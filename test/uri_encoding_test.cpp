// Copyright (c) Glyn Matthews 2011, 2012, 2013.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <gtest/gtest.h>
#include <network/uri.hpp>
#include <iterator>


TEST(uri_encoding_test, encode_user_info_iterator) {
  const std::string unencoded("!#$&\'()*+,/:;=?@[]");

  std::string instance;
  network::uri::encode_user_info(std::begin(unencoded), std::end(unencoded),
				 std::back_inserter(instance));
  ASSERT_EQ("%21%23%24%26%27%28%29%2A%2B%2C%2F:%3B%3D%3F%40%5B%5D", instance);
}

TEST(uri_encoding_test, encode_host_iterator) {
  const std::string unencoded("!#$&\'()*+,/:;=?@[]");

  std::string instance;
  network::uri::encode_host(std::begin(unencoded), std::end(unencoded),
			    std::back_inserter(instance));
  ASSERT_EQ("%21%23%24%26%27%28%29%2A%2B%2C%2F:%3B%3D%3F%40[]", instance);
}

TEST(uri_encoding_test, encode_ipv6_host) {
  const std::string unencoded("[::1]");

  std::string instance;
  network::uri::encode_host(std::begin(unencoded), std::end(unencoded),
			    std::back_inserter(instance));
  ASSERT_EQ("[::1]", instance);
}

TEST(uri_encoding_test, encode_port_iterator) {
  const std::string unencoded("!#$&\'()*+,/:;=?@[]");

  std::string instance;
  network::uri::encode_port(std::begin(unencoded), std::end(unencoded),
			    std::back_inserter(instance));
  ASSERT_EQ("%21%23%24%26%27%28%29%2A%2B%2C%2F%3A%3B%3D%3F%40%5B%5D", instance);
}

TEST(uri_encoding_test, encode_path_iterator) {
  const std::string unencoded("!#$&\'()*+,/:;=?@[]");

  std::string instance;
  network::uri::encode_path(std::begin(unencoded), std::end(unencoded),
			    std::back_inserter(instance));
  ASSERT_EQ("%21%23%24%26%27%28%29%2A%2B%2C/%3A%3B%3D%3F@%5B%5D", instance);
}

TEST(uri_encoding_test, encode_query_iterator) {
  const std::string unencoded("!#$&\'()*+,/:;=?@[]");

  std::string instance;
  network::uri::encode_query(std::begin(unencoded), std::end(unencoded),
			     std::back_inserter(instance));
  ASSERT_EQ("%21%23%24&%27%28%29%2A%2B%2C/%3A;=%3F@%5B%5D", instance);
}

TEST(uri_encoding_test, encode_fragment_iterator) {
  const std::string unencoded("!#$&\'()*+,/:;=?@[]");

  std::string instance;
  network::uri::encode_fragment(std::begin(unencoded), std::end(unencoded),
				std::back_inserter(instance));
  ASSERT_EQ("%21%23%24&%27%28%29%2A%2B%2C/%3A;=%3F@%5B%5D", instance);
}

TEST(uri_encoding_test, decode_iterator) {
  const std::string encoded("%21%23%24%26%27%28%29%2A%2B%2C%2F%3A%3B%3D%3F%40%5B%5D");

  std::string instance;
  network::uri::decode(std::begin(encoded), std::end(encoded),
		       std::back_inserter(instance));
  ASSERT_EQ("!#$&\'()*+,/:;=?@[]", instance);
}

TEST(uri_encoding_test, decode_iterator_error_1) {
  const std::string encoded("%");

  std::string instance;
  ASSERT_THROW(network::uri::decode(std::begin(encoded), std::end(encoded),
				    std::back_inserter(instance)),
	       network::percent_decoding_error);
}

TEST(uri_encoding_test, decode_iterator_error_2) {
  const std::string encoded("%2");

  std::string instance;
  ASSERT_THROW(network::uri::decode(std::begin(encoded), std::end(encoded),
				    std::back_inserter(instance)),
	       network::percent_decoding_error);
}

TEST(uri_encoding_test, decode_iterator_error_3) {
  const std::string encoded("%%%");

  std::string instance;
  ASSERT_THROW(network::uri::decode(std::begin(encoded), std::end(encoded),
				    std::back_inserter(instance)),
	       network::percent_decoding_error);
}

TEST(uri_encoding_test, decode_iterator_error_4) {
  const std::string encoded("%2%");

  std::string instance;
  ASSERT_THROW(network::uri::decode(std::begin(encoded), std::end(encoded),
				    std::back_inserter(instance)),
	       network::percent_decoding_error);
}

TEST(uri_encoding_test, decode_iterator_error_5) {
  const std::string encoded("%GF");

  std::string instance;
  ASSERT_THROW(network::uri::decode(std::begin(encoded), std::end(encoded),
				    std::back_inserter(instance)),
	       network::percent_decoding_error);
}

TEST(uri_encoding_test, decode_iterator_error_6) {
  const std::string encoded("%FG");

  std::string instance;
  ASSERT_THROW(network::uri::decode(std::begin(encoded), std::end(encoded),
				    std::back_inserter(instance)),
	       network::percent_decoding_error);
}

TEST(uri_encoding_test, decode_iterator_not_an_error) {
  const std::string encoded("%20");

  std::string instance;
  ASSERT_NO_THROW(network::uri::decode(std::begin(encoded), std::end(encoded),
				       std::back_inserter(instance)));
}

TEST(uri_encoding_test, DISABLED_fail_to_decode) {
  const std::string encoded("%80");

  std::string instance;
  ASSERT_THROW(network::uri::decode(std::begin(encoded), std::end(encoded),
				    std::back_inserter(instance)),
	       network::percent_decoding_error);
}
