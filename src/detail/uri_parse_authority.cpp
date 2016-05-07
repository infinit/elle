// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "uri_parse_authority.hpp"
#include <iterator>
#include <limits>
#include "uri_parts.hpp"
#include "parse/scheme.hpp"
#include "parse/pchar.hpp"

namespace network {
namespace detail {
namespace {
enum class authority_state {
  user_info,
  host,
  host_ipv6,
  port
};

inline bool is_valid_port(iterator_pair port) {
  const char* port_first = &(*port.first);
  char* port_last = 0;
  unsigned long value = std::strtoul(port_first, &port_last, 10);
  return (value < std::numeric_limits<unsigned short>::max());
}
}  // namespace

bool parse_authority(string_view::const_iterator &it,
                     string_view::const_iterator last,
                     optional<iterator_pair> &user_info,
                     optional<iterator_pair> &host,
                     optional<iterator_pair> &port) {
  auto first = it;

  auto state = authority_state::user_info;
  while (it != last) {
    if (state == authority_state::user_info) {
      if (is_in(first, "@:")) {
        return false;
      }

      if (*it == '@') {
        user_info = std::make_pair(first, it);
        state = authority_state::host;
        ++it;
        first = it;
        continue;
      }
      else if (*it == '[') {
        // this is an IPv6 address
        state = authority_state::host_ipv6;
        first = it;
        continue;
      }
      else if (*it == ':') {
        // this is actually the host, and the next part is expected to be the port
        host = std::make_pair(first, it);
        state = authority_state::port;
        ++it;
        first = it;
        continue;
      }
    }
    else if (state == authority_state::host) {
      if (*first == ':') {
        return false;
      }

      if (*it == ':') {
        host = std::make_pair(first, it);
        state = authority_state::port;
        ++it;
        first = it;
        continue;
      }
    }
    else if (state == authority_state::host_ipv6) {
      if (*first != '[') {
        return false;
      }

      if (*it == ']') {
        host = std::make_pair(first, it);
        ++it;
        // Then test if the next part is a host, part, or the end of the file
        if (it == last) {
          break;
        }
        else if (*it == ':') {
          host = std::make_pair(first, it);
          state = authority_state::port;
          ++it;
          first = it;
        }
      }
    }
    else if (state == authority_state::port) {
      if (*first == '/') {
        // the port is empty, but valid
        port = std::make_pair(first, it);
        if (!is_valid_port(*port)) {
          return false;
        }

        continue;
      }

      if (!isdigit(it)) {
        return false;
      }
    }

    ++it;
  }

  if (state == authority_state::user_info) {
    host = std::make_pair(first, last);
  }
  else if (state == authority_state::host) {
    host = std::make_pair(first, last);
  }
  else if (state == authority_state::host_ipv6) {
    host = std::make_pair(first, last);
  }
  else if (state == authority_state::port) {
    port = std::make_pair(first, last);
    if (!is_valid_port(*port)) {
      return false;
    }
  }

  return true;
}
}  // namespace detail
}  // namespace network
