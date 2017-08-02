#pragma once
#include <elle/serialization/Serializer.hxx>

namespace elle
{
  namespace serialization
  {
    template <typename Impl, typename T>
    void
    SerializerOut::serialize_forward(T const& v)
    {
      this->serialize_switch<Impl>(*this, const_cast<T&>(v));
    }

    template <typename Impl, typename T>
    void
    SerializerOut::serialize_forward(T const*& v)
    {
      this->serialize_switch<Impl>(*this, const_cast<T*&>(v));
    }

    template <typename Impl, typename T>
    void
    SerializerOut::serialize(std::string const& name, T const& v)
    {
      Details::serialize<Impl>(*this, name, elle::unconst(v));
    }
  }
}
