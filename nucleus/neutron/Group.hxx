#ifndef NUCLEUS_NEUTRON_GROUP_HXX
# define NUCLEUS_NEUTRON_GROUP_HXX

# include <cassert>
# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/serialize/Pointer.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Group);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Group,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  archive >> static_cast<nucleus::proton::ImprintBlock&>(value);

  archive >> value._description;
  archive >> elle::serialize::pointer(value._pass);
  archive >> value._size;
  archive >> value._modification_stamp;
  archive >> value._ensemble;
  archive >> elle::serialize::pointer(value._manager.token);
  archive >> value._signature;
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Group,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  assert(value._signature != elle::cryptography::Signature::Null);

  archive << static_cast<nucleus::proton::ImprintBlock const&>(value);

  archive << value._description;

  // XXX[hack because the serialization mechanism does not support pointer
  //     on const pointers]
  elle::cryptography::PublicKey* pk(value._pass);
  archive << elle::serialize::pointer(pk);

  archive << value._size;
  archive << value._modification_stamp;
  archive << value._ensemble;

  // XXX[hack because the serialization mechanism does not support pointer
  //     on const pointers]
  nucleus::neutron::Token* t(value._manager.token);
  archive << elle::serialize::pointer(t);

  archive << value._signature;
}

#endif
