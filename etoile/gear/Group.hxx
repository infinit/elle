#ifndef ETOILE_GEAR_GROUP_HXX
# define ETOILE_GEAR_GROUP_HXX

# include <cassert>

# include <elle/serialize/Pointer.hh>

# include <nucleus/proton/Location.hh>
# include <nucleus/neutron/Ensemble.hh>
# include <nucleus/neutron/Group.hh>
# include <nucleus/neutron/Role.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Group,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.location;
  archive & elle::serialize::pointer(value.group);
  archive & elle::serialize::pointer(value.ensemble);
  archive & value.rights.role;
}

#endif
