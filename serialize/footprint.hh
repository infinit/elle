#ifndef  ELLE_SERIALIZE_FOOTPRINT_HH
# define ELLE_SERIALIZE_FOOTPRINT_HH

# include "BinaryArchive.hh"

namespace elle
{
  namespace serialize
  {

    /// Compute the size of an object T serialized with a Archive.
    template<typename Archive = OutputBinaryArchive, typename T>
    size_t
    footprint(T const& value);

  }
}

# include "footprint.hxx"

#endif
