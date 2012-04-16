//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 23:13:51 2009]
//

#ifndef ETOILE_GEAR_DIRECTORY_HH
#define ETOILE_GEAR_DIRECTORY_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Object.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/path/Slice.hh>

#include <etoile/automaton/Directory.hh>

#include <etoile/wall/Directory.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a directory-specific context.
    ///
    /// this context derives the Object context and therefore benefits from
    /// all the object-related attributes plus the contents i.e the catalog
    /// in the case of a directory.
    ///
    class Directory:
      public Object
    {
    public:
      //
      // constants
      //
      static const Nature                       N = NatureDirectory;

      //
      // types
      //
      typedef wall::Directory                   W;
      typedef automaton::Directory              A;
      typedef elle::Callback<
                elle::Status::,
                elle::Parameters<
                  Directory&
                  >
                >                               S;

      typedef nucleus::Catalog                  C;

      //
      // constructors & destructors
      //
      Directory();
      ~Directory();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      nucleus::Contents<C>*     contents;
    };

  }
}

#endif
