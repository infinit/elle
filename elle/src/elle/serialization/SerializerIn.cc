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

    bool
    SerializerIn::_option_filled()
    {
      return true;
    }
  }
}
