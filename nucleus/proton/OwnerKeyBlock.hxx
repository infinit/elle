#ifndef  NUCLEUS_PROTON_OWNERKEYBLOCK_HXX
# define NUCLEUS_PROTON_OWNERKEYBLOCK_HXX

# include <cassert>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::OwnerKeyBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & base_class<nucleus::proton::MutableBlock>(value);

  archive & value.K;
  archive & value.stamp;
  archive & value.owner.K;
  archive & value.owner.signature;
}

#endif
