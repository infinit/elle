#ifndef  NUCLEUS_NEUTRON_SUBJECTSERIALIZER_HXX
# define NUCLEUS_NEUTRON_SUBJECTSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/cryptography/PublicKeySerializer.hxx>

# include <nucleus/proton/AddressSerializer.hxx>

# include <nucleus/neutron/Subject.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Subject);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Subject,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  using namespace nucleus::neutron;

  archive & value.type;
  switch (value.type)
  {
  case Subject::TypeUser:
    assert(value.user != nullptr);
    archive << *value.user;
    break;
  case Subject::TypeGroup:
    assert(value.group != nullptr);
    archive << *value.group;
    break;
  default:
    throw std::runtime_error("Unknown group!");
    break;
  }
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Subject,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  using namespace nucleus::neutron;

  delete value.user;
  value.user = nullptr;

  delete value.group;
  value.group = nullptr;

  archive >> value.type;
  switch (value.type)
  {
  case Subject::TypeUser:
    value.user = archive.template Construct<elle::cryptography::PublicKey>().release();
    break;
  case Subject::TypeGroup:
    value.group = archive.template Construct<nucleus::proton::Address>().release();
    break;
  default:
    throw std::runtime_error("Unknown group!");
    break;
  }
}

#endif
