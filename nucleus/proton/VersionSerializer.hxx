#ifndef  NUCLEUS_PROTON_VERSION_HXX
# define NUCLEUS_PROTON_VERSION_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/proton/Version.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Version,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.number;
}

#endif
