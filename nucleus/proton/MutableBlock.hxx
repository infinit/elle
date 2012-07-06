#ifndef  NUCLEUS_PROTON_MUTABLE_BLOCK__HXX
# define NUCLEUS_PROTON_MUTABLE_BLOCK__HXX

# include <elle/serialize/ArchiveSerializer.hxx>

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
