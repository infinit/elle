#ifndef ELLE_SERIALIZE_JSONARCHIVE_HH
# define ELLE_SERIALIZE_JSONARCHIVE_HH

# include <cassert>

# include <elle/format/json.fwd.hh>

# include <elle/serialize/BaseArchive.hh>
# include <elle/serialize/JSONArchive.fwd.hh> // Check if forward declarations match definitions

namespace elle { namespace serialize {

    namespace json = elle::format::json;

///
/// Output json archive convert an object to its json representation
///
/// Since json data cannot be a stream, the serialization is exclusively
/// done in the archive constructor.
///
class OutputJSONArchive :
  public BaseArchive<ArchiveMode::Output, OutputJSONArchive>
{
private:
  typedef BaseArchive<ArchiveMode::Output, OutputJSONArchive> BaseClass;
  typedef typename BaseClass::StreamType StreamType;
  friend class BaseClass::Access;
  class _DictStream;

public:
  OutputJSONArchive(StreamType& stream, json::Object&& obj) :
    BaseClass(stream)
  { obj.Save(*this); }

  OutputJSONArchive(StreamType& stream, json::Object const& obj) :
    BaseClass(stream)
  { obj.Save(*this); }

  template<typename T>
  OutputJSONArchive(StreamType& stream, T const& obj) :
    BaseClass(stream)
  { *this << obj; }

  ~OutputJSONArchive()
  {}

protected:
  inline void Save(int16_t val)  { this->stream() << val; }
  inline void Save(int32_t val)  { this->stream() << val; }
  inline void Save(int64_t val)  { this->stream() << val; }
  inline void Save(float val)    { this->stream() << val; }
  inline void Save(double val)   { this->stream() << val; }
  inline void Save(std::string const& val)
  {
    auto& ss = this->stream();
    ss << '"';
    for (auto it = val.cbegin(), end = val.cend(); it != end; ++it)
      {
        switch (*it)
          {
          case '"':
            ss << "\\\""; break;
          case '\\':
            ss << "\\\\"; break;
          case '/':
            ss << "\\/"; break;
          case '\b':
            ss << "\\b"; break;
          case '\f':
            ss << "\\f"; break;
          case '\n':
            ss << "\\n"; break;
          case '\r':
            ss << "\\r"; break;
          case '\t':
            ss << "\\t"; break;
          default:
            ss << *it; break;
          }
      }
    ss << '"';
  }
  template<typename T> inline typename std::enable_if<
      std::is_base_of<json::Object, T>::value
  >::type Save(T const& value);
  template<typename T> inline typename std::enable_if<
      !std::is_base_of<json::Object, T>::value
  >::type Save(T const& val);

  friend class json::detail::BasicObject<int32_t>;
  friend class json::detail::BasicObject<bool>;
  friend class json::detail::BasicObject<double>;
  friend class json::detail::BasicObject<std::string>;
  friend struct json::Null;
  friend struct json::Array;
  friend struct json::Dictionary;

protected:
  using BaseClass::operator <<;
};


///
/// The InputJSONArchive class deserialize a json encoded stream
/// to an object.
///
class InputJSONArchive :
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
  InputJSONArchive(StreamType& stream, T& out)
    : BaseClass(stream)
  {
    *this >> out;
  }

protected:
  template<typename T> void Load(T& val);
  using BaseClass::operator >>;
};


}} // !namespace elle::serialize

#endif /* ! JSONARCHIVE_HH */


