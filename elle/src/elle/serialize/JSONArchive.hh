#ifndef ELLE_SERIALIZE_JSONARCHIVE_HH
# define ELLE_SERIALIZE_JSONARCHIVE_HH

# include <cassert>

# include <elle/format/json/fwd.hh>
# include <elle/format/json/types.hh>

# include <elle/serialize/BaseArchive.hh>
# include <elle/serialize/fwd.hh>

namespace elle
{
  namespace serialize
  {
    /// JSON archive convert an object to its json representation, or build
    /// objects from a JSON stream.
    ///
    /// XXX does not support containers
    /// XXX does not support type versionning
    template <ArchiveMode mode>
    class JSONArchive:
      public BaseArchive<mode, JSONArchive<mode>>
    {
    private:
      typedef BaseArchive<mode, JSONArchive<mode>> BaseClass;
      typedef typename BaseClass::StreamType StreamType;
      friend class BaseClass::Access;

      enum Kind {array, dict, base_class, none};
      struct State
      {
        Kind kind;
        bool is_first;
      };
      std::vector<State> _save_stack;
    protected:
      void
      _push_save(Kind const kind);
      void
      _pop_save(Kind const kind);
      void
      _elem_save();

    public:
      JSONArchive(StreamType& stream);
      JSONArchive(StreamType& stream,
                  format::json::Object&& obj);
      JSONArchive(StreamType& stream,
                  format::json::Object const& obj);

      template <typename T>
      JSONArchive(StreamType& stream,
                  T&& obj);

    protected:
      void Save(uint16_t val);
      void Save(uint32_t val);
      void Save(uint64_t val);
      void Save(int16_t val);
      void Save(int32_t val);
      void Save(int64_t val);
      void Save(float val);
      void Save(double val);
      void Save(std::string const& val);
      template <typename T>
      typename std::enable_if<std::is_enum<T>::value>::type
      Save(T const value);

      template <typename T>
      inline
      typename std::enable_if<std::is_base_of<format::json::Object, T>::value>::type
      Save(T const& value);

      template <typename T>
      inline
      typename std::enable_if<
          !std::is_base_of<format::json::Object, T>::value &&
          !std::is_arithmetic<T>::value &&
          !std::is_same<T, std::string>::value &&
          !std::is_enum<T>::value &&
          !elle::format::json::detail::IsArray<T>::value
      >::type
      Save(T const& val);

      template <typename T>
      inline
      typename std::enable_if<
        elle::format::json::detail::IsArray<T>::value
      >::type
      Save(T& value);

      template <typename T>
      inline
      void
      Save(Concrete<T> const& concrete);

      template <typename T>
      inline
      void
      Save(NamedValue<T> const& pair);

      friend class format::json::detail::BasicObject<int32_t>;
      friend class format::json::detail::BasicObject<bool>;
      friend class format::json::detail::BasicObject<double>;
      friend class format::json::detail::BasicObject<std::string>;
      friend class format::json::Null;
      friend class format::json::Array;
      friend class format::json::Dictionary;

    private:
      std::vector<format::json::Object const*> _load_stack;
    protected:
      template<typename T>
      typename std::enable_if<!format::json::detail::ObjectCanLoad<T>::value>::type
      Load(T& val);

      template<typename T>
      typename std::enable_if<format::json::detail::ObjectCanLoad<T>::value>::type
      Load(T& val);

      template <typename T>
      void
      Load(Concrete<T> const& val);

      template <typename T>
      void
      Load(NamedValue<T> const& pair);
    };

    typedef JSONArchive<ArchiveMode::output> OutputJSONArchive;
    typedef JSONArchive<ArchiveMode::input> InputJSONArchive;
  }
}

# include "JSONArchive.hxx"

#endif
