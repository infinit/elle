#include <elle/container/map.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <class K, class V>
  std::ostream&
  operator<<(ostream& out, pair<K, V> const &p)
  {
    return out << p.first <<  ": " << p.second;
  }

  template <class K, class V, class ...OTHER>
  std::ostream&
  operator<<(ostream& out, map<K, V, OTHER...> const &m)
  {
    return out << "{" << elle::join(begin(m), end(m), ", ") << "}";
  }
} /* std */
