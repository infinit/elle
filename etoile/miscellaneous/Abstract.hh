//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed mar 31 16:05:57 2010]
//

#ifndef ETOILE_MISCELLANEOUS_ABSTRACT_HH
#define ETOILE_MISCELLANEOUS_ABSTRACT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

namespace etoile
{
  namespace miscellaneous
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class describes an object's meta data.
    ///
    class Abstract:
      public elle::Object
    {
    public:
      //
      // constants
      //
      static const Abstract             Null;

      //
      // constructors & destructors
      //
      Abstract();

      //
      // method
      //
      elle::Status      Create(const nucleus::Object&);

      //
      // interfaces
      //

      // object
      declare(Abstract);
      elle::Boolean     operator==(const Abstract&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      nucleus::Genre            genre;

      struct
      {
        elle::Time              creation;
        elle::Time              modification;
      }                         stamps;

      nucleus::Offset           size;

      struct
      {
        elle::PublicKey         owner;
        elle::PublicKey         author;
      }                         keys;

      struct
      {
        nucleus::Permissions    owner;
      }                         permissions;

      struct
      {
        nucleus::Version        data;
        nucleus::Version        meta;
      }                         versions;
    };

  }
}

#endif
