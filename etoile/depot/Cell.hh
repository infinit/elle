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
// updated       julien quintard   [thu may  5 16:00:14 2011]
//

#ifndef ETOILE_DEPOT_CELL_HH
#define ETOILE_DEPOT_CELL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

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
      elle::Status	Set(nucleus::Block*);
      elle::Status	Get(nucleus::Block*&);
      elle::Status	Destroy();

      //
      // interfaces
      //
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      nucleus::Block*	block;
    };

  }
}

#endif
