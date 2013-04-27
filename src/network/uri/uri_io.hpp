// Copyright (c) Glyn Matthews 2011, 2012, 2013.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_URI_IO_INC
#define NETWORK_URI_URI_IO_INC

#include <network/uri/uri.hpp>
#include <iosfwd>

namespace network {
  template <typename CharT, class CharTraits = std::char_traits<CharT> >
  std::basic_ostream<CharT, CharTraits> &operator << (std::basic_ostream<CharT, CharTraits> &os,
						      const uri &uri_) {
    return os << uri_.string<CharT, CharTraits>();
  }

  template <typename CharT, class CharTraits = std::char_traits<CharT> >
  std::basic_istream<CharT, CharTraits> &operator >> (std::basic_istream<CharT, CharTraits> &is,
						      uri &uri_) {
    std::basic_string<CharT, CharTraits> uri_string;
    is >> uri_string;
    uri_ = uri(uri_string);
  }
} // namespace network

#endif // NETWORK_URI_URI_IO_INC
