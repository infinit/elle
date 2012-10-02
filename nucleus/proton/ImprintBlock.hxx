#ifndef NUCLEUS_PROTON_IMPRINTBLOCK_HXX
# define NUCLEUS_PROTON_IMPRINTBLOCK_HXX


# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ImprintBlock,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::MutableBlock>(value);

  archive & value._owner_K;

  enforce(value.family() == nucleus::proton::Family::imprint_block);
}

#endif
