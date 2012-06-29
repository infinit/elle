#ifndef  NUCLEUS_PROTON_MUTABLE_BLOCK__HXX
# define NUCLEUS_PROTON_MUTABLE_BLOCK__HXX

# include <nucleus/proton/Block.hh>
# include <nucleus/proton/Version.hh>

# include <nucleus/proton/MutableBlock.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::MutableBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & static_cast<nucleus::proton::Block&>(value);
  archive & value.version;
}

#endif
