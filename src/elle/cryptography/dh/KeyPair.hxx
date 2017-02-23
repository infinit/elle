#ifndef ELLE_CRYPTOGRAPHY_DH_KEYPAIR_HXX
# define ELLE_CRYPTOGRAPHY_DH_KEYPAIR_HXX

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<elle::cryptography::dh::KeyPair>
  {
    size_t
    operator ()(elle::cryptography::dh::KeyPair const& value) const
    {
      return (std::hash<elle::cryptography::dh::PublicKey>()(value.K()));
    }
  };
}

#endif
