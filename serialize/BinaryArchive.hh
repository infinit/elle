#ifndef ELLE_SERIALIZE_BINARYARCHIVE_HH
# define ELLE_SERIALIZE_BINARYARCHIVE_HH

# include "BaseArchive.hh"

namespace elle { namespace serialize {

    ///
    /// Binary archives are intended for high performace packing. It uses
    /// exactly the size of serialized objects, with a few extra data when
    /// serializing classes. The serialization itself is portable accross
    /// architectures, but DOES NOT protect you from using non-portable
    /// types, like 'int'.
    ///
    /// « You should always serialize fixed size types like 'int32_t'. »
    ///
    /// @see elle::serialize::BaseArchive for general informations about archives
    ///
    template<
          ArchiveMode mode
        , typename CharType = DefaultCharType
        , template<ArchiveMode, typename>
            class StreamTypeSelect = DefaultStreamTypeSelect
      >
      class BinaryArchive
        : public BaseArchive<
              mode
            , BinaryArchive<mode>
            , CharType
            , StreamTypeSelect
          >
      {
      private:
        typedef BaseArchive<
            mode
          , BinaryArchive<mode>
          , CharType
          , StreamTypeSelect
        >                                         BaseClass;
        typedef typename BaseClass::StreamType    StreamType;

      public:
        BinaryArchive(StreamType& stream)
          : BaseClass(stream)
        {}

        template<typename T> BinaryArchive(StreamType& stream, T& value)
          : BaseClass(stream, value)
        {}
      public:
        using BaseClass::SaveBinary;
        using BaseClass::LoadBinary;
      };

}} // !elle::serialize

#endif /* ! BINARYARCHIVE_HH */


