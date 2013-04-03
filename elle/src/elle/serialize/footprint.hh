#ifndef ELLE_SERIALIZE_FOOTPRINT_HH
# define ELLE_SERIALIZE_FOOTPRINT_HH

# include "BinaryArchive.hh"

namespace elle
{
  namespace serialize
  {
    /// The size of the footprint.
    typedef elle::Natural64 FootprintSize;

    /// Compute the size of an object T serialized with a Archive.
    template <typename Archive = OutputBinaryArchive, typename T>
    FootprintSize
    footprint(T&& value);

    /// Compute the size of a type T once serialized. Note that
    /// an Object is instantiated through its default constructor.
    template <typename T>
    FootprintSize
    footprint();
  }
}

# include <elle/serialize/footprint.hxx>

#endif
