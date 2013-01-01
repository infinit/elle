// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef NETWORK_STRING_REF_IO_INC
#define NETWORK_STRING_REF_IO_INC

#include <network/utility/string_ref.hpp>
#include <ostream>

namespace network {
  inline
  std::ostream &operator << (std::ostream &os, const string_ref &sr) {
    return os << std::string(std::begin(sr), std::end(sr));
  }

  inline
  std::wostream &operator << (std::wostream &os, const wstring_ref &sr) {
    return os << std::wstring(std::begin(sr), std::end(sr));
  }
} // namespace network

#endif // NETWORK_STRING_REF_IO_INC
