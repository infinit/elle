// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_ERRORS_INC
#define NETWORK_URI_ERRORS_INC

#include <network/uri/config.hpp>
#include <system_error>

namespace network {

  enum class uri_error {
    // parser errors
    invalid_syntax = 1,

    // builder errors
    invalid_uri,
    invalid_scheme,
    invalid_user_info,
    invalid_host,
    invalid_port,
    invalid_path,
    invalid_query,
    invalid_fragment,

    // encoding errors
    not_enough_input,
    non_hex_input,
  };

  class NETWORK_URI_DECL uri_category_impl : public std::error_category {

  public:

    uri_category_impl() NETWORK_URI_DEFAULTED_FUNCTION;

    virtual ~uri_category_impl() NETWORK_URI_NOEXCEPT;

    virtual const char *name() const NETWORK_URI_NOEXCEPT;

    virtual std::string message(int ev) const;

  };

  const std::error_category &uri_category();

  std::error_code make_error_code(uri_error e);

  class uri_syntax_error : public std::system_error {

  public:

    uri_syntax_error();

    virtual ~uri_syntax_error() NETWORK_URI_NOEXCEPT;

  };

  class uri_builder_error : public std::system_error {

  public:

    uri_builder_error();

    virtual ~uri_builder_error() NETWORK_URI_NOEXCEPT;

  };

  class percent_decoding_error : public std::system_error {

  public:

    explicit percent_decoding_error(uri_error error);

    virtual ~percent_decoding_error() NETWORK_URI_NOEXCEPT;

  };
} // namespace network


#endif // NETWORK_URI_ERRORS_INC
