#ifndef  NUCLEUS_PROTON_IMPRINTBLOCKSERIALIZER_HXX
# define NUCLEUS_PROTON_IMPRINTBLOCKSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/utility/Time.hh>
# include <elle/cryptography/PublicKey.hh>
# include <nucleus/proton/MutableBlock.hh>

# include <nucleus/proton/ImprintBlock.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ImprintBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::MutableBlock&>(value);
  archive & value.stamp;
  archive & value.salt;
  archive & value.owner.K;
}

#endif
