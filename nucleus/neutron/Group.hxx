#ifndef NUCLEUS_NEUTRON_GROUP_HXX
# define NUCLEUS_NEUTRON_GROUP_HXX

# include <cassert>
# include <stdexcept>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Group);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Group,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive >> base_class<nucleus::proton::ImprintBlock>(value);

  archive >> value._description;
  archive >> value._pass_K;
  archive >> value._size;
  archive >> value._modification_timestamp;
  archive >> value._ensemble;
  archive >> value._manager_token;
  archive >> value._signature;
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Group,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  enforce(value._signature != elle::cryptography::Signature::Null);

  archive << base_class<nucleus::proton::ImprintBlock>(value);

  archive << value._description;
  archive << value._pass_K;
  archive << value._size;
  archive << value._modification_timestamp;
  archive << value._ensemble;
  archive << value._manager_token;
  archive << value._signature;
}

#endif
