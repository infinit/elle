// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "scheme.hpp"
#include "pchar.hpp"
#include <cctype>

namespace network {
namespace detail {
namespace {
enum class scheme_state { first_char, other };

inline bool is_valid_scheme_char(string_view::const_iterator &it) {
  return isalnum(it) || is_in(it, "+-.");
}
}  // namespace

bool parse_scheme(uri::const_iterator &it, uri::const_iterator last,
                  v2::uri_parts &parts) {
  auto state = scheme_state::first_char;

  if (it == last) {
    return false;
  }

  auto first = it;
  while (it != last) {
    // first char is alpha
    if (state == scheme_state::first_char) {
      if (std::isalpha(*it) == 0) {
        return false;
      }

      state = scheme_state::other;
    }
    else if (state == scheme_state::other) {
      if (*it == ':') {
        parts.scheme = std::make_pair(first, it);

        // move past the scheme delimiter
        ++it;
        break;
      }
      else if (!is_valid_scheme_char(it)) {
        return false;
      }
    }
    // ++it;
  }

  return true;
}
}  // namespace detail
}  // namespace network
