#include <elle/serialization/SerializerIn.hh>

namespace elle
{
  namespace serialization
  {
    SerializerIn::SerializerIn(std::istream& input)
      : _input(input)
    {}
  }
}
