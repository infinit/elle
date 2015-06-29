#ifndef INFINIT_CRYPTOGRAPHY_DSA_PUBLICKEY_HXX
# define INFINIT_CRYPTOGRAPHY_DSA_PUBLICKEY_HXX

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
