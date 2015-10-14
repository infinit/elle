#ifndef INFINIT_CRYPTOGRAPHY_DH_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DH_PRIVATEKEY_HXX

//
// ---------- Hash ------------------------------------------------------------
//

# include <openssl/dh.h>

# include <cryptography/bn.hh>

# include <elle/serialization/binary.hh>

namespace std
{
  template <>
  struct hash<infinit::cryptography::dh::PrivateKey>
  {
    size_t
    operator ()(infinit::cryptography::dh::PrivateKey const& value) const
    {
      std::stringstream stream;
      ELLE_ASSERT(value.key()->pkey.dh->priv_key != nullptr);
      stream << value.key()->pkey.dh->priv_key;

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
    }
  };
}

#endif
