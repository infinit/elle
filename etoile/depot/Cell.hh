//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Cell.hh
//
// created       julien quintard   [tue jan 26 14:00:35 2010]
// updated       julien quintard   [wed jan 27 23:41:53 2010]
//

#ifndef ETOILE_DEPOT_CELL_HH
#define ETOILE_DEPOT_CELL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Hole.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Cell:
      public Dumpable
    {
    public:
      //
      // constructors & destructors
      //
      Cell();

      //
      // methods
      //
      Status		Set(hole::Block*);
      Status		Get(hole::Block*&);
      Status		Destroy();

      //
      // interfaces
      //
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      hole::Block*	block;
    };

  }
}

#endif
