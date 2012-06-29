#ifndef  NUCLEUS_NEUTRON_AUTHOR_HXX
# define NUCLEUS_NEUTRON_AUTHOR_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/neutron/Author.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Author,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.role;

  switch (value.role)
  {
  case nucleus::neutron::RoleOwner:
    break;
  case nucleus::neutron::RoleLord:
    archive & value.lord.index;
    break;
  case nucleus::neutron::RoleVassal:
    assert(false && "XXX: Not implemented");
  default:
    throw std::runtime_error("unknown role");
  }
}

#endif
