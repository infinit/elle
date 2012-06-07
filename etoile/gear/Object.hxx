#ifndef  ETOILE_GEAR_OBJECTSERIALIZER_HXX
# define ETOILE_GEAR_OBJECTSERIALIZER_HXX

# include <cassert>

# include <elle/cryptography/SecretKey.hh>
# include <elle/serialize/Pointer.hh>

# include <nucleus/neutron/Author.hh>
# include <nucleus/neutron/Object.hh>
# include <nucleus/proton/Location.hh>

# include <etoile/gear/Object.hh>

ELLE_SERIALIZE_SIMPLE(etoile::gear::Object,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.location;
  archive & value.object;
  archive & elle::serialize::pointer(value.access);
  archive & value.rights.role;
  archive & value.rights.permissions;
  archive & value.rights.key;
  archive & value.rights.record;
  archive & value.author;
}

#endif
