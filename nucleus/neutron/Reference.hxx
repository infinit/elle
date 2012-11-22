#ifndef  NUCLEUS_NEUTRON_REFERENCE_HXX
# define NUCLEUS_NEUTRON_REFERENCE_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Reference,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  // XXX base value

  archive & value.target;
}

#endif
