//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [tue nov  1 16:19:50 2011]
//

#ifndef ELLE_CONCURRENCY_ENTRANCE_HXX
#define ELLE_CONCURRENCY_ENTRANCE_HXX

//
// ---------- includes --------------------------------------------------------
//

#include <elle/concurrency/Callback.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    template <typename R,
              typename... T>
    Entrance< R, Parameters<T...> >::Entrance(
        const Closure< R, Parameters<T...> >&                   closure):
      closure(closure)
    {
      // create the timer.
      if (this->timer.Create(Timer::ModeSingle) == Status::Error)
        yield(_(), "unable to create the timer");

      // subscribe to the timer's signal.
      if (this->timer.signal.timeout.Subscribe(
            Callback<>::Infer(&Entrance::Timeout, this)) == Status::Error)
        yield(_(), "unable to subscribe to the signal");

      // start the timer.
      if (this->timer.Start() == Status::Error)
        yield(_(), "unable to start the timer");
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the entrance.
    ///
    template <typename R,
              typename... T>
    Status
    Entrance< R, Parameters<T...> >::Dump(const Natural32       margin) const
    {
      String            alignment(margin, ' ');

      std::cout << alignment << "[Entrance]" << std::endl;

      // dump the closure.
      if (this->closure.Dump(margin + 2) == Status::Error)
        escape("unable to dump the closure");

      // dump the timer.
      if (this->timer.Dump(margin + 2) == Status::Error)
        escape("unable to dump the timer");

      return Status::Ok;
    }

//
// ---------- callbacks -------------------------------------------------------
//


    ///
    /// this callbacks is triggered once the timer expires.
    ///
    template <typename R,
              typename... T>
    Status
    Entrance< R, Parameters<T...> >::Timeout()
    {
      // trigger the closure.
      if (this->closure.Call() == Status::Error)
        escape("an error occured in the closure");

      return Status::Ok;
    }

  }
}

#endif
