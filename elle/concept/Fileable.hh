#ifndef  ELLE_CONCEPT_FILEABLE_HH
# define ELLE_CONCEPT_FILEABLE_HH

///
/// This module provides the 'Fileable' concept.
/// Requirements:
///   - The class is serializable (XXX make doc on serializable concept)
/// Effects:
///   - Provides two virtual methods: Store(Path) and Load(Path)
///
/// Example 1: This makes 'MyClass' `fileable` with a BinaryArchive
/// ---------------------------------------------------------------------------
///   class MyClass
///     : public elle::concept::Fileable<>
///   {};
///
/// Example 2: When some base class are also fileable
/// ---------------------------------------------------------------------------
///   class MyClass2
///     : public MyClass // MyClass is fileable too
///     , public elle::concept::Fileable<>
///   {
///   public:
///     ELLE_CONCEPT_FILEABLE_METHODS(MyClass2); // desambiguation is needed
///   };
/// ---------------------------------------------------------------------------
///
/// Example 3: If you need an abstract base class that enforce the fileable
///            contract you will need to inherit from AbstractFileable
/// ---------------------------------------------------------------------------
///   class MyClassInterface
///     : public elle::concept::AbstractFileable<>
///   {};
///
///   class MyClassImplem
///     : public elle::concept::Fileable<MyclassImplem>
///   {};
/// ---------------------------------------------------------------------------
///

# include <string>
# include <typeinfo>

# include <elle/io/Path.hh>
# include <elle/serialize/BinaryArchive.fwd.hh>

///
/// - exposed macros ----------------------------------------------------------
///

// Desambiguate Fileable<T> methods
# define ELLE_CONCEPT_FILEABLE_METHODS(cls)                                   \
  ELLE_CONCEPT_FILEABLE_LOAD(cls)                                             \
  ELLE_CONCEPT_FILEABLE_STORE(cls)                                            \
  /**/

// This version does not work in any case
//  using elle::concept::Fileable<cls>::Load;                                   \
//  using elle::concept::Fileable<cls>::Store                                   \
//  /**/



# define ELLE_CONCEPT_FILEABLE_STORE(cls)                                     \
  virtual elle::Status Store(elle::io::Path const& path) const                \
  {                                                                           \
    return this->elle::concept::Fileable<cls>::Store(path);                   \
  }                                                                           \
  /**/

# define ELLE_CONCEPT_FILEABLE_LOAD(cls)                                      \
  virtual elle::Status Load(elle::io::Path const& path)                       \
  {                                                                           \
    return this->elle::concept::Fileable<cls>::Load(path);                    \
  }                                                                           \
  /**/

// Force using methods from AbstractFileable class.
# define ELLE_CONCEPT_FILEABLE_ABSTRACT_METHODS()                             \
  using elle::concept::AbstractFileable<>::Load;                              \
  using elle::concept::AbstractFileable<>::Store                              \
  /**/

namespace elle
{
  namespace concept
  {

    ///
    /// - macros helpers ------------------------------------------------------
    ///

# define _ELLE_CONCEPT_FILEABLE_TPL_AR_DECL                                   \
  template<elle::serialize::ArchiveMode> class Archive                        \
  /**/

# define _ELLE_CONCEPT_FILEABLE_TPL_AR_DECL_DEFAULT                           \
  _ELLE_CONCEPT_FILEABLE_TPL_AR_DECL = elle::serialize::BinaryArchive         \
  /**/

    ///
    /// - internals -----------------------------------------------------------
    ///
    namespace detail
    {

      // common interface, not exposed
      template<_ELLE_CONCEPT_FILEABLE_TPL_AR_DECL>
      struct IFileable
      {
      public:
        virtual elle::Status Load(elle::io::Path const& path) = 0;
        virtual elle::Status Store(elle::io::Path const&) const = 0;
        virtual ~IFileable() {}
      };

    } // !detail

    ///
    /// - concept implem ------------------------------------------------------
    ///

    /// Makes a type T `fileable` when it inherits Fileable<T>
    template<typename T, _ELLE_CONCEPT_FILEABLE_TPL_AR_DECL_DEFAULT>
    struct Fileable
      : virtual detail::IFileable<Archive>
    {
      Status Load(elle::io::Path const& path);
      Status Store(elle::io::Path const& path) const;
    };


    ///
    /// - concept contract ----------------------------------------------------
    ///

    ///
    /// This interface enforces that the final type is also `fileable`.
    ///
    template<_ELLE_CONCEPT_FILEABLE_TPL_AR_DECL_DEFAULT>
    struct AbstractFileable
      : public virtual detail::IFileable<Archive>
    {};

  } // !concept
} // !elle

# include "Fileable.hxx"

#endif
