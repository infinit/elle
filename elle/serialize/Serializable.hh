#ifndef  ELLE_SERIALIZE_SERIALIZABLE_HH
# define ELLE_SERIALIZE_SERIALIZABLE_HH

# include "fwd.hh"

/// Desambiguate Serializable<T, ArchiveType = BinaryArchive> methods
# define ELLE_SERIALIZE_SERIALIZABLE_METHODS(...)                       \
  __ESS_SERIALIZE(__VA_ARGS__)                                          \
  __ESS_DESERIALIZE(__VA_ARGS__)                                        \
/**/

///
/// - internal macros ---------------------------------------------------------
///

# define __ESS_DEFAULT_ARCHIVE  elle::serialize::BinaryArchive

// shortcut for template argument that accept an archive
# define __ESS_ARCHIVE_TPL(Archive)                                           \
  template<elle::serialize::ArchiveMode> class Archive                        \
/**/

// shortcut for defaulted archive type as template argument
# define __ESS_DEFAULT_ARCHIVE_TPL(Archive)                                   \
  __ESS_ARCHIVE_TPL(Archive) = __ESS_DEFAULT_ARCHIVE                          \
/**/

namespace elle
{
  namespace serialize
  {

    template <__ESS_ARCHIVE_TPL(Archive)>
    struct _Serializable;

    ///
    /// Contract for virtual serializable classes. You might want to inherit
    /// this class when you want to ensure that final types will be
    /// serializable polymorphically.
    ///
    template <__ESS_DEFAULT_ARCHIVE_TPL(Archive)>
    struct Serializable:
      public virtual _Serializable<Archive>
    {};

    ///
    /// Extend a serializable class (that has a defined serializer) by
    /// implementing the Serializable interface.
    ///
    template <typename T, __ESS_DEFAULT_ARCHIVE_TPL(Archive)>
    struct SerializableMixin;

    /// Is T has Serializable<Archive> contract ?
    template <typename T, typename Archive>
    struct IsSerializable;

    /// Retreive Serializable interface for any archive type.
    template<typename Archive>
    struct SerializableFor;

  }
}

# include "Serializable.hxx"

#endif
