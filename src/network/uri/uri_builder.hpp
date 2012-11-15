// Copyright (c) Glyn Matthews 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_BUILDER_INC
#define NETWORK_URI_BUILDER_INC


namespace network {
  class uri_builder {

    uri_builder(const uri_builder &); // = delete;
    uri_builder &operator = (const uri_builder &); // = delete;

  public:

    typedef uri::string_type string_type;

    uri_builder(); // = default;
    ~uri_builder(); // = default;

    template <typename Source>
    uri_builder &scheme(const Source &scheme) {
      return *this;
    }

    template <typename Source>
    uri_builder &host(const Source &host) {
      return *this;
    }

    template <typename Source>
    uri_builder &path(const Source &path) {
      return *this;
    }

    network::uri uri() const;

  private:

    string_type scheme_, user_name, host_, port_, path_, query_, fragment_;

  };
} // namespace network


#endif // NETWORK_URI_BUILDER_INC
