//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun may 29 13:15:23 2011]
//

#ifndef ELLE_CONCURRENCY_BROKER_HH
#define ELLE_CONCURRENCY_BROKER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/system/Platform.hh>

#include <elle/types.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Callback.hh>
#include <elle/concurrency/Signal.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
#  include <QSocketNotifier>
# elif defined(INFINIT_WINDOWS)
#  include <private/qwineventnotifier_p.h>
# endif
#include <elle/idiom/Open.hh>

namespace elle
{

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
    class Broker:
      public ::QObject
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      Broker(const Natural16);
#elif defined(INFINIT_WINDOWS)
      Broker(HANDLE);
#else
# error "unsupported platform"
#endif

      //
      // methods
      //
      Status            Activate();
      Status            Deactivate();

      //
      // callbacks
      //
      Status            Trigger();

      //
      // attributes
      //
#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
      Natural16                 descriptor;
      ::QSocketNotifier         notifier;
#elif defined(INFINIT_WINDOWS)
      ::QWinEventNotifier       notifier;
#else
# error "unsupported platform"
#endif

      //
      // signals
      //
      struct                    Signals
      {
        Signal<
          Parameters<
#if defined(INFINIT_LINUX) || defined(INFINIT_MACOSX)
            Natural16
#elif defined(INFINIT_WINDOWS)
            HANDLE
#else
# error "unsupported platform"
#endif
            >
          >                     ready;
      }                         signal;

    private slots:
      //
      // slots
      //
      void              _trigger();
    };

  }
}

#endif
