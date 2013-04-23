#ifndef ELLE_CONTAINER_VECTOR_HH
#define ELLE_CONTAINER_VECTOR_HH

#include <iosfwd>
#include <vector>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator<<(ostream& out, vector<T, OTHER...> const &v);
}

#include "elle/container/vector.hxx"

#endif /* end of include guard: ELLE_CONTAINER_ */
