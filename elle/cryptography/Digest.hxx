#ifndef ELLE_CRYPTOGRAPHY_DIGEST_HXX
# define ELLE_CRYPTOGRAPHY_DIGEST_HXX

# include <cassert>

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
