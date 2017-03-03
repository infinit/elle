#include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    SerializerOut::SerializerOut(bool versioned)
      : Super(versioned)
    {}

    SerializerOut::SerializerOut(Versions versions,
                                 bool versioned)
      : Super(std::move(versions), versioned)
    {}

    void
    SerializerOut::serialize(std::string const& name, char const* v)
    {
      this->serialize(name, std::string(v));
    }

    bool
    SerializerOut::out() const
    {
      return true;
    }
  }
}
