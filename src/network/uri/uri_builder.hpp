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

    uri_builder &base_uri(const network::uri &base_uri);

    template <typename Source>
    uri_builder &scheme(const Source &scheme) {
      set_scheme(detail::translate(scheme));
      return *this;
    }

    template <typename Source>
    uri_builder &user_info(const Source &user_info) {
      set_user_info(detail::translate(user_info));
      return *this;
    }

    template <typename Source>
    uri_builder &host(const Source &host) {
      set_host(detail::translate(host));
      return *this;
    }

    template <typename Source>
    uri_builder &port(const Source &port) {
      set_port(detail::translate(port));
      return *this;
    }

    template <typename Source>
    uri_builder &path(const Source &path) {
      set_path(detail::translate(path));
      return *this;
    }

    template <typename Source>
    uri_builder &query(const Source &query) {
      set_query(detail::translate(query));
      return *this;
    }

    template <typename Source>
    uri_builder &fragment(const Source &fragment) {
      set_fragment(detail::translate(fragment));
      return *this;
    }

    network::uri uri() const;

  private:

    void set_scheme(const string_type &scheme);
    void set_user_info(const string_type &user_info);
    void set_host(const string_type &host);
    void set_port(const string_type &port);
    void set_path(const string_type &path);
    void set_query(const string_type &query);
    void set_fragment(const string_type &fragment);

    boost::optional<network::uri> base_uri_;
    boost::optional<string_type> scheme_, user_info_, host_, port_, path_, query_, fragment_;

  };
} // namespace network


#endif // NETWORK_URI_BUILDER_INC
