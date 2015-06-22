#ifndef INFINIT_CRYPTOGRAPHY_DH_KEYPAIR_HXX
# define INFINIT_CRYPTOGRAPHY_DH_KEYPAIR_HXX

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<infinit::cryptography::dh::KeyPair>
  {
    size_t
    operator ()(infinit::cryptography::dh::KeyPair const& value) const
    {
      return (std::hash<infinit::cryptography::dh::PublicKey>()(value.K()));
    }
  };
}

#endif
