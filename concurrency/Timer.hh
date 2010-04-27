//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/libraries/elle/concurrency/Timer.hh
//
// created       julien quintard   [wed mar 17 11:40:38 2010]
// updated       julien quintard   [mon apr 26 19:06:41 2010]
//

#ifndef ELLE_CONCURRENCY_TIMER_HH
#define ELLE_CONCURRENCY_TIMER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Meta.hh>

#include <elle/miscellaneous/Status.hh>
#include <elle/miscellaneous/Callback.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QTimer>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace miscellaneous;

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
      public ::QObject,

      public Meta
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
