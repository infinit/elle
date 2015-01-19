#include <elle/serialization/SerializerIn.hh>

namespace elle
{
  namespace serialization
  {
    SerializerIn::SerializerIn(std::istream& input)
      : SerializerIn(input, elle::Version())
    {}

    SerializerIn::SerializerIn(std::istream& input, elle::Version version)
      : Super(std::move(version))
      , _input(input)
    {}
  }
}
