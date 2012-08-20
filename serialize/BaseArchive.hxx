#ifndef ELLE_SERIALIZE_BASEARCHIVE_HXX
# define ELLE_SERIALIZE_BASEARCHIVE_HXX

# include <elle/concept/Serializable.hh>

# include "BaseArchive.hh"
# include "ArchiveSerializer.hxx"

# include <elle/idiom/Close.hh>
#  include <iostream>
#  include <boost/detail/endian.hpp>
# include <elle/idiom/Open.hh>

namespace elle
{
  namespace serialize
  {

    //-------------------------------------------------------------------------

    // Methods enabler.
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
    template<typename T, ArchiveMode _mode>
      struct BaseArchive<mode_, Archive, CT, STS>::_EnableFor
      {

        typedef typename std::enable_if<
                 (
                      std::is_same<
                          typename boost::call_traits<T>::param_type,
                          typename std::add_lvalue_reference<
                              typename std::add_const<T>::type
                          >::type
                      >::value
                  ||  (!std::is_pointer<T>::value && std::is_array<T>::value)
                )
            &&  mode == _mode
            &&  !IsNamedValue<T>::value
          , Archive&
        > Ref;

        typedef typename std::enable_if<
                std::is_same<
                    typename boost::call_traits<T>::param_type,
                    typename std::add_const<T>::type
                >::value
            &&  mode == _mode
            &&  !IsNamedValue<T>::value
            && std::is_pointer<T>::value == false
          , Archive&
        > Val;

        typedef typename std::enable_if<
                (!std::is_pointer<T>::value || std::is_array<T>::value)
            &&  !std::is_const<T>::value
            &&  mode == _mode
            &&  IsNamedValue<T>::value == false
          , Archive&
        > NotPointer;

        typedef typename std::enable_if<
            !std::is_pointer<T>::value && mode == _mode,
            std::unique_ptr<T>
        > ConstructPtr;

        typedef typename std::enable_if<
         IsNamedValue<T>::value && mode == _mode,
          Archive&
        > NamedValue;
      };

    //-------------------------------------------------------------------------

    /// Generic save method. Uses an explicit specialization of
    /// ArchiveSerializer.
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
    template<typename T>
    inline typename std::enable_if<
            std::is_enum<T>::value == false
      >::type
      BaseArchive<mode_, Archive, CT, STS>::Save(T const& val)
      {

        // XXX
//# ifndef NDEBUG
//        std::cout << "Saving type: " << std::string(typeid(T).name())
//                  << std::endl;
//        Access::Save(this->self(), std::string(typeid(T).name()));
//# endif

        if (StoreClassVersion<T>::value == true)
            Access::Save(
                this->self(),
                ClassVersionType(ArchivableClass<T>::version)
            );

        typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
        // this const_cast is safe since the archive is in output mode
        Serializer::Serialize(
            this->self(),
            const_cast<T&>(val),
            ArchivableClass<T>::version
        );
      }

    //-------------------------------------------------------------------------

    /// Save Serializable objects.
    //template<ArchiveMode mode_, typename Archive, typename CT,
    //         template<ArchiveMode, typename> class STS>
    //template<typename T>
    //inline typename std::enable_if<
    //    elle::concept::IsSerializable<T, Archive>::value == true
    //  >::type
    //  BaseArchive<mode_, Archive, CT, STS>::Save(T const& val)
    //  {
    //    ELLE_LOG_COMPONENT("elle.serialize");
    //    ELLE_TRACE(
    //        "Saving %p from its virtual type %s",
    //        this, ELLE_PRETTY_OBJECT_TYPE(this)
    //    ) static_cast<
    //      typename elle::concept::SerializableFor<Archive>::Type const&
    //    >(val).serialize(this->self());
    //  }

    //-------------------------------------------------------------------------

