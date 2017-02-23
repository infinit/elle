#ifndef ELLE_CRYPTOGRAPHY_DSA_KEYPAIR_HXX
# define ELLE_CRYPTOGRAPHY_DSA_KEYPAIR_HXX

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<elle::cryptography::dsa::KeyPair>
  {
    size_t
    operator ()(elle::cryptography::dsa::KeyPair const& value) const
    {
      return (std::hash<elle::cryptography::dsa::PublicKey>()(value.K()));
    }
  };
}

#endif
