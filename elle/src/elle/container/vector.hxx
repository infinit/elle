#include <elle/container/vector.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator<<(ostream& out, vector<T, OTHER...> const& v)
  {
    return out << "[" << elle::join(begin(v), end(v), ", ") << "]";
  }
} /* std */
