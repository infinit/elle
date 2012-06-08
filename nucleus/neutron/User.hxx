#ifndef  NUCLEUS_NEUTRON_USER_HXX
# define NUCLEUS_NEUTRON_USER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/neutron/User.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::User,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  typedef nucleus::proton::PublicKeyBlock Super;
  archive & static_cast<Super&>(value);
  archive & value.attributes;
}

#endif
