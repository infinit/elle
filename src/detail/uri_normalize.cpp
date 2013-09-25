// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <vector>
#include <boost/optional.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/range/as_literal.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include "uri_normalize.hpp"
#include "uri_percent_encode.hpp"

namespace network {
  namespace detail {
    uri::string_type normalize_path(uri::string_view path,
                                    uri_comparison_level level) {
      uri::string_type normalized(std::begin(path), std::end(path));

      if (uri_comparison_level::syntax_based == level) {
        // case normalization
        boost::for_each(normalized, percent_encoded_to_upper());

        // % encoding normalization
        normalized.erase(detail::decode_encoded_chars(std::begin(normalized),
                                                      std::end(normalized)),
                         std::end(normalized));

        // % path segment normalization
        normalized = normalize_path_segments(normalized);
      }

      return normalized;
    }

    uri::string_type normalize_path_segments(uri::string_view path) {
      using namespace boost;
      using namespace boost::algorithm;

      uri::string_type normalized;
      if (!path.empty()) {
        std::vector<uri::string_type> path_segments;
        boost::split(path_segments, path, is_any_of("/"));

        // remove single dot segments
        boost::remove_erase_if(path_segments, [](const uri::string_type &
                                                 s) { return (s == "."); });

        // remove double dot segments
        std::vector<uri::string_type> normalized_segments;
        boost::for_each(path_segments,
                        [&normalized_segments](const uri::string_type & s) {
          if (s == "..") {
            // in a valid path, the minimum number of segments is 1
            if (normalized_segments.size() <= 1) {
              throw uri_builder_error();
            }

            normalized_segments.pop_back();
          } else {
            normalized_segments.push_back(s);
          }
        });

        // remove adjacent slashes
        boost::optional<uri::string_type> prev_segment;
        boost::remove_erase_if(
            normalized_segments,
            [&prev_segment](const uri::string_type & segment) {
              bool has_adjacent_slash =
                  ((prev_segment && prev_segment->empty()) && segment.empty());
              if (!has_adjacent_slash) {
                prev_segment.reset(segment);
              }
              return has_adjacent_slash;
            });

        normalized = boost::join(normalized_segments, "/");
      }

      if (normalized.empty()) {
        normalized = "/";
      }

      return normalized;
    }
  }  // namespace detail
}  // namespace network
