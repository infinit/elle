#ifndef INFINIT_CRYPTOGRAPHY_SEED_HXX
# define INFINIT_CRYPTOGRAPHY_SEED_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Seed,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.region;
}

#endif
