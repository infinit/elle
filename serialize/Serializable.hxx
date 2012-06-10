#ifndef  ELLE_SERIALIZE_SERIALIZABLE_HXX
# define ELLE_SERIALIZE_SERIALIZABLE_HXX

# include "ArchiveSerializer.hxx"

ELLE_SERIALIZE_SPLIT_T1(elle::serialize::Serializable)

ELLE_SERIALIZE_SPLIT_T1_SAVE(elle::serialize::Serializable, ar, value, version)
{
  assert(version == 0);
  value.value.serialize(ar);
}

ELLE_SERIALIZE_SPLIT_T1_LOAD(elle::serialize::Serializable, ar, value, version)
{
  assert(version == 0);
  value.value.deserialize(ar);
}

namespace elle
{
  namespace serialize
  {

    template<typename T>
    struct StoreClassVersion<Serializable<T>>
    {
      static bool const value = false;
    };

  }
}


#endif

