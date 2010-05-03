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
// updated       julien quintard   [mon may  3 17:40:12 2010]
//

#ifndef ETOILE_DEPOT_CELL_HH
#define ETOILE_DEPOT_CELL_HH

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
    /// this class holds a block in its memory form/
    ///
    class Cell:
      public elle::Entity
    {
    public:
      //
      // constructors & destructors
      //
      Cell();

      //
      // methods
      //
      elle::Status	Set(hole::Block*);
      elle::Status	Get(hole::Block*&);
      elle::Status	Destroy();

      //
      // interfaces
      //
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      hole::Block*	block;
    };

  }
}

#endif
