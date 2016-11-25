#ifndef INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_RSA_KEYPAIR_HXX

# include <cryptography/rsa/PublicKey.hh>

namespace std
{
  template <>
  struct hash<infinit::cryptography::rsa::KeyPair>
  {
    size_t
    operator ()(infinit::cryptography::rsa::KeyPair const& value) const
    {
      return (std::hash<infinit::cryptography::rsa::PublicKey>()(value.K()));
    }
  };
}

#endif
