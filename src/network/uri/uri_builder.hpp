// Copyright (c) Glyn Matthews 2012, 2013.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_BUILDER_INC
#define NETWORK_URI_BUILDER_INC

#include <cstdint>
#include <utility>
#include <type_traits>
#include <boost/asio/ip/address.hpp>
#include <boost/filesystem/path.hpp>

namespace network {
  namespace detail {

    template <class T, class Enable = void>
    struct host_converter {

      uri::string_type operator () (const T &host) const {
	return detail::translate(host);
      }

    };

    template <class T>
    struct host_converter<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, boost::asio::ip::address_v4>::value>::type> {

      uri::string_type operator () (const T &host) const {
    	return host.to_string();
      }

    };

    template <class T>
    struct host_converter<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, boost::asio::ip::address_v6>::value>::type> {

      uri::string_type operator () (const T &host) const {
	uri::string_type host_str;
	host_str.append("[");
	host_str.append(host.to_string());
	host_str.append("]");
	return host_str;
      }

    };

    template <class T, class Enable = void>
    struct port_converter {

      uri::string_type operator () (const T &port) const {
	return detail::translate(port);
      }

    };

    template <class T>
    struct port_converter<T, typename std::enable_if<std::is_integral<typename std::decay<T>::type>::value>::type> {

      uri::string_type operator () (std::uint16_t port) const {
	return std::to_string(port);
      }

    };

    template <class T, class Enable = void>
    struct path_converter {

      uri::string_type operator () (const T &path) const {
	return detail::translate(path);
      }

    };

    template <class T>
    struct path_converter<T, typename std::enable_if<std::is_same<typename std::decay<T>::type, boost::filesystem::path>::value>::type> {

      uri::string_type operator () (const T &path) const {
	return path.string();
      }

    };

  } // namespace detail

  class NETWORK_URI_DECL uri_builder {

    friend class uri;

    uri_builder(const uri_builder &) NETWORK_URI_DELETED_FUNCTION;
    uri_builder &operator = (const uri_builder &) NETWORK_URI_DELETED_FUNCTION;

  public:

    typedef network::uri::string_type string_type;

    uri_builder() NETWORK_URI_DEFAULTED_FUNCTION;

    explicit uri_builder(const uri &base_uri);

    ~uri_builder() NETWORK_URI_NOEXCEPT;

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
      detail::host_converter<Source> convert;
      set_host(convert(host));
      return *this;
    }

    template <typename Source>
    uri_builder &port(const Source &port) {
      detail::port_converter<Source> convert;
      set_port(convert(port));
      return *this;
    }

    template <typename Source>
    uri_builder &authority(const Source &authority) {
      set_authority(detail::translate(authority));
      return *this;
    }

    template <typename Source>
    uri_builder &path(const Source &path) {
      detail::path_converter<Source> convert;
      set_path(convert(path));
      return *this;
    }

    template <typename Source>
    uri_builder &query(const Source &query) {
      set_query(detail::translate(query));
      return *this;
    }

    template <typename Key, typename Value>
    uri_builder &query(const Key &key, const Value &value) {
      if (!query_) {
	query_.reset(string_type());
      }
      else {
	query_->append("&");
      }
      query_->append(detail::translate(key));
      query_->append("=");
      query_->append(detail::translate(value));
      return *this;
    }

    template <typename Source>
    uri_builder &fragment(const Source &fragment) {
      set_fragment(detail::translate(fragment));
      return *this;
    }

    network::uri uri() const;

  private:

    void set_scheme(string_type scheme);
    void set_user_info(string_type user_info);
    void set_host(string_type host);
    void set_port(string_type port);
    void set_path(string_type path);
    void set_query(string_type query);
    void set_fragment(string_type fragment);

    boost::optional<string_type> scheme_, user_info_, host_, port_, path_, query_, fragment_;

  };
} // namespace network


#endif // NETWORK_URI_BUILDER_INC
