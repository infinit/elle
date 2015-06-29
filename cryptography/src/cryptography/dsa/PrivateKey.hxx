#ifndef INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DSA_PRIVATEKEY_HXX

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
#  warning "XXX new serialization"
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
