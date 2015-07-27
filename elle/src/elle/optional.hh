#ifndef ELLE_OPTIONAL_HH
# define ELLE_OPTIONAL_HH

# include <boost/optional.hpp>

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

#endif
