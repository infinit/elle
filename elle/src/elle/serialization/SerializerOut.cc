#include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    SerializerOut::SerializerOut(std::ostream& output,
                                 bool versioned)
      : SerializerOut(output, elle::Version(), versioned)
    {}

    SerializerOut::SerializerOut(std::ostream& output,
                                 elle::Version version,
                                 bool versioned)
      : Super(std::move(version), versioned)
      , _output(output)
    {}

    std::ostream&
    SerializerOut::output()
    {
      return this->_output;
    }
  }
}
