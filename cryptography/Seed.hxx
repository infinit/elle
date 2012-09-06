#ifndef  ELLE_CRYPTOGRAPHY_SEED_HXX
# define ELLE_CRYPTOGRAPHY_SEED_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Seed,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.region;
}

#endif
