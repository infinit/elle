#ifndef NUCLEUS_PROTON_BLOCK_HXX
# define NUCLEUS_PROTON_BLOCK_HXX


# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Block,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._network;
  archive & value._family;
  archive & value._component;
  archive & value._creator;
  archive & value._creation_timestamp;
  archive & value._salt;
}

#endif
