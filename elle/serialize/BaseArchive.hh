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

# include "ArchiveMode.hh"
# include "ArchivableClass.hh"

namespace elle { namespace serialize {


    ///
    /// Using a NamedValue wrapper for each serialized variable, serialization process
    /// is also available JSON, XML, ...
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

    template<typename T> struct IsNamedValue
      {
        static bool const value = false;
      };
    template<typename T> struct IsNamedValue<NamedValue<T>>
      {
        static bool const value = true;
      };

    template<typename T>
    static inline NamedValue<T> named(std::string const& name, T& value)
      {
        return NamedValue<T>(name, value);
      }

    template<typename T>
    static inline NamedValue<T const> named(std::string const& name, T const& value)
      {
        return NamedValue<T const>(name, value);
      }

    /// Strong typed unsigned int representing the class version for Save()/Load()
    /// overloads.
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


    // Helper to select to right stream type depending on the archive mode
    template<ArchiveMode mode, typename CharType> struct _TypeToStreamType;

    ///
    /// The base archive class implements portable raw binary operations, but
    /// should not be used directly.
    ///
    template<ArchiveMode mode_, typename Archive, typename CharType_ = char>
    class BaseArchive : private boost::noncopyable
    {
    public:
      /// The mode of the archive
      static ArchiveMode const mode = mode_;

      /// CharType implies string type and stream type
      typedef CharType_                                     CharType;

      /// The StreamType might be overrided by a derived class
      typedef typename _TypeToStreamType<
          mode_, CharType_
      >::StreamType                                         StreamType;

      // String type
      typedef std::basic_string<CharType_>                  StringType;

      /// Strings size are limited by the type of their size. You may
      /// want to override it in a derived class.
      typedef uint32_t                                      StringSizeType;
      typedef uint32_t                                      ListSizeType;


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

    protected:
      // Helper to select proper operators overloads
      template<typename T, ArchiveMode _mode> struct _EnableFor
      {
        template<typename _T> struct _IsNamedValue
          { static bool const value = false; };
        template<typename _T> struct _IsNamedValue<NamedValue<_T>>
          { static bool const value = true; };

        typedef typename std::enable_if<
            std::is_same<
                typename boost::call_traits<T>::param_type,
                typename std::add_lvalue_reference<
                    typename std::add_const<T>::type
                >::type
            >::value && mode == _mode && !_IsNamedValue<T>::value,
            Archive&
        > Ref;

        typedef typename std::enable_if<
            std::is_same<
                typename boost::call_traits<T>::param_type,
                typename std::add_const<T>::type
            >::value && mode == _mode && !_IsNamedValue<T>::value,
            Archive&
        > Val;

        typedef typename std::enable_if<
            !std::is_pointer<T>::value && mode == _mode,
            Archive&
        > NotPointer;

        typedef typename std::enable_if<
            !std::is_pointer<T>::value && mode == _mode,
            std::unique_ptr<T, ConstructDeleter<T>>
        > ConstructPtr;

        typedef typename std::enable_if<
          _IsNamedValue<T>::value && mode == _mode,
          Archive&
        > NamedValue;
      };

    private:
      StreamType& _stream;

    protected:
      BaseArchive(StreamType& stream) :
        _stream(stream)
      {}

    protected:
      /// return the most derived type
      inline Archive& self()   { return *static_cast<Archive*>(this); }

      /// return the stream used to read/write raw data
      inline StreamType&  stream() { return this->_stream; }

    public:
      /// Write into the archive (by value overload)
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::Val::
        type operator <<(T val)
        {
          Access::Save(this->self(), val);
          return this->self();
        }

      /// Write into the archive (by reference overload)
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::Ref::
        type operator <<(T const& val)
        {
          Access::Save(this->self(), val);
          return this->self();
        }

      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::NamedValue::
        type operator <<(T const& val)
        {
          Access::SaveNamed(this->self(), val.name, val.value);
          return this->self();
        }

      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NamedValue::
        type operator >>(T const& val)
        {
          Access::LoadNamed(this->self(), val.name, val.value);
          return this->self();
        }

