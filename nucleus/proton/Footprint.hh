#ifndef NUCLEUS_PROTON_FOOTPRINT_HH
# define NUCLEUS_PROTON_FOOTPRINT_HH

# include <elle/serialize/footprint.hh>

namespace nucleus
{
  namespace proton
  {
    /// Define the size of a block once serialized. Such an information is
    /// required for the system to be able to decide when and how to split
    /// or merge blocks together so as to optimize the block-based data
    /// structures.
    typedef elle::serialize::FootprintSize Footprint;
  }
}

#endif
