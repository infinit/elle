#ifndef  NUCLEUS_NEUTRON_CATALOGSERIALIZER_HXX
# define NUCLEUS_NEUTRON_CATALOGSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <nucleus/neutron/EntrySerializer.hxx>

# include <nucleus/neutron/Catalog.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Catalog,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.range;
}

#endif
