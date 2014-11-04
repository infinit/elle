#ifndef ELLE_CONTAINERS_HH
# define ELLE_CONTAINERS_HH

namespace elle
{
  namespace containers
  {
    template<typename C>
    typename C::iterator
    find_pair_first(C& container,
                    typename C::value_type::first_type const& v);

    template<typename C>
    typename C::iterator
    find_pair_second(C& container,
                     typename C::value_type::second_type const& v);
  }
}

# include <elle/containers.hxx>

#endif
