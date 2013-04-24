#ifndef ELLE_CONTAINER_LIST_HH
#define ELLE_CONTAINER_LIST_HH

#include <iosfwd>
#include <list>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator<<(ostream& out, list<T, OTHER...> const& v);
}

#include <elle/container/list.hxx>

#endif /* end of include guard: ELLE_CONTAINER_LIST_HH */
