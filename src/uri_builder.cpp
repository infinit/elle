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

  uri_builder &uri_builder::base_uri(const network::uri &base) {
    return *this;
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
    std::string uri_;
    if (scheme_) {
      uri_.append(*scheme_);
    }

    if (user_info_ || host_ || port_) {
      uri_.append("://");

      if (user_info_) {
	uri_.append(*user_info_);
	uri_.append("@");
      }

      if (host_) {
	uri_.append(*host_);
      }
      else {
	std::error_code ec = make_error_code(uri_error::invalid_host);
	throw std::system_error(ec);
      }

      if (port_) {
	uri_.append(":");
	uri_.append(*port_);
      }
    }
    else {
      uri_.append(":");
    }

    if (path_) {
      uri_.append(*path_);
    }

    if (query_) {
      uri_.append("?");
      uri_.append(*query_);
    }

    if (fragment_) {
      uri_.append("#");
      uri_.append(*fragment_);
    }

    return network::uri(uri_);
  }
} // namespace network
