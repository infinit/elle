#ifndef  NUCLEUS_PROTON_LOCATION_HXX
# define NUCLEUS_PROTON_LOCATION_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Version.hh>

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
