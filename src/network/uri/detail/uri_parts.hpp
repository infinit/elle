// Copyright 2009-2010 Jeroen Habraken.
// Copyright 2009-2012 Dean Michael Berris, Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URL_DETAIL_URL_PARTS_HPP_
#define NETWORK_URL_DETAIL_URL_PARTS_HPP_


#include <boost/range/iterator_range.hpp>
#include <boost/optional.hpp>
#include <iterator>

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
        return std::begin(user_info);
      }

      FwdIter end() const {
        return std::end(path);
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
        return std::begin(scheme);
      }

      FwdIter end() const {
        return std::end(fragment);
      }

    };
  } // namespace detail
} // namespace network


#endif // NETWORK_URL_DETAIL_URL_PARTS_HPP_
