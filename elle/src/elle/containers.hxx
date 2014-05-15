#ifndef ELLE_CONTAINERS_HXX
#define ELLE_CONTAINERS_HXX

namespace elle
{
  namespace containers
  {
    template<typename C>
    typename C::iterator find_pair_first(C& container,
                                         typename C::value_type::first_type const& v)
    {
      return std::find_if(container.begin(), container.end(),
                          [&v](typename C::value_type const& e)
                          {
                            return v == e.first;
                          });
    }

    template<typename C>
    typename C::iterator find_pair_second(C& container,
                                         typename C::value_type::second_type const& v)
    {
      return std::find_if(container.begin(), container.end(),
                          [&v](typename C::value_type const& e)
                          {
                            return v == e.second;
                          });
    }
  }
}


#endif
