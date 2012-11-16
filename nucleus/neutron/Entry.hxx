#ifndef  NUCLEUS_NEUTRON_ENTRY_HXX
# define NUCLEUS_NEUTRON_ENTRY_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Entry);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Entry,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive >> value._name;
  archive >> value._address;

  value._footprint = elle::serialize::footprint(value);
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Entry,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << value._name;
  archive << value._address;
}

#endif
