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
          , typename CharType
          , template<elle::serialize::ArchiveMode mode__, typename> class ST
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

#endif
