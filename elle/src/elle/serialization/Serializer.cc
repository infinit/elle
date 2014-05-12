#include <elle/serialization/SerializerOut.hh>
#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    void
    Serializer::_enter(std::string const&)
    {}

    void
    Serializer::_leave(std::string const&)
    {}

    bool
    Serializer::in() const
    {
      return !this->_out();
    }

    bool
    Serializer::out() const
    {
      return this->_out();
    }

    bool
    Serializer::_out() const
    {
      return dynamic_cast<SerializerOut const*>(this);
    }
  }
}
