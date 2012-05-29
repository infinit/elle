#ifndef  NUCLEUS_NEUTRON_ATTRIBUTESSERIALIZER_HXX
# define NUCLEUS_NEUTRON_ATTRIBUTESSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
#include <nucleus/neutron/RangeSerializer.hxx>

# include <nucleus/neutron/Attributes.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Attributes,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.range;
}

#endif
