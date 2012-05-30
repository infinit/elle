#ifndef ELLE_SERIALIZE_ARCHIVABLECLASS_HH
# define ELLE_SERIALIZE_ARCHIVABLECLASS_HH

namespace elle { namespace serialize {

    ///
    /// This structure is used to determine the version of a class
    /// serialization.  The version defaults to 0, but you may want to
    /// specialize this class to set a superior version number.
    ///
    template<typename T> struct ArchivableClass
      {
        static unsigned int const version = 0;
      };

}}

/// Convenient macro to declare the version of a serialized class
# define ELLE_ARCHIVE_CLASS_VERSION(Class_, version_)                             \
  namespace elle { namespace serialize {                                          \
    template<> struct ArchivableClass                                             \
      {                                                                           \
        static unsigned int const version = version_;                             \
      };                                                                          \
  }}

#endif /* ! ARCHIVABLECLASS_HH */


