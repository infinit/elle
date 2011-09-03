//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Timer.hh
//
// created       julien quintard   [wed mar 17 11:40:38 2010]
// updated       julien quintard   [fri sep  2 22:11:12 2011]
//

#ifndef ELLE_CONCURRENCY_TIMER_HH
#define ELLE_CONCURRENCY_TIMER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Entity.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Signal.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QTimer>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace core;
  using namespace radix;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a timer functionality i.e the possibility to
    /// trigger an event.
    ///
    /// note that the duration is expressed in milliseconds.
    ///
    class Timer:
      public ::QObject,

      public Entity
    {
      Q_OBJECT;

    public:
      //
      // enumerations
      //
      enum Mode
	{
	  ModeSingle,
	  ModeRepetition
	};

      //
      // constructors & destructors
      //
      Timer();
      ~Timer();

      //
      // methods
      //
      Status		Create(const Mode);
      Status		Start(const Natural32 = 0);
      Status		Stop();
      Status		Restart(const Natural32 = 0);

      //
      // callbacks
      //
      Status		Timeout();

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      Mode		mode;

      ::QTimer*		timer;

      //
      // signals
      //
      struct
      {
	Signal<
	  Parameters<>
	  >		timeout;
      }			signal;

    private slots:
      //
      // slots
      //
      void		_timeout();
    };

  }
}

#endif
