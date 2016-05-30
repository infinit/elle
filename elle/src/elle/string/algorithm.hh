#ifndef ELLE_STRING_ALGORTIHM
#define ELLE_STRING_ALGORTIHM

#include <string>
#include <sstream>

namespace elle
{
  template <typename IT>
  std::string
  join(IT it, IT ite, std::string const& sep = ",");
}

#include <elle/string/algorithm.hxx>

#endif /* end of include guard: ELLE_STRING_ALGORTIHM */
