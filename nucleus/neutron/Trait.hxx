#ifndef  NUCLEUS_NEUTRON_TRAIT_HXX
# define NUCLEUS_NEUTRON_TRAIT_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Trait,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.name;
  archive & value.value;
}

#endif
