#ifndef  NUCLEUS_PROTON_IMMUTABLEBLOCK_HXX
# define NUCLEUS_PROTON_IMMUTABLEBLOCK_HXX

# include <nucleus/proton/Block.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ImmutableBlock,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & base_class<nucleus::proton::Block>(value);
}

#endif


