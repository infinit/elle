#ifndef ELLE_IO_FILEABLE_HH
# define ELLE_IO_FILEABLE_HH

# include <elle/io/Path.hh>
# include <elle/serialize/BinaryArchive.fwd.hh>

namespace elle
{
  using namespace radix;

  namespace io
  {

    ///
    /// Adapt any class to be storable in a file (it uses the Strangly
    /// Recurring Pattern).
    ///
    template<
        typename T
      , template<
            elle::serialize::ArchiveMode
        > class DefaultArchive = elle::serialize::BinaryArchive
    > class Fileable
    {
    public:
      /// Load T given an archive type and a path
      template<template<elle::serialize::ArchiveMode> class Archive = DefaultArchive>
        Status Load(Path const& path);

      /// this method stores the object in the given file.
      template<template<elle::serialize::ArchiveMode> class Archive = DefaultArchive>
        Status Store(Path const&) const;

      /// this method erases the given file.
      Status Erase(const Path&) const;

      /// this method returns true if the given file exists.
      Status Exist(const Path&) const;
    };

  }
}


/// If you need to overload the Load/Store methods of a Fileable class, you'll have to publish the
/// Fileable<T> methods explicitly with this macro.
/// --------------------
/// class MyClass : public elle::io::Fileable<MyClass>
/// {
/// void Load(bool b); // overrides all elle::io::Fileable<MyClass> methods
///
/// ELLE_IO_USING_FILEABLE_LOAD(MyClass);
/// };
/// ---------------------
#define ELLE_IO_USING_FILEABLE_LOAD(T)        \
  using ::elle::io::Fileable<T>::Load         \

/// As `ELLE_IO_USING_LOAD', but for Fileable<T>::Store method.
#define ELLE_IO_USING_FILEABLE_STORE(T)       \
  using ::elle::io::Fileable<T>::Store        \

/// Both Load and Store methods.
#define ELLE_IO_USING_FILEABLE_METHODS(T)     \
  ELLE_IO_USING_FILEABLE_LOAD(T);             \
  ELLE_IO_USING_FILEABLE_STORE(T)             \

#endif
