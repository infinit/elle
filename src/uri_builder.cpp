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
  uri_builder::uri_builder() {

  }

  //uri_builder::uri_builder(const network::uri &base_uri)
  //  : base_uri_(base_uri) {
  //
  //}

  uri_builder::~uri_builder() {

  }

  void uri_builder::set_scheme(const string_type &scheme) {
    // validate scheme is valid and normalize
    scheme_.reset(boost::to_lower_copy(scheme));
  }

  void uri_builder::set_user_info(const string_type &user_info) {
    user_info_.reset(string_type());
    network::uri::encode_user_info(std::begin(user_info), std::end(user_info),
				   std::back_inserter(*user_info_));
  }

  void uri_builder::set_host(const string_type &host) {
    host_.reset(string_type());
    auto end = network::uri::encode_host(std::begin(host), std::end(host),
					 std::back_inserter(*host_));
    std::transform(std::begin(*host_), std::end(*host_), std::begin(*host_),
		   [] (string_type::value_type c) { return std::tolower(c); });
  }

  void uri_builder::set_port(const string_type &port) {
    port_.reset(string_type());
    network::uri::encode_port(std::begin(port), std::end(port),
			      std::back_inserter(*port_));
  }

  void uri_builder::set_path(const string_type &path) {
    path_.reset(string_type());
    network::uri::encode_path(std::begin(path), std::end(path),
			      std::back_inserter(*path_));
  }

  void uri_builder::set_query(const string_type &query) {
    query_.reset(string_type());
    network::uri::encode_query(std::begin(query), std::end(query),
			       std::back_inserter(*query_));
  }

  void uri_builder::set_fragment(const string_type &fragment) {
    fragment_.reset(string_type());
    network::uri::encode_fragment(std::begin(fragment), std::end(fragment),
				  std::back_inserter(*fragment_));
  }
} // namespace network
