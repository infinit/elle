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

  public:

    typedef uri::string_type string_type;

    uri_builder() {

    }

    ~uri_builder() {

    }

    uri_builder(const uri_builder &);
    uri_builder &operator = (const uri_builder &);

    uri_builder &scheme(const string_type &scheme) {
      //uri_.uri_.append(scheme);
      //if (opaque_schemes::exists(scheme)) {
      //	uri_.uri_.append(":");
      //}
      //else {
      //	uri_.uri_.append("://");
      //}
      //uri_.parse();
      return *this;
    }

    uri_builder &host(const string_type &host) {
      //uri_.uri_.append(host);
      //uri_.parse();
      return *this;
    }

    uri_builder &path(const string_type &path) {
      //uri_.uri_.append(path);
      //uri_.parse();
      return *this;
    }

    network::uri uri() const;

  private:

    uri::string_type scheme_, user_name, host_, port_, path_, query_, fragment_;

  };
} // namespace network


#endif // NETWORK_URI_BUILDER_INC
