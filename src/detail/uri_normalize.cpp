// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_normalize.hpp"
#include "uri_percent_encode.hpp"

namespace network {
  namespace detail {
    uri::string_type normalize_path(uri::string_type path, uri_comparison_level level) {
      if ((uri_comparison_level::case_normalization == level) ||
	  (uri_comparison_level::percent_encoding_normalization == level) ||
	  (uri_comparison_level::path_segment_normalization == level)) {
	detail::percent_encoding_to_upper(std::begin(path), std::end(path));
      }

      if ((uri_comparison_level::percent_encoding_normalization == level) ||
	  (uri_comparison_level::path_segment_normalization == level)) {
	path.erase(detail::decode_encoded_chars(std::begin(path), std::end(path)), std::end(path));
      }

      if (uri_comparison_level::path_segment_normalization == level) {
	normalize_path_segments(path);
      }

      return std::move(path);
    }
  } // namespace detail
} // namespace network

