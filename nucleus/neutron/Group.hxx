#ifndef NUCLEUS_NEUTRON_GROUP_HXX
# define NUCLEUS_NEUTRON_GROUP_HXX

# include <cassert>
# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Group);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Group,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  archive >> base_class<nucleus::proton::ImprintBlock>(value);

  archive >> value._description;
  archive >> value._pass_K;
  archive >> value._size;
  archive >> value._modification_stamp;
  archive >> value._ensemble;
  archive >> value._manager.token;
  archive >> value._signature;
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Group,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  assert(value._signature != elle::cryptography::Signature::Null);

  archive << base_class<nucleus::proton::ImprintBlock>(value);

  archive << value._description;
  archive << value._pass_K;
  archive << value._size;
  archive << value._modification_stamp;
  archive << value._ensemble;
  archive << value._manager.token;
  archive << value._signature;
}

#endif
