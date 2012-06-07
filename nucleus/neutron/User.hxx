#ifndef  NUCLEUS_NEUTRON_USERSERIALIZER_HXX
# define NUCLEUS_NEUTRON_USERSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/neutron/User.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::User,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
}

#endif