    /// Generic load method. Uses an explicit specialization of ArchiveSerializer.
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
    template<typename T>
    inline typename std::enable_if<
            std::is_enum<T>::value == false
        &&  IsNamedValue<T>::value == false
      >::type
      BaseArchive<mode_, Archive, CT, STS>::Load(T& val)
      {
        // XXX
//# ifndef NDEBUG
//        std::string type_name;
//        Access::Load(this->self(), type_name);
//        std::cout << "Loading type: " << type_name << std::endl;
//        assert(type_name == typeid(T).name());
//# endif

        ClassVersionType classVersion(0);
        if (StoreClassVersion<T>::value == true)
          Access::Load(this->self(), classVersion);

        typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
        Serializer::Serialize(this->self(), val, classVersion.version);
      }

    //-------------------------------------------------------------------------

    ///// Load Serializable object.
    //template<ArchiveMode mode_, typename Archive, typename CT,
    //         template<ArchiveMode, typename> class STS>
    //template<typename T>
    //inline typename std::enable_if<
    //    elle::concept::IsSerializable<T, Archive>::value == true
    //  >::type
    //  BaseArchive<mode_, Archive, CT, STS>::Load(T& val)
    //  {
    //    ELLE_LOG_COMPONENT("elle.serialize");
    //    ELLE_TRACE(
    //        "Loading %p from its virtual type %s",
    //        this, ELLE_PRETTY_OBJECT_TYPE(this)
    //    ) static_cast<
    //        typename elle::concept::SerializableFor<Archive>::Type&
    //    >(val).deserialize(this->self());
    //  }

    //-------------------------------------------------------------------------

