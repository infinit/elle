#ifndef ELLE_CRYPTOGRAPHY_DIGEST_HXX
# define ELLE_CRYPTOGRAPHY_DIGEST_HXX

# include <cassert>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Digest,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.region;
}

#endif
