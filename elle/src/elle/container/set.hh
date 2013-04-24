#ifndef ELLE_CONTAINER_SET_HH
# define ELLE_CONTAINER_SET_HH

# include <iosfwd>
# include <set>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator<<(ostream& out, set<T, OTHER...> const& v);
}

# include <elle/container/set.hxx>

#endif /* end of include guard: ELLE_CONTAINER_SET_HH */
