// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_DETAIL_URI_PERCENT_ENCODE_INC
#define NETWORK_DETAIL_URI_PERCENT_ENCODE_INC

#include <string>
#include <vector>
#include <cctype>
#include <boost/optional.hpp>
#include <network/uri/detail/decode.hpp>

namespace network {
  namespace detail {

    inline
    boost::optional<char> percent_encode(std::string s) {
      try {
	std::vector<char> output;
	detail::decode_char(std::begin(s), std::back_inserter(output));
	return output[0];
      }
      catch (percent_decoding_error &) {
	return boost::optional<char>();
      }
    }

    struct percent_encoded_to_upper {

      percent_encoded_to_upper()
	: count(0) {}

      void operator () (char &c) {
	if (c == '%') {
	  count = 2;
	}
	else if (count > 0) {
	  c = std::toupper(c);
	  --count;
	}
      }

      unsigned count;

    };

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

#endif // NETWORK_DETAIL_URI_PERCENT_ENCODE_INC
