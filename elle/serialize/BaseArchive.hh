#ifndef ELLE_SERIALIZE_BASEARCHIVE_HH
# define ELLE_SERIALIZE_BASEARCHIVE_HH

# include <cstring>
# include <cstdint>
# include <iosfwd>
# include <memory>
# include <string>
# include <stdexcept>

# include <boost/noncopyable.hpp>
# include <boost/call_traits.hpp>


# include "fwd.hh"
# include "ArchiveMode.hh"
# include "ArchivableClass.hh"
# include "Serializable.hh"

namespace elle { namespace serialize {

    ///
    /// Using a NamedValue wrapper for each serialized variable, serialization
    /// process is also available for JSON, XML, ...
    ///
    /// @see elle::serialize::named() for an helper
    ///
    template<typename T> struct NamedValue
      {
        std::string name;
        T& value;

        NamedValue(std::string const& name, T& value) :
          name(name), value(value)
        {}
        NamedValue(NamedValue&& o) :
          name(std::move(o.name)),
          value(o.value)
        {}
      };

    namespace detail
    {
      // assume clean type
      template<typename T> struct IsNamedValue
        { static bool const value = false; };
      template<typename T> struct IsNamedValue<NamedValue<T>>
        { static bool const value = true;  };

    }

    /// Check whether or not the type T could be expressed as NamedValue<K>.
    template<typename T> struct IsNamedValue
      {
        static bool const value = detail::IsNamedValue<
            typename std::remove_cv<
              typename std::remove_reference<T>::type
            >::type
        >::value;
      };

    /// Helper to infer the right NamedValue<T> type.
    template<typename T> static inline
      NamedValue<T> named(std::string const& name, T& value)
      {
        return NamedValue<T>(name, value);
      }
    template<typename T> static inline
      NamedValue<T const> named(std::string const& name, T const& value)
      {
        return NamedValue<T const>(name, value);
      }

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

    ///
    /// Each specialized type gets its version stored along with the object
    /// itself. To change this behavior, you just have to define a specialization
    /// for each type that doesn't need to store its version.
    ///
    template<typename T> struct StoreClassVersion
      { static bool const value = true; };

    template<typename T> struct StoreClassVersion<T&>
      { static bool const value = StoreClassVersion<T>::value; };
    template<typename T> struct StoreClassVersion<T const>
      { static bool const value = StoreClassVersion<T>::value; };
    template<typename T> struct StoreClassVersion<T volatile>
      { static bool const value = StoreClassVersion<T>::value; };
    template<typename T> struct StoreClassVersion<T*>
      { static_assert(std::is_same<T,T>::value, "Should not happen !"); };

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
    class BaseArchive : private boost::noncopyable
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


    protected:
      ///
      /// This structure has to be a friend of a derived class when SaveBinary()
      /// and LoadBinary() methods are not public (which is recommended).
      ///
      class Access;

      /// unique_ptr returned by the Construct method use this deleter
      template<typename T> struct ConstructDeleter
      {
        void operator()(T *ptr) const
        {
          ptr->~T();
          ::free(ptr);
        }
      };

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

    protected:
      // Helper to select proper operators overloads
      template<typename T, ArchiveMode _mode> struct _EnableFor;

    public:
      ///////////////////////////////////////////////////////////////////////
      // OutputArchive::operator <<

      /// output_ar << value
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::Val::
        type operator <<(T val)
        {
          Access::Save(this->self(), val);
          return this->self();
        }

      /// output_ar << const_ref
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::Ref::
        type operator <<(T const& val)
        {
          Access::Save(this->self(), val);
          return this->self();
        }

      /// output_ar << named_value
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::NamedValue::
        type operator <<(T const& val)
        {
          Access::SaveNamed(this->self(), val.name, val.value);
          return this->self();
        }

      ///////////////////////////////////////////////////////////////////////
      // OutputArchive::operator & (redirect to operator <<)

      /// output_ar & ref
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::Ref::
        type operator &(T const& val)
        {
          return *this << val;
        }

      /// output_ar & value
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::Val::
        type operator &(T val)
        {
          return *this << val;
        }

      /// output_ar & named_value
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::NamedValue::
        type operator &(T const& val)
        {
          return *this << val;
        }

      ///////////////////////////////////////////////////////////////////////
      // InputArchive::operator >>

      /// input_ar >> ref_of_non_pointer
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NotPointer::
        type operator >>(T& val)
        {
          Access::Load(this->self(), val);
          return this->self();
        }

      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NotPointer::
        type operator >>(T const&& val)
        {
          Access::Load(this->self(), val);
          return this->self();
        }

      /// input_ar >> named_value
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NamedValue::
        type operator >>(T const& val)
        {
          Access::LoadNamed(this->self(), val.name, val.value);
          return this->self();
        }

      ///////////////////////////////////////////////////////////////////////
      // InputArchive::operator & (redirect to operator >>)

      /// input_ar & ref_of_non_pointer
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NotPointer::
        type operator &(T& val)
        {
          return *this >> val;
        }

      /// intput_ar & named_value
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NamedValue::
        type operator &(T const& val)
        {
          return *this >> val;
        }

      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NotPointer::
        type operator &(T const&& val)
        {
          Access::Load(this->self(), val);
          return this->self();
        }

      ///
      /// Load an object on the heap.  If the constructors need arguments, you
      /// will have to override the LoadConstruct function of the
      /// ArchiveSerializer<T> specialization.
      ///
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::ConstructPtr::
        type Construct()
        {
          T* ptr = static_cast<T*>(::operator new(sizeof(T)));
          if (ptr == nullptr)
            throw std::bad_alloc();
          try
            {
              Access::LoadConstruct(this->self(), ptr);
            }
          catch (std::exception const&)
            {
              //::free(ptr);
              delete ptr;
              throw;
            }
          return std::unique_ptr<T, ConstructDeleter<T>>(ptr);
        }

    protected:
      // Default implementation ignores the name
      template<typename T> inline void SaveNamed(std::string const&, T const& value)
      {
        *this << value;
      }
      template<typename T> inline void LoadNamed(std::string const&, T& value)
      {
        *this >> value;
      }

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
      template<typename T>
        inline typename std::enable_if<std::is_enum<T>::value == true>::type
        Save(T value);
      template<typename T>
        inline typename std::enable_if<
              std::is_enum<T>::value == false
        >::type
        Save(T const& value);
      //template<typename T>
      //  inline typename std::enable_if<
      //    elle::concept::IsSerializable<T, Archive>::value == true
      //  >::type
      //  Save(T const& value);
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
      template<typename T>
        inline typename std::enable_if<std::is_enum<T>::value == true>::type
        Load(T& value);
      template<typename T>
        inline typename std::enable_if<
              std::is_enum<T>::value == false
          &&  IsNamedValue<T>::value == false
        >::type Load(T& val);

      //template<typename T>
      //  inline typename std::enable_if<
      //    elle::concept::IsSerializable<T, Archive>::value == true
      //  >::type
      //  Load(T& val);
      template<typename T>
        inline void LoadConstruct(T* ptr);
      inline void LoadBinary(void* data, std::streamsize size);

      // This allows you to write raw c string into an archive
      //inline void Save(char const* val)
      //  { this->self().Save(std::string(val)); }
      //inline void Save(wchar_t const* val)
      //  { this->self().Save(std::wstring(val)); }
    };

}} // !namespace elle::serialize

#endif

