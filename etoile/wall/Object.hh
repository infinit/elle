//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Object.hh
//
// created       julien quintard   [fri aug 14 15:36:23 2009]
// updated       julien quintard   [mon may  3 15:59:35 2010]
//

#ifndef ETOILE_WALL_OBJECT_HH
#define ETOILE_WALL_OBJECT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/Manifest.hh>

#include <etoile/path/Way.hh>

#include <etoile/context/Identifier.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides general-purpose method for manipulating objects.
    ///
    /// this is very useful when the caller does not know the genre of
    /// the object i.e file, directory or link.
    ///
    class Object
    {
    public:
      //
      // static methods
      //
      static elle::Status	Load(const path::Way&);
      static elle::Status	Lock(const context::Identifier&);
      static elle::Status	Release(const context::Identifier&);
      static elle::Status	Information(const context::Identifier&);
      static elle::Status	Discard(const context::Identifier&);
      static elle::Status	Store(const context::Identifier&);
    };

  }
}

#endif
