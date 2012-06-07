#ifndef  ELLE_CRYPTOGRAPHY_KEYPAIRSERIALIZER_HXX
# define ELLE_CRYPTOGRAPHY_KEYPAIRSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/cryptography/PublicKey.hh>
# include <elle/cryptography/PrivateKey.hh>

# include <elle/cryptography/KeyPair.hh>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::KeyPair,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.K;
  archive & value.k;
}

#endif
