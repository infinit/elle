#ifndef ELLE_SERIALIZE_BASEARCHIVE_HH
# define ELLE_SERIALIZE_BASEARCHIVE_HH

# include <cstring>
# include <cstdint>
# include <iosfwd>
# include <list>
# include <memory>
# include <string>
# include <stdexcept>

# include <boost/noncopyable.hpp>
# include <boost/call_traits.hpp>
# include <boost/detail/endian.hpp>

# include "ArchiveMode.hh"

namespace elle { namespace serialize {

    ///
    /// This structure is used to determine the version of a class serialization.
    /// The version defaults to 0, but you may want to specialize this class
    /// to set a superior version number.
    ///
    template<typename T> struct ArchivableClass
    {
      static const unsigned int version = 0;
    };

    /// Convenient macro to declare the version of a serialized class
# define ELLE_ARCHIVE_CLASS_VERSION(Class_, version_)                             \
    template<> struct ArchivableClass                                             \
    {                                                                             \
      static const unsigned int version = version_;                               \
    };                                                                            \

    ///
    /// The BaseArchiveSerializer define common standard function involved in
    /// the serialization process. You might want to derive from this struct for
    /// each specialization of the template class ArchiveSerializer.
    ///
    template<typename T> struct BaseArchiveSerializer
    {
      ///
      /// Having a chance to construct yourself the object (with a placement new)
      /// can be achieved by overriding this function.
      /// The default implementation just construct the object without parameters
      /// and forward to standard Load() method.
      ///
      template<typename Archive>
      static inline void LoadConstruct(Archive&, T* ptr, unsigned int)
      {
        assert(ptr != nullptr);
        static_assert(!std::is_pointer<T>::value, "You cannot construct pointers...");
        new (ptr) T;
      }

    };

    ///
    /// This type is instanciated for any user defined type. You will have to
    /// make a specialization for each user defined type.
    ///
    template<typename T> struct ArchiveSerializer : BaseArchiveSerializer<T>
    {
      ///
      /// Generic method used by either serialization and deserialization
      /// process. You may want to use the '&' operator that do the right
      /// operation depending on the archive mode.
      ///
      template<typename Archive>
      static void Serialize(Archive&, T&, unsigned int)
      {
        static_assert(
            ArchivableClass<T>::version != 0,
            "You have to specialize ArchiveSerializer<T> for your type"
        );
      }
    };

    template<typename T>
    struct NamedValue
    {
      std::string name;
      T const& value;

      NamedValue(std::string const& name, T const& value) :
        name(name), value(value)
      {}
    };



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
      ///
      /// This structure has to be a friend of a derived class when SaveBinary()
      /// and LoadBinary() methods are not public (which is recommended).
      ///
      class Access
      {
      public:
        static inline void SaveBinary(Archive& ar, void const* data, size_t size)
        {
          ar.SaveBinary(data, size);
        }
        static inline void LoadBinary(Archive& ar, void* data, size_t size)
        {
          ar.LoadBinary(data, size);
        }

        static inline void Save(Archive& ar, char val)      { ar.Save(val); }
        static inline void Save(Archive& ar, int8_t val)    { ar.Save(val); }
        static inline void Save(Archive& ar, uint8_t val)   { ar.Save(val); }
        static inline void Save(Archive& ar, int16_t val)   { ar.Save(val); }
        static inline void Save(Archive& ar, uint16_t val)  { ar.Save(val); }
        static inline void Save(Archive& ar, uint32_t val)  { ar.Save(val); }
        static inline void Save(Archive& ar, int32_t val)   { ar.Save(val); }
        static inline void Save(Archive& ar, uint64_t val)  { ar.Save(val); }
        static inline void Save(Archive& ar, int64_t val)   { ar.Save(val); }

        static inline void Load(Archive& ar, char& val)     { ar.Load(val); }
        static inline void Load(Archive& ar, int8_t& val)   { ar.Load(val); }
        static inline void Load(Archive& ar, uint8_t& val)  { ar.Load(val); }
        static inline void Load(Archive& ar, int16_t& val)  { ar.Load(val); }
        static inline void Load(Archive& ar, uint16_t& val) { ar.Load(val); }
        static inline void Load(Archive& ar, int32_t& val)  { ar.Load(val); }
        static inline void Load(Archive& ar, uint32_t& val) { ar.Load(val); }
        static inline void Load(Archive& ar, int64_t& val)  { ar.Load(val); }
        static inline void Load(Archive& ar, uint64_t& val) { ar.Load(val); }

