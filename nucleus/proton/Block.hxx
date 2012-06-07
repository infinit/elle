#ifndef  NUCLEUS_PROTON_BLOCKSERIALIZER_HXX
# define NUCLEUS_PROTON_BLOCKSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <nucleus/proton/Network.hh>
# include <nucleus/proton/Block.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Block,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.network;
  archive & value.family;
  archive & value.component;
}

#endif
