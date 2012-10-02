#ifndef NUCLEUS_NEUTRON_FELLOW_HXX
# define NUCLEUS_NEUTRON_FELLOW_HXX


# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Fellow,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._subject;
  archive & value._token;
}

#endif
