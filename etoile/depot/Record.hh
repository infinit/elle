//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/depot/Record.hh
//
// created       julien quintard   [thu dec  3 03:07:53 2009]
// updated       julien quintard   [thu may  5 16:37:39 2011]
//

#ifndef ETOILE_DEPOT_RECORD_HH
#define ETOILE_DEPOT_RECORD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/depot/Cell.hh>
#include <etoile/depot/Unit.hh>
#include <etoile/depot/Location.hh>

namespace etoile
{
  namespace depot
  {

//
// ---------- forward declarations --------------------------------------------
//

    ///
    /// XXX
    ///
    class Cell;
    class Unit;

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class holds a block state, either in its cell form i.e in main
    /// memory cache or in its unit form i.e in the reserve.
    ///
    class Record:
      public elle::Entity
    {
    public:
      //
      // constructors & destructors
      //
      Record();

      //
      // attributes
      //
      Location		location;

      nucleus::Address	address;

      union
      {
	Cell*		cell;
	Unit*		unit;
      };

      elle::Timer*	timer;

      //
      // methods
      //
      elle::Status	Create(const nucleus::Address&);
      elle::Status	Monitor();
      elle::Status	Destroy();

      //
      // interfaces
      //
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // callbacks
      //
      elle::Status	Discard();
    };

  }
}

#endif
