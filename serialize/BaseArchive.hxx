#ifndef ELLE_SERIALIZE_BASEARCHIVE_HXX
# define ELLE_SERIALIZE_BASEARCHIVE_HXX

# include "BaseArchive.hh"
# include "Serializer.hh"
# include "StoreFormat.hh"
# include "StaticFormat.hh"

# include <elle/idiom/Close.hh>
#  include <iostream> // XXX
#  include <boost/detail/endian.hpp>
#  include <boost/call_traits.hpp>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace serialize
  {

# define __ELLE_SERIALIZE_BASE_ARCHIVE_TPL                                    \
    template<                                                                 \
        ArchiveMode mode_,                                                    \
        typename Archive,                                                     \
        typename CT,                                                          \
        template<ArchiveMode, typename> class STS                             \
    >                                                                         \
/**/

    //-------------------------------------------------------------------------



    struct AccessDynamicFormat
    {
      template <typename T>
      struct Clean
      {
        typedef typename std::remove_cv<T>::type Type;
      };

      template <typename T>
      struct _HasDynamicFormat
      {
        typedef typename Clean<T>::Type Derived;
        typedef DynamicFormat<Derived> Base;
        static bool const value = std::is_base_of<Derived, Base>::value;
      };

      template <typename T>
      static inline
      typename std::enable_if<_HasDynamicFormat<T>::value, uint16_t>::type
      fetch(T const& val)
      {
        return static_cast<
            DynamicFormat<typename Clean<T>::Type> const&
        >(val).version();
      }

      template <typename T>
      static inline
      typename std::enable_if<!_HasDynamicFormat<T>::value, uint16_t>::type
      fetch(T const&)
      {
        return StaticFormat<typename Clean<T>::Type>::version;
      }

      template <typename T>
      static inline
      typename std::enable_if<_HasDynamicFormat<T>::value>::type
      set(T& val, uint16_t version)
      {
        return static_cast<
            DynamicFormat<typename Clean<T>::Type>&
        >(val).version(version);
      }

      template<typename T>
      static inline
      typename std::enable_if<!_HasDynamicFormat<T>::value>::type
      set(T& val, uint16_t)
      {/* Do nothing */}

    };

    /// Generic save method. Uses an explicit specialization of
    /// Serializer.
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    typename std::enable_if<std::is_enum<T>::value == false>::type
    BaseArchive<mode_, Archive, CT, STS>::Save(T const& val)
    {
      Format format(AccessDynamicFormat::fetch(val));

      if (StoreFormat<T>::value == true)
          Access::Save(this->self(), format);

      typedef Serializer<typename std::remove_cv<T>::type> Serializer;
      // this const_cast is safe since the archive is in output mode
      Serializer::Serialize(this->self(), const_cast<T&>(val), format.version);
    }

    //-------------------------------------------------------------------------

    /// Generic load method. Uses an explicit specialization of
    /// Serializer.
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    typename std::enable_if<std::is_enum<T>::value == false>::type
    BaseArchive<mode_, Archive, CT, STS>::Load(T& val)
    {
      Format format(0);
      if (StoreFormat<T>::value == true)
        {
          Access::Load(this->self(), format);
          AccessDynamicFormat::set(val, format.version);
        }

      typedef Serializer<typename std::remove_cv<T>::type> Serializer;
      Serializer::Serialize(this->self(), val, format.version);
    }

    //-------------------------------------------------------------------------

    /// Generic load construct. Uses an explicit specialization of
    /// Serializer.
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::LoadConstruct(T*& ptr)
    {
      assert(ptr == nullptr);
      //Format classVersion(0);
      //if (StoreFormat<T>::value == true)
      //  Access::Load(this->self(), classVersion);
      typedef Serializer<typename std::remove_cv<T>::type> Serializer;
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      ELLE_TRACE("Load construct %p with its concrete type %s",
                 this, ELLE_PRETTY_TYPE(T)
        ) Serializer::template LoadConstruct<Archive>(this->self(), ptr);
    }

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(NamedValue<T> const& value)
    {
      Access::Save(this->self(), value.value);
    }

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(NamedValue<T> const& value)
    {
      Access::Load(this->self(), value.value);
    }


    // Polymorphic
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(Polymorphic<T> const& value)
    {
      static_assert(
          IsSerializable<T, Archive>::value,
          "Polymorphic used with a type that is not Serializable!"
      );
      typedef typename SerializableFor<Archive>::Type interface_t;
      static_cast<interface_t const&>(value.value).serialize(this->self());
    }

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(Polymorphic<T> const& value)
    {
      static_assert(
          IsSerializable<T, Archive>::value,
          "Polymorphic used with a type that is not Serializable!"
      );
      typedef typename SerializableFor<Archive>::Type interface_t;
      static_cast<interface_t&>(value.value).deserialize(this->self());
    }

    // Concrete
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(Concrete<T> const& value)
    {
      Access::Save(this->self(), value.value);
    }

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(Concrete<T> const& value)
    {
      Access::Load(this->self(), value.value);
    }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(int16_t val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      ELLE_TRACE("Saving int16_t '%s'", val);

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

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(int16_t& val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");

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
      ELLE_TRACE("Loaded int16_t '%s'", val);
    }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(int32_t val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      ELLE_TRACE("Saving int32_t '%s'", val);

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

    //-------------------------------------------------------------------------

      /// Load int32_t
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(int32_t& val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");

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
      ELLE_TRACE("Loaded int32_t '%s'", val);
    }

    //-------------------------------------------------------------------------

      /// Save int64_t
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(int64_t val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      ELLE_TRACE("Saving int64_t '%s'", val);

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

    //-------------------------------------------------------------------------

      /// Load int64_t
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(int64_t& val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");

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
      ELLE_TRACE("Loaded int64_t '%s'", val);
    }

    //-------------------------------------------------------------------------

      /// Save float
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(float val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      ELLE_TRACE("Saving float '%s'", val);

      static_assert(sizeof(val) == 4, "float size is not standard");
      Access::SaveBinary(this->self(), &val, sizeof(val));
    }

    //-------------------------------------------------------------------------

      /// Load float
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(float& val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");

      static_assert(sizeof(val) == 4, "float size is not standard");
      Access::LoadBinary(this->self(), &val, sizeof(val));
      ELLE_TRACE("Loaded float '%s'", val);
    }

    //-------------------------------------------------------------------------

      /// Save double
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(double val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      ELLE_TRACE("Saving double '%s'", val);

      static_assert(sizeof(val) == 8, "double size is not standard");
      Access::SaveBinary(this->self(), &val, sizeof(val));
    }

    //-------------------------------------------------------------------------

      /// Load double
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(double& val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");

      static_assert(sizeof(val) == 8, "double size is not standard");
      Access::LoadBinary(this->self(), &val, sizeof(val));
      ELLE_TRACE("Loaded double '%s'", val);
    }

    //-------------------------------------------------------------------------

      /// Save std::string
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(std::string const& val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      ELLE_TRACE("Saving string '%s'", val);

      typedef typename Archive::StringSizeType SizeType;
      static_assert(std::is_unsigned<SizeType>::value, "A string size type have to be unsigned");
      size_t sz = val.size();
      if (static_cast<size_t>(static_cast<SizeType>(-1)) < sz)
        throw std::runtime_error("String size too big");
      Access::Save(this->self(), static_cast<SizeType>(sz));
      Access::SaveBinary(this->self(), val.c_str(), sz);
    }

    //-------------------------------------------------------------------------

      /// Load std::string
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(std::string& val)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");

      typedef typename Archive::StringSizeType SizeType;
      static_assert(std::is_unsigned<SizeType>::value, "A string size type have to be unsigned");
      SizeType size;
      Access::Load(this->self(), size);
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
      ELLE_TRACE("Loaded string '%s'", val);
    }

    //-------------------------------------------------------------------------

      /// Save class version
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(Format const& value)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      ELLE_TRACE("Saving class version '%s'", value.version);
      Access::Save(this->self(), value.version);
    }

    //-------------------------------------------------------------------------

      /// Load class version
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(Format& value)
    {
      ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
      Access::Load(this->self(), value.version);
      ELLE_TRACE("Loaded class version '%s'", value.version);
    }


    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template <typename T>
    inline
    typename std::enable_if<std::is_enum<T>::value == true>::type
    BaseArchive<mode_, Archive, CT, STS>::Save(T value)
    {
      assert(static_cast<unsigned int>(value) < 65536);
      Access::Save(this->self(), static_cast<uint16_t>(value));
    }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    template<typename T>
    inline
    typename std::enable_if<std::is_enum<T>::value == true>::type
    BaseArchive<mode_, Archive, CT, STS>::Load(T& value)
    {
      uint16_t value_;
      Access::Load(this->self(), value_);
      value = static_cast<T>(value_);
    }

    //-------------------------------------------------------------------------

      /// This is the last method called by any serialization method
      /// You may want to override it to change the serialization format
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::SaveBinary(void const* data,
                                                     std::streamsize size)
    {
      this->stream().write(static_cast<char const*>(data), size);
    }

    //-------------------------------------------------------------------------

    /// This is the last method called by any serialization method
    /// You may want to override it to change the serialization format
    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::LoadBinary(void* data,
                                                     std::streamsize size)
    {
      this->stream().read(static_cast<char*>(data), size);
    }


    //-------------------------------------------------------------------------


    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(int8_t val)
    { Access::SaveBinary(this->self(), &val, sizeof(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(int8_t& val)
    { Access::LoadBinary(this->self(), &val, sizeof(val)); }

    //-------------------------------------------------------------------------


    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(bool val)
    { Access::Save(this->self(), static_cast<int8_t>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(char val)
    { Access::Save(this->self(), static_cast<int8_t>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(uint8_t val)
    { Access::Save(this->self(), static_cast<int8_t>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(uint16_t val)
    { Access::Save(this->self(), static_cast<int16_t>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(uint32_t val)
    { Access::Save(this->self(), static_cast<int32_t>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Save(uint64_t val)
    { Access::Save(this->self(), static_cast<int64_t>(val)); }

    //-------------------------------------------------------------------------


    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(bool& val)
    {
      int8_t val_;
      Access::Load(this->self(), val_);
      val = val_;
    }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(char& val)
    { Access::Load(this->self(), reinterpret_cast<int8_t&>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(uint8_t& val)
    { Access::Load(this->self(), reinterpret_cast<int8_t&>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(uint16_t& val)
    { Access::Load(this->self(), reinterpret_cast<int16_t&>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(uint32_t& val)
    { Access::Load(this->self(), reinterpret_cast<int32_t&>(val)); }

    //-------------------------------------------------------------------------

    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    inline
    void
    BaseArchive<mode_, Archive, CT, STS>::Load(uint64_t& val)
    { Access::Load(this->self(), reinterpret_cast<int64_t&>(val)); }

    //-------------------------------------------------------------------------


    __ELLE_SERIALIZE_BASE_ARCHIVE_TPL
    class BaseArchive<mode_, Archive, CT, STS>::Access
    {
    public:
      static inline void SaveBinary(Archive& ar,
                                    void const* data,
                                    size_t size)
      {
        ar.SaveBinary(data, size);
      }
      static inline void LoadBinary(Archive& ar,
                                    void* data,
                                    size_t size)
      {
        ar.LoadBinary(data, size);
      }

      static inline void Save(Archive& ar, bool val)      { ar.Save(val); }
      static inline void Save(Archive& ar, char val)      { ar.Save(val); }
      static inline void Save(Archive& ar, int8_t val)    { ar.Save(val); }
      static inline void Save(Archive& ar, uint8_t val)   { ar.Save(val); }
      static inline void Save(Archive& ar, int16_t val)   { ar.Save(val); }
      static inline void Save(Archive& ar, uint16_t val)  { ar.Save(val); }
      static inline void Save(Archive& ar, uint32_t val)  { ar.Save(val); }
      static inline void Save(Archive& ar, int32_t val)   { ar.Save(val); }
      static inline void Save(Archive& ar, uint64_t val)  { ar.Save(val); }
      static inline void Save(Archive& ar, int64_t val)   { ar.Save(val); }

      static inline void Load(Archive& ar, bool& val)     { ar.Load(val); }
      static inline void Load(Archive& ar, char& val)     { ar.Load(val); }
      static inline void Load(Archive& ar, int8_t& val)   { ar.Load(val); }
      static inline void Load(Archive& ar, uint8_t& val)  { ar.Load(val); }
      static inline void Load(Archive& ar, int16_t& val)  { ar.Load(val); }
      static inline void Load(Archive& ar, uint16_t& val) { ar.Load(val); }
      static inline void Load(Archive& ar, int32_t& val)  { ar.Load(val); }
      static inline void Load(Archive& ar, uint32_t& val) { ar.Load(val); }
      static inline void Load(Archive& ar, int64_t& val)  { ar.Load(val); }
      static inline void Load(Archive& ar, uint64_t& val) { ar.Load(val); }

      static inline void Save(Archive& ar, Format const& version)
      { ar.Save(version); }

      static inline void Load(Archive& ar, Format& version)
      { ar.Load(version); }


      template <typename T>
      static inline
      void
      Save(Archive& ar, T const& val)
      { ar.Save(val); }

      template <typename T>
      static inline
      void
      Load(Archive& ar, T& val)
      { ar.Load(val); }

      template <template <typename> class Wrapper, typename T>
      static inline
      void
      Load(Archive& ar, Wrapper<T> const& val)
      { ar.Load(val); }

      template <template <typename> class Wrapper, typename T>
      static inline
      void
      Save(Archive& ar, Wrapper<T> const& val)
      { ar.Save(val); }

      template <typename T>
      static inline
      void
      LoadConstruct(Archive& ar, T*& ptr)
      { ar.template LoadConstruct<T>(ptr); }
    };

}}

#endif