      /// Read from the archive
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NotPointer::
        type operator >>(T& val)
      {
        Access::Load(this->self(), val);
        return this->self();
      }

      /// Redirect to read or write operation depending on the archive type
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Output>::Ref::
        type operator &(T const& val)
        {
          return *this << val;
        }

      /// Redirect to read or write operation depending on the archive type
      template<typename T>
        inline typename _EnableFor<T, ArchiveMode::Output>::Val::type operator &(T const& val)
        {
          return *this << val;
        }

      template<typename T>
        inline typename _EnableFor<T, ArchiveMode::Input>::NamedValue::type operator &(T const& val)
        {
          return *this >> val;
        }
      template<typename T>
        inline typename _EnableFor<T, ArchiveMode::Output>::NamedValue::type operator &(T const& val)
        {
          return *this << val;
        }

      /// Redirect to read or write operation depending on the archive type
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::NotPointer::
        type operator &(T& val)
        {
          return *this >> val;
        }

      /// Load an object on the heap
      /// If the constructors need arguments you will have to override the LoadConstruct
      /// function member of ArchiveSerializer<T>
      template<typename T> inline typename _EnableFor<T, ArchiveMode::Input>::ConstructPtr::
        type Construct()
        {
          T* ptr = static_cast<T*>(::malloc(sizeof(T)));
          if (ptr == nullptr)
            throw std::bad_alloc();
          try
            {
              Access::LoadConstruct(this->self(), ptr);
            }
          catch (std::exception const&)
            {
              ::free(ptr);
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
      template<typename T> inline void Save(T const& val);
      inline void SaveBinary(void const* data, std::streamsize size);

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
      template<typename T> inline void Load(T& val);
      template<typename T> inline void LoadConstruct(T* ptr);
      inline void LoadBinary(void* data, std::streamsize size);

      // This allows you to write raw c string into an archive
      //inline void Save(char const* val)
      //  { this->self().Save(std::string(val)); }
      //inline void Save(wchar_t const* val)
      //  { this->self().Save(std::wstring(val)); }
    };

    template<typename CharType>
      struct _TypeToStreamType<ArchiveMode::Input, CharType>
      {
        typedef std::basic_istream<CharType> StreamType;
      };
    template<typename CharType>
      struct _TypeToStreamType<ArchiveMode::Output, CharType>
      {
        typedef std::basic_ostream<CharType> StreamType;
      };


    //template<typename T>
    //  struct ArchiveSerializer<std::list<T>> :
    //    public SplittedSerializer<std::list<T>>
    //  {
    //    template<typename Archive>
    //      static inline void Save(Archive& ar, std::list<T> const& val, unsigned int)
    //      {
    //        typedef typename Archive::ListSizeType SizeType;
    //        static_assert(std::is_unsigned<SizeType>::value, "SizeType has to be unsigned");
    //        size_t size = val.size();
    //        if (size > static_cast<SizeType>(-1))
    //          throw std::runtime_error("List too big");
    //        ar << static_cast<SizeType>(size);
    //        auto it = val.begin(), end = val.end();
    //        for (; it != end; ++it)
    //          {
    //            ar << *it;
    //          }
    //      }

    //    template<typename Archive>
    //      static inline void Load(Archive& ar, std::list<T>& val, unsigned int)
    //      {
    //        typedef typename Archive::ListSizeType SizeType;
    //        static_assert(std::is_unsigned<SizeType>::value, "SizeType has to be unsigned");
    //        SizeType size;
    //        ar >> size;
    //        for (SizeType i = 0; i < size; ++i)
    //          {
    //            T t;
    //            ar >> t;
    //            val.push_back(t);
    //          }
    //      }
    //};

}} // !namespace elle::serialize

#endif

