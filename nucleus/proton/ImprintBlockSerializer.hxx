#ifndef  NUCLEUS_PROTON_IMPRINTBLOCKSERIALIZER_HXX
# define NUCLEUS_PROTON_IMPRINTBLOCKSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/utility/TimeSerializer.hxx>
# include <elle/cryptography/PublicKeySerializer.hxx>
# include <nucleus/proton/MutableBlockSerializer.hxx>

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
