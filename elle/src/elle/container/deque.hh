#ifndef ELLE_CONTAINER_DEQUE_HH
# define ELLE_CONTAINER_DEQUE_HH

# include <iosfwd>
# include <deque>

namespace std
{
  template <typename T, typename ...OTHER>
  std::ostream&
  operator <<(ostream& out,
              deque<T, OTHER...> const& v);
}

# include <elle/container/deque.hxx>

#endif
