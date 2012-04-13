//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [sat may 21 13:21:51 2011]
//

#ifndef NUCLEUS_PROTON_BASE_HH
#define NUCLEUS_PROTON_BASE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <nucleus/proton/Version.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// this makes sure the MutableBlock type is declared.
    ///
    class MutableBlock;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// a base references a precise version of a mutable block.
    ///
    /// this construct is useful to make sure a mutable block derives
    /// from another one, and another one down to the original mutable
    /// block whose ownership can usually be statically verified.
    ///
    /// by following this chain, one can make sure a mutable block lies
    /// in the legitimate block's history, in other words, branches have
    /// not been author .
    ///
    class Base:
      public elle::Object
    {
    public:
      //
      // methods
      //
      elle::Status      Create(const Version&,
                               const elle::Digest&);
      elle::Status      Create(const MutableBlock&);

      elle::Status      Match(const MutableBlock&) const;

      //
      // interfaces
      //

      // object
      declare(Base);
      elle::Boolean     operator==(const Base&) const;

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Version           version;
      elle::Digest      digest;
    };

  }
}

#endif
