#ifndef  NUCLEUS_PROTON_LOCATIONSERIALIZER_HXX
# define NUCLEUS_PROTON_LOCATIONSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <nucleus/proton/AddressSerializer.hxx>
# include <nucleus/proton/VersionSerializer.hxx>

# include <nucleus/proton/Location.hh>

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
