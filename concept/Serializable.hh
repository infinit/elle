#ifndef  ELLE_CONCEPT_SERIALIZABLE_HH
# define ELLE_CONCEPT_SERIALIZABLE_HH

# include <string>
# include <typeinfo>

# include <elle/idiom/Close.hh>
#  include <elle/log.hh>
#  include <elle/serialize/fwd.hh>
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
        typedef Archive<elle::serialize::ArchiveMode::Input>  InputArchive;
        typedef typename InputArchive::StreamType             InputStream;
        typedef Archive<elle::serialize::ArchiveMode::Output> OutputArchive;
        typedef typename OutputArchive::StreamType            OutputStream;

        virtual ~_Serializable() {}

        virtual void serialize(OutputArchive& archive) const = 0;
        virtual void deserialize(InputArchive& archive) = 0;

        virtual void serialize(OutputStream& out) const = 0;
        virtual void deserialize(InputStream& in) = 0;
      };

      // wrapper to ensure virtual inheritance
      template<__ECS_DEFAULT_ARCHIVE_TPL(Archive)>
      struct Serializable
        : virtual _Serializable<Archive>
      {};

    } // !contract

    template<typename T, __ECS_DEFAULT_ARCHIVE_TPL(Archive)>
    struct Serializable
      : virtual contract::Serializable<Archive>
    {
      // in / out archive types.
      typedef
        typename contract::Serializable<Archive>::InputArchive  InputArchive;
      typedef
        typename contract::Serializable<Archive>::InputStream   InputStream;
      typedef
        typename contract::Serializable<Archive>::OutputArchive OutputArchive;
      typedef
        typename contract::Serializable<Archive>::OutputStream  OutputStream;

      // implem methods
      virtual void serialize(OutputArchive& archive) const
        { archive << static_cast<T const&>(*this); }
      virtual void deserialize(InputArchive& archive)
        { archive >> static_cast<T&>(*this); }

      virtual void serialize(OutputStream& out) const
        { OutputArchive(out, static_cast<T const&>(*this)); }
      virtual void deserialize(InputStream& in)
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

// dump a virtual serialize method into a serializable class
# define __ECS_SERIALIZE(cls, ...)                                            \
 virtual void serialize(__ECS_OARCHIVE(cls, ##__VA_ARGS__)& ar) const         \
  {                                                                           \
    ELLE_LOG_TRACE_COMPONENT("elle.concept");                                 \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    ELLE_LOG_TRACE(                                                           \
        "Serialize %p using %s implem", this, ELLE_PRETTY_OBJECT_TYPE(this)   \
    ) { return this->Implem::serialize(ar); }                                 \
  }                                                                           \
 virtual void serialize(__ECS_OSTREAM(cls, ##__VA_ARGS__)& out) const         \
  {                                                                           \
    ELLE_LOG_TRACE_COMPONENT("elle.concept");                                 \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    ELLE_LOG_TRACE(                                                           \
        "Serialize %p using %s implem", this, ELLE_PRETTY_OBJECT_TYPE(this)   \
    ) { return this->Implem::serialize(out); }                                \
  }                                                                           \
  /**/

// dump a virtual deserialize method into a serializable class
# define __ECS_DESERIALIZE(cls, ...)                                          \
  virtual void deserialize(__ECS_IARCHIVE(cls, ##__VA_ARGS__)& ar)            \
  {                                                                           \
    ELLE_LOG_TRACE_COMPONENT("elle.concept");                                 \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    ELLE_LOG_TRACE(                                                           \
        "Deserialize %p using %s implem", this, ELLE_PRETTY_OBJECT_TYPE(this) \
    ) { return this->Implem::deserialize(ar); }                               \
  }                                                                           \
  virtual void deserialize(__ECS_ISTREAM(cls, ##__VA_ARGS__)& in)             \
  {                                                                           \
    ELLE_LOG_TRACE_COMPONENT("elle.concept");                                 \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    ELLE_LOG_TRACE(                                                           \
        "Deserialize %p using %s implem", this, ELLE_PRETTY_OBJECT_TYPE(this) \
    ) { return this->Implem::deserialize(in); }                               \
  }                                                                           \
  /**/

#endif

