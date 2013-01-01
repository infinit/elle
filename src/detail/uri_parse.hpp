// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef NETWORK_DETAIL_URI_PARSE_INC
#define NETWORK_DETAIL_URI_PARSE_INC

#include <network/uri/uri.hpp>
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
    };

    template <
      class FwdIter
      >
    struct uri_parts {
      boost::optional<boost::iterator_range<FwdIter> > scheme;
      hierarchical_part<FwdIter> hier_part;
      boost::optional<boost::iterator_range<FwdIter> > query;
      boost::optional<boost::iterator_range<FwdIter> > fragment;
    };

    bool parse(uri::string_type::iterator first,
	       uri::string_type::iterator last,
	       uri_parts<uri::string_type::iterator> &parts);
  } // namespace detail
} // namespace network

#endif // NETWORK_DETAIL_URI_PARSE_INC
