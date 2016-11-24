#ifndef INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HXX

//
// ---------- Hash ------------------------------------------------------------
//

# include <elle/serialization/binary.hh>

namespace std
{
  template <>
  struct hash<infinit::cryptography::dsa::PrivateKey>
  {
    size_t
    operator ()(infinit::cryptography::dsa::PrivateKey const& value) const
    {
      std::stringstream stream;
      {
        elle::serialization::binary::SerializerOut output(stream);
        output.serialize("value", value);
      }

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
    }
  };
}

#endif
