#ifndef INFINIT_CRYPTOGRAPHY_DH_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DH_PUBLICKEY_HXX

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<infinit::cryptography::dh::PublicKey>
  {
    size_t
    operator ()(infinit::cryptography::dh::PublicKey const& value) const
    {
      std::stringstream stream;
      elle::serialize::OutputBinaryArchive archive(stream);

      // Note that this is not a great way to represent a key but OpenSSL
      // does not provide DH-specific DER functions while Diffie Hellman keys
      // are not exactly supposed to be serialized since used for one-time
      // key exchanges.
      ELLE_ASSERT(value.key()->pkey.dh->pub_key != nullptr);
      archive << *value.key()->pkey.dh->pub_key;

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
    }
  };
}

#endif
