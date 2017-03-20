#pragma once
#include <elle/serialization/Serializer.hxx>

namespace elle
{
  namespace serialization
  {
    template <typename S, typename T>
    void
    SerializerOut::serialize_forward(T const& v)
    {
      this->serialize_switch<S>(*this, const_cast<T&>(v));
    }

    template <typename S, typename T>
    void
    SerializerOut::serialize_forward(T const*& v)
    {
      this->serialize_switch<S>(*this, const_cast<T*&>(v));
    }

    template <typename S, typename T>
    void
    SerializerOut::serialize(std::string const& name, T const& v)
    {
      if (auto entry = this->enter(name))
        this->serialize_switch<S>(*this, const_cast<T&>(v));
    }
  }
}
