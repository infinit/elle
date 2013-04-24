#include <elle/container/initializer_list.hh>
#include <elle/string/algorithm.hh>

namespace std
{
  template <class T>
  std::ostream&
  operator<<(ostream& out, initializer_list<T> const& l)
  {
    return out << "{" << elle::join(begin(l), end(l), ", ") << "}";
  }
} /* std */
