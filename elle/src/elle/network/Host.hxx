#ifndef  ELLE_NETWORK_HOST_HXX
# define ELLE_NETWORK_HOST_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(elle::network::Host,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.type;
  archive & value.location;
}

#endif
