//              Copyright 2012 Glyn Matthews.
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include <network/uri/uri.ipp>
#include <network/uri/uri.hpp>
#include <network/uri/normalize.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <map>

namespace network {
bool operator == (const uri &lhs, const uri &rhs) {

	// if both URIs are empty, then we should define them as equal even though they're still invalid.
	if (boost::empty(lhs) && boost::empty(rhs)) {
		return true;
	}

	if (!valid(lhs) || !valid(rhs)) {
		return false;
	}

	// the scheme can be compared insensitive to case
	bool equal = boost::iequals(lhs.scheme_range(), rhs.scheme_range());
	if (equal) {
		// the user info must be case sensitive
		equal = boost::equals(lhs.user_info_range(), rhs.user_info_range());
	}

	if (equal)	{
		// the host can be compared insensitive to case
		equal = boost::iequals(lhs.host_range(), rhs.host_range());
	}

	if (equal) {
		if (lhs.port_range() && rhs.port_range()) {
			equal = boost::equals(lhs.port_range(), rhs.port_range());
		}
		else if (!lhs.port_range() && rhs.port_range()) {
			auto port = default_port(lhs.scheme());
			if (port) {
				equal = boost::equals(*port, rhs.port_range());
			}
		}
		else if (lhs.port_range() && !rhs.port_range()) {
			auto port = default_port(rhs.scheme());
			if (port) {
				equal = boost::equals(lhs.port_range(), *port);
			}
		}
	}

	if (equal) {
		// test normalized paths
		equal = boost::iequals(normalize_path(lhs.path_range()), normalize_path(rhs.path_range()));
	}

	if (equal) {
		// test query, independent of order
		std::map<uri::string_type, uri::string_type> lhs_query_params, rhs_query_params;
		equal = (query_map(lhs, lhs_query_params) == query_map(rhs, rhs_query_params));
	}

	return equal;
}
} // namespace network
