#ifndef ELLE_CONTAINER_INITIALIZER_LIST_HH
# define ELLE_CONTAINER_INITIALIZER_LIST_HH

# include <iosfwd>
# include <initializer_list>

namespace std
{
  template <class T>
  std::ostream&
  operator <<(ostream& out,
              initializer_list<T> const& m);
}

# include <elle/container/initializer_list.hxx>

#endif /* end of include guard: ELLE_CONTAINER_INITIALIZER_LIST_HH */
