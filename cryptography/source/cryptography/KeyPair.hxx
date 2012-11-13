#ifndef INFINIT_CRYPTOGRAPHY_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_KEYPAIR_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::KeyPair,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._K;
  archive & value._k;
}

#endif
