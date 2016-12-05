#include <elle/serialization/SerializerIn.hh>

namespace elle
{
  namespace serialization
  {
    SerializerIn::SerializerIn(std::istream& input,
                               bool versioned)
      : Super(false, versioned)
      , _input(input)
    {}

    SerializerIn::SerializerIn(std::istream& input,
                               Versions versions,
                               bool versioned)
      : Super(false, std::move(versions), versioned)
      , _input(input)
    {}
  }
}
