//            Copyright (c) Glyn Matthews 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


# include <network/uri/normalize.hpp>
# include <vector>
# include <stack>
# include <boost/algorithm/string/split.hpp>
# include <boost/algorithm/string/join.hpp>
# include <boost/algorithm/string/predicate.hpp>
# include <boost/algorithm/string/classification.hpp>
# include <boost/range/as_literal.hpp>
# include <boost/range/algorithm_ext/erase.hpp>
# include <boost/range/algorithm/for_each.hpp>

namespace network {
uri::string_type normalize_path(const uri::const_range_type &path) {
	using namespace boost;
	using namespace boost::algorithm;

	// add trailing /
	if (empty(path)) {
		return uri::string_type("/");
	}

	std::vector<uri::string_type> path_segments;
	split(path_segments, path, is_any_of("/"));

	// remove single dot segments
	remove_erase_if(path_segments, [] (const uri::string_type &s) {
			return equal(s, boost::as_literal("."));
		});

	// remove double dot segments
	std::vector<std::string> normalized_segments;
	auto depth = 0;
	for_each(path_segments, [&normalized_segments, &depth] (const uri::string_type &s) {
			assert(depth >= 0);
			if (equal(s, boost::as_literal(".."))) {
				normalized_segments.pop_back();
			}
			else {
				normalized_segments.push_back(s);
			}
		});

	if (!empty(normalized_segments.back()) &&
		!contains(normalized_segments.back(), as_literal("."))) {
		normalized_segments.push_back(uri::string_type());
	}

	return join(normalized_segments, "/");
}
} // namespace network
