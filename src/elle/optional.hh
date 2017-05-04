#pragma once

#include <functional>

#include <boost/functional/hash.hpp>
#include <boost/optional.hpp>

#include <elle/printf.hh>

namespace boost
{
  /// Pretty printer for boost::optionals.
  template <typename T>
  std::ostream&
  operator <<(std::ostream& s, boost::optional<T> const& o)
  {
    if (o)
      elle::fprintf(s, "%s", o.get());
    else
      elle::fprintf(s, "null");
    return s;
  }

  template <typename T>
  std::size_t
  hash_value(boost::optional<T> const& address)
  {
    return address ? hash_value(address.get()) : 0;
  }
}

namespace std
{
  template<typename T>
  class hash<boost::optional<T>>
  {
  public:
    size_t
    operator()(boost::optional<T> const& o) const
    {
      return boost::hash_value(o);
    }
  };
}
