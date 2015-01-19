#include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    SerializerOut::SerializerOut(std::ostream& output)
      : SerializerOut(output, elle::Version())
    {}

    SerializerOut::SerializerOut(std::ostream& output, elle::Version version)
      : Super(std::move(version))
      , _output(output)
    {}

    std::ostream&
    SerializerOut::output()
    {
      return this->_output;
    }
  }
}
