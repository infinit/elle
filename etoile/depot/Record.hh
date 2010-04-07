//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/depot/Record.hh
//
// created       julien quintard   [thu dec  3 03:07:53 2009]
// updated       julien quintard   [fri apr  2 13:55:20 2010]
//

#ifndef ETOILE_DEPOT_RECORD_HH
#define ETOILE_DEPOT_RECORD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/hole/Address.hh>

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
      public Dumpable
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

      hole::Address	address;

      union
      {
	Cell*		cell;
	Unit*		unit;
      };

      Timer*		timer;

      //
      // methods
      //
      Status		Create(const hole::Address&);
      Status		Monitor();
      Status		Destroy();

      //
      // interfaces
      //
      Status		Dump(const Natural32 = 0) const;

      //
      // callbacks
      //
      Status		Discard();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/depot/Repository.hh>

#endif