    /// Generic load construct. Uses an explicit specialization of ArchiveSerializer.
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
    template<typename T>
      inline void BaseArchive<mode_, Archive, CT, STS>::LoadConstruct(T*& ptr)
      {
        assert(ptr == nullptr);
        //ClassVersionType classVersion(0);
        //if (StoreClassVersion<T>::value == true)
        //  Access::Load(this->self(), classVersion);
        typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
        ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
        ELLE_TRACE("Load construct %p with its concrete type %s",
                   this, ELLE_PRETTY_TYPE(T)
          ) Serializer::LoadConstruct(this->self(), ptr);
      }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(int16_t val)
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

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(int16_t& val)
        {
          ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
          ELLE_TRACE("Loading int16_t '%s'", val);

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

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(int32_t val)
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
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(int32_t& val)
        {
          ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
          ELLE_TRACE("Loading int32_t '%s'", val);

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

    //-------------------------------------------------------------------------

      /// Save int64_t
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(int64_t val)
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
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(int64_t& val)
        {
          ELLE_LOG_COMPONENT("infinig.elle.serialize");
          ELLE_TRACE("Loading int64_t '%s'", val);

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

    //-------------------------------------------------------------------------

      /// Save float
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(float val)
        {
          ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
          ELLE_TRACE("Saving float '%s'", val);

          static_assert(sizeof(val) == 4, "float size is not standard");
          Access::SaveBinary(this->self(), &val, sizeof(val));
        }

    //-------------------------------------------------------------------------

      /// Load float
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(float& val)
        {
          ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
          ELLE_TRACE("Loading float '%s'", val);

          static_assert(sizeof(val) == 4, "float size is not standard");
          Access::LoadBinary(this->self(), &val, sizeof(val));
        }

    //-------------------------------------------------------------------------

      /// Save double
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(double val)
        {
          ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
          ELLE_TRACE("Saving double '%s'", val);

          static_assert(sizeof(val) == 8, "double size is not standard");
          Access::SaveBinary(this->self(), &val, sizeof(val));
        }

    //-------------------------------------------------------------------------

      /// Load double
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(double& val)
        {
          ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
          ELLE_TRACE("Loading double '%s'", val);

          static_assert(sizeof(val) == 8, "double size is not standard");
          Access::LoadBinary(this->self(), &val, sizeof(val));
        }

    //-------------------------------------------------------------------------

      /// Save std::string
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(std::string const& val)
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
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(std::string& val)
        {
          ELLE_LOG_COMPONENT("elle.serialize.BaseArchive");
          ELLE_TRACE("Loading string '%s'", val);

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
        }

    //-------------------------------------------------------------------------

      /// Save class version
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(ClassVersionType const& classVersion)
      {
        Access::Save(this->self(), classVersion.version);
      }

    //-------------------------------------------------------------------------

      /// Load class version
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(ClassVersionType& classVersion)
      {
        Access::Load(this->self(), classVersion.version);
      }


    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
    template<typename T>
      inline typename std::enable_if<std::is_enum<T>::value == true>::type
      BaseArchive<mode_, Archive, CT, STS>::Save(T value)
      {
        assert(static_cast<unsigned int>(value) < 65536);
        Access::Save(this->self(), static_cast<uint16_t>(value));
      }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
    template<typename T>
      inline typename std::enable_if<std::is_enum<T>::value == true>::type
      BaseArchive<mode_, Archive, CT, STS>::Load(T& value)
      {
        uint16_t value_;
        Access::Load(this->self(), value_);
        value = static_cast<T>(value_);
      }

    //-------------------------------------------------------------------------

      /// This is the last method called by any serialization method
      /// You may want to override it to change the serialization format
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::SaveBinary(void const* data, std::streamsize size)
        {
          // std::cerr << "REGION SERIALIZED: " << data << " (" << size << ")" << std::endl;
          this->stream().write(static_cast<char const*>(data), size);
        }

    //-------------------------------------------------------------------------

      /// This is the last method called by any serialization method
      /// You may want to override it to change the serialization format
    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::LoadBinary(void* data, std::streamsize size)
        {
          this->stream().read(static_cast<char*>(data), size);
        }


    //-------------------------------------------------------------------------


    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(int8_t val)
        { Access::SaveBinary(this->self(), &val, sizeof(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(int8_t& val)
        { Access::LoadBinary(this->self(), &val, sizeof(val)); }

    //-------------------------------------------------------------------------


    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(bool val)
        { Access::Save(this->self(), static_cast<int8_t>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(char val)
        { Access::Save(this->self(), static_cast<int8_t>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(uint8_t val)
        { Access::Save(this->self(), static_cast<int8_t>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(uint16_t val)
        { Access::Save(this->self(), static_cast<int16_t>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(uint32_t val)
        { Access::Save(this->self(), static_cast<int32_t>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Save(uint64_t val)
        { Access::Save(this->self(), static_cast<int64_t>(val)); }

    //-------------------------------------------------------------------------


    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(bool& val)
        {
          int8_t val_;
          Access::Load(this->self(), val_);
          val = val_;
        }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(char& val)
        { Access::Load(this->self(), reinterpret_cast<int8_t&>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(uint8_t& val)
        { Access::Load(this->self(), reinterpret_cast<int8_t&>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(uint16_t& val)
        { Access::Load(this->self(), reinterpret_cast<int16_t&>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(uint32_t& val)
        { Access::Load(this->self(), reinterpret_cast<int32_t&>(val)); }

    //-------------------------------------------------------------------------

    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      inline void BaseArchive<mode_, Archive, CT, STS>::Load(uint64_t& val)
        { Access::Load(this->self(), reinterpret_cast<int64_t&>(val)); }

    //-------------------------------------------------------------------------


    template<ArchiveMode mode_, typename Archive, typename CT,
             template<ArchiveMode, typename> class STS>
      class BaseArchive<mode_, Archive, CT, STS>::Access
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

        static inline void Save(Archive& ar, ClassVersionType const& classVersion)
          { ar.Save(classVersion); }
        static inline void Load(Archive& ar, ClassVersionType& classVersion)
          { ar.Load(classVersion); }


        template<typename T> static inline void Save(Archive& ar, T const& val)
          { ar.Save(val); }
        template<typename T> static inline void Load(Archive& ar, T& val)
          { ar.Load(val); }
        template<typename T> static inline void LoadConstruct(Archive& ar, T*& ptr)
          { ar.LoadConstruct(ptr); }
        template<typename T>
          static inline void SaveNamed(Archive& ar,
                                       std::string const& name,
                                       T const& value)
          { ar.SaveNamed(name, value); }
        template<typename T>
          static inline void LoadNamed(Archive& ar, std::string const& name, T& value)
          { ar.LoadNamed(name, value); }
      };

    //-------------------------------------------------------------------------

}}

#endif
