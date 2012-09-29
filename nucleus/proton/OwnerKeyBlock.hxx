#ifndef  NUCLEUS_PROTON_OWNERKEYBLOCK_HXX
# define NUCLEUS_PROTON_OWNERKEYBLOCK_HXX

# include <cassert>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::OwnerKeyBlock,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::MutableBlock>(value);

  archive & value._owner_K;
  archive & value._owner_signature;
}

#endif
