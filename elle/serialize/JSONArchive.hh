#ifndef ELLE_SERIALIZE_JSONARCHIVE_HH
# define ELLE_SERIALIZE_JSONARCHIVE_HH

# include <cassert>

# include <elle/format/json.hh>

# include "BaseArchive.hh"
# include "JSONArchive.fwd.hh"

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
  void Save(int16_t val)  { this->stream() << val; }
  void Save(int32_t val)  { this->stream() << val; }
  void Save(int64_t val)  { this->stream() << val; }
  void Save(float val)    { this->stream() << val; }
  void Save(double val)   { this->stream() << val; }
  void Save(std::string const& val)
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

  class _DictStream
  {
  public:
    static ArchiveMode const mode = ArchiveMode::Output;
    typedef uint32_t ListSizeType;
  private:
    OutputJSONArchive&  _archive;
    bool                _isFirst;

  public:
    _DictStream(OutputJSONArchive& archive) :
      _archive(archive),
      _isFirst(true)
    { _archive.stream() << '{'; }

    ~_DictStream()
    { _archive.stream() << '}'; }

    template<typename T>
    inline _DictStream& operator <<(NamedValue<T> const& pair)
    {
      if (_isFirst)
        _isFirst = false;
      else
        _archive.stream() << ',';
      _archive << pair.name;
      _archive.stream() << ':';
      _archive << pair.value;
      return *this;
    }

    template<typename T>
    inline _DictStream& operator &(NamedValue<T> const& pair)
    {
      return *this << pair;
    }
  };

  template<typename T> inline typename std::enable_if<
      !std::is_base_of<json::Object, T>::value
  >::type Save(T const& val)
    {
      _DictStream dict(*this);
      dict << NamedValue<int32_t>("_class_version", ArchivableClass<T>::version);

      typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
      Serializer::Serialize(
          dict,
          const_cast<T&>(val),
          ArchivableClass<T>::version
      );
    }

  friend struct json::Null;
  friend struct json::Bool;
  friend struct json::Int;
  friend struct json::Float;
  friend struct json::String;
  friend struct json::Array;
  friend struct json::Dict;

  template<typename T> inline typename std::enable_if<
      std::is_base_of<json::Object, T>::value
  >::type Save(T const& value)
    // static_cast is needed in g++
    { static_cast<json::Object const&>(value).Save(*this); }

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

  friend class BaseClass::Access;

public:
  template<typename T>
  InputJSONArchive(StreamType& stream, T& out) : BaseClass(stream)
  {
    *this >> out;
  }

protected:
  class _DictStream
  {
  public:
    static ArchiveMode const mode = ArchiveMode::Output;
    typedef uint32_t ListSizeType;

  private:
    json::Dict const& _dict;

  public:
    _DictStream(json::Dict const& dict)
      : _dict(dict)
    {}

    template<typename T>
    inline _DictStream& operator >>(NamedValue<T>& pair)
    {
      Object const& obj = _dict[pair.name];
      return *this;
    }

    template<typename T>
    inline _DictStream& operator &(NamedValue<T>& pair)
    {
      return *this >> pair;
    }

    template<typename T>
    inline _DictStream& operator >>(NamedValue<T>&& pair)
    {
      Object const& obj = _dict[pair.name];
      return *this;
    }

    template<typename T>
    inline _DictStream& operator &(NamedValue<T>&& pair)
    {
      return *this >> pair;
    }
  };

  template<typename T> void Load(T& val)
  {
    auto parser = json::Parser<StringType>();
    auto obj = parser.Parse(this->stream());
    assert(obj.get() != nullptr);
    json::Dict const* dict = dynamic_cast<json::Dict const*>(obj.get());
    if (dict == nullptr)
      throw std::runtime_error("Cannot convert to dictionary");
    _DictStream dstream(*dict);

    unsigned int version;
    dstream >> NamedValue<unsigned int>("_class_version", version);
    typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
    Serializer::Serialize(
        dstream,
        const_cast<T&>(val),
        version
    );
  }

protected:
  using BaseClass::operator >>;
};


}} // !namespace elle::serialize

#endif /* ! JSONARCHIVE_HH */


