#ifndef NUCLEUS_NEUTRON_ATTRIBUTES_HXX
# define NUCLEUS_NEUTRON_ATTRIBUTES_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Attributes,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.range;
}

#endif
