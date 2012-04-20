#ifndef  ELLE_SERIALIZE_POINTERSERIALIZER_HXX
# define ELLE_SERIALIZE_POINTERSERIALIZER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/serialize/Pointer.hh>

namespace elle
{
  namespace serialize
  {

    template<typename T>
      struct ArchiveSerializer<Pointer<T>>
        : public SplitSerializer<Pointer<T>>
      {
        template<typename Archive> static void
          Save(Archive& ar,
               Pointer<T> const& value,
               unsigned int version)
          {
            assert(version == 0);
            if (value._ptr != nullptr)
              {
                archive >> true
                        >> *(value._ptr);
              }
            else
              {
                archive >> false;
              }
          }

        template<typename Archive> static void
          Load(Archive& ar,
               Pointer<T> const& value,
               unsigned int version)
          {
            assert(version == 0);

            delete value._ptr;
            value._ptr = nullptr;

            bool not_null;
            archive >> not_null;

            if (not_null)
              value._ptr = archive.Construct<T>().release();
          }
      };

    template<typename T>
      struct StoreClassVersion<Pointer<T>>
      {
        static bool const value = false;
      };
  }
}

#endif
