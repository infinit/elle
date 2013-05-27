#include <elle/container/map.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <class K, class V>
  std::ostream&
  operator <<(ostream& out,
              pair<K, V> const& p)
  {
    return out << p.first <<  ": " << p.second;
  }

  template <class K, class V, class ...OTHER>
  std::ostream&
  operator <<(ostream& out,
              map<K, V, OTHER...> const& m)
  {
    return out << "{" << elle::join(begin(m), end(m), ", ") << "}";
  }

  template <class K, class V, class ...OTHER>
  std::ostream&
  operator <<(ostream& out, unordered_map<K, V, OTHER...> const& m)
  {
    return out << "{" << elle::join(begin(m), end(m), ", ") << "}";
  }

  template <class K, class V, class ...OTHER>
  std::ostream&
  operator <<(std::ostream& out, multimap<K, V, OTHER...> const& m)
  {
    int n = 0;

    out << "+{";
    for (auto it = m.begin(), ite = m.end();
         it != ite;
         it = m.upper_bound(it->first))
    {
      auto itpair = m.equal_range(it->first);
      out << (n++ ? ", " : "") << it->first << ": [";
      int m = 0;
      for (; itpair.first != itpair.second; ++itpair.first)
      {
        auto const& item = *itpair.first;
        out << (m++ ? ", " : "") << item.second;
      }
      out << "]";
    }
    out << "}+";
    return out;
  }
} /* std */
