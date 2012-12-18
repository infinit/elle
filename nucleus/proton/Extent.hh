#ifndef NUCLEUS_PROTON_EXTENT_HH
# define NUCLEUS_PROTON_EXTENT_HH

# include <elle/types.hh>

namespace nucleus
{
  namespace proton
  {
    /// Define the maximum size of the blocks when serialized i.e when
    /// transmitted over the network and stored.
    typedef elle::Natural64 Extent;
  }
}

#endif
