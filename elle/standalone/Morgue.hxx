//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [fri aug 26 17:08:22 2011]
//

#ifndef ELLE_STANDALONE_MORGUE_HXX
#define ELLE_STANDALONE_MORGUE_HXX

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

  namespace standalone
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method registers an address to delete.
    ///
    /// note that should have the timer not yet been allocated and set up,
    /// this is done in this method.
    ///
    template <typename T>
    Status              Morgue::Register(T*                     instance)
    {
      // if the timer has not been allocated, set it up.
      if (this->timer == NULL)
        {
          // allocate the timer.
          this->timer = new Timer;

          // create the timer.
          if (this->timer->Create(Timer::ModeRepetition) == Status::Error)
            escape("unable to create the timer");

          // subscribe to the timer's signal.
          if (this->timer->signal.timeout.Subscribe(
                Callback<>::Infer(&Morgue::Bury, this)) == Status::Error)
            escape("unable to subscribe to the signal");

          // start the timer.
          if (this->timer->Start(Morgue::Frequency) == Status::Error)
            escape("unable to start the timer");
        }

      // add the instance to the container.
      this->container.push_back(static_cast<Meta*>(instance));

      return Status::Ok;
    }

  }
}

#endif
