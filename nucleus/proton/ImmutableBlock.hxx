#ifndef  NUCLEUS_PROTON_IMMUTABLEBLOCK_HXX
# define NUCLEUS_PROTON_IMMUTABLEBLOCK_HXX

# include <nucleus/proton/Block.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ImmutableBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & static_cast<nucleus::proton::Block&>(value);
}

#endif


