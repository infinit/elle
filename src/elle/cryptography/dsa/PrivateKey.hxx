#include <elle/serialization/binary.hh>

namespace std
{
  template <>
  struct hash<elle::cryptography::dsa::PrivateKey>
  {
    size_t
    operator ()(elle::cryptography::dsa::PrivateKey const& value) const
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
