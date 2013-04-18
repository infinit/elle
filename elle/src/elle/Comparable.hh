#ifndef ELLE_COMPARABLE_HH
# define ELLE_COMPARABLE_HH

namespace elle
{
  template <typename Self, typename T = Self>
  class Comparable
  {
  public:
    bool
    operator == (T const& other) const;
    bool
    operator != (T const& other) const;
  };
}

# include <elle/Comparable.hxx>

#endif
