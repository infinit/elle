#ifndef  ELLE_CRYPTOGRAPHY_CIPHER_HXX
# define ELLE_CRYPTOGRAPHY_CIPHER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/standalone/Region.hh>

# include <elle/cryptography/Cipher.hh>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Cipher,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.region;
}

#endif
