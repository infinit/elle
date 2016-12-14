#ifndef ELLE_STRING_ALGORTIHM
#define ELLE_STRING_ALGORTIHM

#include <string>
#include <sstream>

namespace elle
{
  template <typename IT>
  std::string
  join(IT it, IT ite,
       std::string const& sep = ",",
       std::function<std::string (typename IT::value_type const&)> const& t = {});

  template <typename C>
  std::string
  join(C const& container,
       std::string const& sep = ",",
       std::function<std::string (typename C::const_reference)> const& t = {});
}

#include <elle/string/algorithm.hxx>

#endif /* end of include guard: ELLE_STRING_ALGORTIHM */
