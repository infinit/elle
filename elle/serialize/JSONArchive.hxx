#ifndef ELLE_SERIALIZE_JSONARCHIVE_HXX
# define ELLE_SERIALIZE_JSONARCHIVE_HXX

# include <elle/format/json/Object.hxx>
# include <elle/format/json/Parser.hh>
# include <elle/format/json/Dictionary.hh>

# include "JSONArchive.hh"
# include "BaseArchive.hxx"

namespace elle { namespace serialize {

    ///
    /// an ArchiveSerializer while receive this class which proxies
    /// calls to a dictionary
    ///
    class OutputJSONArchive::_DictStream
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

      template<typename Pair> inline typename std::enable_if<
          IsNamedValue<Pair>::value,
          _DictStream&
      >::type operator <<(Pair const& pair)
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
      template<typename T> inline _DictStream& operator &(NamedValue<T const> const& pair)
      {
        return *this << pair;
      }
      template<typename T> inline _DictStream& operator &(NamedValue<T const>& pair)
      {
        return *this << pair;
      }

      template<typename T> inline _DictStream& operator &(NamedValue<T> const& pair)
      {
        return *this << NamedValue<T const>(pair.name, pair.value);
      }
    };


    class InputJSONArchive::_DictStream
    {
    public:
      static ArchiveMode const mode = ArchiveMode::Output;
      typedef uint32_t ListSizeType;

    private:
      json::Dictionary const& _dict;

    public:
      _DictStream(json::Dictionary const& dict)
        : _dict(dict)
      {}

      template<typename T> _DictStream& operator >>(NamedValue<T> const& pair)
      {
        json::Object const& obj = _dict[pair.name];
        obj.Load(pair.value);
        return *this;
      }
      template<typename T> _DictStream& operator &(NamedValue<T> const& pair)
      {
        return *this >> pair;
      }
      template<typename T> _DictStream& operator >>(NamedValue<T>&& pair)
      {
        json::Object const& obj = _dict[pair.name];
        return *this;
      }
      template<typename T> _DictStream& operator &(NamedValue<T>&& pair)
      {
        return *this >> pair;
      }
    };


    template<typename T> inline typename std::enable_if<
        std::is_base_of<json::Object, T>::value
    >::type OutputJSONArchive::Save(T const& value)
    {
      static_cast<json::Object const&>(value).Save(*this);
    }

    template<typename T> inline typename std::enable_if<
        !std::is_base_of<json::Object, T>::value &&
        !std::is_arithmetic<T>::value &&
        !std::is_same<T, std::string>::value
    >::type OutputJSONArchive::Save(T const& val)
    {
      _DictStream dict(*this);
      dict << NamedValue<int32_t const>("_class_version", ArchivableClass<T>::version);

      typedef ArchiveSerializer<typename std::remove_cv<T>::type> Serializer;
      Serializer::Serialize(
          dict,
          const_cast<T&>(val),
          ArchivableClass<T>::version
      );
    }

    template<typename T> void InputJSONArchive::Load(T& val)
    {
      auto parser = json::Parser<std::string>();
      auto obj = parser.Parse(this->stream());
      assert(obj.get() != nullptr);
      json::Dictionary const* dict = dynamic_cast<json::Dictionary const*>(obj.get());
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


}}

#endif

