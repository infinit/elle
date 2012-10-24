#ifndef ELLE_SERIALIZE_JSONARCHIVE_HH
# define ELLE_SERIALIZE_JSONARCHIVE_HH

# include <cassert>

# include <elle/format/json/fwd.hh>

# include <elle/serialize/BaseArchive.hh>
# include <elle/serialize/fwd.hh>

namespace elle
{
  namespace serialize
  {

    namespace json = elle::format::json; // XXX

    template<ArchiveMode mode>
    class JSONArchive;

    typedef JSONArchive<ArchiveMode::output> OutputJSONArchive;

    ///
    /// Output json archive convert an object to its json representation
    ///
    /// Since json data cannot be a stream, the serialization is exclusively
    /// done in the archive constructor.
    ///
    template<>
    class JSONArchive<ArchiveMode::output>:
      public BaseArchive<ArchiveMode::output, OutputJSONArchive>
    {
    private:
      typedef BaseArchive<ArchiveMode::Output, OutputJSONArchive> BaseClass;
      typedef typename BaseClass::StreamType StreamType;
      friend class BaseClass::Access;
      class _DictStream;

    public:
      JSONArchive(StreamType& stream, json::Object&& obj);
      JSONArchive(StreamType& stream, json::Object const& obj);

      template<typename T>
      JSONArchive(StreamType& stream, T const& obj):
        BaseClass(stream)
      {
        *this << obj;
      }

    protected:
      inline void Save(uint16_t val)  { this->stream() << val; }
      inline void Save(uint32_t val)  { this->stream() << val; }
      inline void Save(uint64_t val)  { this->stream() << val; }
      inline void Save(int16_t val)  { this->stream() << val; }
      inline void Save(int32_t val)  { this->stream() << val; }
      inline void Save(int64_t val)  { this->stream() << val; }
      inline void Save(float val)    { this->stream() << val; }
      inline void Save(double val)   { this->stream() << val; }
      void Save(std::string const& val);

      template<typename T> inline typename std::enable_if<
          std::is_base_of<json::Object, T>::value
      >::type Save(T const& value);

      template<typename T> inline typename std::enable_if<
          !std::is_base_of<json::Object, T>::value &&
          !std::is_arithmetic<T>::value &&
          !std::is_same<T, std::string>::value
      >::type Save(T const& val);

      friend class json::detail::BasicObject<int32_t>;
      friend class json::detail::BasicObject<bool>;
      friend class json::detail::BasicObject<double>;
      friend class json::detail::BasicObject<std::string>;
      friend class json::Null;
      friend class json::Array;
      friend class json::Dictionary;

    protected:
      using BaseClass::operator <<;
    };


    ///
    /// The InputJSONArchive class deserialize a json encoded stream
    /// to an object.
    ///
    template<>
    class JSONArchive<ArchiveMode::input>:
      public BaseArchive<ArchiveMode::Input, InputJSONArchive>
    {
    private:
      typedef BaseArchive<ArchiveMode::Input, InputJSONArchive> BaseClass;
      typedef typename BaseClass::StreamType                    StreamType;
      typedef typename BaseClass::StringType                    StringType;
      class _DictStream;

      friend class BaseClass::Access;

    public:
      template<typename T>
      JSONArchive(StreamType& stream, T& out):
        BaseClass(stream)
      {
        *this >> out;
      }

    protected:
      template<typename T> void Load(T& val);
      using BaseClass::operator >>;
    };

  }
} // !namespace elle::serialize

# include "JSONArchive.hxx"

#endif /* ! JSONARCHIVE_HH */
