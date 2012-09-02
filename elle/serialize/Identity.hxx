#ifndef  ELLE_SERIALIZE_IDENTITY_HXX
# define ELLE_SERIALIZE_IDENTITY_HXX

# include "ArchiveSerializer.hxx"

namespace elle
{
  namespace serialize
  {

    template <typename T>
    struct ArchiveSerializer<Identity<T>>
    {
      template <typename Archive>
      static inline
      void
      Serialize(Archive& ar,
                Identity<T> const& value,
                unsigned int version)
      {
        (void) version;
        ar & value.value;
      }
    };

    template <typename T>
    struct StoreClassVersion<Identity<T>>
    {
      static bool const value = false;
    };

  }
}

#endif
