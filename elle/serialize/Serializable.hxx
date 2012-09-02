#ifndef  ELLE_SERIALIZE_SERIALIZABLE_HXX
# define ELLE_SERIALIZE_SERIALIZABLE_HXX

# include "Concrete.hh"

# include <type_traits>

namespace elle
{
  namespace serialize
  {

    template <__ESS_ARCHIVE_TPL(Archive)>
    struct _Serializable
    {
    public:
      typedef Archive<elle::serialize::ArchiveMode::Input>  InputArchive;
      typedef typename InputArchive::StreamType             InputStream;
      typedef Archive<elle::serialize::ArchiveMode::Output> OutputArchive;
      typedef typename OutputArchive::StreamType            OutputStream;

      virtual
      ~_Serializable()
      {}

      virtual
      void
      serialize(OutputArchive& archive) const = 0;

      virtual
      void
      deserialize(InputArchive& archive) = 0;

      virtual
      void
      serialize(OutputStream& out, OutputArchive* = nullptr) const = 0;

      virtual
      void
      deserialize(InputStream& in, InputArchive* = nullptr) = 0;
    };

    template <typename T, __ESS_ARCHIVE_TPL(Archive)>
    struct SerializableMixin:
      public virtual _Serializable<Archive>
    {
    public:
      typedef Archive<elle::serialize::ArchiveMode::Input>  InputArchive;
      typedef typename InputArchive::StreamType             InputStream;
      typedef Archive<elle::serialize::ArchiveMode::Output> OutputArchive;
      typedef typename OutputArchive::StreamType            OutputStream;

      // implem methods
      virtual
      void
      serialize(OutputArchive& archive) const
      {
        archive << Concrete<T const>(static_cast<T const&>(*this));
      }

      virtual
      void
      deserialize(InputArchive& archive)
      {
        archive >> Concrete<T>(static_cast<T&>(*this));
      }

      virtual
      void
      serialize(OutputStream& out, OutputArchive* = nullptr) const
      {
        OutputArchive(out) << Concrete<T const>(static_cast<T const&>(*this));
      }

      virtual
      void
      deserialize(InputStream& in, InputArchive* = nullptr)
      {
        InputArchive(in) >> Concrete<T>(static_cast<T&>(*this));
      }
    };

    template<
        typename T
      , __ESS_ARCHIVE_TPL(Archive)
      , ArchiveMode mode
    >
    struct IsSerializable<T, Archive<mode>>
    {
      static bool const value = std::is_base_of<
        _Serializable<Archive>,
        typename std::remove_cv<
          typename std::remove_reference<T>::type
        >::type
      >::value;
    };

    template<
        __ESS_ARCHIVE_TPL(Archive)
      , elle::serialize::ArchiveMode mode
    >
    struct SerializableFor<Archive<mode>>
    {
      typedef _Serializable<Archive> Type;
    };

  }
}

/// - internal macros ---------------------------------------------------------
///

# define __ESS_OARCHIVE(...)                                                  \
  typename elle::serialize::SerializableMixin<__VA_ARGS__>::OutputArchive     \
/**/
# define __ESS_IARCHIVE(...)                                                  \
  typename elle::serialize::SerializableMixin<__VA_ARGS__>::InputArchive      \
/**/
# define __ESS_OSTREAM(...)                                                   \
  __ESS_OARCHIVE(__VA_ARGS__)::StreamType                                     \
/**/
# define __ESS_ISTREAM(...)                                                   \
  __ESS_IARCHIVE(__VA_ARGS__)::StreamType                                     \
/**/

// dump a virtual serialize methods into a serializable class
# define __ESS_SERIALIZE(...)                                                 \
 virtual void serialize(__ESS_OARCHIVE(__VA_ARGS__)& ar) const                \
  {                                                                           \
    typedef elle::serialize::SerializableMixin<__VA_ARGS__> Implem;           \
    __ESS_LOG_ACTION(serialize, this);                                        \
    return this->Implem::serialize(ar);                                       \
  }                                                                           \
 virtual void serialize(__ESS_OSTREAM(__VA_ARGS__)& out,                      \
                        __ESS_OARCHIVE(__VA_ARGS__)* = nullptr) const         \
  {                                                                           \
    typedef elle::serialize::SerializableMixin<__VA_ARGS__> Implem;           \
    __ESS_LOG_ACTION(serialize, this);                                        \
    return this->Implem::serialize(out);                                      \
  }                                                                           \
/**/

// dump a virtual deserialize methods into a serializable class
# define __ESS_DESERIALIZE(...)                                               \
  virtual void deserialize(__ESS_IARCHIVE(__VA_ARGS__)& ar)                   \
  {                                                                           \
    typedef elle::serialize::SerializableMixin<__VA_ARGS__> Implem;           \
    __ESS_LOG_ACTION(deserialize, this);                                      \
    return this->Implem::deserialize(ar);                                     \
  }                                                                           \
  virtual void deserialize(__ESS_ISTREAM(__VA_ARGS__)& in,                    \
                           __ESS_IARCHIVE(__VA_ARGS__)* = nullptr)            \
  {                                                                           \
    typedef elle::serialize::SerializableMixin<__VA_ARGS__> Implem;           \
    __ESS_LOG_ACTION(deserialize, this);                                      \
    return this->Implem::deserialize(in);                                     \
  }                                                                           \
/**/

# define __ESS_LOG_ACTION(action, ptr)                                        \
    ELLE_LOG_COMPONENT("elle.serialize.Serializable");                        \
    ELLE_TRACE_SCOPE((#action " %p using type %s"),                           \
                         ptr, ELLE_PRETTY_OBJECT_TYPE(ptr))                   \
/**/

#endif
