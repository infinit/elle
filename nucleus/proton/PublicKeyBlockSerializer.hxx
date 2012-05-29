#ifndef  NUCLEUS_PROTON_PUBLICKEYBLOCKSERIALIZER_HXX
# define NUCLEUS_PROTON_PUBLICKEYBLOCKSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/cryptography/PublicKeySerializer.hxx>

# include <nucleus/proton/MutableBlockSerializer.hxx>

# include <nucleus/proton/PublicKeyBlock.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::PublicKeyBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::MutableBlock&>(value);

  if (Archive::mode == ArchiveMode::Input
      && value.family != FamilyPublicKeyBlock)
    throw std::runtime_error("Invalid family");

  archive & value.K;
}

#endif
