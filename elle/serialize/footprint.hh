#ifndef  ELLE_SERIALIZE_FOOTPRINT_HH
# define ELLE_SERIALIZE_FOOTPRINT_HH

# include <elle/serialize/BinaryArchive.hh>

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

# include <elle/serialize/footprint.hxx>

#endif
