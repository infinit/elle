#pragma once

#include <boost/optional.hpp>

#include <elle/printf.hh>

namespace boost
{
  template <typename T>
  std::ostream&
  operator <<(std::ostream& s, boost::optional<T> const& o)
  {
    if (o)
      elle::fprintf(s, "%s", o.get());
    else
      s << "null";
    return s;
  }
}
