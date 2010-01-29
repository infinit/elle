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
// updated       julien quintard   [thu jan 28 15:10:22 2010]
//

#ifndef ETOILE_DEPOT_RECORD_HH
#define ETOILE_DEPOT_RECORD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/hole/Hole.hh>

#include <etoile/depot/Cell.hh>
#include <etoile/depot/Unit.hh>
#include <etoile/depot/Repository.hh>
#include <etoile/depot/Location.hh>

#include <QObject>
#include <QTimer>

namespace etoile
{
  using namespace core;

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
    class		Record:
      public ::QObject,
      public Dumpable
    {
      Q_OBJECT;

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
      }			data;

      ::QTimer*		timer;

      //
      // methods
      //
      Status		Create(const hole::Address&);
      Status		Timer();
      Status		Destroy();

      //
      // interfaces
      //
      Status		Dump(const Natural32 = 0) const;

    private slots:
      //
      // slots
      //
      void		Timeout();
    };

  }
}

#endif
