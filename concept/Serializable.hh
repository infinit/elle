#ifndef  ELLE_CONCEPT_SERIALIZABLE_HH
# define ELLE_CONCEPT_SERIALIZABLE_HH

# include <elle/log.hh>
# include <elle/serialize/fwd.hh>

# include <elle/idiom/Close.hh>
#  include <string>
#  include <typeinfo>
# include <elle/idiom/Open.hh>

///
/// - exposed macros ----------------------------------------------------------
///

// Desambiguate Serializable<T, ArchiveType = BinaryArchive> methods
# define ELLE_CONCEPT_SERIALIZABLE_METHODS(cls, ...)                          \
  __ECS_SERIALIZE(cls, ##__VA_ARGS__)                                         \
  __ECS_DESERIALIZE(cls, ##__VA_ARGS__)                                       \
  /**/

///
/// - internal macros ---------------------------------------------------------
///

# define __ECS_DEFAULT_ARCHIVE  elle::serialize::BinaryArchive

// shortcut for template argument that accept an archive
# define __ECS_ARCHIVE_TPL(Archive)                                           \
  template<elle::serialize::ArchiveMode> class Archive                        \
  /**/

// shortcut for defaulted archive type as template argument
# define __ECS_DEFAULT_ARCHIVE_TPL(Archive)                                   \
  __ECS_ARCHIVE_TPL(Archive) = __ECS_DEFAULT_ARCHIVE                          \
  /**/

namespace elle
{
  namespace concept
  {
    namespace contract
    {
      // the interface
      template<__ECS_DEFAULT_ARCHIVE_TPL(Archive)>
      struct _Serializable
      {
      public:
        typedef Archive<elle::serialize::ArchiveMode::Input>  InputArchive;
        typedef typename InputArchive::StreamType             InputStream;
        typedef Archive<elle::serialize::ArchiveMode::Output> OutputArchive;
        typedef typename OutputArchive::StreamType            OutputStream;

        virtual ~_Serializable() {}

        virtual void serialize(OutputArchive& archive) const = 0;
        virtual void deserialize(InputArchive& archive) = 0;

        virtual void serialize(OutputStream& out, OutputArchive* = nullptr) const = 0;
        virtual void deserialize(InputStream& in, InputArchive* = nullptr) = 0;
      };

      // wrapper to ensure virtual inheritance
      template<__ECS_DEFAULT_ARCHIVE_TPL(Archive)>
      struct Serializable
        : virtual _Serializable<Archive>
      {};

    } // !contract

    template<typename T, __ECS_DEFAULT_ARCHIVE_TPL(Archive)>
    struct Serializable
      : public virtual contract::_Serializable<Archive>
    {
    public:
      typedef Archive<elle::serialize::ArchiveMode::Input>  InputArchive;
      typedef typename InputArchive::StreamType             InputStream;
      typedef Archive<elle::serialize::ArchiveMode::Output> OutputArchive;
      typedef typename OutputArchive::StreamType            OutputStream;

      // implem methods
      virtual void serialize(OutputArchive& archive) const
        { archive << static_cast<T const&>(*this); }
      virtual void deserialize(InputArchive& archive)
        { archive >> static_cast<T&>(*this); }

      virtual void serialize(OutputStream& out, OutputArchive* = nullptr) const
        { OutputArchive(out, static_cast<T const&>(*this)); }
      virtual void deserialize(InputStream& in, InputArchive* = nullptr)
        { InputArchive(in) >> static_cast<T&>(*this); }
    };

    /// Is T has Serializable<Archive> contract ?
    template<typename T, typename Archive> struct IsSerializable;

    template<
        typename T
      , __ECS_ARCHIVE_TPL(Archive)
      , elle::serialize::ArchiveMode mode
    >
    struct IsSerializable<T, Archive<mode>>
    {
      static bool const value = std::is_base_of<
          elle::concept::contract::_Serializable<Archive>
        , typename std::remove_cv<T>::type
      >::value == true /*&& std::is_base_of<
          elle::concept::Serializable<typename std::remove_cv<T>::type, Archive>
        , typename std::remove_cv<T>::type
      >::value == false*/;
      // XXX
    };

    template<typename Archive> struct SerializableFor;
    template<
        __ECS_ARCHIVE_TPL(Archive)
      , elle::serialize::ArchiveMode mode
    >
    struct SerializableFor<Archive<mode>>
    {
      typedef contract::_Serializable<Archive> Type;
    };

  }
}

///
/// - internal macros ---------------------------------------------------------
///

# define __ECS_OARCHIVE(...)                                                  \
  typename elle::concept::Serializable<__VA_ARGS__>::OutputArchive            \
  /**/
# define __ECS_IARCHIVE(...)                                                  \
  typename elle::concept::Serializable<__VA_ARGS__>::InputArchive             \
  /**/
# define __ECS_OSTREAM(...)                                                   \
  __ECS_OARCHIVE(__VA_ARGS__)::StreamType                                     \
  /**/
# define __ECS_ISTREAM(...)                                                   \
  __ECS_IARCHIVE(__VA_ARGS__)::StreamType                                     \
  /**/

# define __ECS_LOG_ACTION(action, ptr)                                        \
    ELLE_LOG_COMPONENT("elle.concept");                                 \
    ELLE_LOG_TRACE_SCOPE((#action " %p using type %s"),                       \
                         ptr, ELLE_PRETTY_OBJECT_TYPE(ptr))                   \

// dump a virtual serialize methods into a serializable class
# define __ECS_SERIALIZE(cls, ...)                                            \
 virtual void serialize(__ECS_OARCHIVE(cls, ##__VA_ARGS__)& ar) const         \
  {                                                                           \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    __ECS_LOG_ACTION(serialize, this);                                        \
    return this->Implem::serialize(ar);                                       \
  }                                                                           \
 virtual void serialize(__ECS_OSTREAM(cls, ##__VA_ARGS__)& out,               \
                        __ECS_OARCHIVE(cls, ##__VA_ARGS__)* = nullptr) const  \
  {                                                                           \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    __ECS_LOG_ACTION(serialize, this);                                        \
    return this->Implem::serialize(out);                                      \
  }                                                                           \
  /**/

// dump a virtual deserialize methods into a serializable class
# define __ECS_DESERIALIZE(cls, ...)                                          \
  virtual void deserialize(__ECS_IARCHIVE(cls, ##__VA_ARGS__)& ar)            \
  {                                                                           \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    __ECS_LOG_ACTION(deserialize, this);                                      \
    return this->Implem::deserialize(ar);                                     \
  }                                                                           \
  virtual void deserialize(__ECS_ISTREAM(cls, ##__VA_ARGS__)& in,             \
                           __ECS_IARCHIVE(cls, ##__VA_ARGS__)* = nullptr)     \
  {                                                                           \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    __ECS_LOG_ACTION(deserialize, this);                                      \
    return this->Implem::deserialize(in);                                     \
  }                                                                           \
  /**/

#endif
