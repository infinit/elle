// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_parse.hpp"
#include <iterator>
#include "uri_parts.hpp"
#include "parse/scheme.hpp"

namespace network {
namespace detail {
namespace {
enum class uri_state {
  scheme,
  hier_part,
  query,
  fragment
};

enum class hier_part_state {
  first_slash,
  second_slash,
  user_info,
  host,
  port,
  path
};
} // namespace

bool parse(uri::const_iterator &it, uri::const_iterator last, v2::uri_parts &parts) {
  auto state = uri_state::scheme;

  uri::const_iterator first = it;

  if (it == last) {
    return false;
  }

  if (parse_scheme(it, last, parts)) {
    state = uri_state::hier_part;
  }
  else {
    return false;
  }

  // Hierarchical part
  auto hp_state = hier_part_state::first_slash;
  while (it != last) {
    if (hp_state == hier_part_state::first_slash) {
      if (*it == '/') {
        hp_state = hier_part_state::second_slash;
        ++it;
        continue;
      }
      else {
        hp_state = hier_part_state::path;
        first = it;
      }
    }
    else if (hp_state == hier_part_state::second_slash) {
      if (*it == '/') {
        hp_state = hier_part_state::user_info;
        ++it;
        first = it;
        continue;
      }
      else {
        return false;
      }
    }
    else if (hp_state == hier_part_state::user_info) {
      if ((*first == '@') || (*first == ':')) {
        return false;
      }

      if (*it == '@') {
        parts.hier_part.user_info = std::make_pair(first, it);
        hp_state = hier_part_state::host;
        ++it;
        first = it;
        continue;
      }
      else if (*it == ':') {
        // this is actually the host, and the next part is expected to be the port
        parts.hier_part.host = std::make_pair(first, it);
        hp_state = hier_part_state::port;
        ++it;
        first = it;
        continue;
      }
      else if (*it == '/') {
        // we skipped right past the host and port, and are at the path.
        parts.hier_part.host = std::make_pair(first, it);
        hp_state = hier_part_state::path;
        first = it;
        continue;
      }
    }
    else if (hp_state == hier_part_state::host) {
      if (*first == ':') {
        return false;
      }

      if (*it == ':') {
        parts.hier_part.host = std::make_pair(first, it);
        hp_state = hier_part_state::port;
        ++it;
        first = it;
        continue;
      }
      else if (*it == '/') {
        parts.hier_part.host = std::make_pair(first, it);
        hp_state = hier_part_state::path;
        first = it;
        continue;
      }
    }
    else if (hp_state == hier_part_state::port) {
      if (*first == '/') {
        // the port isn't set, but the path is
        hp_state = hier_part_state::path;
        continue;
      }

      if (*it == '/') {
        parts.hier_part.port = std::make_pair(first, it);
        hp_state = hier_part_state::path;
        first = it;
        continue;
      }
    }
    else if (hp_state == hier_part_state::path) {
      if (*it == '?') {
        parts.hier_part.path = std::make_pair(first, it);
        // move past the query delimiter
        ++it;
        first = it;
        state = uri_state::query;
        break;
      }
      else if (*it == '#') {
        parts.hier_part.path = std::make_pair(first, it);
        // move past the fragment delimiter
        ++it;
        first = it;
        state = uri_state::fragment;
        break;
      }
    }

    ++it;
  }

  // Query
  if (state == uri_state::query) {
    while (it != last) {
      if (*it == '#') {
        parts.query = std::make_pair(first, it);
        // move past the fragment delimiter
        ++it;
        first = it;
        state = uri_state::fragment;
        continue;
      }
      ++it;
    }
  }

  // we're done!
  if (state == uri_state::hier_part) {
    if (hp_state == hier_part_state::user_info) {
      parts.hier_part.host = std::make_pair(first, last);
      parts.hier_part.path = std::make_pair(last, last);
    }
    else if (hp_state == hier_part_state::host) {
      parts.hier_part.host = std::make_pair(first, last);
      parts.hier_part.path = std::make_pair(last, last);
    }
    else if (hp_state == hier_part_state::port) {
      parts.hier_part.port = std::make_pair(first, last);
      parts.hier_part.path = std::make_pair(last, last);
    }
    else if (hp_state == hier_part_state::path) {
      parts.hier_part.path = std::make_pair(first, last);
    }
  }
  else if (state == uri_state::query) {
    parts.query = std::make_pair(first, last);
  }
  else if (state == uri_state::fragment) {
    parts.fragment = std::make_pair(first, last);
  }

  return true;
}
}  // namespace detail
}  // namespace network
