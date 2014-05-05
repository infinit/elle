#include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    SerializerOut::SerializerOut(std::ostream& output)
      : _output(output)
    {}

    std::ostream&
    SerializerOut::output()
    {
      return this->_output;
    }
  }
}
