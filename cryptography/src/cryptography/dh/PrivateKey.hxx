#ifndef INFINIT_CRYPTOGRAPHY_DH_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DH_PRIVATEKEY_HXX

//
// ---------- Hash ------------------------------------------------------------
//

# include <openssl/dh.h>

# include <cryptography/bn.hh>

namespace std
{
  template <>
  struct hash<infinit::cryptography::dh::PrivateKey>
  {
    size_t
    operator ()(infinit::cryptography::dh::PrivateKey const& value) const
    {
      /* XXX replace with new serialization
      std::stringstream stream;
      elle::serialize::OutputBinaryArchive archive(stream);

      // Note that this is not a great way to represent a key but OpenSSL
      // does not provide DH-specific DER functions while Diffie Hellman keys
      // are not exactly supposed to be serialized since used for one-time
      // key exchanges.
      ELLE_ASSERT(value.key()->pkey.dh->pub_key != nullptr);
      ELLE_ASSERT(value.key()->pkey.dh->priv_key != nullptr);
      archive << *value.key()->pkey.dh->pub_key
              << *value.key()->pkey.dh->priv_key;

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
      */
      return (0);
    }
  };
}

#endif
