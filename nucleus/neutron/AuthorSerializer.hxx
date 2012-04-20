#ifndef  NUCLEUS_NEUTRON_AUTHORSERIALIZER_HXX
# define NUCLEUS_NEUTRON_AUTHORSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/neutron/Author.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Author);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Author,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  archive & value.role;

  switch (value.role)
  {
  case RoleOwner:
    break;
  case RoleLord:
    archive & lord.index;
    break;
  case RoleVassal:
    assert(false && "XXX: Not implemented");
  default:
    throw std::runtime_error("unknown role");
  }
}

#endif
