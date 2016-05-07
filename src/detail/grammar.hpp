// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_GRAMMAR_INC
#define NETWORK_DETAIL_URI_GRAMMAR_INC

#include <network/string_view.hpp>
#include <cstdlib>
#include <cctype>
#include <cstring>

namespace network {
namespace detail {
inline bool isalnum(string_view::const_iterator &it) {
  if (std::isalnum(*it) != 0) {
    ++it;
    return true;
  }
  return false;
}

inline bool isdigit(string_view::const_iterator &it) {
  if (std::isdigit(*it) != 0) {
    ++it;
    return true;
  }
  return false;
}

inline bool is_in(string_view::const_iterator &it, const char *chars) {
  auto length = std::strlen(chars);
  for (std::size_t i = 0; i < length; ++i) {
    if (*it == chars[i]) {
      ++it;
      return true;
    }
  }
  return false;
}

inline bool is_sub_delim(string_view::const_iterator &it) {
  return is_in(it, "!$&'()*+,;=");
}

inline bool is_unreserved(string_view::const_iterator &it) {
  return isalnum(it) || is_in(it, "-._~");
}

inline bool is_pct_encoded(string_view::const_iterator &it) {
  if (*it == '%') {
    ++it;
    if (std::isxdigit(*it) != 0) {
      ++it;
      if (std::isxdigit(*it) != 0) {
        ++it;
        return true;
      }
    }
  }
  return false;
}

inline bool is_pchar(string_view::const_iterator &it) {
  return
    is_unreserved(it) ||
    is_pct_encoded(it) ||
    is_sub_delim(it) ||
    is_in(it, ":@")
          ;
}

inline bool is_valid_scheme(string_view::const_iterator &it) {
  return isalnum(it) || is_in(it, "+-.");
}

inline bool is_valid_user_info(string_view::const_iterator &it) {
  return
    is_unreserved(it) ||
    is_pct_encoded(it) ||
    is_sub_delim(it) ||
    is_in(it, ":")
    ;
}

inline bool is_valid_port(string_view::const_iterator it) {
  const char* port_first = &(*it);
  char* port_last = 0;
  unsigned long value = std::strtoul(port_first, &port_last, 10);
  return (value < std::numeric_limits<unsigned short>::max());
}
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_URI_GRAMMAR_INC
