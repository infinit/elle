// Copyright 2013, 2014 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <string>
#include <network/uri/uri_errors.hpp>

namespace network {

  class uri_category_impl : public std::error_category {

  public:

    uri_category_impl() = default;

    virtual ~uri_category_impl() NETWORK_URI_NOEXCEPT;

    virtual const char *name() const NETWORK_URI_NOEXCEPT;

    virtual std::string message(int ev) const;

  };

  uri_category_impl::~uri_category_impl() NETWORK_URI_NOEXCEPT {}

  const char *uri_category_impl::name() const NETWORK_URI_NOEXCEPT {
    static const char name[] = "uri_error";
    return name;
  }

  std::string uri_category_impl::message(int ev) const {
    switch (uri_error(ev)) {
      case uri_error::invalid_syntax:
        return "Unable to parse URI string.";
      case uri_error::not_enough_input:
        return "Percent decoding: Not enough input.";
      case uri_error::non_hex_input:
        return "Percent decoding: Non-hex input.";
      case uri_error::conversion_failed:
        return "Percent decoding: Conversion failed.";
    default:
        break;
    }
    return "Unknown URI error.";
  }

  const std::error_category &uri_category() {
    static uri_category_impl uri_category;
    return uri_category;
  }

  std::error_code make_error_code(uri_error e) {
    return std::error_code(static_cast<int>(e), uri_category());
  }

  uri_syntax_error::uri_syntax_error()
      : std::system_error(make_error_code(uri_error::invalid_syntax)) {}

  uri_syntax_error::~uri_syntax_error() NETWORK_URI_NOEXCEPT {}

  uri_builder_error::uri_builder_error()
      : std::system_error(make_error_code(uri_error::invalid_uri)) {}

  uri_builder_error::~uri_builder_error() NETWORK_URI_NOEXCEPT {}

  percent_decoding_error::percent_decoding_error(uri_error error)
      : std::system_error(make_error_code(error)) {}

  percent_decoding_error::~percent_decoding_error() NETWORK_URI_NOEXCEPT {}

}  // namespace network
