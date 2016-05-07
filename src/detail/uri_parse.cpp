// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_parse.hpp"
#include <iterator>
#include <cctype>
#include "uri_parts.hpp"

namespace network {
namespace detail {
namespace {
enum class uri_state {
  scheme,
  hier_part,
  query,
  fragment
};

enum class scheme_state {
  first_char,
  other
};

inline
bool is_valid_scheme_char(char c) {
  return ((std::isalnum(c) != 0) || (c == '+') || (c == '-') || (c == '.'));
}

enum class hier_part_state {
  user_info,
  host,
  port,
  path
};

struct print_helper {

  print_helper() {
    std::printf(" <<< ");
  }

  ~print_helper() {
    std::printf(" >>>\n");
  }

};
} // namespace


bool parse(uri::const_iterator &it, uri::const_iterator last, v2::uri_parts &parts) {
  auto state = uri_state::scheme;
  auto hp_state = hier_part_state::user_info;

  uri::const_iterator first = it;

  if (it == last) {
    return false;
  }

  // Scheme
  auto s_state = scheme_state::first_char;
  while (it != last) {
    // first char is alpha
    if (s_state == scheme_state::first_char) {
      if (std::isalpha(*it) == 0) {
        return false;
      }

      s_state = scheme_state::other;

    }
    else if (s_state == scheme_state::other) {
      if (*it == ':') {
        parts.scheme = std::make_pair(first, it);

        // move past the scheme delimiter
        ++it;
        state = uri_state::hier_part;
        break;
      }
      else if (!is_valid_scheme_char(*it)) {
        return false;
      }
    }

    ++it;
  }

  // Hierarchical part
  while (it != last) {
    if (hp_state == hier_part_state::user_info) {

      if (*it == '@') {
        parts.hier_part.user_info = std::make_pair(first, it);
      }

    }


    if (*it == '?') {

      // move past the query delimiter
      ++it;
      state = uri_state::query;
      continue;
    }
    else if (*it == '#') {
      // move past the fragment delimiter
      ++it;
      state = uri_state::fragment;
      continue;
    }
    ++it;
  }

  // Query
  if (state == uri_state::query) {
    while (it != last) {
      if (*it == '#') {
        // move past the fragment delimiter
        ++it;
        state = uri_state::fragment;
        continue;
      }
      ++it;
    }
  }

  // Fragment
  if (state == uri_state::fragment) {
    while (it != last) {

      ++it;
    }
  }

  return true;
}
}  // namespace detail
}  // namespace network
