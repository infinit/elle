// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_parse.hpp"
#include <iterator>
#include <limits>
#include "uri_parts.hpp"
#include "parse/scheme.hpp"
#include "parse/pchar.hpp"

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
  host_ipv6,
  port,
  path
};

inline bool is_valid_port(v2::iterator_pair port) {
  const char* port_first = &(*port.first);
  char* port_last = 0;
  unsigned long value = std::strtoul(port_first, &port_last, 10);
  return (value < std::numeric_limits<unsigned short>::max());
}
} // namespace

bool parse(string_view::const_iterator &it, string_view::const_iterator last, v2::uri_parts &parts) {
  auto state = uri_state::scheme;

  auto first = it;

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
        // set the first iterator in case the second slash is not forthcoming
        first = it;
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
        // it's a valid URI, and this is the beginning of the path
        hp_state = hier_part_state::path;
      }
    }
    else if (hp_state == hier_part_state::user_info) {
      if (is_in(first, "@:")) {
        return false;
      }

      if (*it == '@') {
        parts.hier_part.user_info = std::make_pair(first, it);
        hp_state = hier_part_state::host;
        ++it;
        first = it;
        continue;
      }
      else if (*it == '[') {
        // this is an IPv6 address
        hp_state = hier_part_state::host_ipv6;
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
    else if (hp_state == hier_part_state::host_ipv6) {
      if (*first != '[') {
        return false;
      }

      if (*it == ']') {
        ++it;
        // Then test if the next part is a host, part, or the end of the file
        if (it == last) {
          break;
        }
        else if (*it == ':') {
          parts.hier_part.host = std::make_pair(first, it);
          hp_state = hier_part_state::port;
          ++it;
          first = it;
        }
        else if (*it == '/') {
          parts.hier_part.host = std::make_pair(first, it);
          hp_state = hier_part_state::path;
          first = it;
        }
        continue;
      }
    }
    else if (hp_state == hier_part_state::port) {
      if (*first == '/') {
        // the port is empty, but valid
        parts.hier_part.port = std::make_pair(first, it);
        if (!is_valid_port(*parts.hier_part.port)) {
          return false;
        }

        // the port isn't set, but the path is
        hp_state = hier_part_state::path;
        continue;
      }

      if (*it == '/') {
        parts.hier_part.port = std::make_pair(first, it);
        if (!is_valid_port(*parts.hier_part.port)) {
          return false;
        }
        hp_state = hier_part_state::path;
        first = it;
        continue;
      }
      else if (!isdigit(it)) {
        return false;
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

  if (state == uri_state::query) {
    while (it != last) {
      if (!is_pchar(it) && !is_in(it, "?/")) {
        // If this is a fragment, keep going
        if (*it == '#') {
          parts.query = std::make_pair(first, it);
          // move past the fragment delimiter
          ++it;
          first = it;
          state = uri_state::fragment;
          continue;
        }
        else {
          return false;
        }
      }
    }
  }

  if (state == uri_state::fragment) {
    while (it != last) {
      if (!is_pchar(it) && !is_in(it, "?/")) {
        return false;
      }
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
    else if (hp_state == hier_part_state::host_ipv6) {
      parts.hier_part.host = std::make_pair(first, last);
      parts.hier_part.path = std::make_pair(last, last);
    }
    else if (hp_state == hier_part_state::port) {
      parts.hier_part.port = std::make_pair(first, last);
      if (!is_valid_port(*parts.hier_part.port)) {
        return false;
      }
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
