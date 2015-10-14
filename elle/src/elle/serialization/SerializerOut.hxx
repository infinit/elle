#ifndef ELLE_SERIALIZATION_SERIALIZER_OUT_HXX
# define ELLE_SERIALIZATION_SERIALIZER_OUT_HXX

namespace elle
{
  namespace serialization
  {
    template <typename T>
    void
    SerializerOut::serialize_forward(T const& v)
    {
      this->Serializer::serialize_forward(const_cast<T&>(v));
    }

    template <typename T>
    void
    SerializerOut::serialize_forward(T const* &v)
    {
      this->Serializer::serialize_forward(const_cast<T*&>(v));
    }

    template <typename T>
    void
    SerializerOut::serialize(std::string const& name, T const& v)
    {
      this->Serializer::serialize(name, const_cast<T&>(v));
    }

    template <typename T>
    void
    SerializerOut::serialize_ptr(std::string const& name, T* v)
    {
      this->Serializer::serialize(name, v);
    }
  }
}

#endif
