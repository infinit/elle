#ifndef ELLE_CRYPTOGRAPHY_CIPHER_HXX
# define ELLE_CRYPTOGRAPHY_CIPHER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Cipher,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.region;
}

#endif
