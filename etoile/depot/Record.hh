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
// updated       julien quintard   [thu jan 28 00:49:07 2010]
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
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a cache record.
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

      // XXX probleme de la taille (inaccessible de cell et unit) +
      // probleme du timer a faire.
      Natural32		size;

      ::QTimer*		timer;

      //
      // methods
      //
      Status		Create(const hole::Address&,
			       const Natural32);
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
