#ifndef NUCLEUS_PROTON_BALANCING_HH
# define NUCLEUS_PROTON_BALANCING_HH

# include <elle/types.hh>

namespace nucleus
{
  namespace proton
  {
    /// Define the ratio of a block's extent under which a block is
    /// considered to small in which case the system could try to merge
    /// it with neighbour blocks.
    ///
    /// For example, given an extent of 1024 and a balancing of 0.2,
    /// a block with a footprint lower than 204 bytes would be elligible
    /// for merging, assuming the resulting merge would create a block
    /// with a footprint lower than the extent.
    typedef elle::Real Balancing;
  }
}

#endif
