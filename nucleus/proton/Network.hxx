#ifndef NUCLEUS_PROTON_NETWORK_HXX
# define NUCLEUS_PROTON_NETWORK_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(nucleus::proton::Network);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::Network,
                          archive,
                          value,
                          version)
{
  ELLE_LOG_COMPONENT("infinit.nucleus.proton.Network");

  enforce(version == 0);

  // XXX[to add when Addess::Any no longer exists]
  // enforce(value._name.length() != 0);
  if (value._name.length() == 0)
    ELLE_WARN("the network name is empty");

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
