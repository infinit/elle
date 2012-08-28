#ifndef  NUCLEUS_PROTON_CONTENTHASHBLOCK_HXX
# define NUCLEUS_PROTON_CONTENTHASHBLOCK_HXX

# include <nucleus/proton/ImmutableBlock.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ContentHashBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::ImmutableBlock&>(value);

  archive & value._creation_stamp;
  archive & value._salt;

  if (value.family != nucleus::proton::FamilyContentHashBlock)
    throw std::runtime_error("Invalid family");
}

#endif