        static inline void Save(Archive& ar, ClassVersionType const& classVersion)
          { ar.Save(classVersion); }
        static inline void Load(Archive& ar, ClassVersionType& classVersion)
          { ar.Load(classVersion); }


        template<typename T> static inline void Save(Archive& ar, T const& val)
          { ar.Save(val); }
        template<typename T> static inline void Load(Archive& ar, T& val)
          { ar.Load(val); }
        template<typename T> static inline void LoadConstruct(Archive& ar, T* ptr)
          { ar.LoadConstruct(ptr); }
        template<typename T> static inline void SaveNamed(Archive& ar,
                                                          std::string const& name,
                                                          T const& value)
          { ar.SaveNamed(name, value); }
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
      template<typename T>
      inline typename _EnableFor<T, ArchiveMode::Output>::Val::type operator <<(T val)
      {
        Access::Save(this->self(), val);
        return this->self();
      }

      /// Write into the archive (by reference overload)
      template<typename T>
      inline typename _EnableFor<T, ArchiveMode::Output>::Ref::type operator <<(T const& val)
      {
        Access::Save(this->self(), val);
        return this->self();
      }

      template<typename T>
      inline typename _EnableFor<T, ArchiveMode::Output>::NamedValue::type operator <<(T const& val)
      {
        Access::SaveNamed(this->self(), val.name, val.value);
        return this->self();
      }

      /// Read from the archive
      template<typename T>
      inline typename _EnableFor<T, ArchiveMode::Input>::NotPointer::type operator >>(T& val)
      {
        Access::Load(this->self(), val);
        return this->self();
      }

      /// Redirect to read or write operation depending on the archive type
      template<typename T>
      inline typename _EnableFor<T, ArchiveMode::Output>::Ref::type operator &(T const& val)
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
      inline typename _EnableFor<T, ArchiveMode::Output>::NamedValue::type operator &(T const& val)
      {
        Access::SaveNamed(this->self(), val.name, val.value);
        return this->self();
      }

      /// Redirect to read or write operation depending on the archive type
      template<typename T>
      inline typename _EnableFor<T, ArchiveMode::Input>::NotPointer::type operator &(T& val)
      {
        return *this >> val;
      }

      template<typename T>
      inline typename _EnableFor<T, ArchiveMode::Input>::ConstructPtr::type Construct()
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
      template<typename T>
      inline void SaveNamed(std::string const&, T const& value)
      {
        *this << value;
      }

      inline void Save(char val)      { Access::Save(this->self(), static_cast<int8_t>(val)); }
      inline void Save(uint8_t val)   { Access::Save(this->self(), static_cast<int8_t>(val)); }
      inline void Save(uint16_t val)  { Access::Save(this->self(), static_cast<int16_t>(val)); }
      inline void Save(uint32_t val)  { Access::Save(this->self(), static_cast<int32_t>(val)); }
      inline void Save(uint64_t val)  { Access::Save(this->self(), static_cast<int64_t>(val)); }

      inline void Load(char& val)     { Access::Load(this->self(), reinterpret_cast<int8_t&>(val)); }
      inline void Load(uint8_t& val)  { Access::Load(this->self(), reinterpret_cast<int8_t&>(val)); }
      inline void Load(uint16_t& val) { Access::Load(this->self(), reinterpret_cast<int16_t&>(val)); }
      inline void Load(uint32_t& val) { Access::Load(this->self(), reinterpret_cast<int32_t&>(val)); }
      inline void Load(uint64_t& val) { Access::Load(this->self(), reinterpret_cast<int64_t&>(val)); }

      // This allows you to write raw c string into an archive
      //inline void Save(char const* val)
      //  { this->self().Save(std::string(val)); }
      //inline void Save(wchar_t const* val)
      //  { this->self().Save(std::wstring(val)); }

      inline void Save(int8_t val)  { Access::SaveBinary(this->self(), &val, sizeof(val)); }
      inline void Load(int8_t& val) { Access::LoadBinary(this->self(), &val, sizeof(val)); }

      /// Save int16_t
      inline void Save(int16_t val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::SaveBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)] = {
              static_cast<char>((val & 0x00ff)      ),
              static_cast<char>((val & 0xff00) >> 8 ),
          };
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::SaveBinary(this->self(), tab, sizeof(tab));
# endif
        }

