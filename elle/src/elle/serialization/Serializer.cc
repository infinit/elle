#include <elle/serialization/SerializerOut.hh>
#include <elle/serialization/Serializer.hh>

namespace elle
{
  namespace serialization
  {
    bool
    Serializer::_enter(std::string const&)
    {
      return true;
    }

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

    void
    Serializer::serialize_virtual_object(std::string const& name,
                                 VirtuallySerializable& object)
    {
      if (this->out())
      {
        std::string type_name = demangle(typeid(object).name());
        this->serialize(".type", type_name);
      }
    }

    void
    Serializer::_serialize(std::string const& name, elle::WeakBuffer& v)
    {
      if (this->in())
      {
        elle::Buffer buf;
        this->_serialize(name, buf);
        ELLE_ASSERT(buf.size() == v.size());
        memcpy(v.mutable_contents(), buf.mutable_contents(), v.size());
      }
      else
      {
        elle::Buffer buf(v.mutable_contents(), v.size());
        this->_serialize(name, buf);
      }
    }
  }
}
