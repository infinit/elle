#ifndef  NUCLEUS_NEUTRON_ENTRY_HXX
# define NUCLEUS_NEUTRON_ENTRY_HXX


# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Entry,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.name;
  archive & value.address;
}

#endif
