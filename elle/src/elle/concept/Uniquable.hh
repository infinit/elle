#pragma once

#include <elle/types.hh>

#include <elle/io/fwd.hh>
#include <elle/serialize/fwd.hh>
#include <elle/serialize/Serializable.hh>

/// The unique default archive type is defined with this macros.
#define ELLE_CONCEPT_UNIQUABLE_DEFAULT_ARCHIVE                          \
  elle::serialize::Base64Archive                                        \
  /**/

// internal shortcut
#define __ECU_ARCHIVE_TPL(Archive)                                      \
  __ESS_ARCHIVE_TPL(Archive)                                            \
  /**/

// internal shortcut
#define __ECU_DEFAULT_ARCHIVE_TPL(Archive)                              \
  __ECU_ARCHIVE_TPL(Archive) = ELLE_CONCEPT_UNIQUABLE_DEFAULT_ARCHIVE   \
  /**/

namespace elle
{
  namespace concept
  {

    ///
    /// This class enables the inherited class to be represented in a unique
    /// compact string-based form.
    ///
    /// Besides, this unique string representation can be used to save and
    /// restore the original object from/to main memory. indeed a unique
    /// instance is nothing more that a base64/hexadecimal/etc.  representation
    /// of its archive.
    ///
    template<__ECU_DEFAULT_ARCHIVE_TPL(Archive)>
    struct Uniquable :
      serialize::Serializable<Archive>
    {
    public:
      ///
      /// this method returns in the Unique, the string-based representation of
      /// the object's archive.
      ///
      Status Save(elle::io::Unique& out) const;

      ///
      /// this method reconstructs the living object based on the unique string
      /// representation.
      ///
      Status Restore(elle::io::Unique const& in);
    };

    /// Makes your class Uniquable, but also Serializable for the default
    /// archive type used by uniquable concept. You should only use this when
    /// you know that you are working on the final class.
    template <typename T, __ECU_DEFAULT_ARCHIVE_TPL(Archive)>
    struct UniquableMixin:
      serialize::SerializableMixin<T, Archive>,
      Uniquable<Archive>
    {};

    /// This is an alias to UniquableMixin should is often preferred for
    /// non-polymorphic classes.
    template <typename T, __ECU_DEFAULT_ARCHIVE_TPL(Archive)>
    struct MakeUniquable:
      serialize::SerializableMixin<T, Archive>,
      Uniquable<Archive>
    {};
  }
}

#include <elle/concept/Uniquable.hxx>
