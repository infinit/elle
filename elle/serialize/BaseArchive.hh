#ifndef ELLE_SERIALIZE_BASEARCHIVE_HH
# define ELLE_SERIALIZE_BASEARCHIVE_HH

# include <cstring>
# include <cstdint>
# include <iosfwd>
# include <memory>
# include <string>
# include <stdexcept>

# include <boost/noncopyable.hpp>

# include "fwd.hh"

# include "ArchiveMode.hh"
# include "ArchivableClass.hh"
# include "NamedValue.hh"
# include "Polymorphic.hh"
# include "Concrete.hh"
# include "Identity.hh"
# include "StoreClassVersion.hh"
# include "detail/BaseArchiveOperators.hh"

namespace elle
{
  namespace serialize
  {

    ///
    /// Strong typed unsigned short representing the class version for
    /// Save()/Load() overloads. (it gives you a chance to catch the call)
    ///
    struct ClassVersionType
    {
    public:
      uint16_t version;

    public:
      ClassVersionType(uint16_t version) : version(version) {}
      ClassVersionType(ClassVersionType const& other) : version(other.version) {}
      ClassVersionType& operator =(ClassVersionType const& other)
      { version = other.version; return *this; }
    };


    /// Default stream type selector. Select the right stream type depending on
    /// the archive mode.
    template<
        ArchiveMode mode
      , typename CharType
    > struct DefaultStreamTypeSelect;

    template<typename CharType>
      struct DefaultStreamTypeSelect<ArchiveMode::Input, CharType>
        { typedef std::basic_istream<CharType> type; };

    template<typename CharType>
      struct DefaultStreamTypeSelect<ArchiveMode::Output, CharType>
        { typedef std::basic_ostream<CharType> type; };


    /// Default char type used in BaseArchive
    typedef char DefaultCharType;

