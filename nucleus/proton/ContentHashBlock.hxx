#ifndef NUCLEUS_PROTON_CONTENTHASHBLOCK_HXX
# define NUCLEUS_PROTON_CONTENTHASHBLOCK_HXX

#include <elle/cryptography/Random.hh>

# include <nucleus/proton/ImmutableBlock.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ContentHashBlock,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::ImmutableBlock>(value);

  if (value.family() != nucleus::proton::FamilyContentHashBlock)
    throw Exception("Invalid family");
}

#endif
