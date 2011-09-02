//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Broker.hh
//
// created       julien quintard   [sun may 29 13:15:23 2011]
// updated       julien quintard   [thu sep  1 16:11:38 2011]
//

#ifndef ELLE_CONCURRENCY_BROKER_HH
#define ELLE_CONCURRENCY_BROKER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Natural.hh>

#include <elle/radix/Status.hh>
#include <elle/radix/Parameters.hh>

#include <elle/concurrency/Callback.hh>

#include <elle/idiom/Close.hh>
# include <QObject>
# include <QSocketNotifier>
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
    /// so that it can take action accordingly.
    ///
    class Broker:
      public ::QObject
    {
      Q_OBJECT;

    public:
      //
      // constructors & destructors
      //
      Broker(const Natural16,
	     const Callback<
	       Status,
	       Parameters<Natural16>
	       >&);

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
      Natural16			descriptor;
      Callback<
	Status,
	Parameters<Natural16>
	>			callback;

      ::QSocketNotifier		notifier;

    private slots:
      //
      // slots
      //
      void		_trigger();
    };

  }
}

#endif
