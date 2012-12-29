// Copyright (c) Glyn Matthews 2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <network/uri.hpp>


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

  struct uri_builder::impl {

    boost::optional<network::uri> base_uri_;
    boost::optional<string_type> scheme_, user_info_, host_, port_, path_, query_, fragment_;

  };

  uri_builder::uri_builder()
    : pimpl_(new impl) {

  }

  uri_builder::~uri_builder() {

  }

  uri_builder &uri_builder::base_uri(const network::uri &base_uri) {
    pimpl_->base_uri_ = base_uri;
    return *this;
  }

  void uri_builder::set_scheme(const string_type &scheme) {
    // validate scheme is valid
    pimpl_->scheme_ = scheme;
  }

  void uri_builder::set_user_info(const string_type &user_info) {
    pimpl_->user_info_ = user_info;
  }

  void uri_builder::set_host(const string_type &host) {
    pimpl_->host_ = host;
  }

  void uri_builder::set_port(const string_type &port) {
    pimpl_->port_ = port;
  }

  void uri_builder::set_path(const string_type &path) {
    pimpl_->path_ = path;
  }

  void uri_builder::set_query(const string_type &query) {
    pimpl_->query_ = query;
  }

  void uri_builder::set_fragment(const string_type &fragment) {
    pimpl_->fragment_ = fragment;
  }

  network::uri uri_builder::uri() const {
    return network::uri(pimpl_->scheme_,
			pimpl_->user_info_,
			pimpl_->host_,
			pimpl_->port_,
			pimpl_->path_,
			pimpl_->query_,
			pimpl_->fragment_);
  }
} // namespace network
