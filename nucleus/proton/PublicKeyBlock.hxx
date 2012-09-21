#ifndef  NUCLEUS_PROTON_PUBLICKEYBLOCK_HXX
# define NUCLEUS_PROTON_PUBLICKEYBLOCK_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(nucleus::proton::PublicKeyBlock);

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::PublicKeyBlock,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::MutableBlock>(value);

  archive << value.K;
}

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::proton::PublicKeyBlock,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive >> base_class<nucleus::proton::MutableBlock>(value);

  archive >> value.K;

  enforce(value.family() == nucleus::proton::FamilyPublicKeyBlock);
}

#endif
