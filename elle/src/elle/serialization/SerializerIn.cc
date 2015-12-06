#include <elle/serialization/SerializerIn.hh>

namespace elle
{
  namespace serialization
  {
    SerializerIn::SerializerIn(std::istream& input,
                               bool versioned)
      : Super(versioned)
      , _input(input)
    {}

    SerializerIn::SerializerIn(std::istream& input,
                               Versions versions)
      : Super(std::move(versions))
      , _input(input)
    {}
  }
}
