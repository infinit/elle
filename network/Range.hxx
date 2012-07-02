#ifndef ELLE_NETWORK_RANGE_HXX
# define ELLE_NETWORK_RANGE_HXX

#include <elle/network/Tag.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this template specializes the Dependency structure with
    /// a dependency D.
    ///
    template <const Character* D>
    struct Dependency<D>
    {
      static const Natural32    First = Range<D>::First;
      static const Natural32    Last = Range<D>::Last;
      static const Natural32    Size = Range<D>::Size;
    };

    ///
    /// this template specializes the Dependency structure with
    /// no dependency.
    ///
    template <>
    struct Dependency<>
    {
      static const Natural32    First = TagUnknown + 1;
      static const Natural32    Last = TagUnknown + 1;
      static const Natural32    Size = 1;
    };

  }
}

#endif
