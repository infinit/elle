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
                               Versions versions,
                               bool versioned)
      : Super(std::move(versions), versioned)
      , _input(input)
    {}

    bool
    SerializerIn::out() const
    {
      return false;
    }
  }
}
