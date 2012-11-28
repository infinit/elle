// Copyright (c) Glyn Matthews 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <network/uri.hpp>


namespace network {
  uri_builder::uri_builder() {

  }

  uri_builder::~uri_builder() {

  }

  void uri_builder::set_scheme(const string_type &scheme) {
    // validate scheme is valid
    scheme_ = scheme;
  }

  void uri_builder::set_user_info(const string_type &user_info) {
    user_info_ = user_info;
  }

  void uri_builder::set_host(const string_type &host) {
    host_ = host;
  }

  void uri_builder::set_port(const string_type &port) {
    port_ = port;
  }

  void uri_builder::set_path(const string_type &path) {
    path_ = path;
  }

  void uri_builder::set_query(const string_type &query) {
    query_ = query;
  }

  void uri_builder::set_fragment(const string_type &fragment) {
    fragment_ = fragment;
  }

  network::uri uri_builder::uri() const {
      return network::uri();
  }
} // namespace network
