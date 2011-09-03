//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Morgue.hxx
//
// created       julien quintard   [fri aug 26 17:08:22 2011]
// updated       julien quintard   [fri sep  2 23:52:08 2011]
//

#ifndef ELLE_RADIX_MORGUE_HXX
#define ELLE_RADIX_MORGUE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/concurrency/Callback.hh>

namespace elle
{
  using namespace standalone;
  using namespace concurrency;

  namespace radix
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// XXX
    ///
    template <typename T>
    Status		Morgue::Register(T*			instance)
    {
      enter();

      // if the timer has not been allocated, set it up.
      if (this->timer == NULL)
	{
	  // allocate the timer.
	  this->timer = new Timer;

	  // create the timer.
	  if (this->timer->Create(Timer::ModeRepetition) == StatusError)
	    escape("unable to create the timer");

	  // subscribe to the timer's signal.
	  if (this->timer->signal.timeout.Subscribe(
	        Callback<>::Infer(&Morgue::Bury, this)) == StatusError)
	    escape("unable to subscribe to the signal");

	  // start the timer.
	  if (this->timer->Start(Morgue::Frequency) == StatusError)
	    escape("unable to start the timer");
	}

      // add the instance to the container.
      this->container.push_back(static_cast<Meta*>(instance));

      leave();
    }

  }
}

#endif
