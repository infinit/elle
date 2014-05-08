#include <elle/container/deque.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator <<(ostream& out,
              deque<T, OTHER...> const& v)
  {
    return out << "[" << elle::join(begin(v), end(v), ", ") << "]";
  }
}
