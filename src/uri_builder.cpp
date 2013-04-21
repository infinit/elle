// Copyright (c) Glyn Matthews 2012, 2013.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <network/uri.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

namespace network {
  // alpha range %41-%5A, %61-%7A

  // digit %30-%39

  // hyphen %2D

  // period %2E

  // underscore %5F

  // tilde %7E

  template <typename Iter>
  Iter replace_encoded_chars(Iter first, Iter last, Iter out) {
    auto it = first;
    while (it != last) {
      if (*it == '%') {

      }
      else {
	++it;
      }
    }
    return out;
  }

  uri_builder::uri_builder() {

  }

  uri_builder::uri_builder(const network::uri &base_uri)
    : base_uri_(base_uri) {

  }

  uri_builder::~uri_builder() {

  }

  void uri_builder::set_scheme(const string_type &scheme) {
    // validate scheme is valid and normalize
    scheme_.reset(boost::to_lower_copy(scheme));
  }

  void uri_builder::set_user_info(const string_type &user_info) {
    user_info_.reset(user_info);
  }

  void uri_builder::set_host(const string_type &host) {
    host_.reset(host);
  }

  void uri_builder::set_port(const string_type &port) {
    port_.reset(port);
  }

  void uri_builder::set_path(const string_type &path) {
    path_.reset(path);
  }

  void uri_builder::set_query(const string_type &query) {
    query_.reset(query);
  }

  void uri_builder::set_fragment(const string_type &fragment) {
    fragment_.reset(fragment);
  }
} // namespace network
