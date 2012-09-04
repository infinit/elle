#ifndef  NUCLEUS_PROTON_IMPRINTBLOCK_HXX
# define NUCLEUS_PROTON_IMPRINTBLOCK_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ImprintBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & base_class<nucleus::proton::MutableBlock>(value);
  archive & value._creation_stamp;
  archive & value._salt;
  archive & value._owner.K;
}

#endif
