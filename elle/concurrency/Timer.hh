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
// updated       julien quintard   [tue apr  6 18:53:47 2010]
//

#ifndef ELLE_CONCURRENCY_TIMER_HH
#define ELLE_CONCURRENCY_TIMER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>

#include <elle/misc/Status.hh>
#include <elle/misc/Callback.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QTimer>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace misc;

  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides a timer functionality i.e the possibility to
    /// trigger an event.
    ///
    class Timer:
      public ::QObject
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
      Status		Create(const Mode,
			       const Callback<>&);
      Status		Start(Natural32 = 0);
      Status		Stop();
      Status		Restart(Natural32 = 0);

      //
      // entrances
      //
      Status		Timeout();

      //
      // attributes
      //
      ::QTimer*		timer;

      Mode		mode;
      Callback<>	callback;

    private slots:
      //
      // slots
      //
      void		_timeout();
    };

  }
}

#endif
