#include <elle/container/list.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator<<(ostream& out, list<T, OTHER...> const &l)
  {
    return out << "(" << elle::join(begin(l), end(l), ", ") << ")";
  }
} /* std */
