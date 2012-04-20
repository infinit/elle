#ifndef  NUCLEUS_NEUTRON_TRAITSERIALIZER_HXX
# define NUCLEUS_NEUTRON_TRAITSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/neutron/Trait.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Trait,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.name;
  archive & value.value;
}

#endif
