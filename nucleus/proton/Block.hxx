#ifndef  NUCLEUS_PROTON_BLOCK_HXX
# define NUCLEUS_PROTON_BLOCK_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

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
