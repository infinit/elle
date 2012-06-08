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

// shortcut for template argument that accept an archive
# define __ECS_ARCHIVE_TPL(Archive)                                           \
  template<elle::serialize::ArchiveMode> class Archive                        \
  /**/

// shortcut for defaulted archive type as template argument
# define __ECS_DEFAULT_ARCHIVE_TPL(Archive)                                   \
  __ECS_ARCHIVE_TPL(Archive) = elle::serialize::BinaryArchive                 \
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
        typedef Archive<elle::serialize::ArchiveMode::Input> InputArchive;
        typedef Archive<elle::serialize::ArchiveMode::Output> OutputArchive;

        virtual ~_Serializable() {}
        virtual void serialize(OutputArchive& archive) const = 0;
        virtual void deserialize(InputArchive& archive) = 0;
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
        typename contract::Serializable<Archive>::OutputArchive OutputArchive;
      typedef
        typename contract::Serializable<Archive>::InputArchive  InputArchive;

      // implem methods
      virtual void serialize(OutputArchive& archive) const
        { archive << static_cast<T const&>(*this); }
      virtual void deserialize(InputArchive& archive)
        { archive >> static_cast<T&>(*this); }
    };


  }
}

///
/// - internal macros ---------------------------------------------------------
///

# define __ECS_OARCHIVE(...)                                                  \
  typename elle::concept::Serializable<__VA_ARGS__>::OutputArchive                     \
  /**/
# define __ECS_IARCHIVE(...)                                                  \
  typename elle::concept::Serializable<__VA_ARGS__>::InputArchive                      \
  /**/

// dump a virtual serialize method into a serializable class
# define __ECS_SERIALIZE(cls, ...)                                            \
 virtual void serialize(__ECS_OARCHIVE(cls, ##__VA_ARGS__)& ar) const         \
  {                                                                           \
    ELLE_LOG_TRACE_COMPONENT("elle.concept");                                 \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    ELLE_LOG_TRACE("Serialize %p using %s implem", this, #cls)                \
    { return this->Implem::serialize(ar); }                                   \
  }                                                                           \
  /**/

// dump a virtual deserialize method into a serializable class
# define __ECS_DESERIALIZE(cls, ...)                                          \
  virtual void deserialize(__ECS_IARCHIVE(cls, ##__VA_ARGS__)& ar)            \
  {                                                                           \
    ELLE_LOG_TRACE_COMPONENT("elle.concept");                                 \
    typedef elle::concept::Serializable<cls, ##__VA_ARGS__> Implem;           \
    ELLE_LOG_TRACE("Deserialize %p using %s implem", this, #cls)              \
    { return this->Implem::deserialize(ar); }                                 \
  }                                                                           \
  /**/

#endif

