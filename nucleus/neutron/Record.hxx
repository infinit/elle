#ifndef  NUCLEUS_NEUTRON_RECORD_HXX
# define NUCLEUS_NEUTRON_RECORD_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Record,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.subject;
  archive & value.permissions;
  archive & value.token;
}

#endif
