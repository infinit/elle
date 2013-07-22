// Copyright (c) Glyn Matthews 2011, 2012, 2013.
// Copyright 2012 Dean Michael Berris <dberris@google.com>
// Copyright 2012 Google, Inc.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_URI_DECODE_INC
#define NETWORK_URI_DECODE_INC

#include <network/uri/uri_errors.hpp>
#include <iterator>
#include <cassert>

namespace network {
  namespace detail {
    template <
      typename CharT
      >
    CharT letter_to_hex(CharT in) {
      if ((in >= '0') && (in <= '9')) {
	return in - '0';
      }

      if ((in >= 'a') && (in <= 'f')) {
        return in + 10 - 'a';
      }

      if ((in >= 'A') && (in <= 'F')) {
	return in + 10 - 'A';
      }

      throw uri_encoding_error(uri_error::decoding_error);
    }

    template <
      class InputIterator,
      typename CharT
      >
    InputIterator decode_char(InputIterator it, CharT &v) {
      assert(*it == '%');
      ++it;
      auto v0 = detail::letter_to_hex(*it);
      ++it;
      auto v1 = detail::letter_to_hex(*it);
      ++it;
      v = (0x10 * v0) + v1;
      return it;
    }

    template <
      class InputIterator,
      class OutputIterator
      >
    OutputIterator decode(InputIterator in_begin,
			  InputIterator in_end,
			  OutputIterator out_begin) {
      auto it = in_begin;
      auto out = out_begin;
      while (it != in_end) {
	if (*it == '%') {
	  it = decode_char(it, *out);
	  ++out;
	}
	else {
	  *out++ = *it++;
	}
      }
      return out;
    }

    template <
      class String
      >
    String decode(const String &source) {
      String unencoded;
      decode(std::begin(source), std::end(source),
	     std::back_inserter(unencoded));
      return unencoded;
    }
  } // namespace detail
} // namespace network

#endif // NETWORK_URI_DECODE_INC
