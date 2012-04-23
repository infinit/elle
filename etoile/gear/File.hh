//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 23:13:51 2009]
//

#ifndef ETOILE_GEAR_FILE_HH
#define ETOILE_GEAR_FILE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>
#include <elle/concurrency/Callback.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Object.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/automaton/File.hh>

#include <etoile/wall/File.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a file-specific context.
    ///
    /// this context derives the Object context and therefore benefits from
    /// all the object-related attributes plus the contents i.e the catalog
    /// in the case of a file.
    ///
    class File:
      public Object
    {
    public:
      //
      // constants
      //
      static const Nature                       N = NatureFile;

      //
      // types
      //
      typedef wall::File                        W;
      typedef automaton::File                   A;
      typedef elle::concurrency::Callback<
                elle::Status,
                elle::radix::Parameters<
                  File&
                  >
                >                               S;

      typedef nucleus::Data                     C;

      //
      // constructors & destructors
      //
      File();
      ~File();

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
