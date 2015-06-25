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

  template <typename T, typename ... Fields>
  class Serializer<das::Object<T, Fields ...>>
  {
  public:
    typedef typename das::Object<T, Fields ...>::Model Model;
    Serializer(Model& o)
      : _object(o)
    {}

    Serializer()
      : _object(*(Model*)(nullptr))
    {
      ELLE_ABORT("not handled yet");
    }

    void
    serialize(elle::serialization::Serializer& s)
    {
      das::Object<T, Fields...>::template each_field<das::SerializeMember>(
        this->_object, s);
    }
    ELLE_ATTRIBUTE_R(Model&, object);
  };
}

#define DAS_MODEL_SERIALIZE(Class)              \
  namespace elle                                \
  {                                             \
    namespace serialization                     \
    {                                           \
      template <>                               \
        struct Serialize<Class>                 \
      {                                         \
        typedef das::Serializer<Class> Wrapper; \
      };                                        \
    }                                           \
  }                                             \


#endif
