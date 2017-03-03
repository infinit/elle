#include <elle/serialization/SerializerIn.hh>

namespace elle
{
  namespace serialization
  {
    SerializerIn::SerializerIn(bool versioned)
      : Super(versioned)
    {}

    SerializerIn::SerializerIn(Versions versions,
                               bool versioned)
      : Super(std::move(versions), versioned)
    {}

    bool
    SerializerIn::out() const
    {
      return false;
    }
  }
}
