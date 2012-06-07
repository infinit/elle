#ifndef  NUCLEUS_NEUTRON_DATASERIALIZER_HXX
# define NUCLEUS_NEUTRON_DATASERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/standalone/Region.hh>

# include <nucleus/neutron/Data.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Data,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.region;
}

#endif
