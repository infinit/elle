#ifndef NUCLEUS_PROTON_NETWORK_HXX
# define NUCLEUS_PROTON_NETWORK_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(nucleus::proton::Network);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::Network,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  enforce(value._name.length());

  archive << value._name;
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::proton::Network,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive >> value._name;
}

#endif
