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

  archive & static_cast<nucleus::proton::MutableBlock&>(value);
  archive & value._stamp;
  archive & value._salt;
  archive & value._owner.K;
}

#endif
