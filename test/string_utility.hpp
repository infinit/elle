// Copyright 2013 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt of copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef URI_TEST_STRING_UTILITY_INC
#define URI_TEST_STRING_UTILITY_INC

#include <boost/utility/string_ref.hpp>
#include <cstring>

namespace boost {
  inline
  bool operator == (const char *lhs, string_ref rhs) {
    return string_ref(lhs, std::strlen(lhs)) == rhs;
  }
} // namespace boost

#endif // URI_TEST_STRING_UTILITY_INC
