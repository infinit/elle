#ifndef DAS_SERIALIZE_HXX
# define DAS_SERIALIZE_HXX

namespace das
{
  template <typename O, typename M>
  O
  Serializer<O, M>::deserialize(elle::serialization::SerializerIn& s)
  {
    return _details::deserialize_switch<O, M>(s);
  }
}

#endif
