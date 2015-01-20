#include <elle/serialization/SerializerIn.hh>

namespace elle
{
  namespace serialization
  {
    SerializerIn::SerializerIn(std::istream& input,
                               bool versioned)
      : SerializerIn(input, elle::Version(), versioned)
    {}

    SerializerIn::SerializerIn(std::istream& input,
                               elle::Version version,
                               bool versioned)
      : Super(std::move(version), versioned)
      , _input(input)
    {}
  }
}
