#ifndef  NUCLEUS_PROTON_OWNERKEYBLOCK_HXX
# define NUCLEUS_PROTON_OWNERKEYBLOCK_HXX

# include <cassert>

# include <elle/cryptography/PublicKey.hh>
# include <elle/cryptography/Signature.hh>
# include <elle/utility/Time.hh>

# include <nucleus/proton/MutableBlock.hh>

# include <nucleus/proton/OwnerKeyBlock.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::OwnerKeyBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::MutableBlock&>(value);

  archive & value.K;
  archive & value.stamp;
  archive & value.owner.K;
  archive & value.owner.signature;
}

#endif
