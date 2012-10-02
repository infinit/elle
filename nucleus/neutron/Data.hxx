#ifndef NUCLEUS_NEUTRON_DATA_HXX
# define NUCLEUS_NEUTRON_DATA_HXX


# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Data,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.region;
}

#endif
