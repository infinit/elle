//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun may 29 13:15:23 2011]
//

#ifndef ELLE_CONCURRENCY_WINEVENTBROKER_HH
#define ELLE_CONCURRENCY_WINEVENTBROKER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Signal.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <private/qwineventnotifier_p.h>
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
    /// this class represents a broker of event which can be attached to
    /// an event loop so as to be taken into account.
    ///
    /// the broker is notified whenever data is ready on its file descriptor
    /// so that it can take action accordingly i.e emit the ready signal.
    ///
    class WinEventBroker:
      public ::QObject
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
      WinEventBroker(HANDLE);

      //
      // methods
      //
      Status		Activate();
      Status		Deactivate();

      //
      // callbacks
      //
      Status		Trigger();

      //
      // attributes
      //
      ::QWinEventNotifier       notifier;

      //
      // signals
      //
      struct			Signals
      {
	Signal<
	  Parameters<
	    HANDLE
	    >
	  >			ready;
      }				signal;

    private slots:
      //
      // slots
      //
      void		_trigger();
    };

  }
}

#endif
