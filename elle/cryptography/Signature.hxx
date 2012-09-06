#ifndef ELLE_CRYPTOGRAPHY_SIGNATURE_HXX
# define ELLE_CRYPTOGRAPHY_SIGNATURE_HXX

# include <cassert>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Signature,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.region;
}

#endif