      /// Load int16_t
      inline void Load(int16_t& val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::LoadBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)];
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::LoadBinary(this->self(), &tab, sizeof(tab));
          val =
              (static_cast<int16_t>(tab[0])      ) +
              (static_cast<int16_t>(tab[1]) << 8 )
            ;
# endif
        }

      /// Save int32_t
      inline void Save(int32_t val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::SaveBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)] = {
              static_cast<char>((val & 0x000000ff)      ),
              static_cast<char>((val & 0x0000ff00) >> 8 ),
              static_cast<char>((val & 0x00ff0000) >> 16),
              static_cast<char>((val & 0xff000000) >> 24),
          };
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::SaveBinary(this->self(), &tab, sizeof(tab));
# endif
        }

      /// Load int32_t
      inline void Load(int32_t& val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::LoadBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)];
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::LoadBinary(this->self(), &tab, sizeof(tab));
          val =
              (static_cast<int32_t>(tab[0])      ) +
              (static_cast<int32_t>(tab[1]) << 8 ) +
              (static_cast<int32_t>(tab[2]) << 16) +
              (static_cast<int32_t>(tab[3]) << 24)
            ;
# endif
        }

      /// Save int64_t
      inline void Save(int64_t val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::SaveBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)] = {
              static_cast<char>((val & 0x00000000000000ff)      ),
              static_cast<char>((val & 0x000000000000ff00) >> 8 ),
              static_cast<char>((val & 0x0000000000ff0000) >> 16),
              static_cast<char>((val & 0x00000000ff000000) >> 24),
              static_cast<char>((val & 0x000000ff00000000) >> 32),
              static_cast<char>((val & 0x0000ff0000000000) >> 40),
              static_cast<char>((val & 0x00ff000000000000) >> 48),
              static_cast<char>((val & 0xff00000000000000) >> 56),
          };
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::SaveBinary(this->self(), tab, sizeof(tab));
# endif
        }

      /// Load int64_t
      inline void Load(int64_t& val)
        {
# ifdef BOOST_LITTLE_ENDIAN
          Access::LoadBinary(this->self(), &val, sizeof(val));
# else
          char tab[sizeof(val)];
          static_assert(sizeof(val) == sizeof(tab), "Something is wrong with me");
          Access::LoadBinary(this->self(), &tab, sizeof(tab));
          val =
              (static_cast<int64_t>(tab[0])      ) +
              (static_cast<int64_t>(tab[1]) << 8 ) +
              (static_cast<int64_t>(tab[2]) << 16) +
              (static_cast<int64_t>(tab[3]) << 24) +
              (static_cast<int64_t>(tab[4]) << 32) +
              (static_cast<int64_t>(tab[5]) << 40) +
              (static_cast<int64_t>(tab[6]) << 48) +
              (static_cast<int64_t>(tab[7]) << 56)
            ;
# endif
        }

      /// Save float
      inline void Save(float val)
        {
          static_assert(sizeof(val) == 4, "float size is not standard");
          Access::SaveBinary(this->self(), &val, sizeof(val));
        }

      /// Load float
      inline void Load(float& val)
        {
          static_assert(sizeof(val) == 4, "float size is not standard");
          Access::LoadBinary(this->self(), &val, sizeof(val));
        }

      /// Save double
      inline void Save(double val)
        {
          static_assert(sizeof(val) == 8, "double size is not standard");
          Access::SaveBinary(this->self(), &val, sizeof(val));
        }

      /// Load double
      inline void Load(double& val)
        {
          static_assert(sizeof(val) == 8, "double size is not standard");
          Access::LoadBinary(this->self(), &val, sizeof(val));
        }

      /// Save std::string
      inline void Save(std::string const& val)
        {
          typedef typename Archive::StringSizeType SizeType;
          static_assert(std::is_unsigned<SizeType>::value, "A string size type have to be unsigned");
          size_t sz = val.size();
          if (static_cast<size_t>(static_cast<SizeType>(-1)) < sz)
            throw std::runtime_error("String size too big");
          this->self().Save(static_cast<SizeType>(sz));
          Access::SaveBinary(this->self(), val.data(), sz);
        }

      /// Load std::string
      inline void Load(std::string& val)
        {
          typedef typename Archive::StringSizeType SizeType;
          static_assert(std::is_unsigned<SizeType>::value, "A string size type have to be unsigned");
          SizeType size;
          this->self().Load(size);
          val.resize(size);
          char tab[256];
          size_t idx = 0;
          while (size > 256)
            {
              Access::LoadBinary(this->self(), tab, 256);
              val.replace(idx, 256, tab, 256);
              size -= 256;
              idx += 256;
            }
          if (size > 0)
            {
              Access::LoadBinary(this->self(), tab, size);
              val.replace(idx, size, tab, size);
            }
        }

      /// Save class version
      inline void Save(ClassVersionType const& classVersion)
      {
        Access::Save(this->self(), classVersion.version);
      }

      /// Load class version
      inline void Load(ClassVersionType& classVersion)
      {
        Access::Load(this->self(), classVersion.version);
      }

      //inline void Save(std::wstring const& val)
      //  {
      //    typedef typename ArchiveType::StringSizeType SizeType;
      //    static_assert(std::is_unsigned<SizeType>::value, "A string size type have to be unsigned");
      //    if (static_cast<size_t>(static_cast<SizeType>(-1)) < val.size())
      //      throw std::runtime_error("String size too big");
      //  }


      /// Generic save method. Uses an explicit specialization of ArchiveSerializer.
      template<typename T> inline void Save(T const& val)
        {
          Access::Save(this->self(), ClassVersionType(ArchivableClass<T>::version));
          typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
          // this const_cast is safe since the archive is in output mode
          Serializer::Serialize(
              this->self(),
              const_cast<T&>(val),
              ArchivableClass<T>::version
          );
        }

      /// Generic load method. Uses an explicit specialization of ArchiveSerializer.
      template<typename T> inline void Load(T& val)
        {
          ClassVersionType classVersion(0);
          Access::Load(this->self(), classVersion);

          typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
          Serializer::Serialize(this->self(), val, classVersion.version);
        }

      template<typename T> inline void LoadConstruct(T* ptr)
        {
          assert(ptr != nullptr);
          ClassVersionType classVersion(0);
          Access::Load(this->self(), classVersion);
          ArchiveSerializer<T>::LoadConstruct(this->self(), ptr, classVersion.version);
        }

      /// This is the last method called by any serialization method
      /// You may want to override it to change the serialization format
      inline void SaveBinary(void const* data, std::streamsize size)
        {
          this->stream().write(static_cast<char const*>(data), size);
        }

      /// This is the last method called by any serialization method
      /// You may want to override it to change the serialization format
      inline void LoadBinary(void* data, std::streamsize size)
        {
          this->stream().read(static_cast<char*>(data), size);
        }
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

    template<typename T, ArchiveMode mode> struct _SplittedSerializerSelectMethod;

    template<typename T>
      struct _SplittedSerializerSelectMethod<T, ArchiveMode::Input>
      {
        template<typename Archive>
          static inline void Serialize(Archive& ar, T& val, unsigned int version)
          {
            ArchiveSerializer<T>::Load(ar, val, version);
          }
      };

    template<typename T>
      struct _SplittedSerializerSelectMethod<T, ArchiveMode::Output>
      {
        template<typename Archive>
        static inline void Serialize(Archive& ar, T const& val, unsigned int version)
        {
          ArchiveSerializer<T>::Save(ar, val, version);
        }
      };

    template<typename T>
      struct SplittedSerializer : BaseArchiveSerializer<T>
      {
        template<typename Archive>
        static inline void Serialize(Archive& ar, T& val, unsigned int version)
        {
          _SplittedSerializerSelectMethod<T, Archive::mode>::Serialize(ar, val, version);
        }
      };


    template<typename T>
      struct ArchiveSerializer<std::list<T>> :
        public SplittedSerializer<std::list<T>>
      {
        template<typename Archive>
          static inline void Save(Archive& ar, std::list<T> const& val, unsigned int)
          {
            typedef typename Archive::ListSizeType SizeType;
            static_assert(std::is_unsigned<SizeType>::value, "SizeType has to be unsigned");
            size_t size = val.size();
            if (size > static_cast<SizeType>(-1))
              throw std::runtime_error("List too big");
            ar << static_cast<SizeType>(size);
            auto it = val.begin(), end = val.end();
            for (; it != end; ++it)
              {
                ar << *it;
              }
          }

        template<typename Archive>
          static inline void Load(Archive& ar, std::list<T>& val, unsigned int)
          {
            typedef typename Archive::ListSizeType SizeType;
            static_assert(std::is_unsigned<SizeType>::value, "SizeType has to be unsigned");
            SizeType size;
            ar >> size;
            for (SizeType i = 0; i < size; ++i)
              {
                T t;
                ar >> t;
                val.push_back(t);
              }
          }
    };

}} // !namespace elle::serialize

#endif

