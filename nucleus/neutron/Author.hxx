#ifndef NUCLEUS_NEUTRON_AUTHOR_HXX
# define NUCLEUS_NEUTRON_AUTHOR_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Author,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.role;

  switch (value.role)
    {
    case nucleus::neutron::Object::RoleOwner:
      break;
    case nucleus::neutron::Object::RoleLord:
      archive & value.lord.index;
      break;
    case nucleus::neutron::Object::RoleVassal:
      throw std::runtime_error("not implemented yet"); // XXX
    default:
      throw std::runtime_error("unknown role");
    }
}

#endif
