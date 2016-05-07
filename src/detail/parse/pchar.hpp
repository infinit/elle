// Copyright 2016 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PARSE_PCHAR_INC
#define NETWORK_DETAIL_URI_PARSE_PCHAR_INC

#include <network/string_view.hpp>
#include <cctype>
#include <cstring>

namespace network {
namespace detail {
inline bool is_sub_delim(char c) {
  return (c == '!') || (c == '$') || (c == '&') || (c == '\'') || (c == '(') ||
         (c == ')') || (c == '*') || (c == '+') || (c == ',') || (c == ';') ||
         (c == '=');
}

inline bool is_sub_delim(string_view::const_iterator &it) {
  if (is_sub_delim(*it)) {
    ++it;
    return true;
  }
  return false;
}

inline bool is_gen_delim(char c) {
  return (c == ':') || (c == '/') || (c == '?') || (c == '#') || (c == '[') ||
         (c == ']') || (c == '@');
}

inline bool is_reserved(char c) { return is_sub_delim(c) || is_gen_delim(c); }

inline bool is_unreserved(char c) {
  return std::isalnum(c) || (c == '-') || (c == '.') || (c == '_') ||
         (c == '~');
}

inline bool is_unreserved(string_view::const_iterator &it) {
  if (is_unreserved(*it)) {
    ++it;
    return true;
  }
  return false;
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

inline bool is_pchar(string_view::const_iterator &it) {
  return
    is_unreserved(it) ||
    is_pct_encoded(it) ||
    is_sub_delim(it) ||
    is_in(it, ":@")
          ;
}
}  // namespace detail
}  // namespace network

#endif  // NETWORK_DETAIL_URI_PARSE_PCHAR_INC
