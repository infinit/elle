// Copyright 2009-2012 Dean Michael Berris, Jeroen Habraken, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URL_DETAIL_URL_PARTS_HPP_
#define NETWORK_URL_DETAIL_URL_PARTS_HPP_


#include <boost/range/iterator_range.hpp>
#include <boost/optional.hpp>

namespace network {
namespace detail {
template <
    class FwdIter
    >
struct hierarchical_part {
    boost::optional<boost::iterator_range<FwdIter> > user_info;
    boost::optional<boost::iterator_range<FwdIter> > host;
    boost::optional<boost::iterator_range<FwdIter> > port;
    boost::optional<boost::iterator_range<FwdIter> > path;

    FwdIter begin() const {
        return boost::begin(user_info);
    }

    FwdIter end() const {
        return boost::end(path);
    }

    void update() {
        if (!user_info) {
            if (host) {
                user_info = boost::iterator_range<FwdIter>(boost::begin(host.get()),
                                                           boost::begin(host.get()));
            }
            else if (path) {
                user_info = boost::iterator_range<FwdIter>(boost::begin(path.get()),
                                                           boost::begin(path.get()));
            }
        }

        if (!host) {
            host = boost::iterator_range<FwdIter>(boost::begin(path.get()),
                                                  boost::begin(path.get()));
        }

        if (!port) {
            port = boost::iterator_range<FwdIter>(boost::end(host.get()),
                                                  boost::end(host.get()));
        }

        if (!path) {
            path = boost::iterator_range<FwdIter>(boost::end(port.get()),
                                                  boost::end(port.get()));
        }
    }

};

template <
    class FwdIter
    >
struct uri_parts {
boost::iterator_range<FwdIter> scheme;
    hierarchical_part<FwdIter> hier_part;
    boost::optional<boost::iterator_range<FwdIter> > query;
    boost::optional<boost::iterator_range<FwdIter> > fragment;

    FwdIter begin() const {
        return boost::begin(scheme);
    }

    FwdIter end() const {
        return boost::end(fragment);
    }

    void update() {

        hier_part.update();

        if (!query) {
            query = boost::iterator_range<FwdIter>(boost::end(hier_part.path.get()),
                                                   boost::end(hier_part.path.get()));
        }

        if (!fragment) {
            fragment = boost::iterator_range<FwdIter>(boost::end(query.get()),
                                                      boost::end(query.get()));
        }
    }
};
} // namespace detail
} // namespace network


#endif // NETWORK_URL_DETAIL_URL_PARTS_HPP_
