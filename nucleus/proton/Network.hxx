#ifndef  NUCLEUS_PROTON_NETWORK_HXX
# define NUCLEUS_PROTON_NETWORK_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/proton/Network.hh>


ELLE_SERIALIZE_SIMPLE(nucleus::proton::Network,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.name;
}

#endif