    ///
    /// The base archive class implements portable raw binary operations, but
    /// should not be used directly (@see BinaryArchive).
    ///
    /// c string are not supported by default, but you can make your own archive
    /// and add methods Load(char const*) and Save(char const*) to achieve that.
    ///
    /// STL containers and strings are fully supported with some limitations:
    ///   - string size is limited to the maximum number contained in
    ///     BaseArchive::StringSizeType
    ///   - list size is limited to the maximum number  contained in
    ///     BaseArchive::SequenceSizeType
    ///
    /// Template parameters:
    ///   - mode_: an archive can only in Input mode, or in Output mode.
    ///   - Archive: the child class (SRTP)
    ///   - CharType_: expected character type for the stream
    ///   - StreamTypeSelect: allows to select any kind of stream types. See
    ///     the default value DefaultStreamTypeSelect for an example.
    ///
    /// A stream type has to implement some of `std::[io]stream' methods in
    /// order to be compatible with archive types. Fortunatly, most of them
    /// only need that streams provide the two methods `read(char*, size_t)'
    /// and `write(char const*, size_t)'.
    ///
    template<
        ArchiveMode mode_
      , typename Archive
      , typename CharType_ = DefaultCharType
      , template<ArchiveMode mode__, typename CharType__>
          class StreamTypeSelect = DefaultStreamTypeSelect
    >
    class BaseArchive:
      public detail::BaseArchiveOperators<mode_, Archive>,
      private boost::noncopyable
    {
    public:
      /// The mode of the archive
      static ArchiveMode const mode = mode_;

      /// CharType implies string type and stream type
      typedef CharType_                                     CharType;

      /// The StreamType might be overriden by a derived class
      typedef typename StreamTypeSelect<
          mode_
        , CharType_
      >::type                                               StreamType;

      // String type
      typedef std::basic_string<CharType_>                  StringType;

      /// Strings size are limited by the type of their size. You may
      /// want to override it in a derived class.
      typedef uint32_t                                      StringSizeType;
      typedef uint32_t                                      SequenceSizeType;


    public:
      ///
      /// This structure has to be a friend of a derived class when SaveBinary()
      /// and LoadBinary() methods are not public (which is recommended).
      ///
      class Access;
      friend struct detail::BaseArchiveOperators<mode_, Archive>; // Operators use Access

    private:
      StreamType& _stream;

    protected:
      BaseArchive(StreamType& stream) :
        _stream(stream)
      {}

      template<typename T>
      BaseArchive(StreamType& stream, T& value) :
        _stream(stream)
      { *this & value; }

    protected:
      /// return the most derived type
      inline Archive& self()   { return *static_cast<Archive*>(this); }

      /// return the stream used to read/write raw data
      inline StreamType&  stream() { return this->_stream; }

    public:
      ///
      /// Load an object on the heap.  If the constructors need arguments, you
      /// will have to override the LoadConstruct function of the
      /// ArchiveSerializer<T> specialization.
      ///
      template <typename T>
      inline
      std::unique_ptr<T>
      Construct()
      {
        T* ptr = nullptr;
        try
          {
            Access::template LoadConstruct<T>(this->self(), ptr);
          }
        catch (std::exception const& err)
          {
            delete ptr;
            throw;
          }
        assert(ptr != nullptr);
        return std::unique_ptr<T>(ptr);
      }

    protected:
      // NamedValue
      template <typename T>
      inline
      void
      Save(NamedValue<T> const& value);
      template <typename T>
      inline
      void
      Load(NamedValue<T> const& value);

      // Polymorphic
      template <typename T>
      inline
      void
      Save(Polymorphic<T> const& value);
      template <typename T>
      inline
      void
      Load(Polymorphic<T> const& value);

      // Concrete
      template <typename T>
      inline
      void
      Save(Concrete<T> const& value);
      template <typename T>
      inline
      void
      Load(Concrete<T> const& value);


      inline void Save(bool val);
      inline void Save(char val);
      inline void Save(uint8_t val);
      inline void Save(uint16_t val);
      inline void Save(uint32_t val);
      inline void Save(uint64_t val);
      inline void Save(int8_t val);
      inline void Save(int16_t val);
      inline void Save(int32_t val);
      inline void Save(int64_t val);
      inline void Save(float val);
      inline void Save(double val);
      inline void Save(std::string const& val);
      inline void Save(ClassVersionType const& classVersion);

      template <typename T>
      inline
      typename std::enable_if<std::is_enum<T>::value == true>::type
      Save(T value);

      template <typename T>
      inline
      typename std::enable_if<std::is_enum<T>::value == false>::type
      Save(T const& value);

      inline void SaveBinary(void const* data, std::streamsize size);

      inline void Load(bool& val);
      inline void Load(char& val);
      inline void Load(int8_t& val);
      inline void Load(uint8_t& val);
      inline void Load(int16_t& val);
      inline void Load(uint16_t& val);
      inline void Load(int32_t& val);
      inline void Load(uint32_t& val);
      inline void Load(int64_t& val);
      inline void Load(uint64_t& val);
      inline void Load(float& val);
      inline void Load(double& val);
      inline void Load(std::string& val);
      inline void Load(ClassVersionType& classVersion);

      template <typename T>
      inline
      typename std::enable_if<std::is_enum<T>::value == true>::type
      Load(T& value);

      template <typename T>
      inline
      typename std::enable_if<std::is_enum<T>::value == false>::type
      Load(T& val);

      template <typename T>
      inline
      void
      LoadConstruct(T*& ptr);

      inline
      void
      LoadBinary(void* data,
                 std::streamsize size);

      // This allows you to write raw c string into an archive
      //inline void Save(char const* val)
      //  { this->self().Save(std::string(val)); }
      //inline void Save(wchar_t const* val)
      //  { this->self().Save(std::wstring(val)); }
    };

  }
} // !namespace elle::serialize

# include "BaseArchive.hxx"

#endif

