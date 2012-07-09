#ifndef ELLE_SERIALIZE_BASE64ARCHIVE_HH
# define ELLE_SERIALIZE_BASE64ARCHIVE_HH

///
/// Simple Base64 archive encoder and decoder.
///
/// WARNING: Be aware that the serialization may not be completed until
/// the archive object is destroyed. (This applies particularly to
/// this archiver since it has to fill the last bytes with '=' char).
///

# include <cassert>

# include <elle/serialize/fwd.hh>
# include <elle/serialize/BaseArchive.hxx>

namespace elle { namespace serialize {

    // Output specialisation
    template<> class Base64Archive<ArchiveMode::Output> :
      public BaseArchive<ArchiveMode::Output, Base64Archive<ArchiveMode::Output>>
    {
    protected:
      typedef BaseArchive<ArchiveMode::Output, Base64Archive<ArchiveMode::Output>> BaseClass;
      friend class BaseClass::Access;

    private:
      static char const* _base64Chars;

    private:
      std::string _trailingChars;
      std::string _temp;

    public:
      template<typename T>
        Base64Archive(StreamType& stream, T const& value)
          : BaseClass(stream)
        { *this << value; }

      Base64Archive(StreamType& stream);
      ~Base64Archive();

      void SaveBinary(void const* data, size_t size);
    };

    // Input specialization
    template<> class Base64Archive<ArchiveMode::Input> :
      public BaseArchive<ArchiveMode::Input, Base64Archive<ArchiveMode::Input>>
    {
    protected:
      typedef BaseArchive<ArchiveMode::Input, Base64Archive<ArchiveMode::Input>> BaseClass;
      friend class BaseClass::Access;

    private:
      std::string _dataLeft;

    public:
       Base64Archive(StreamType& stream);
      void LoadBinary(void* data, size_t size);

    private:
      static char _DecodeChar(unsigned char c);
    };

}} // !namespace elle::serialize

#endif /* ! BASE64ARCHIVE_HH */


