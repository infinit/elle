#ifndef DAS_SERIALIZER_HH
# define DAS_SERIALIZER_HH

# include <das/model.hh>

namespace das
{
  template <typename Field>
  struct SerializeMember
  {
    template <typename T>
    static
    void
    apply(T& o, elle::serialization::Serializer& s)
    {
      s.serialize(Field::name, Field::get(o));
    }
  };

  template <typename T>
  class Serializer
  {
  public:
    Serializer(T& o)
      : _object(o)
    {}

    void
    serialize(elle::serialization::Serializer& s)
    {
      das::Das<T>::Model::template each_field<das::SerializeMember>(
        this->_object, s);
    }
    ELLE_ATTRIBUTE_R(T&, object);
  };
}

#endif
