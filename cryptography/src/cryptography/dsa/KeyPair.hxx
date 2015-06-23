#ifndef INFINIT_CRYPTOGRAPHY_DSA_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_DSA_KEYPAIR_HXX

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<infinit::cryptography::dsa::KeyPair>
  {
    size_t
    operator ()(infinit::cryptography::dsa::KeyPair const& value) const
    {
      return (std::hash<infinit::cryptography::dsa::PublicKey>()(value.K()));
    }
  };
}

#endif
