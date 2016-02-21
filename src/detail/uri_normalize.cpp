// Copyright 2013-2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <vector>
#include <algorithm>
#include "../boost/algorithm/string/split.hpp"
#include "../boost/algorithm/string/join.hpp"
#include "uri_normalize.hpp"
#include "uri_percent_encode.hpp"
#include "algorithm.hpp"

namespace network {
namespace detail {
uri::string_type normalize_path(uri::string_view path,
                                uri_comparison_level level) {
  uri::string_type result = path.to_string();

  if (uri_comparison_level::syntax_based == level) {
    // case normalization
    detail::for_each(result, percent_encoded_to_upper());

    // % encoding normalization
    result.erase(
        detail::decode_encoded_unreserved_chars(std::begin(result),
                                                std::end(result)),
        std::end(result));

    // % path segment normalization
    result = normalize_path_segments(result);
  }

  return result;
}

uri::string_type normalize_path_segments(uri::string_view path) {
  uri::string_type result;

  if (!path.empty()) {
    std::vector<uri::string_type> path_segments;
    network_boost::split(path_segments, path,
                 [](char ch) { return ch == '/'; });

    // remove single dot segments
    detail::remove_erase_if(
        path_segments, [](const uri::string_type& s) { return (s == "."); });

    // remove double dot segments
    std::vector<uri::string_type> normalized_segments;
    for (auto& segment : path_segments) {
      if (segment == "..") {
        if (normalized_segments.size() <= 1) {
          throw uri_builder_error();
        }
        normalized_segments.pop_back();
      }
      else {
        normalized_segments.push_back(segment);
      }
    }

    // remove adjacent slashes
    optional<uri::string_type> prev_segment;
    detail::remove_erase_if(
        normalized_segments, [&prev_segment](const uri::string_type& segment) {
          bool has_adjacent_slash =
              ((prev_segment && prev_segment->empty()) && segment.empty());
          if (!has_adjacent_slash) {
            prev_segment = segment;
          }
          return has_adjacent_slash;
        });

    result = network_boost::join(normalized_segments, "/");
  }

  if (result.empty()) {
    result = "/";
  }

  return result;
}
}  // namespace detail
}  // namespace network
