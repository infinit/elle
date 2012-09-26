#ifndef  NUCLEUS_PROTON_CONTENTHASHBLOCK_HXX
# define NUCLEUS_PROTON_CONTENTHASHBLOCK_HXX

#include <elle/cryptography/Random.hh>

# include <nucleus/proton/ImmutableBlock.hh>

// XXX[upgrade 0->1]
/*
        if (archive.mode == elle::serialize::ArchiveMode::Input)
          {
            if (elle::cryptography::Random::Generate(value._salt) ==
                elle::Status::Error)
              throw std::runtime_error("unable to generate the seed");
          }
*/

ELLE_SERIALIZE_STATIC_FORMAT(nucleus::proton::ContentHashBlock, 1);

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ContentHashBlock,
                      archive,
                      value,
                      version)
{
  archive & base_class<nucleus::proton::ImmutableBlock>(value);

  switch (version)
    {
    case 0:
      {
        break;
      }
    case 1:
      {
        archive & value._creation_stamp;
        archive & value._salt;

        break;
      }
    default:
      throw std::runtime_error(
        elle::sprintf("unknown format '%s'", version));
    }

  /* XXX
  if (value.family() != nucleus::proton::FamilyContentHashBlock)
    throw Exception("Invalid family");
  */
}

#endif
