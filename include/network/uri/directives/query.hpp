// Copyright (c) Glyn Matthews 2011, 2012.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_DIRECTIVES_QUERY_INC
#define NETWORK_URI_DIRECTIVES_QUERY_INC

#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>

namespace network {
struct query_directive {

    explicit query_directive(const std::string &query)
        : query(query)
    {}

    template <
        class Uri
        >
    void operator () (Uri &uri) const {
        uri.append("?");
        uri.append(query);
    }

    std::string query;

};

inline
query_directive query(const std::string &query)  {
    return query_directive(query);
}

struct query_key_query_directive {

    query_key_query_directive(const std::string &key, const std::string &query)
        : key(key), query(query)
    {}

    template <
        class Uri
        >
    void operator () (Uri &uri) const {
        std::string encoded_key, encoded_query;
        if (boost::empty(uri.query()))
        {
            uri.append("?");
        }
        else
        {
            uri.append("&");
        }
        uri.append(key);
        uri.append("=");
        uri.append(query);
    }

    std::string key;
    std::string query;

};

inline
query_key_query_directive query(const std::string &key, const std::string &query)  {
    return query_key_query_directive(key, query);
}
} // namespace network

#endif // NETWORK_URI_DIRECTIVES_QUERY_INC
