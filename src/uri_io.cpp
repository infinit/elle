//            Copyright (c) Glyn Matthews 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <network/uri/uri_io.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <istream>
#include <ostream>
#include <iterator>


namespace network {
  std::ostream &operator << (std::ostream &os, const uri &uri_) {
    //return os << uri_.string();
    return os;
  }

  std::wostream &operator << (std::wostream &os, const uri &uri_) {
    //return os << uri_.wstring();
    return os;
  }
} // namespace network
