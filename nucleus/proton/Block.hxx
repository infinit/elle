#ifndef NUCLEUS_PROTON_BLOCK_HXX
# define NUCLEUS_PROTON_BLOCK_HXX

# include <cassert>

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
  archive & value._creator_K;

  // XXX[make these check cleaner]
  /* XXX[problem with operator <]
  enforce(static_cast<int>(value.family) < static_cast<int>(nucleus::proton::Families));
  enforce(static_cast<int>(value.component) < static_cast<int>(nucleus::neutron::Components));
  */
}

#endif
