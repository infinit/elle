#include <elle/serialization/SerializerOut.hh>

namespace elle
{
  namespace serialization
  {
    SerializerOut::SerializerOut(std::ostream& output,
                                 bool versioned)
      : Super(versioned)
      , _output(output)
    {}

    void
    SerializerOut::serialize(std::string const& name, char const* v)
    {
      this->serialize(name, std::string(v));
    }

    std::ostream&
    SerializerOut::output()
    {
      return this->_output;
    }
  }
}
