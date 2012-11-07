#ifndef INFINIT_CRYPTOGRAPHY_CIPHER_HXX
# define INFINIT_CRYPTOGRAPHY_CIPHER_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Cipher,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.region;
}

#endif
