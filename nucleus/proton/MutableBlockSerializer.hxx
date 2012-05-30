#ifndef  NUCLEUS_PROTON_MUTABLE_BLOCK_SERIALIZER_HXX
# define NUCLEUS_PROTON_MUTABLE_BLOCK_SERIALIZER_HXX

# include <nucleus/proton/BlockSerializer.hxx>
# include <nucleus/proton/VersionSerializer.hxx>

# include <nucleus/proton/MutableBlock.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::MutableBlock,
                      archive,
                      value,
                      version)
{
  archive & static_cast<nucleus::proton::Block&>(value);
  archive & value.version;
}

#endif
