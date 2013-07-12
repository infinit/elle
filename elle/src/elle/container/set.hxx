#include <elle/container/set.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator <<(ostream& out,
              set<T, OTHER...> const& s)
  {
    return out << "{" << elle::join(begin(s), end(s), ", ") << "}";
  }

  template <typename T, typename ...OTHER>
  std::ostream&
  operator <<(ostream& out,
              unordered_set<T, OTHER...> const& s)
  {
    return out << "{" << elle::join(begin(s), end(s), ", ") << "}";
  }

  template <typename V, typename T, typename ...OTHER>
  bool
  contains(unordered_set<T, OTHER...> const& set,
           V&& value)
  {
    return set.find(std::forward<V>(value)) != set.end();
  }
}
