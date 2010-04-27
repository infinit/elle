//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Unit.hh
//
// created       julien quintard   [tue jan 26 14:00:35 2010]
// updated       julien quintard   [mon apr 26 18:37:20 2010]
//

#ifndef ETOILE_DEPOT_UNIT_HH
#define ETOILE_DEPOT_UNIT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Block.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class holds a block in the reserve i.e on a more stable but
    /// slower storage medium that main memory such as a hard disk.
    ///
    class Unit:
      public Meta,
      public Dumpable
    {
    public:
      //
      // methods
      //
      Status		Load(const String);

      Status		Set(hole::Block*);
      Status		Get(hole::Block*);
      Status		Destroy();

      //
      // interfaces
      //
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      String		path;
      Natural32		size;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Repository.hh>

#endif
