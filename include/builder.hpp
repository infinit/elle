// Copyright (c) Glyn Matthews 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_URI_BUILDER_INC
#define NETWORK_URI_BUILDER_INC

#include <boost/asio/ip/address.hpp>

namespace network {
class builder {

    typedef uri::string_type string_type;

public:

    builder(uri &uri_)
        : uri_(uri_) {

    }

    builder &scheme(const string_type &scheme) {
        uri_.uri_.append(scheme);
        if (opaque_schemes::exists(scheme)) {
            uri_.uri_.append(":");
        }
        else {
            uri_.uri_.append("://");
        }
        uri_.parse();
        return *this;
    }

    builder &user_info(const string_type &user_info) {
        uri_.uri_.append(user_info);
        uri_.uri_.append("@");
        uri_.parse();
        return *this;
    }

    builder &host(const string_type &host) {
        uri_.uri_.append(host);
        uri_.parse();
        return *this;
    }

    builder &host(const boost::asio::ip::address &host) {
        uri_.uri_.append(host.to_string());
        uri_.parse();
        return *this;
    }

    builder &host(const boost::asio::ip::address_v4 &host) {
        uri_.uri_.append(host.to_string());
        uri_.parse();
        return *this;
    }

    builder &host(const boost::asio::ip::address_v6 &host) {
        uri_.uri_.append("[");
        uri_.uri_.append(host.to_string());
        uri_.uri_.append("]");
        uri_.parse();
        return *this;
    }

    builder &port(const string_type &port) {
        uri_.uri_.append(":");
        uri_.uri_.append(port);
        uri_.parse();
        return *this;
    }

    builder &port(uint16_t port) {
        return this->port(boost::lexical_cast<string_type>(port));
    }

    builder &path(const string_type &path) {
        uri_.uri_.append(path);
        uri_.parse();
        return *this;
    }

    builder &encoded_path(const string_type &path) {
        string_type encoded_path;
        encode(path, std::back_inserter(encoded_path));
        return this->path(encoded_path);
    }

    builder &query(const string_type &query) {
        uri_.uri_.append("?");
        uri_.uri_.append(query);
        uri_.parse();
        return *this;
    }

    builder &query(const string_type &key, const string_type &value) {
        if (!uri_.query_range())
        {
            uri_.uri_.append("?");
        }
        else
        {
            uri_.uri_.append("&");
        }
        uri_.uri_.append(key);
        uri_.uri_.append("=");
        uri_.uri_.append(value);
        uri_.parse();
        return *this;
    }

    builder &fragment(const string_type &fragment) {
        uri_.uri_.append("#");
        uri_.uri_.append(fragment);
        uri_.parse();
        return *this;
    }

private:

    uri &uri_;

};
} // namespace network


#endif // NETWORK_URI_BUILDER_INC
