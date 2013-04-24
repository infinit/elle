#include <elle/container/set.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator<<(ostream& out, set<T, OTHER...> const& s)
  {
    return out << "{" << elle::join(begin(s), end(s), ", ") << "}";
  }
} /* std */
