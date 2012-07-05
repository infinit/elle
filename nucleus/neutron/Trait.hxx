#ifndef  NUCLEUS_NEUTRON_TRAIT_HXX
# define NUCLEUS_NEUTRON_TRAIT_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

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
