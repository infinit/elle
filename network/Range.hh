#ifndef ELLE_NETWORK_RANGE_HH
# define ELLE_NETWORK_RANGE_HH

#include <elle/types.hh>

namespace elle
{
  namespace network
  {

    ///
    /// this structure defines the range capacity.
    ///
    template <const Natural32 S>
    struct Capacity
    {
      static const Natural32    Size = S;
    };

    ///
    /// this structure calculates the dependency' ranges.
    ///
    template <const Character*... D>
    struct Dependency
    {
    };

    ///
    /// this structure defines a range of tags.
    ///
    template <const Character* C>
    struct Range
    {
    };

  }
}

#include <elle/network/Range.hxx>

#endif
