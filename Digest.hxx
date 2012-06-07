#ifndef  ELLE_CRYPTOGRAPHY_DIGESTSERIALIZER_HXX
# define ELLE_CRYPTOGRAPHY_DIGESTSERIALIZER_HXX

# include <cassert>

# include <elle/standalone/Region.hh>

# include <elle/cryptography/Digest.hh>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Digest,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.region;
}

#endif
