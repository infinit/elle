#ifndef  NUCLEUS_PROTON_CONTENTHASHBLOCK_HXX
# define NUCLEUS_PROTON_CONTENTHASHBLOCK_HXX

#include <elle/cryptography/Random.hh>

# include <nucleus/proton/ImmutableBlock.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ContentHashBlock,
                      archive,
                      value,
                      version)
{
  archive & base_class<nucleus::proton::ImmutableBlock>(value);

  if (version == 0)
    {
      if (archive.mode == elle::serialize::ArchiveMode::Input)
        {
          if (elle::cryptography::Random::Generate(value._salt) ==
              elle::Status::Error)
            throw std::runtime_error("unable to generate the seed");
        }
    }
  else if (version >= 1)
    {
      archive & value._creation_stamp;
      archive & value._salt;
    }
  else
    {
      enforce(false);
    }

  if (value.family != nucleus::proton::FamilyContentHashBlock)
    throw std::runtime_error("Invalid family");
}

ELLE_SERIALIZE_CLASS_VERSION(nucleus::proton::ContentHashBlock, 1);

#endif
