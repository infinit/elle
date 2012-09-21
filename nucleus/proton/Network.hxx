#ifndef NUCLEUS_PROTON_NETWORK_HXX
# define NUCLEUS_PROTON_NETWORK_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Network,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  enforce(value._name.length() != 0);

  archive & value._name;
}

#endif
