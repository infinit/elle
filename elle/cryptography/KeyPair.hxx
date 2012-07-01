#ifndef  ELLE_CRYPTOGRAPHY_KEYPAIR_HXX
# define ELLE_CRYPTOGRAPHY_KEYPAIR_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

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
