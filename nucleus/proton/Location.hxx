#ifndef  NUCLEUS_PROTON_LOCATION_HXX
# define NUCLEUS_PROTON_LOCATION_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Location,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.address;
  archive & value.version;
}

#endif
