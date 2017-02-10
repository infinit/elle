#include <cryptography/Error.hh>

#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace cryptography
  {
    /*-------------.
    | Construction |
    `-------------*/

    Error::Error(std::string const& message):
      elle::Error(message)
    {}

    Error::Error(elle::serialization::SerializerIn& input):
      elle::Error(input)
    {}

    static const elle::serialization::Hierarchy<elle::Exception>::
    Register<Error> _register_serialization;
  }
}
