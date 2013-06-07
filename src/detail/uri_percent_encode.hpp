// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __NETWORK_DETAIL_URI_PERCENT_ENCODE_INC__
#define __NETWORK_DETAIL_URI_PERCENT_ENCODE_INC__

#include <string>
#include <cctype>
#include <boost/optional.hpp>

namespace network {
  namespace detail {

    boost::optional<char> percent_encode(std::string s);

    template <class Iter>
    void percent_encoding_to_upper(Iter first, Iter last) {
      auto it = first;
      while (it != last) {
	if (*it == '%') {
	  ++it; *it = std::toupper(*it);
	  ++it; *it = std::toupper(*it);
	}
	++it;
      }
    }

    template <class Iter>
    Iter decode_encoded_chars(Iter first, Iter last) {
      auto it = first, it2 = first;
      while (it != last) {
	if (*it == '%') {
	  auto sfirst = it, slast = it;
	  std::advance(slast, 3);
	  auto opt_char = percent_encode(std::string(sfirst, slast));
	  if (opt_char) {
	    *it2 = *opt_char;
	  }
	  ++it; ++it;
	}
	else {
	  *it2 = *it;
	}
	++it; ++it2;
      }
      return it2;
    }

  } // namespace detail
} // namespace network

#endif // __NETWORK_DETAIL_URI_PERCENT_ENCODE_INC__
