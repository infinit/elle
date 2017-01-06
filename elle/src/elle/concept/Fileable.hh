#pragma once

/// This module provides the 'Fileable' concept.
/// Requirements:
///   - The class is serializable (XXX make doc on serializable concept)
/// Effects:
///   - Provides virtual methods: store(Path), load(Path), erase(Path) and
///     exists(Path)
///
/// Example 1: This makes 'MyClass' `fileable` with a BinaryArchive
/// ---------------------------------------------------------------------------
///   class MyClass:
///     public elle::concept::Fileable<>
///   {};
///
/// Example 2: When some base class are also fileable
/// ---------------------------------------------------------------------------
///   class MyClass2:
///     public MyClass, // MyClass is fileable too
///     public elle::concept::Fileable<>
///   {
///   public:
///     ELLE_CONCEPT_FILEABLE_METHODS(MyClass2); // desambiguation is needed
///   };
/// ---------------------------------------------------------------------------
///
/// Example 3: If you need an abstract base class that enforce the fileable
///            contract you will need to inherit from Serializable
/// ---------------------------------------------------------------------------
///   class MyClassInterface:
///     public elle::Serializable<>
///   {};
///
///   class MyClassImplem:
///     public elle::concept::Fileable<MyclassImplem>
///   {};
/// ---------------------------------------------------------------------------

#include <elle/types.hh>
#include <elle/io/fwd.hh>
#include <elle/serialize/Serializable.hh>

#include <string>
#include <typeinfo>

#define ELLE_CONCEPT_FILEABLE_METHODS(...)                              \
  using elle::concept::Fileable<__VA_ARGS__>::store;                    \
  using elle::concept::Fileable<__VA_ARGS__>::load;                     \
  using elle::concept::Fileable<__VA_ARGS__>::erase;                    \
  using elle::concept::Fileable<__VA_ARGS__>::exists;

namespace elle
{
  namespace concept
  {
    template <__ESS_DEFAULT_ARCHIVE_TPL(Archive)>
    struct Fileable:
      virtual serialize::_Serializable<Archive>
    {
      /// Deserializes the instance from a file.
      void
      load(elle::io::Path const& path);
      /// Serializes the instance to a file.
      void
      store(elle::io::Path const& path) const;
      /// Erases the file containing the instance's serialization
      /// representation.
      static
      void
      erase(elle::io::Path const& path);
      /// Returns true if the instance's serialization representation
      /// file exists.
      static
      Boolean
      exists(elle::io::Path const& path);
    };

    template <typename T, __ESS_DEFAULT_ARCHIVE_TPL(Archive)>
    struct MakeFileable:
      serialize::SerializableMixin<T, Archive>,
      Fileable<Archive>
    {};

  }
}

#include <elle/concept/Fileable.hxx>

