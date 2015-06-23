#ifndef INFINIT_CRYPTOGRAPHY_DSA_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DSA_PUBLICKEY_HXX

//
// ---------- Class -----------------------------------------------------------
//

# include <cryptography/Plain.hh>
# include <cryptography/serialization.hh>
# include <cryptography/hash.hh>

# include <elle/Buffer.hh>
# include <elle/log.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace dsa
    {
      /*--------.
      | Methods |
      `--------*/

      template <typename T>
      elle::Boolean
      PublicKey::verify(Signature const& signature,
                        T const& value) const
      {
        ELLE_LOG_COMPONENT("infinit.cryptography.dsa.PublicKey");
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("signature: %x", signature);
        ELLE_DUMP("value: %x", value);

        static_assert(std::is_same<T, Digest>::value == false,
                      "this call should never have occured");

        elle::Buffer _value = cryptography::serialize(value);

        Digest digest = hash(Plain(_value), this->_digest_algorithm);

        return (this->verify(signature, digest));
      }
    }
  }
}

//
// ---------- Hash ------------------------------------------------------------
//

namespace std
{
  template <>
  struct hash<infinit::cryptography::dsa::PublicKey>
  {
    size_t
    operator ()(infinit::cryptography::dsa::PublicKey const& value) const
    {
      /* XXX replace with new serialization
      std::stringstream stream;
      elle::serialize::OutputBinaryArchive archive(stream);
      archive << value;

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
      */
      return (0);
    }
  };
}

#endif
