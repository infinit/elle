#ifndef ELLE_SERIALIZATION_SERIALIZER_OUT_HXX
# define ELLE_SERIALIZATION_SERIALIZER_OUT_HXX

#  include <elle/serialization/Serializer.hxx>

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

    template <typename S, typename T>
    void
    SerializerOut::serialize_ptr(std::string const& name, T* v)
    {
      this->Serializer::serialize<S>(name, v);
    }
  }
}

#endif
