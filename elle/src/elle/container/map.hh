#ifndef ELLE_CONTAINER_MAP_HH
# define ELLE_CONTAINER_MAP_HH

# include <iosfwd>
# include <map>

namespace std
{
  template <class K, class V>
  ostream&
  operator <<(ostream& out, pair<K, V> const& p);

  template <class K, class V, class ...OTHER>
  std::ostream&
  operator <<(ostream& out, map<K, V, OTHER...> const& m);

  template <class K, class V, class ...OTHER>
  std::ostream&
  operator <<(ostream& out, multimap<K, V, OTHER...> const& m);
}

# include <elle/container/map.hxx>

#endif /* end of include guard: ELLE_CONTAINER_MAP_HH */
