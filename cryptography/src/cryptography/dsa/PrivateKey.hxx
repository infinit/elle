#ifndef INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HXX

//
// ---------- Class -----------------------------------------------------------
//

# include <cryptography/serialization.hh>
# include <cryptography/hash.hh>
# include <cryptography/Plain.hh>
# include <cryptography/Digest.hh>

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
      Signature
      PrivateKey::sign(T const& value) const
      {
        /* XXX
        ELLE_LOG_COMPONENT("infinit.cryptography.dsa.PrivateKey");
        ELLE_TRACE_METHOD("");
        ELLE_DUMP("value: %x", value);

        static_assert(std::is_same<T, Digest>::value == false,
                      "this call should never have occured");

        elle::Buffer _value = cryptography::serialize(value);

        Digest digest = hash(Plain(_value), this->_digest_algorithm);

        return (this->sign(digest));
        */
        elle::unreachable();
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
  struct hash<infinit::cryptography::dsa::PrivateKey>
  {
    size_t
    operator ()(infinit::cryptography::dsa::PrivateKey const& value) const
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
