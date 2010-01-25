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
// updated       julien quintard   [thu jan  7 13:20:41 2010]
//

#ifndef ETOILE_DEPOT_RECORD_HH
#define ETOILE_DEPOT_RECORD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Core.hh>
#include <etoile/hole/Hole.hh>

#include <QObject>
#include <QTimer>

namespace etoile
{
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
      ~Record();

      //
      // attributes
      //
      hole::Block*	block;
      // XXX Unit*		unit;

      Natural32		expiration;
      ::QTimer*		timer;

      //
      // methods
      //
      Status		Create(hole::Block*,
			       const core::Time&);

      Status		Update(hole::Block*,
			       const core::Time&);

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
