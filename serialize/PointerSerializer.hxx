#ifndef  ELLE_SERIALIZE_POINTERSERIALIZER_HXX
# define ELLE_SERIALIZE_POINTERSERIALIZER_HXX

# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/serialize/Pointer.hh>

ELLE_SERIALIZE_SPLIT_T1(elle::serialize::Pointer)
ELLE_SERIALIZE_SPLIT_T1_SAVE(elle::serialize::Pointer,
                             archive, value, version)
{
  assert(version == 0);
  if (value._ptr != nullptr)
    {
      archive << true
        << *(value._ptr);
    }
  else
    {
      archive << false;
    }
}

ELLE_SERIALIZE_SPLIT_T1_LOAD(elle::serialize::Pointer,
                             archive, value, version)
{
  assert(version == 0);

  delete value._ptr;
  value._ptr = nullptr;

  bool not_null;
  archive >> not_null;

  if (not_null)
    value._ptr = archive.template Construct<T1>().release();
}

ELLE_SERIALIZE_SPLIT_T1(elle::serialize::AlivePointer);

ELLE_SERIALIZE_SPLIT_T1_SAVE(elle::serialize::AlivePointer,
                             archive, value, version)
{
  //assert(version == 0);
  //if (value._ptr == nullptr)
  //  throw std::runtime_error("Pointer is null, cannot archive it");
  //archive << *(value._ptr);
}

ELLE_SERIALIZE_SPLIT_T1_LOAD(elle::serialize::AlivePointer,
                             archive, value, version)
{
  //assert(version == 0);

  //delete value._ptr;
  //value._ptr = nullptr;

  //value._ptr = archive.template Construct<T1>().release();
}

namespace elle
{
  namespace serialize
  {

    template<typename T>
      struct StoreClassVersion<Pointer<T>>
      {
        static bool const value = false;
      };
    template<typename T>
      struct StoreClassVersion<AlivePointer<T>>
      {
        static bool const value = false;
      };

  }
}

#endif
